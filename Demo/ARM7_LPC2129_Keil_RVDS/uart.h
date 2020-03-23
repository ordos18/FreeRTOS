
#define RECEIVER_SIZE 100
#define TRANSMITER_SIZE 100

enum eTransmitterStatus {FREE, BUSY};
enum eReceiverStatus {EMPTY, READY, OVERFLOW};
struct ReceiverBuffer {
	char cData[RECEIVER_SIZE];
	unsigned char ucCharCtr;
	enum eReceiverStatus eStatus;
};

void UART_InitWithInt(unsigned int uiBaudRate);
char Transmitter_GetCharacterFromBuffer (void);
void Transmitter_SendString (char cString[]);
enum eTransmitterStatus eTransmitter_GetStatus (void);
void UART_GetString (char *);
