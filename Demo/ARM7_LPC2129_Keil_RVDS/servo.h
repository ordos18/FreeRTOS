
enum ServoState {_IDLE, _CALIBRATION, _IN_PROGRESS, _WAITING};
struct Servo {
	
	enum ServoState eState;
	unsigned int uiCurrentPosition;
};

void Automat (void *pvParameters);
void ServoCalib (void);
void ServoGoTo (unsigned int uiPosition);
void ServoInit (unsigned int uiServoFrequency);
void ServoMoveDegrees (unsigned int uiDegrees);
void ServoWait (unsigned int uiDelay);
void ServoSpeed (unsigned int uiDelay);
struct Servo Servo_State (void);
