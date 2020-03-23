#include "FreeRTOS.h"
#include "task.h"
//#include "led.h"
#include "semphr.h"
#include "string.h"
#include "uart.h"
#include "keyboard.h"

	
xSemaphoreHandle xSemaphore;


void Rtos_Transmitter_SendString (char pcString[]) {
	
	TickType_t tTickStart;
	char cStringToSend[TRANSMITER_SIZE];
	
	tTickStart = xTaskGetTickCount();
	if (pdTRUE == xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
		CopyString(pcString, cStringToSend);
		AppendString(":",cStringToSend);
		AppendUIntToString(xTaskGetTickCount()-tTickStart, cStringToSend);
		AppendString("\n",cStringToSend);
		Transmitter_SendString(cStringToSend);
		while (eTransmitter_GetStatus()!=FREE){};
		xSemaphoreGive(xSemaphore);
	}
}

void LettersTx (void *pvParameters){
	
	while(1){
		Rtos_Transmitter_SendString("-ABCDEEFGH-");
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
	
	while(1){
		if( RELEASED != eKeyboardRead() ){
			Rtos_Transmitter_SendString("-Keyboard-");
			vTaskDelay(300);
		}
	}
}

int main( void ){
	
	vSemaphoreCreateBinary(xSemaphore);
	KeyboardInit();
	UART_InitWithInt(300);
	xTaskCreate(LettersTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 1, NULL );
	vTaskStartScheduler();
	while(1);
}
