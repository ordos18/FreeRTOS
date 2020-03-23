#include <LPC21xx.H>
#include "uart.h"
#include "string.h"
#include "FreeRTOS.h"
//#include "task.h"
#include "queue.h"

/************ UART ************/
// U0LCR Line Control Register
#define mDIVISOR_LATCH_ACCES_BIT                   0x00000080
#define m8BIT_UART_WORD_LENGTH                     0x00000003

// UxIER Interrupt Enable Register
#define mRX_DATA_AVALIABLE_INTERRUPT_ENABLE        0x00000001
#define mTHRE_INTERRUPT_ENABLE                     0x00000002

// UxIIR Pending Interrupt Identification Register
#define mINTERRUPT_PENDING_IDETIFICATION_BITFIELD  0x0000000F
#define mTHRE_INTERRUPT_PENDING                    0x00000002
#define mRX_DATA_AVALIABLE_INTERRUPT_PENDING       0x00000004

// PINSEL0 Pin Connect Block
#define mUART0_RX								   0x00000004
#define mUART0_TX								   0x00000001

/************ Interrupts **********/
// VIC (Vector Interrupt Controller) channels
#define VIC_UART0_CHANNEL_NR  6
#define VIC_UART1_CHANNEL_NR  7

// VICVectCntlx Vector Control Registers
#define mIRQ_SLOT_ENABLE                           0x00000020

//#define NULL '\0'
#define QUEUE_WAIT 1

////////////// Zmienne globalne ////////////
char cOdebranyZnak, cWysylanyZnak;

typedef struct TransmitterBuffer {
   char cData[TRANSMITER_SIZE];
   enum eTransmitterStatus eStatus;
   unsigned char fLastCharacter;
   unsigned char cCharCtr;
} TransmitterBuffer;

struct TransmitterBuffer sTransmitterBuffer;

QueueHandle_t xQueueUART;

///////////////////////////////////////////
__irq void UART0_Interrupt (void) {
   // jesli przerwanie z odbiornika (Rx)
   
   unsigned int uiCopyOfU0IIR=U0IIR; // odczyt U0IIR powoduje jego kasowanie wiec lepiej pracowac na kopii

   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING) // odebrano znak
   {
     cOdebranyZnak = U0RBR;
		 xQueueSendFromISR(xQueueUART, &cOdebranyZnak, NULL);
   } 
   
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING)              // wyslano znak - nadajnik pusty 
   {
	   cWysylanyZnak = Transmitter_GetCharacterFromBuffer();
	   if (cWysylanyZnak != NULL) {
		   U0THR = cWysylanyZnak;
	   }
      
   }

   VICVectAddr = 0; // Acknowledge Interrupt
}
/*
////////////////////////////////////////////
void UART_InitWithInt(unsigned int uiBaudRate){

   // UART0
   PINSEL0 = PINSEL0 | mUART0_RX | mUART0_TX;                   // ustawic pina na odbiornik uart0
   U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT; // dlugosc slowa, DLAB = 1
   U0DLL   = ((15000000)/16)/uiBaudRate;                        // predkosc transmisji
   U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       // DLAB = 0
   U0IER  |= mRX_DATA_AVALIABLE_INTERRUPT_ENABLE;               // aktywacja przerwan po odebraniu danych (RDA)
   U0IER  |= mTHRE_INTERRUPT_PENDING;            			    // aktywacja przerwan po wyslaniu danych (THRE)

   // INT
   VICVectAddr1  = (unsigned long) UART0_Interrupt;             // set interrupt service routine address
   VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
   VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel
}
*/
void UART_InitWithInt(unsigned int uiBaudRate){
	
	unsigned long ulDivisor, ulWantedClock;
	ulWantedClock=uiBaudRate*16;
	ulDivisor=15000000/ulWantedClock;
	// UART
	PINSEL0 = PINSEL0 | 0x55;                                     // ustawic piny uar0 odbiornik nadajnik
	U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT; // d³ugosc s³owa, DLAB = 1
	U0DLL = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	ulDivisor >>= 8;
	U0DLM = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       // DLAB = 0
	U0IER  |= mRX_DATA_AVALIABLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE ;               

	// INT
	VICVectAddr1  = (unsigned long) UART0_Interrupt;             // set interrupt service routine address
	VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
	VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel
	
	xQueueUART = xQueueCreate(RECEIVER_SIZE, sizeof(char));
}

char Transmitter_GetCharacterFromBuffer (void) {
	
	if (NULL == sTransmitterBuffer.fLastCharacter) {
		sTransmitterBuffer.eStatus = FREE;
		return NULL;
	} else {
		sTransmitterBuffer.fLastCharacter = sTransmitterBuffer.cData[sTransmitterBuffer.cCharCtr];
		sTransmitterBuffer.cCharCtr = sTransmitterBuffer.cCharCtr + 1;
		if (sTransmitterBuffer.fLastCharacter != NULL) {
			return sTransmitterBuffer.fLastCharacter;
		} else {
			return '\r';
		}
	}
}

void Transmitter_SendString (char cString[]) {
	
	CopyString(cString, sTransmitterBuffer.cData);
	//AppendString("\n", sTransmitterBuffer.cData);
	U0THR = cString[0];
	sTransmitterBuffer.fLastCharacter = cString[0];
	sTransmitterBuffer.eStatus = BUSY;
	sTransmitterBuffer.cCharCtr = 1;
}

enum eTransmitterStatus eTransmitter_GetStatus (void) {
	return sTransmitterBuffer.eStatus;
}

void UART_GetString (char *pcChar) {
	
	while (1) {
		xQueueReceive(xQueueUART, pcChar, portMAX_DELAY);
		if('\r' == *pcChar) {
			*pcChar = NULL;
			break;
		}
		pcChar++;
	}
}
