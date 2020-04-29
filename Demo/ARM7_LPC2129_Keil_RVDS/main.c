#include "FreeRTOS.h"
#include "task.h"
#include "servo.h"
#include "keyboard.h"


void Keyboard (void *pvParameters) {
	
	enum eButtons eCurrButton, ePrevButton = RELEASED;
	
	while(1) {
		eCurrButton = eKeyboardRead();
		if (eCurrButton != ePrevButton) {
			switch(eCurrButton) {
				case BUTTON_0: ServoCalib();
					break;
				case BUTTON_1: ServoGoTo(12);
					break;
				case BUTTON_2: ServoGoTo(24);
					break;
				case BUTTON_3:
					ServoSpeed(16);
					ServoGoTo(12);
					ServoSpeed(12);
					ServoGoTo(24);
					ServoSpeed(8);
					ServoGoTo(36);
					ServoSpeed(4);
					ServoGoTo(0);
					break;
				default:
					break;
			}
		}
		vTaskDelay(100);
		ePrevButton = eCurrButton;
	}
}

int main( void ){
	
	KeyboardInit();
	ServoInit(100);
	
	xTaskCreate(Keyboard, NULL, 128, NULL, 2, NULL );
	vTaskStartScheduler();
	while(1);
}
