#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "keyboard.h"
#include "uart.h"
#include "string.h"
#include "command_decoder.h"
#include "servo.h"

#define QUEUE_SIZE 10
#define QUEUE_WAIT 1

QueueHandle_t xQueueMain;

void UartRx_MainThread( void *pvParameters ) {
	
	extern Token asToken[];
	extern unsigned char ucTokenNr;
	char cQueueEvent[RECEIVER_SIZE];
	
	while(1){
		UART_GetString(cQueueEvent);
		xQueueSend(xQueueMain, cQueueEvent, QUEUE_WAIT);
		
		DecodeMsg(cQueueEvent);
		if( (ucTokenNr > 0) && (asToken[0].eType == KEYWORD) ) {
			switch(asToken[0].uValue.eKeyword) {
				case CALIB:
					UART_PutString("\nok\n");
					break;
				case GOTO:
					if (ucTokenNr > 1) {
						UART_PutString("\nok\n");
					}
					break;
				default: {}
			}
		}
		
		vTaskDelay(10);
	}
}

void Keyboard_MainThread( void *pvParameters ) {
	
	enum eButtons eButton;
	char cQueueEvent[RECEIVER_SIZE];
	
	while(1){
		eButton = eKeyboardRead();
		switch (eButton) {
			case BUTTON_0:
				CopyString("calib", cQueueEvent);
				break;
			case BUTTON_1:
				CopyString("goto ", cQueueEvent);
				AppendUIntToString(12, cQueueEvent);
				break;
			case BUTTON_2:
				CopyString("goto ", cQueueEvent);
				AppendUIntToString(24, cQueueEvent);
				break;
			case BUTTON_3:
				CopyString("goto ", cQueueEvent);
				AppendUIntToString(36, cQueueEvent);
				break;
			default:
				break;
		}
		xQueueSend(xQueueMain, cQueueEvent, QUEUE_WAIT);
		vTaskDelay(10);
	}
}

void Executor_MainThread( void *pvParameters ) {
	
	extern Token asToken[];
	extern unsigned char ucTokenNr;
	char cQueueEvent[RECEIVER_SIZE];
	
	while(1){
		xQueueReceive(xQueueMain, cQueueEvent, portMAX_DELAY);
		DecodeMsg(cQueueEvent);
		
		if( (ucTokenNr > 0) && (asToken[0].eType == KEYWORD) ) {
			switch(asToken[0].uValue.eKeyword) {
				case ID:
					UART_PutString("\nID: MTM\n");
					break;
				case CALIB:
					ServoCalib();
					break;
				case GOTO:
					if (ucTokenNr > 1) {
						ServoGoTo(asToken[1].uValue.uiNumber);
					}
					break;
				default: {}
			}
		}
		
		vTaskDelay(10);
	}
}

int main( void ) {
	
	ServoInit(100);
	KeyboardInit();
	UART_InitWithInt(9600);
	
	xQueueMain = xQueueCreate(QUEUE_SIZE, RECEIVER_SIZE);
	xTaskCreate( UartRx_MainThread, NULL , 100 , NULL, 1 , NULL );
	xTaskCreate( Keyboard_MainThread, NULL , 100 , NULL, 1 , NULL );
	xTaskCreate( Executor_MainThread, NULL , 100 , NULL, 1 , NULL );
	vTaskStartScheduler();
	
	while(1);
}
