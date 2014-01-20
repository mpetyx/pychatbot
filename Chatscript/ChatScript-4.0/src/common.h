#ifndef _MAXH_
#define _MAXH_

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

#define INPUT_BUFFER_SIZE   4000
#define MAX_BUFFER_SIZE		80000

// These can be used to shed components of the system to save space
 // #define DISCARDSERVER 1
 // #define DISCARDSCRIPTCOMPILER 1
 // #define DISCARDPARSER 1
 // #define DISCARDTESTING 1
 // #define DISCARDCLIENT 1
 // #define DISCARDTCPOPEN
#ifdef WIN32
#define DISCARDDICTIONARYBUILD 1 // only a windows version can build a dictionary from scratch
#else
#define DISCARDDICTIONARYBUILD 1  
#endif

// These can be used to include LINUX EVSERVER component - this is automatically done by the makefile in src
// #define EVSERVER 1

// These can be used to embed chatscript within another application (then you must call InitSystem on startup yourself)
// #define NOMAIN 1

typedef unsigned long long int  uint64;
typedef signed long long  int64;
#define ALWAYS (1 == always)

#ifndef WIN32
	typedef long long long_t; 
	typedef unsigned long long ulong_t; 
	#include <sys/time.h> 
	#include <termios.h> 
	#include <unistd.h>
	#define stricmp strcasecmp 
	#define strnicmp strncasecmp 
#else
	typedef long long_t;       
	typedef unsigned long ulong_t; 
#endif

#ifdef WIN32
	#pragma comment(lib, "ws2_32.lib") 
	#pragma warning( disable : 4100 ) 
	#pragma warning( disable : 4189 ) 
	#pragma warning( disable : 4290 ) 
	#pragma warning( disable : 4706 )
	#pragma warning( disable : 4996 ) 

	#define _CRT_SECURE_NO_DEPRECATE
	#define _CRT_SECURE_NO_WARNINGS 1
	#define _CRT_NONSTDC_NO_DEPRECATE
	#define _CRT_NONSTDC_NO_WARNINGS 1
	#include <conio.h>
	#include <direct.h>
	#include <io.h>
	#include <process.h>
	#include <windows.h>
#elif IOS
	#include <dirent.h>
	#include <mach/clock.h>
	#include <mach/mach.h>
#elif __MACH__
	#include <dirent.h>
	#include <mach/clock.h>
	#include <mach/mach.h>
#else  // LINUX
	#include <dirent.h>
#endif

#ifdef IOS
#elif __MACH__
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif

#include <algorithm>
#include <assert.h>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <ctype.h>
#include <errno.h>
#include <exception>  
#include <fcntl.h>
#include <iostream>
//#include <map>
#include <math.h>
#include <memory.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string>  
#include <string.h>
#include <sys/stat.h>
#include <sys/timeb.h> 
#include <sys/types.h>
#include <time.h>
#include <utility>
#include <vector>

using namespace std;

#undef WORDP //   remove windows version (unsigned short) for ours

#ifdef EVSERVER
#include "cs_ev.h"
#include "evserver.h"
#define EVSERVER_FORK 1		// use ev server in fork
#define LOCKUSERFILE 1		// protext from multiple servers hitting same file
#define USERPATHPREFIX 1	// do binary tree of log file
#endif

#include "dictionarySystem.h"
#include "mainSystem.h"
#include "os.h"

#include "csocket.h"
#include "constructCode.h"
#include "debugger.h"
#include "english.h"
#include "factSystem.h"
#include "functionExecute.h"
#include "infer.h"
#include "markSystem.h"
#include "outputSystem.h"
#include "patternSystem.h"
#include "readrawdata.h"
#include "scriptCompile.h"
#include "spellcheck.h"
#include "systemVariables.h"
#include "testing.h"
#include "textUtilities.h"
#include "tagger.h"
#include "tokenSystem.h"
#include "topicSystem.h"
#include "userCache.h"
#include "userSystem.h"
#include "variableSystem.h"

#endif
