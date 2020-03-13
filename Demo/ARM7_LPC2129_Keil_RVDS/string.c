#include "string.h"

#define NULL '\0'

void UIntToHexStr ( unsigned int uiValue, char pcStr[] ) {
	
	unsigned char ucNibbleCounter, ucNibble;
	
	pcStr[0] = '0';
	pcStr[1] = 'x';
	for( ucNibbleCounter = 0; ucNibbleCounter < 4; ucNibbleCounter++ ) {
		ucNibble = (uiValue >> (ucNibbleCounter*4)) & 0x0F;
		if( ucNibble < 10) {
			pcStr[5-ucNibbleCounter] = '0' + ucNibble;
		} else {
			pcStr[5-ucNibbleCounter] = 'A' - 10 + ucNibble;
		}
	}
	pcStr[6] = NULL;
}

enum Result eHexStringToUInt ( char pcStr[], unsigned int *puiValue) {
	
	unsigned char ucCharacterIterator;
	char cCharacter;
	
	if( (pcStr[0] != '0') || (pcStr[1] != 'x') || (NULL == pcStr[2]) ) {
		return ERROR;
	}
	*puiValue = 0;
	for( ucCharacterIterator = 2; pcStr[ucCharacterIterator] != NULL ; ucCharacterIterator++ ) {
		cCharacter = pcStr[ucCharacterIterator];
		if( 6 == ucCharacterIterator ) {return ERROR;}
		*puiValue = (*puiValue << 4) + cCharacter;
		if( cCharacter >= 'A') {
			*puiValue = *puiValue - 'A' + 10;
		} else {
			*puiValue = *puiValue - '0';
		}
	}
	return OK;
}

void AppendUIntToString ( unsigned int uiValue, char pcDestinationStr[] ) {
	
	unsigned char ucDestinationEnd;
	
	for( ucDestinationEnd = 0; pcDestinationStr[ucDestinationEnd] != NULL; ucDestinationEnd++ ) {}
	UIntToHexStr( uiValue, pcDestinationStr+ucDestinationEnd);
}

void CopyString ( char pcSource[], char pcDestination[] ) {
	
	unsigned char ucCharCounter;
	
	for( ucCharCounter = 0; NULL != pcSource[ucCharCounter]; ucCharCounter++ ) {
		pcDestination[ucCharCounter] = pcSource[ucCharCounter];
	}
	pcDestination[ucCharCounter] = NULL;
}

enum CompareResult eCompareString ( char pcStr1[], char pcStr2[] ) {
	
	unsigned char ucCharCounter;
	
	for( ucCharCounter = 0; pcStr1[ucCharCounter] == pcStr2[ucCharCounter]; ucCharCounter++ ) {
		if( NULL == pcStr1[ucCharCounter] ) {
			return EQUAL;
		}
	}
	return NOTEQUAL;
}

void AppendString ( char pcSourceStr[], char pcDestinationStr[] ) {
	
	unsigned char ucDestinationEnd;
	
	for( ucDestinationEnd = 0; NULL != pcDestinationStr[ucDestinationEnd]; ucDestinationEnd++ ) {}
	CopyString( pcSourceStr, pcDestinationStr + ucDestinationEnd );
}

void ReplaceCharactersInString ( char pcString[], char cOldChar, char cNewChar ) {
	
	unsigned char ucCharCounter;
	
	for( ucCharCounter = 0; NULL != pcString[ucCharCounter]; ucCharCounter++ ) {
		if( pcString[ucCharCounter] == cOldChar ) {
			pcString[ucCharCounter] = cNewChar;
		}
	}
}
