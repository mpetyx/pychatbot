#ifndef _EXECUTE
#define _EXECUTE


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

#include "common.h"

#define MAX_ARGUMENT_COUNT 900

typedef unsigned int (*EXECUTEPTR)(char* buffer);

#define OWNTRACE 1
#define SAMELINE 2

typedef struct SystemFunctionInfo 
{
	const char* word;					//   dictionary word entry
	EXECUTEPTR fn;				//   function to use to get it
	int argumentCount;			//   how many callArgumentList it takes
	int	properties;				//   non-zero means does its own argument tracing
	const char* comment;
} SystemFunctionInfo;

//   special argeval codes
#define VARIABLE_ARGS -1
#define STREAM_ARG -2

//   DoFunction results
#define ENDRULE_BIT 1
#define FAILRULE_BIT 2
#define RETRYRULE_BIT 4
#define RETRYTOPRULE_BIT 8

#define ENDTOPIC_BIT 16
#define FAILTOPIC_BIT 32
#define RETRYTOPIC_BIT 64

#define ENDSENTENCE_BIT 128
#define FAILSENTENCE_BIT 256
#define RETRYSENTENCE_BIT 512

#define ENDINPUT_BIT 1024
#define FAILINPUT_BIT 2048

#define FAIL_MATCH 4096			// transient result of TestRule, converts to FAILRULE_BIT

#define UNDEFINED_FUNCTION 8192	//   potential function call has no definition so isnt

#define FAILCODES ( FAILINPUT_BIT | FAILTOPIC_BIT|FAILRULE_BIT | FAILSENTENCE_BIT | RETRYSENTENCE_BIT | RETRYTOPIC_BIT )
#define SUCCESSCODES ( ENDINPUT_BIT | ENDSENTENCE_BIT | ENDTOPIC_BIT | ENDRULE_BIT )
#define ENDCODES ( FAILCODES | SUCCESSCODES )
#define RESULTBEYONDTOPIC ( FAILSENTENCE_BIT | ENDSENTENCE_BIT | RETRYSENTENCE_BIT | ENDINPUT_BIT | FAILINPUT_BIT )
							
//   argument data for system calls
extern int wasCommand;

#define MAX_ARG_LIST 200
#define MAX_ARG_LIMIT 30 // max args to a call -- limit using 1 bit per arg for table mapping behavior

extern unsigned int currentIterator;

extern char lastInputSubstitution[INPUT_BUFFER_SIZE];
extern int globalDepth;
#define ARGUMENT(n) callArgumentList[callArgumentBase+n]

//   argument data for user calls
extern char callArgumentList[MAX_ARGUMENT_COUNT+1][MAX_WORD_SIZE*2];    //   function callArgumentList
extern unsigned int callArgumentIndex;
extern unsigned int callArgumentBase;
extern unsigned int fnVarBase;
extern SystemFunctionInfo systemFunctionSet[];

extern bool planning;
extern bool nobacktrack;

void InitFunctionSystem(); 
char* DoFunction(char* name, char* ptr, char* buffer,unsigned int &result);
void DumpFunctions();
unsigned int Callback(WORDP D,char* arguments);
void ResetFunctionSystem();
void SaveMark(char* buffer,unsigned int iterator);

unsigned int KeywordTopicsCode(char* buffer);

// utilities
char* ResultCode(unsigned int result);
unsigned int FLR(char* buffer,char which);
void ResetUser(char* input);
bool RuleTest(char* rule);
unsigned int DebugCode(char* buffer);
unsigned int MatchCode(char* buffer);
#endif
