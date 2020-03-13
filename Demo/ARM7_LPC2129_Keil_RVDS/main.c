#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"

void PulseTrigger( void *pvParameters ){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle *) pvParameters);
	
	while(1) {
		vTaskDelay(1000);
		xSemaphoreGive(xSemaphore);
	}
}

void PulseTrigger2( void *pvParameters ){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle *) pvParameters);
	
	vTaskDelay(333);
	while(1) {
		vTaskDelay(333);
		xSemaphoreGive(xSemaphore);
	}
}

void Pulse_LED0( void *pvParameters ){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle *) pvParameters);
	
	while(1) {
		if (pdTRUE == xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
			Led_Set(0);
			vTaskDelay(100);
			Led_Clr(0);
		}
	}
}

void Pulse_LED1( void *pvParameters ){
	
	xSemaphoreHandle xSemaphore = *((xSemaphoreHandle *) pvParameters);
	
	while(1) {
		if (pdTRUE == xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
			Led_Set(1);
			vTaskDelay(100);
			Led_Clr(1);
		}
	}
}

int main(void){
	
	xSemaphoreHandle xSemaphore;
	
	vSemaphoreCreateBinary(xSemaphore);
	LedInit();
	xTaskCreate(Pulse_LED0, NULL , 100 , &xSemaphore, 2 , NULL );
	xTaskCreate(Pulse_LED1, NULL , 100 , &xSemaphore, 2 , NULL );
	xTaskCreate(PulseTrigger, NULL , 100 , &xSemaphore, 2 , NULL );
	xTaskCreate(PulseTrigger2, NULL , 100 , &xSemaphore, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}
