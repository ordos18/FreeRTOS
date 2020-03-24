#include "watch.h"
#include "timer_interrupts.h"
#include "FreeRTOS.h"
#include "queue.h"

#define QUEUE_SIZE 2
#define QUEUE_WAIT 1

struct Watch sWatch;
QueueHandle_t xQueueWatch;

void Watch_Thread( void *pvParameters ) {
	
	struct WatchEvent sWatchEvent;
	
	while(1){
		if (1 == sWatch.fSecondsValueChanged || 1 == sWatch.fMinutesValueChanged) {
			if (1 == sWatch.fMinutesValueChanged) {
				sWatch.fMinutesValueChanged = 0;
				sWatchEvent.eTimeUnit = MINUTES;
				sWatchEvent.TimeValue = sWatch.ucMinutes;
				xQueueSend(xQueueWatch, &sWatchEvent, QUEUE_WAIT);
			}
			if (1 == sWatch.fSecondsValueChanged) {
				sWatch.fSecondsValueChanged = 0;
				sWatchEvent.eTimeUnit = SECONDS;
				sWatchEvent.TimeValue = sWatch.ucSeconds;
				xQueueSend(xQueueWatch, &sWatchEvent, QUEUE_WAIT);
			}
			vTaskDelay(990);
		}
	}
}

void WatchUpdate (void) {
	sWatch.ucSeconds = sWatch.ucSeconds + 1;
	sWatch.fSecondsValueChanged = 1;
	if (60 == sWatch.ucSeconds) {
		sWatch.ucSeconds = 0;
		sWatch.ucMinutes = (sWatch.ucMinutes + 1) % 60;
		sWatch.fMinutesValueChanged = 1;
	}
}

void Watch_Init(void) {
	
	Timer1Interrupts_Init(1000000, &WatchUpdate);
	xQueueWatch = xQueueCreate(QUEUE_SIZE, sizeof(struct WatchEvent));
	xTaskCreate( Watch_Thread, NULL , 100 , NULL, 2 , NULL );
}

struct WatchEvent sWatch_Read(void) {
	
	struct WatchEvent sWatchEvent;
	
	xQueueReceive(xQueueWatch, &sWatchEvent, portMAX_DELAY);
	return sWatchEvent;
}
