/*	Author: Brandon Tran
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: Connect LEDs to PB0, PB1, PB2, and PB3. 
In one state machine (ThreeLEDsSM), output to a shared variable (threeLEDs) the following behavior: set only bit 0 to 1, then only bit 1, then only bit 2 in sequence for 1 second each. 
In a second state machine (BlinkingLEDSM), output to a shared variable (blinkingLED) the following behavior: set bit 3 to 1 for 1 second, then 0 for 1 second. 
In a third state machine (CombineLEDsSM), combine both shared variables and output to the PORTB. 
Note: only one SM is writing to outputs. Do this for the rest of the quarter.

 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=Nu_LDypYsBw
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum TL_states { Tstart, ooi, oio, ioo } TL_state;
unsigned char TLoutput;
void TickFct_ThreeLeds() {
	switch(TL_state) { // state transitions
		case Tstart:
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
		case ooi:
			TLoutput = 0x01;
		break;
		
		case oio:
			TLoutput = 0x02;
		break;
		
		case ioo:
			TLoutput = 0x04;
		break;
		
		default:
		break;
	}
}

enum BL_states { Bstart, on, off} BL_state;
unsigned char BLoutput;
void TickFct_BlinkLed() {
	switch (BL_state) {
		case Bstart:
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
		case on:
			BLoutput = 0x08;
		break;
		
		case off:
			BLoutput = 0x00;
		break;
		
		default:
		break;
	}
}

void TickFct_CombineLEDs() {
	PORTB = TLoutput | BLoutput;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
    unsigned long TL_elapsedTime = 1000;
    unsigned long BL_elapsedTime = 1000;
    const unsigned long timerPeriod = 100;
        
    TimerSet(timerPeriod);
    TimerOn();
    TL_state = Tstart;
    BL_state = Bstart;    
    while (1) {
		if(TL_elapsedTime >= 1000) {
			TickFct_ThreeLeds();
			TL_elapsedTime = 0;
		}
		if(BL_elapsedTime >= 1000) {
			TickFct_BlinkLed();
			BL_elapsedTime = 0;
		}
		TickFct_CombineLEDs();
		while (!TimerFlag) {}
		TimerFlag = 0;
		TL_elapsedTime += timerPeriod;
		BL_elapsedTime += timerPeriod;
    }
    return 1;
}
