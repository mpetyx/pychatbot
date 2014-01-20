#ifndef _OSH_
#define _OSH_

#ifdef INFORMATION
Copyright (C) 2012 by Bruce Wilcox

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#endif

// EXCEPTION/ERROR
// error recovery
#define SERVER_RECOVERY 4
extern jmp_buf scriptJump[5];
extern int jumpIndex;

void JumpBack();
void myexit(char* msg);



// MEMORY SYSTEM

extern unsigned int maxBufferLimit;
extern unsigned int maxBufferSize;
extern unsigned int maxBufferUsed;	
extern unsigned int bufferIndex;
extern unsigned int baseBufferIndex;
extern unsigned int overflowIndex;
extern char* buffers;
extern bool showmem;

void ResetBuffers();
char* AllocateAlignedBuffer();
char* AllocateBuffer();
void FreeBuffer();
void CloseBuffers();

// FILE SYSTEM

extern int currentFileLine;
extern char currentFilename[MAX_WORD_SIZE];

void InitFileSystem(char* untouchedPath,char* readablePath,char* writeablePath);

void StartFile(const char* name);

FILE* FopenStaticReadOnly(const char* name);
FILE* FopenReadOnly(const char* name);
FILE* FopenReadNormal(char* name);
FILE* FopenWrite(const char* name,const char* how);
FILE* FopenReadWritten(const char* name);
FILE* FopenUTF8Write(const char* filename);
FILE* FopenUTF8WriteAppend(const char* filename);

typedef void (*FILEWALK)(char* name, uint64 flag);
void WalkDirectory(char* directory,FILEWALK function, uint64 flags);

char* GetUserPath(char* name);

// TIME

#define SKIPWEEKDAY 4 // from gettimeinfo

char* GetTimeInfo();
char* GetMyTime(time_t curr);

#ifdef __MACH__
void clock_get_mactime(struct timespec &ts);
#endif
clock_t ElapsedMilliseconds();
#ifndef WIN32
unsigned int GetFutureSeconds(unsigned int seconds);
#endif

// LOGGING

#define LOGGING_NOT_SET 2

#define SERVERLOG 0
#define STDUSERLOG 1
#define STDDEBUGLOG 2
#define STDPUBLOG 3
#define BADSCRIPTLOG 9
#define BUGLOG 10
#define STDUSERTABLOG 101
#define STDUSERATTNLOG 201

extern bool echo;
extern bool silent;
extern uint64 logCount;
extern char* testOutput;

#define ReportBug(...) {Bug(); Log(BUGLOG, __VA_ARGS__);}

extern char logFilename[MAX_WORD_SIZE];
extern bool logUpdated;
extern char* logmainbuffer;
extern char serverLogfileName[200];
extern int userLog;
extern int serverLog;


unsigned int Log(unsigned int spot,const char * fmt, ...);
void Bug();
void ChangeDepth(int value,char* where);

// RANDOM NUMBERS

#define MAXRAND 256

extern unsigned int randIndex;

unsigned int random(unsigned int range);
uint64 Hashit(unsigned char * data, int len);
extern bool hasUpperCharacters;
extern bool hasUTF8Characters;

#endif
