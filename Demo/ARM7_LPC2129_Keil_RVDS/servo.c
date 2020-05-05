#include <LPC21xx.H>
#include "led.h"
#include "servo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DETECTOR_bm (1 << 10)		//P0.10
#define FULL_ROTATION 48
#define CONTROL_QUEUE_SIZE 3
#define STATUS_QUEUE_SIZE 1
#define QUEUE_WAIT 1

enum DetectorState {ACTIVE, INACTIVE};
enum Function {F_GOTO, F_WAIT, F_SPEED, F_CALIB};

struct ServoControl {
	
	enum Function eFunction;
	unsigned int uiValue;
};

QueueHandle_t xQueueControl, xQueueStatus;


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
	struct Servo sServo;
	unsigned int uiServoDelay = 1000/((unsigned int) pvParameters);
	
	while (1) {
		xQueuePeek(xQueueStatus, &sServo, QUEUE_WAIT);
		
		switch( sServo.eState ) {
			case _IDLE:
				xQueueReceive(xQueueControl, &sControl, portMAX_DELAY);
				switch (sControl.eFunction) {
					case F_GOTO:
						sServo.eState = _IN_PROGRESS;
						break;
					case F_WAIT:
						sServo.eState = _WAITING;
							xQueueOverwrite(xQueueStatus, &sServo);
						vTaskDelay(sControl.uiValue);
						break;
					case F_SPEED:
						uiServoDelay = sControl.uiValue;
						break;
					case F_CALIB:
						sServo.eState = _CALIBRATION;
						break;
					default:
						break;
				}
				break;
			case _CALIBRATION:
				if( 0 == eReadDetector() ) {
					sServo.eState = _IDLE;
					sServo.uiCurrentPosition = 0;
				} else {
					LedStepLeft();
					sServo.uiCurrentPosition--;
				}
				break;
			case _IN_PROGRESS:
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
						sServo.eState = _IDLE;
					}
				}
				break;
			default:
				sServo.eState = _IDLE;
				break;
		}
		xQueueOverwrite(xQueueStatus, &sServo);
		vTaskDelay(uiServoDelay);
	}
}

void ServoCalib (void) {
	
	struct ServoControl sControl;
	
	sControl.eFunction = F_CALIB;
	xQueueSend(xQueueControl, &sControl, QUEUE_WAIT);
}

void ServoGoTo (unsigned int uiValue) {
	
	struct ServoControl sControl;
	
	sControl.eFunction = F_GOTO;
	sControl.uiValue = uiValue % FULL_ROTATION;
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
struct Servo Servo_State (void) {
	
	return sServo;
}
*/
struct Servo Servo_State(void){
	
	struct Servo sServoStatus;
	
	xQueuePeek(xQueueStatus, &sServoStatus, portMAX_DELAY);
	return sServoStatus;
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
	
	struct Servo sServo = {_IDLE, 0};
	
	xQueueControl = xQueueCreate(CONTROL_QUEUE_SIZE, sizeof(struct ServoControl));
	xQueueStatus = xQueueCreate(STATUS_QUEUE_SIZE, sizeof(struct Servo));
	
	xQueueSend(xQueueStatus, &sServo, QUEUE_WAIT);

	LedInit();
	DetectorInit();
	ServoCalib();

	xTaskCreate(Automat, NULL, 128, (void*)uiServoFrequency, 2, NULL );
}
