#include "FreeRTOS.h"
#include "task.h"
//#include "led.h"
#include "semphr.h"
#include "string.h"
#include "uart.h"
#include "keyboard.h"

void Rtos_Transmitter_SendString (char pcString[]) {
	
	char cStringToSend[TRANSMITER_SIZE];
	
	CopyString(pcString, cStringToSend);
	ReplaceCharactersInString(cStringToSend, '\n', ':');
	AppendUIntToString(xTaskGetTickCount(), cStringToSend);
	AppendString("\n",cStringToSend);
	Transmitter_SendString(cStringToSend);
}

void LettersTx (void *pvParameters){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle *) pvParameters);
	
	while(1){
		if (pdTRUE == xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
			Rtos_Transmitter_SendString("-ABCDEEFGH-\n");
			while (eTransmitter_GetStatus()!=FREE){};
			xSemaphoreGive(xSemaphore);
			vTaskDelay(300);
		}
	}
}

void KeyboardTx (void *pvParameters){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle *) pvParameters);
	
	while(1){
		if( RELEASED != eKeyboardRead() ){
			if (pdTRUE == xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
				Transmitter_SendString("-Keyboard-\n");
				while (eTransmitter_GetStatus()!=FREE){};
				xSemaphoreGive(xSemaphore);
				vTaskDelay(300);
			}
		}
	}
}

int main( void ){
	
	xSemaphoreHandle xSemaphore;
	
	vSemaphoreCreateBinary(xSemaphore);
	KeyboardInit();
	UART_InitWithInt(300);
	xTaskCreate(LettersTx, NULL, 128, &xSemaphore, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, &xSemaphore, 1, NULL );
	vTaskStartScheduler();
	while(1);
}
