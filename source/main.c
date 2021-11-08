/*	Author: Brandon Tran
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #10  Exercise #4
 *	Exercise Description: (Challenge) Extend the previous exercise to allow a user to adjust the sound frequency up or down using buttons connected to PA0 (up) and PA1 (down). Using our 1 ms timer abstraction, the fastest you'll be able to pulse is 1 ms on and 1 ms off, meaning 500 Hz. Again: Don’t use the PWM for this task. Hint: You'll probably want to introduce another synchSM that polls the buttons and sets a global variable storing the current frequency that in turn is read by the frequency generator task. 

 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=-YVZ9WbN3Cs
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum TL_states { T_start, ooi, oio, ioo } TL_state;
unsigned char TL_output;
void TickFct_ThreeLeds() {
	switch(TL_state) { // state transitions
		case T_start:
			TL_state = ooi;
		break;
		
		case ooi:
			TL_state = oio;
		break;
		
		case oio:
			TL_state = ioo;
		break;
		
		case ioo:
			TL_state = ooi;
		break;
		
		default:
		break;
	}
	
	switch(TL_state) {
		case T_start:
		break;
		
		case ooi:
			TL_output = 0x01;
		break;
		
		case oio:
			TL_output = 0x02;
		break;
		
		case ioo:
			TL_output = 0x04;
		break;
		
		default:
		break;
	}
}

enum BL_states { B_start, on, off} BL_state;
unsigned char BL_output;
void TickFct_BlinkLed() {
	switch (BL_state) {
		case B_start:
			BL_state = on;
		break;
		
		case on:
			BL_state = off;
		break;
		
		case off:
			BL_state = on;
		break;
		
		default:
		break;
	}
	
	switch (BL_state) {
		case B_start:
		break;
		
		case on:
			BL_output = 0x08;
		break;
		
		case off:
			BL_output = 0x00;
		break;
		
		default:
		break;
	}
}

enum Freq_States { neutral, inc, dec } F_state;
unsigned char freq;
void TickFct_Freq() {
	unsigned char input = ~PINA & 0x03;
	
	switch (F_state) {
		case neutral:
			if(input == 0x02) { F_state = inc; freq++; }
			if(input == 0x01 && freq != 1) { F_state = dec; freq--; }
		break;
		
		case inc:
			if(input == 0x02) {}
			if(input == 0x01 && freq != 1) { F_state = dec; freq--; }
			if(input == 0x00) { F_state = neutral; }
		break;
		
		case dec:
			if(input == 0x02) { F_state = inc; freq++; }
			if(input == 0x01) {}
			if(input == 0x00) { F_state = neutral; }
		break;
	}
}

enum Speaker_states { S_start, r, p } S_state;
unsigned char S_output;
void TickFct_Speaker() {
	unsigned char input = ~PINA & 0x04;
	static unsigned char i = 0;
	
	switch (S_state) {
		case S_start:
			S_state = r;
		break;
		
		case r:
			input ? S_state = p : r;
		break;
		
		case p:
			!input ? S_state = r : p;
		break;
	}
	
	switch (S_state) {
		case S_start:
		break;
		
		case r:
			S_output = 0x00;
			i = 0;
		break;
		
		case p:
			S_output = i / freq % 2 ? 0x10 : 0x00;
			i++;
		break;
	}
}

void TickFct_CombineLEDs() {
	PORTB = TL_output | BL_output | S_output;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
    unsigned long TL_elapsedTime = 300;
    unsigned long BL_elapsedTime = 1000;
    const unsigned long timerPeriod = 1;
        
    TimerSet(timerPeriod);
    TimerOn();
    TL_state = T_start;
    BL_state = B_start;
    S_state = S_start;
    freq = 2;
    while (1) {
		if(TL_elapsedTime >= 300) {
			TickFct_ThreeLeds();
			TL_elapsedTime = 0;
		}
		if(BL_elapsedTime >= 1000) {
			TickFct_BlinkLed();
			BL_elapsedTime = 0;
		}
		TickFct_Freq();
		TickFct_Speaker();
		TickFct_CombineLEDs();
		while (!TimerFlag) {}
		TimerFlag = 0;
		TL_elapsedTime += timerPeriod;
		BL_elapsedTime += timerPeriod;
    }
    return 1;
}
