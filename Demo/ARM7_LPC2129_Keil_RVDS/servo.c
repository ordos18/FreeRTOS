#include <LPC21xx.H>
#include "led.h"
#include "servo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DETECTOR_bm (1 << 10)		//P0.10
#define FULL_ROTATION 48
#define QUEUE_SIZE 3
#define QUEUE_WAIT 1

enum ServoState {IDLE, CALLIB, IN_PROGRESS};
enum DetectorState {ACTIVE, INACTIVE};
enum Function {GOTO, WAIT, SPEED};

struct Servo {
	
	enum ServoState eState;
	unsigned int uiCurrentPosition;
};

struct ServoControl {
	
	enum Function eFunction;
	unsigned int uiValue;
};

struct Servo sServo;

QueueHandle_t xQueueControl;


void DetectorInit (void) {
	
	IO0DIR = IO0DIR & ~DETECTOR_bm;
}

enum DetectorState eReadDetector (void) {
	
	if( 0 == (IO0PIN & DETECTOR_bm) ){
		return ACTIVE;
	}
	else {
		return INACTIVE;
	}
}

void Automat (void *pvParameters) {
	
	struct ServoControl sControl;
	unsigned int uiServoDelay = 1000/((unsigned int) pvParameters);
	
	while (1) {
		
		if (IDLE == sServo.eState && pdPASS == xQueueReceive(xQueueControl, &sControl, QUEUE_WAIT)) {
			switch (sControl.eFunction) {
				case GOTO:
					sServo.eState = IN_PROGRESS;
					break;
				case WAIT:
					vTaskDelay(sControl.uiValue);
					break;
				case SPEED:
					uiServoDelay = sControl.uiValue;
					break;
				default:
					break;
			}
		}
		
		switch( sServo.eState ) {
			case IDLE:
				break;
			case CALLIB:
				if( 0 == eReadDetector() ) {
					sServo.eState = IDLE;
					sServo.uiCurrentPosition = 0;
				} else {
					LedStepLeft();
				}
				break;
			case IN_PROGRESS:
				if( sServo.uiCurrentPosition < sControl.uiValue ) {
					LedStepRight();
					sServo.uiCurrentPosition++;
				} 
				else if ( sServo.uiCurrentPosition > sControl.uiValue ) {
					LedStepLeft();
					sServo.uiCurrentPosition--;
				}
				else {
					sServo.uiCurrentPosition = sServo.uiCurrentPosition % FULL_ROTATION;
					if (sControl.uiValue == sServo.uiCurrentPosition) {
						sServo.eState = IDLE;
					}
				}
				break;
			default: {}
		}
		vTaskDelay(uiServoDelay);
	}
}

void ServoCalib (void) {
	
	//while(sServo.eState != IDLE) {}
	sServo.eState = CALLIB;
}

void ServoGoTo (unsigned int uiValue) {
	
	struct ServoControl sControl;
	
	sControl.eFunction = GOTO;
	sControl.uiValue = uiValue % FULL_ROTATION;
	xQueueSend(xQueueControl, &sControl, QUEUE_WAIT);
}

void ServoWait (unsigned int uiValue) {
	
	struct ServoControl sControl;
	
	sControl.eFunction = WAIT;
	sControl.uiValue = uiValue;
	xQueueSend(xQueueControl, &sControl, QUEUE_WAIT);
}

void ServoSpeed (unsigned int uiValue) {
	
	struct ServoControl sControl;
	
	sControl.eFunction = SPEED;
	sControl.uiValue = uiValue;
	xQueueSend(xQueueControl, &sControl, QUEUE_WAIT);
}

/*
void ServoMoveDegrees (unsigned int uiDegrees) {
	
	sServo.uiDesiredPosition = sServo.uiDesiredPosition + FULL_ROTATION*uiDegrees/360;
}

void ServoMoveSteps (unsigned int uiSteps) {
	
	sServo.uiDesiredPosition = sServo.uiDesiredPosition + uiSteps;
}
*/
void ServoInit (unsigned int uiServoFrequency) {
	
	LedInit();
	DetectorInit();
	ServoCalib();
	xTaskCreate(Automat, NULL, 128, (void*)uiServoFrequency, 2, NULL );
	xQueueControl = xQueueCreate(QUEUE_SIZE, sizeof(struct ServoControl));
}
