#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "uart.h"
#include "string.h"

void UartRx( void *pvParameters ) {
	
	char acBuffer[RECEIVER_SIZE];
	
	while(1){
		UART_GetString(acBuffer);
		if (EQUAL == eCompareString(acBuffer, "zero")) {
			Led_Toggle(0);
		} else if (EQUAL == eCompareString(acBuffer, "jeden")) {
			Led_Toggle(1);
		}
	}
}

int main( void ) {
	
	LedInit();
	UART_InitWithInt(9600);
	
	xTaskCreate( UartRx, NULL , 100 , NULL, 1 , NULL );
	vTaskStartScheduler();
	
	while(1);
}
