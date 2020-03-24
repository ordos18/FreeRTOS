#include <LPC21xx.H>
#include "keyboard.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define BUTTON0_bm (1 << 4)		//P0.4
#define BUTTON1_bm (1 << 6)		//P0.6
#define BUTTON2_bm (1 << 5)		//P0.5
#define BUTTON3_bm (1 << 7)		//P0.7

#define QUEUE_SIZE 1
#define QUEUE_WAIT 1

QueueHandle_t xQueueKeyboard;

enum eButtons eReadButtons(void) {
	
	if( 0 == (IO0PIN & BUTTON0_bm) ){
		return BUTTON_0;
	} 
	else if( 0 == (IO0PIN & BUTTON1_bm) ){
		return BUTTON_1;
	}	
	else if( 0 == (IO0PIN & BUTTON2_bm) ){
		return BUTTON_2;
	}	
	else if( 0 == (IO0PIN & BUTTON3_bm) ){
		return BUTTON_3;
	}	
	else {
		return RELEASED;
	}
}

void Keyboard_Thread( void *pvParameters ) {
	
	enum eButtons eButton, eButtonPrev;
	
	while(1){
		eButton = eReadButtons();
		if (RELEASED != eButton && eButtonPrev != eButton) {
			xQueueSend(xQueueKeyboard, &eButton, QUEUE_WAIT);
			vTaskDelay(100);
		}
		eButtonPrev = eButton;
		vTaskDelay(10);
	}
}

void KeyboardInit (void) {
	
	IO0DIR = IO0DIR & ~(BUTTON0_bm | BUTTON1_bm | BUTTON2_bm | BUTTON3_bm);
	
	xQueueKeyboard = xQueueCreate(QUEUE_SIZE, sizeof(enum eButtons));
	xTaskCreate( Keyboard_Thread, NULL , 100 , NULL, 2 , NULL );
}

enum eButtons eKeyboardRead(void) {
	
	enum eButtons eButton;
	
	xQueueReceive(xQueueKeyboard, &eButton, portMAX_DELAY);
	return eButton;
}
