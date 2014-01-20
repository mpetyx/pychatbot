#ifndef _TEXTUTILITIESH_
#define _TEXTUTILITIESH_

#ifdef INFORMATION
Copyright (C) 2011-2013 by Bruce Wilcox

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#endif

// end of word status on ptr
#define TOKEN_INCLUSIVE 1
#define TOKEN_EXCLUSIVE 2
#define TOKEN_INCOMPLETE 4

#define SPACES 1			//   \t \r \n 
#define PUNCTUATIONS 2      //    , | -  (see also ENDERS )
#define ENDERS	4			//   . ; : ? ! -
#define BRACKETS 8			//   () [ ] { } < >
#define ARITHMETICS 16		//    % * + - ^ = / .  (but / can be part of a word)
#define SYMBOLS 32			//    $ # @ ~  ($ and # can preceed, & is stand alone)
#define CONVERTERS 64		//   & `
#define QUOTERS 128			//   " ' *

#define UNINIT -1
typedef struct NUMBERDECODE
{
    const char* word;				//   word of a number
    int value;				//   value of word
	unsigned int length;	//   length of word
	int realNumber;		// one two are real, third is fraction
	bool notPennNumber;		//  "pair" and such are normal numbers with no value to penn system
} NUMBERDECODE;
#define FRACTION 2
#define REALNUMBER 1

#define SHOUT 1
#define ABBREVIATION 2

#define NOT_A_NUMBER 2147483646

#define DIGIT_NUMBER 1
#define CURRENCY_NUMBER 2
#define PLACE_NUMBER 3
#define ROMAN_NUMBER 4
#define WORD_NUMBER 5
#define FRACTION_NUMBER 6

// accesses to these arrays MUST use unsigned char in the face of UTF8 strings
extern unsigned char punctuation[];
extern unsigned char toLowercaseData[];
extern unsigned char toUppercaseData[];
extern unsigned char isVowelData[];
extern unsigned char isAlphabeticDigitData[];
extern unsigned char isComparatorData[];
extern unsigned char legalNaming[256];
extern unsigned char realPunctuation[256];
extern signed char nestingData[];

extern bool showBadUTF;
extern char* userRecordSourceBuffer;
extern char tmpWord[MAX_WORD_SIZE];
extern time_t docTime;
extern bool singleSource;
extern bool echoDocument;
extern char* documentBuffer;

#define IsPunctuation(c) (punctuation[(unsigned char)c])
#define IsRealPunctuation(c) (realPunctuation[(unsigned char)c])
#define GetLowercaseData(c) (toLowercaseData[(unsigned char)c])
#define GetUppercaseData(c) (toUppercaseData[(unsigned char)c])
#define GetNestingData(c) (nestingData[(unsigned char)c])


#define IsWhiteSpace(c) (punctuation[(unsigned char)c] == SPACES)
#define IsWordTerminator(c) (punctuation[(unsigned char)c] == SPACES || c == 0)
#define IsVowel(c) (isVowelData[(unsigned char)c] != 0)
#define IsAlphabeticDigitNumeric(c) (isAlphabeticDigitData[(unsigned char)c] != 0)
#define IsUpperCase(c) (isAlphabeticDigitData[(unsigned char)c] == 3)
#define IsLowerCase(c) (isAlphabeticDigitData[(unsigned char)c] == 4)
#define IsAlpha(c) (isAlphabeticDigitData[(unsigned char)c] >= 3)
#define IsLegalNameCharacter(c)  legalNaming[(unsigned char)c] 
#define IsDigit(c) (isAlphabeticDigitData[(unsigned char)c] == 2)
#define IsAlphaOrDigit(c) (isAlphabeticDigitData[(unsigned char)c] >= 2)
#define IsNonDigitNumberStarter(c) (isAlphabeticDigitData[(unsigned char)c] == 1)
#define IsNumberStarter(c) (isAlphabeticDigitData[(unsigned char)c] && isAlphabeticDigitData[(unsigned char)c] <= 2)
#define IsComparison(c) (isComparatorData[(unsigned char)c])

void AcquireDefines(char* fileName);
void AcquirePosMeanings();
char* FindNameByValue(uint64 val); // properties
uint64 FindValueByName(char* name);
char* FindName2ByValue(uint64 val); // flags
uint64 FindValue2ByName(char* name);
void CloseTextUtilities();

void BOMAccess(int &BOMvalue, char &oldc, int &oldCurrentLine);

extern unsigned int startSentence;
extern unsigned int endSentence;

// boolean style tests
bool IsArithmeticOperator(char* word);
char* IsUTF8(char* x);
unsigned IsNumber(char* word,bool placeAllowed = true); // returns kind of number
bool IsPlaceNumber(char* word);
bool IsDigitWord(char* word);
bool IsUrl(char* word, char* end);
unsigned int IsMadeOfInitials(char * word,char* end);
bool IsNumericDate(char* word,char* end);
bool IsFloat(char* word, char* end);
char GetTemperatureLetter (char* ptr);
bool IsLegalName(char* name);
char* GetCurrency(char* ptr,char* &number);
bool IsRomanNumeral(char* word, uint64& val);

// conversion reoutines
void MakeLowerCase(char* ptr);
void MakeUpperCase(char* ptr);
char* MakeLowerCopy(char* to,char* from);
char* MakeUpperCopy(char* to,char* from);
void UpcaseStarters(char* ptr);
void Convert2Underscores(char* buffer,bool upcase,bool removeClasses=true,bool removeBlanks=false);
void Convert2Blanks(char* output);
void ForceUnderscores(char* ptr);
char* TrimSpaces(char* msg,bool start = true);
char* UTF2ExtendedAscii(char* bufferfrom);

// startup
void InitTextUtilities();
bool ReadDocument(char* inBuffer,FILE* sourceFile);

// reading functions
char* ReadFlags(char* ptr,uint64& flags);
char* ReadHex(char* ptr, uint64 & value);
char* ReadInt(char* ptr, unsigned int & value);
char* ReadInt64(char* ptr, int64 & w);
char* ReadQuote(char* ptr, char* buffer,bool backslash = false, bool noblank = true);
char* ReadArgument(char* ptr, char* buffer);
char* ReadCompiledWord(char* ptr, char* word);
char* ReadALine(char* buf,FILE* file,unsigned int limit = maxBufferSize,bool returnEmptyLines = false);
char* SkipWhitespace(char* ptr);
char* BalanceParen(char* ptr,bool within=true);
int64 NumberPower(char* number);
int64 Convert2Integer(char* word);

#endif
