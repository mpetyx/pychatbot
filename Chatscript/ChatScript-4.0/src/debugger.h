#ifndef _DEBUGGERGH
#define _DEBUGGERGH
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

// debugger code
#define ENTER_DEBUGGER 0
#define ENTER_TOPIC 1
#define ENTER_RULE 2
#define ENTER_RULE_OUTPUT 3
#define ENTER_LOOP 4
#define ENTER_REUSE_TOPIC 5
#define ENTER_REFINE 6
#define EXIT_RULE_OUTPUT 7
#define EXIT_LOOP 8
#define EXIT_REUSE_TOPIC 9
#define EXIT_REFINE 10
#define EXIT_RULE 11
#define EXIT_TOPIC 12
#define PARSING_OUTPUT 13
#define ENTER_IF 14
#define EXIT_IF 15
#define ENTER_CALL 16
#define EXIT_CALL 17

#define START_LEVEL 0
#define TOPIC_LEVEL 1
#define RULE_LEVEL 2
#define OUTPUT_LEVEL 3
// run and trace commands
#define OUT2LOOP 6
#define OUT2REFINE 7
#define OUT2REUSE 8
#define OUT2RULE 9
#define OUT2TOPIC 10
#define OUT2IF 11
#define OUT2CALL 12

#define STEP_IN 1000

#define TOPIC_TRACE 1
#define RULE_TRACE 2
#define RULE_MATCH 4

extern bool blocked;
extern int out2level;

void Debugger(int code,unsigned int result = 0,char* at = NULL);
void DebugHeader(char* msg);
char* ReadOutput(char* ptr,char* buffer);

#endif
