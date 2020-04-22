#include <LPC21xx.H>
#include "led.h"
#include "servo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DETECTOR_bm (1 << 10)		//P0.10
#define FULL_ROTATION 48
#define QUEUE_SIZE 10
#define QUEUE_WAIT 1

enum ServoState {IDLE, CALIB, IN_PROGRESS};
enum DetectorState {ACTIVE, INACTIVE};
enum Function {F_GOTO, F_WAIT, F_SPEED, F_CALIB};

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
		
		switch( sServo.eState ) {
			case IDLE:
				xQueueReceive(xQueueControl, &sControl, portMAX_DELAY);
				switch (sControl.eFunction) {
					case F_GOTO:
						sServo.eState = IN_PROGRESS;
						break;
					case F_WAIT:
						vTaskDelay(sControl.uiValue);
						break;
					case F_SPEED:
						uiServoDelay = sControl.uiValue;
						break;
					case F_CALIB:
						sServo.eState = CALIB;
						break;
					default:
						break;
				}
				break;
			case CALIB:
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
	
	struct ServoControl sControl;
	
	sControl.eFunction = F_CALIB;
	sControl.uiValue = 0;
	xQueueSend(xQueueControl, &sControl, QUEUE_WAIT);
}

void ServoGoTo (unsigned int uiValue) {
	
	struct ServoControl sControl;
	
	sControl.eFunction = F_GOTO;
	sControl.uiValue = uiValue;
	xQueueSend(xQueueControl, &sControl, QUEUE_WAIT);
}

void ServoWait (unsigned int uiValue) {
	
	struct ServoControl sControl;
	
	sControl.eFunction = F_WAIT;
	sControl.uiValue = uiValue;
	xQueueSend(xQueueControl, &sControl, QUEUE_WAIT);
}

void ServoSpeed (unsigned int uiValue) {
	
	struct ServoControl sControl;
	
	sControl.eFunction = F_SPEED;
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
	
	xQueueControl = xQueueCreate(QUEUE_SIZE, sizeof(struct ServoControl));
	LedInit();
	DetectorInit();
	ServoCalib();
	xTaskCreate(Automat, NULL, 128, (void*)uiServoFrequency, 2, NULL );
}
