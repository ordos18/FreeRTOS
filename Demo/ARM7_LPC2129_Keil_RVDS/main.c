#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "string.h"
#include "uart.h"
#include "keyboard.h"

#define SIZE 5

char *apcQueue[SIZE];
short cFront = -1, cRear = -1;

char Enqueue (char pcString[]) {
	
	if (cFront != (cRear+1) % SIZE) {
		if(cFront == -1) cFront = 0;
		cRear = (cRear+1) % SIZE;
		apcQueue[cRear] = pcString;
		return 1;
	}
	return 0;
}

char* pcDequeue (void) {
	
	char *pcElement;
	
	if (cFront != -1) {
		pcElement = apcQueue[cFront];
		if (cFront == cRear) {
			cFront = -1;
			cRear = -1;
		} else {
			cFront = (cFront + 1) % SIZE;
		}
		return pcElement;
	}
	return NULL;
}

void Rtos_Transmitter_SendString (void *pvParameters) {
	
	char cStringToSend[TRANSMITER_SIZE];
	char* pcElement;
	
	while(1) {
		if (eTransmitter_GetStatus() == FREE) {
			pcElement = pcDequeue();
			if(NULL != pcElement) {
				CopyString(pcElement, cStringToSend);
				ReplaceCharactersInString(cStringToSend, '\n', ':');
				AppendUIntToString(xTaskGetTickCount(), cStringToSend);
				AppendString("\n",cStringToSend);
				Transmitter_SendString(cStringToSend);
			}
		}
	}
}

void LettersTx (void *pvParameters){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle *) pvParameters);
	
	while(1) {
		if (0 == Enqueue("-ABCDEFGH-\n") ) {
			Led_Toggle(0);
		}
		vTaskDelay(300);
		//vTaskDelay(700);
	}
}

void KeyboardTx (void *pvParameters){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle *) pvParameters);
	
	while(1) {
		if (RELEASED != eKeyboardRead() ) {
			Enqueue("-Keyboard-\n");
			vTaskDelay(300);
		}
	}
}

int main( void ){
	
	KeyboardInit();
	LedInit();
	UART_InitWithInt(300);
	
	xTaskCreate(LettersTx, NULL, 128, NULL, 2, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 2, NULL );
	xTaskCreate(Rtos_Transmitter_SendString, NULL, 128, NULL, 2, NULL );
	vTaskStartScheduler();
	while(1);
}
