
enum TimeUnit {SECONDS, MINUTES};

struct WatchEvent {
	enum TimeUnit eTimeUnit;
	char TimeValue;
};

struct Watch {
	unsigned char ucMinutes,ucSeconds;
	unsigned char fSecondsValueChanged, fMinutesValueChanged;
};

void Watch_Init(void);
struct WatchEvent sWatch_Read(void);
