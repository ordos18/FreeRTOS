#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "uart.h"

void UartRx( void *pvParameters ) {
	
	char acBuffer[RECEIVER_SIZE];
	
	while(1){
		UART_GetString(acBuffer);
		Led_Toggle(acBuffer[0]-'0');
	}
}

int main( void ) {
	
	LedInit();
	UART_InitWithInt(9600);
	
	xTaskCreate( UartRx, NULL , 100 , NULL, 1 , NULL );
	vTaskStartScheduler();
	
	while(1);
}
