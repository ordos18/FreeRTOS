#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "string.h"
#include "uart.h"
#include "keyboard.h"
#include "queue.h"

#define QUEUE_SIZE 5
#define QUEUE_WAIT 10

QueueHandle_t xQueueControl;


void Rtos_Transmitter_SendString (void *pvParameters) {
	
	char cStringToSend[TRANSMITER_SIZE];
	char* pcElement = cStringToSend;
	
	while(1) {
		if (eTransmitter_GetStatus() == FREE) {
			if(pdPASS == xQueueReceive(xQueueControl, &pcElement, QUEUE_WAIT)) {
				CopyString(pcElement, cStringToSend);
				Transmitter_SendString(cStringToSend);
			}
		}
		vTaskDelay(10);
	}
}

void LettersTx (void *pvParameters){
	
	char cString[20];
	char* pcElement = cString;
	TickType_t tTickStart;
	
	while(1) {
		tTickStart = xTaskGetTickCount();
		if (pdPASS != xQueueSend(xQueueControl, &pcElement, QUEUE_WAIT) ) { //portMAX_DELAY
			Led_Toggle(0);
		}
		CopyString("-ABCDEFGH-:", cString);
		AppendUIntToString(xTaskGetTickCount()-tTickStart, cString);
		AppendString("\n",cString);
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
	
	char cString[] = "-Keyboard-\n";
	char* pcElement = cString;
	while(1) {
		if (RELEASED != eKeyboardRead() ) {
			xQueueSend(xQueueControl, &pcElement, QUEUE_WAIT*50);
			vTaskDelay(300);
		}
	}
}

int main( void ){
	
	KeyboardInit();
	LedInit();
	UART_InitWithInt(300);
	
	xQueueControl = xQueueCreate(QUEUE_SIZE, sizeof(char*));
	xTaskCreate(LettersTx, NULL, 128, NULL, 2, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 2, NULL );
	xTaskCreate(Rtos_Transmitter_SendString, NULL, 128, NULL, 2, NULL );
	vTaskStartScheduler();
	while(1);
}
