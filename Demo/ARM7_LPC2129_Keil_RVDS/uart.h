
#define RECEIVER_SIZE 100
//#define TRANSMITER_SIZE 100


void UART_InitWithInt(unsigned int uiBaudRate);
void Transmitter_SendString (char cString[]);
void UART_GetString (char *pcChar);
void UART_PutString (char *pcChar);
