#include <LPC21xx.H>
#include "uart.h"
#include "string.h"
#include "FreeRTOS.h"
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

// U0FCR FIFO Control Register
#define mFIFO_ENABLE								0x00000001

/************ Interrupts **********/
// VIC (Vector Interrupt Controller) channels
#define VIC_UART0_CHANNEL_NR  6
#define VIC_UART1_CHANNEL_NR  7

// VICVectCntlx Vector Control Registers
#define mIRQ_SLOT_ENABLE                           0x00000020

//#define NULL '\0'

////////////// Zmienne globalne ////////////
char cOdebranyZnak, cWysylanyZnak;

QueueHandle_t xQueueUART_Rx, xQueueUART_Tx;

///////////////////////////////////////////
__irq void UART0_Interrupt (void) {
   // jesli przerwanie z odbiornika (Rx)
   
   unsigned int uiCopyOfU0IIR=U0IIR; // odczyt U0IIR powoduje jego kasowanie wiec lepiej pracowac na kopii

   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING) // odebrano znak
   {
     cOdebranyZnak = U0RBR;
		 xQueueSendFromISR(xQueueUART_Rx, &cOdebranyZnak, NULL);
   } 
   
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING)              // wyslano znak - nadajnik pusty 
   {
		 if (pdPASS == xQueueReceiveFromISR(xQueueUART_Tx, &cWysylanyZnak, NULL)) {
			 U0THR = cWysylanyZnak;
		 }
      
   }

   VICVectAddr = 0; // Acknowledge Interrupt
}

////////////////////////////////////////////
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
	U0FCR = mFIFO_ENABLE;

	// INT
	VICVectAddr1  = (unsigned long) UART0_Interrupt;             // set interrupt service routine address
	VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
	VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel
	
	xQueueUART_Rx = xQueueCreate(RECEIVER_SIZE, sizeof(char));
	xQueueUART_Tx = xQueueCreate(TRANSMITER_SIZE, sizeof(char));
}

void UART_GetString (char *pcChar) {
	
	while (1) {
		xQueueReceive(xQueueUART_Rx, pcChar, portMAX_DELAY);
		if('\r' == *pcChar) {
			*pcChar = NULL;
			break;
		}
		pcChar++;
	}
}

void UART_PutString (char *pcChar) {
	
	const char cTerm = '\r';
	unsigned char ucCharCounter = 0;
	char cFirstChar;
	
	if (NULL != *pcChar) {
		while (NULL != *(pcChar+ucCharCounter)) {
			xQueueSend(xQueueUART_Tx, pcChar+ucCharCounter, 0);
			ucCharCounter++;
		}
		xQueueSend(xQueueUART_Tx, &cTerm, 0);
		
		xQueueReceive(xQueueUART_Tx, &cFirstChar, portMAX_DELAY);
		U0THR = cFirstChar;
	}
}
