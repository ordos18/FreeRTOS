#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "led.h"
#include "timer_interrupts.h"

SemaphoreHandle_t xSemaphore = NULL;

void LedBlink(void) {
	
	Led_Toggle(0);
}

void HandlerTask (void *pvParameters) {
	
	while(1) {
		xSemaphoreTake( xSemaphore, portMAX_DELAY);
		LedBlink();
	}
}

void InterruptHandler (void) {
	
	xSemaphoreGiveFromISR( xSemaphore, NULL);
}

int main(void) {

	xSemaphore = xSemaphoreCreateBinary();
	LedInit();
	Timer1Interrupts_Init(500000, &InterruptHandler);
	xTaskCreate(HandlerTask, NULL, 128, NULL, 3, NULL);
	vTaskStartScheduler();
	while(1);
}
