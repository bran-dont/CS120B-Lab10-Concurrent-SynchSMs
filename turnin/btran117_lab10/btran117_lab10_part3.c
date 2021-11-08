/*	Author: Brandon Tran
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #10  Exercise #3
 *	Exercise Description: To the previous exercise's implementation, connect your speaker's red wire to PB4 and black wire to ground. Add a third task that toggles PB4 on for 2 ms and off for 2 ms as long as a switch on PA2 is in the on position. Don’t use the PWM for this task.

 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=2k7VfTsDnMU
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
			S_output = i / 2 % 2 ? 0x10 : 0x00;
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
    while (1) {
		if(TL_elapsedTime >= 300) {
			TickFct_ThreeLeds();
			TL_elapsedTime = 0;
		}
		if(BL_elapsedTime >= 1000) {
			TickFct_BlinkLed();
			BL_elapsedTime = 0;
		}
		TickFct_Speaker();
		TickFct_CombineLEDs();
		while (!TimerFlag) {}
		TimerFlag = 0;
		TL_elapsedTime += timerPeriod;
		BL_elapsedTime += timerPeriod;
    }
    return 1;
}
