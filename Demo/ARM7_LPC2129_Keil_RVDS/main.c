#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

struct LedParams {
	unsigned char ucBlinkingFreq;
	unsigned char ucLedNr;
};

void Delay(unsigned int uiMiliSec) {
	
	unsigned int uiLoopCtr, uiDelayLoopCount;
	uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr=0;uiLoopCtr<uiDelayLoopCount;uiLoopCtr++) {}
}

void LedBlink( void *pvParameters ){
	
	struct LedParams* sLedCtrl = (struct LedParams*)pvParameters;
	
	while(1){
		Led_Toggle((*sLedCtrl).ucLedNr);
		vTaskDelay((1000/((*sLedCtrl).ucBlinkingFreq))/2);
	}
}

void LedCtrl( void *pvControl ){
	
	struct LedParams* sLedCtrl = (struct LedParams*)pvControl;
	static unsigned char ucCounter = 0;
	
	while(1){
		(*sLedCtrl).ucBlinkingFreq = ((*sLedCtrl).ucBlinkingFreq*2)%15;
		if((ucCounter++)%2) {
			(*sLedCtrl).ucLedNr = ((*sLedCtrl).ucLedNr+1)%4;
		}
		vTaskDelay(1000);
	}
}

int main(void){
	
	struct LedParams sLedCtrl = {1, 0};
	
	LedInit();
	xTaskCreate(LedBlink, NULL , 100 , &sLedCtrl, 2 , NULL );
	xTaskCreate(LedCtrl, NULL , 100 , &sLedCtrl, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}
