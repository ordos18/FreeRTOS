
struct Watch {
	unsigned char ucMinutes,ucSeconds;
	unsigned char fSecondsValueChanged, fMinutesValueChanged;
};

void Timer1Interrupts_Init(unsigned int uiPeriod, void (*ptrInterruptFunction1)(void));
void WatchUpdate (void);
