#include "string.h"
#include "command_decoder.h"

#define NULL '\0'


struct Token asToken[MAX_TOKEN_NR];
unsigned char ucTokenNr;

struct Keyword asKeywordList[MAX_KEYWORD_NR] =
{
	{ID, "id"  },
	{CALIB,	"calib"	},
	{GOTO,	"goto"	},
	{STATE,	"state"	}
};

unsigned char ucFindTokensInString (char *pcString)
{
	enum TokenDelimiter {TOKEN, DELIMITER};
	enum TokenDelimiter eTokenDelimiter = DELIMITER;
	unsigned char ucCharacterIterator, ucTokensCounter;
	char cCharacter;
	ucTokensCounter = 0;
	
	for (ucCharacterIterator = 0; ; ucCharacterIterator++)
	{
		cCharacter = pcString[ucCharacterIterator];
		
		switch (eTokenDelimiter)
		{
			case TOKEN:
				if (NULL == cCharacter) {
					return ucTokensCounter;
				} else if (' ' == cCharacter) {
					eTokenDelimiter = DELIMITER;
				}
				break;
			case DELIMITER:
				if ( MAX_TOKEN_NR == ucTokensCounter) {
					return ucTokensCounter;
				} else if (NULL == cCharacter) {
					return ucTokensCounter;
				} else if (' ' != cCharacter) {
					eTokenDelimiter = TOKEN;
					asToken[ucTokensCounter].uValue.pcString = pcString + ucCharacterIterator;
					ucTokensCounter++;
				}
				break;
			default: {}
		}
	}
}

enum Result eStringToKeyword (char pcStr[], enum KeywordCode *peKeywordCode)
{	
	unsigned char ucKeywordIterator;
	
	for (ucKeywordIterator = 0; ucKeywordIterator < MAX_KEYWORD_NR; ucKeywordIterator++)
	{
		if (eCompareString(pcStr, asKeywordList[ucKeywordIterator].cString) == EQUAL)
		{
			*peKeywordCode = asKeywordList[ucKeywordIterator].eCode;
			return OK;
		}
	}
	return ERROR;
}

void DecodeTokens (void)
{
	unsigned char ucTokenIterator;
	Token *psToken;
	
	for (ucTokenIterator = 0; ucTokenIterator < ucTokenNr; ucTokenIterator++)
	{
		psToken = &asToken[ucTokenIterator];
		if (eStringToKeyword(psToken->uValue.pcString, &psToken->uValue.eKeyword) == OK) {
			psToken->eType = KEYWORD;
		} else if (eHexStringToUInt(psToken->uValue.pcString, &psToken->uValue.uiNumber) == OK) {
			psToken->eType = NUMBER;
		} else {
			psToken->eType = STRING;
		}
	}
}

void DecodeMsg (char *pcString)
{
	ucTokenNr = ucFindTokensInString(pcString);
	ReplaceCharactersInString(pcString, ' ', NULL);
	DecodeTokens();
	
}
