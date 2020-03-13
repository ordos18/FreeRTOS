#include <LPC21xx.H>
#include "led.h"

#define LED0_bm (1 << 16)
#define LED1_bm (1 << 17)
#define LED2_bm (1 << 18)
#define LED3_bm (1 << 19)

enum eDirection {LEFT, RIGHT};

void LedOn (unsigned char ucLedIndeks) {
	
	IO1CLR = LED0_bm | LED1_bm | LED2_bm | LED3_bm;
	switch(ucLedIndeks) {
		case 0:
			IO1SET = LED0_bm;
			break;
		case 1:
			IO1SET = LED1_bm;
			break;
		case 2:
			IO1SET = LED2_bm;
			break;
		case 3:
			IO1SET = LED3_bm;
			break;
	}
}

void LedInit (void) {
	
	IO1DIR = IO1DIR | (LED0_bm | LED1_bm | LED2_bm | LED3_bm);
	LedOn(0);
}

void LedStep (enum eDirection direction) {
	
	static unsigned int ucLedIndeks = 0;
	
	switch (direction) {
		case LEFT:
			ucLedIndeks = (ucLedIndeks + 1) % 4;
			break;
		case RIGHT:
			ucLedIndeks = (ucLedIndeks - 1) % 4;
			break;
	}
	LedOn(ucLedIndeks);
}

void LedStepLeft (void) {
	
	LedStep(LEFT);
}

void LedStepRight (void) {
	
	LedStep(RIGHT);
}

void Led_Toggle(unsigned char ucLedIndeks) {
	
	unsigned int uiBM;
	
	switch(ucLedIndeks) {
		case 0:
			uiBM = LED0_bm;
			break;
		case 1:
			uiBM = LED1_bm;
			break;
		case 2:
			uiBM = LED2_bm;
			break;
		case 3:
			uiBM = LED3_bm;
			break;
	}
	
	if (IO1PIN & uiBM)
		IO1CLR = uiBM;
	else
		IO1SET = uiBM;
}
