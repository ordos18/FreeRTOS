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
	char acQueueEvent[RECEIVER_SIZE];
	
	while(1){
		UART_GetString(acQueueEvent);
		xQueueSend(xQueueMain, acQueueEvent, QUEUE_WAIT);
		
		DecodeMsg(acQueueEvent);
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
	char acQueueEvent[RECEIVER_SIZE];
	
	while(1){
		eButton = eKeyboardRead();
		switch (eButton) {
			case BUTTON_0:
				CopyString("calib", acQueueEvent);
				break;
			case BUTTON_1:
				ServoWait(5000);
				CopyString("goto ", acQueueEvent);
				AppendUIntToString(12, acQueueEvent);
				break;
			case BUTTON_2:
				CopyString("goto ", acQueueEvent);
				AppendUIntToString(24, acQueueEvent);
				break;
			case BUTTON_3:
				CopyString("goto ", acQueueEvent);
				AppendUIntToString(36, acQueueEvent);
				break;
			default:
				break;
		}
		xQueueSend(xQueueMain, acQueueEvent, QUEUE_WAIT);
		vTaskDelay(10);
	}
}

void Executor_MainThread( void *pvParameters ) {
	
	extern Token asToken[];
	extern unsigned char ucTokenNr;
	char acQueueEvent[RECEIVER_SIZE];
	char cStringToSend[RECEIVER_SIZE];
	struct Servo sServoStatus;
	
	while(1){
		xQueueReceive(xQueueMain, acQueueEvent, portMAX_DELAY);
		DecodeMsg(acQueueEvent);
		
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
				case STATE:
					sServoStatus = Servo_State();
					switch (sServoStatus.eState){
						case _CALIBRATION	: CopyString("\nstate calib ",cStringToSend);				break;
						case _IDLE 				: CopyString("\nstate idle ",cStringToSend);				break;
						case _IN_PROGRESS	: CopyString("\nstate in_progress ",cStringToSend);	break;
						case _WAITING			: CopyString("\nstate waiting ",cStringToSend);			break;
						default: break;
					};
					AppendUIntToString(sServoStatus.uiCurrentPosition,cStringToSend);
					AppendString("\n",cStringToSend);
					UART_PutString(cStringToSend);
					break;
				default: {}
			}
		}
		
		vTaskDelay(10);
	}
}

int main( void ) {
	
	ServoInit(10);
	KeyboardInit();
	UART_InitWithInt(9600);
	
	xQueueMain = xQueueCreate(QUEUE_SIZE, RECEIVER_SIZE);
	xTaskCreate( UartRx_MainThread, NULL , 100 , NULL, 1 , NULL );
	xTaskCreate( Keyboard_MainThread, NULL , 100 , NULL, 1 , NULL );
	xTaskCreate( Executor_MainThread, NULL , 100 , NULL, 1 , NULL );
	vTaskStartScheduler();
	
	while(1);
}
