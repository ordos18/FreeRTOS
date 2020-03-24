#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "keyboard.h"
#include "watch.h"
#include "uart.h"
#include "string.h"

void UartTxWatch( void *pvParameters ) {
	
	struct WatchEvent sWatchEvent;
	char cStringToSend[15];
	
	while(1){
		sWatchEvent = sWatch_Read();
		switch (sWatchEvent.eTimeUnit) {
			case SECONDS:
				CopyString("sec ", cStringToSend);
				break;
			case MINUTES:
				CopyString("min ", cStringToSend);
				break;
			default:
				break;
		}
		AppendUIntToString(sWatchEvent.TimeValue, cStringToSend);
		AppendString("\n", cStringToSend);
		UART_PutString(cStringToSend);
		vTaskDelay(10);
	}
}

void UartTxButton( void *pvParameters ) {
	
	enum eButtons eButton;
	char cStringToSend[15];
	
	while(1){
		eButton = eKeyboardRead();
		CopyString("button ", cStringToSend);
		switch (eButton) {
			case BUTTON_0:
				AppendUIntToString(0, cStringToSend);
				break;
			case BUTTON_1:
				AppendUIntToString(1, cStringToSend);
				break;
			case BUTTON_2:
				AppendUIntToString(2, cStringToSend);
				break;
			case BUTTON_3:
				AppendUIntToString(3, cStringToSend);
				break;
			default:
				break;
		}
		AppendString("\n", cStringToSend);
		UART_PutString(cStringToSend);
		vTaskDelay(10);
	}
}

int main( void ) {
	
	LedInit();
	Watch_Init();
	KeyboardInit();
	UART_InitWithInt(9600);
	
	xTaskCreate( UartTxWatch, NULL , 100 , NULL, 1 , NULL );
	xTaskCreate( UartTxButton, NULL , 100 , NULL, 1 , NULL );
	vTaskStartScheduler();
	
	while(1);
}
