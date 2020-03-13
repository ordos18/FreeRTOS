
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
void Receiver_PutCharacterToBuffer (char cCharacter);
enum eReceiverStatus eReceiver_GetStatus (void);
void Receiver_GetStringCopy (char * ucDestination);
char Transmitter_GetCharacterFromBuffer (void);
void Transmitter_SendString (char cString[]);
enum eTransmitterStatus eTransmitter_GetStatus (void);
