#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

void UartRx( void *pvParameters ) {
	
	char acBuffer[RECEIVER_SIZE] = "0123456789\n";
	
	while(1){
		vTaskDelay(500);
		UART_PutString(acBuffer);
		//vTaskDelay(20);
		UART_PutString(acBuffer);
		//vTaskDelay(20);
		UART_PutString(acBuffer);
	}
}

int main( void ) {
	
	UART_InitWithInt(9600);
	
	xTaskCreate( UartRx, NULL , 100 , NULL, 1 , NULL );
	vTaskStartScheduler();
	
	while(1);
}
