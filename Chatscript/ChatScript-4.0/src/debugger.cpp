#include "common.h"

bool blocked = false;  // debugger NOT allowed to do/see inside a pattern match itself.

#ifndef DISCARDTESTING
#define MAX_DEBUG_LEVELS 200
#define MAX_BREAK_TOPICS 10
static int level = 0;			// globalDepth associated with this debug level
static unsigned int doing = 0;			// behavior we want debugger to be doing
static unsigned int lastRuleTopic = 0;	// topic we gave command from, whose rules we should enter
static unsigned int lastCommand = 0;
static unsigned int action = 0;
static unsigned int debugTrace = 0;
static char* output = 0;				// current place in output execution system should stop at
static char kind = 0;					// kind of current debug level (rule, topic, loop, etc)
static char* call;						// name of function being called
static char oldKind[MAX_DEBUG_LEVELS];
static unsigned int oldDoing[MAX_DEBUG_LEVELS];
static char* oldCall[MAX_DEBUG_LEVELS];
static int oldLevel[MAX_DEBUG_LEVELS];
static unsigned int oldAction[MAX_DEBUG_LEVELS];
static unsigned int oldRuleTopic[MAX_DEBUG_LEVELS];
static bool displayed[MAX_DEBUG_LEVELS];
static char* oldOutput[MAX_DEBUG_LEVELS];
static unsigned int oldIndex = 0;
static char* lastOutput;
int out2level = -2;					// what deeper levels we should run silently

// transient run til values
static unsigned int transientBreakTopic = 0;
static unsigned int transientBreakRuleID = 0;
static unsigned int transientBreakRuleTopic = 0;
static char* transientBreakRule = NULL;

// breakpoints data
static char* breakRules[MAX_BREAK_TOPICS];
static unsigned int breakRuleID[MAX_BREAK_TOPICS];
static unsigned int breakRuleTopic[MAX_BREAK_TOPICS];
static unsigned int breakRuleIndex = 0;
static unsigned int breakTopics[MAX_BREAK_TOPICS];
static unsigned int breakTopicIndex = 0;
#endif

static bool refreshTopic = false;		// when we exit a topic, do we need to tell user topic we return to

void IndentDebug()
{
	if (refreshTopic) // never say resuming to a top level topic
	{
		refreshTopic = false;
		Log(STDDEBUGLOG,"Debug resuming %s:\r\n",GetTopicName(currentTopicID));
	}
	Log(STDDEBUGLOG,"Debug: ");
	int count = globalDepth;
	while (count-- > 0) Log(STDDEBUGLOG," ");
}

void DebugHeader(char* msg)
{
	bool oldEcho = echo;
	echo = true;
	Log(STDUSERLOG,"\r\nDebugging %s:\r\n",msg);
	echo = oldEcho;
	Debugger(ENTER_DEBUGGER); 
}

#ifndef DISCARDTESTING
static void PushDebugLevel(unsigned int what,char type)
{
	++oldIndex;
	oldDoing[oldIndex] = doing;
	oldCall[oldIndex] = call;
	oldLevel[oldIndex] = level;
	oldRuleTopic[oldIndex] = lastRuleTopic;
	oldAction[oldIndex] = action;
	oldOutput[oldIndex] = output;
	oldKind[oldIndex] = kind;
	displayed[oldIndex] = false;		// default this level has no exit display needed
	level = globalDepth;
	doing = what;
	output = NULL;
	kind = type;
}

static void PopDebugLevel()
{
	if (!oldIndex) return;
	level = oldLevel[oldIndex];
	doing = oldDoing[oldIndex];
	lastRuleTopic = oldRuleTopic[oldIndex];
	output = oldOutput[oldIndex];
	call = oldCall[oldIndex];
	kind = oldKind[oldIndex];
	action = oldAction[oldIndex];
	--oldIndex;
}

char* ReadOutput(char* ptr,char* buffer) // locate next output fragment to display (that will be executed)
{
	char next[MAX_WORD_SIZE];
	char* hold;
	*buffer = 0;
	char* out = buffer;
	while (*ptr != ENDUNIT) // not end of data
	{
		ptr = ReadCompiledWord(ptr,out); // move token 
		char* copied = out;
		out += strlen(out);
		strcpy(out," ");
		++out;
		*out = 0;
		hold = ReadCompiledWord(ptr,next); // and the token after that?
		if (IsAlphaOrDigit(*copied)) // simple output word was copied
		{
			if (!*next || !IsAlphaOrDigit(*next)) break; //  followed by something else simple
		}
		else if (*buffer == ':' && buffer[1]) // testing command occupies the rest always
		{
			char* end = strchr(ptr,ENDUNIT);
			if (end)
			{
				strncpy(out,ptr,end-ptr);
				out += end-ptr;
				*out = 0;
			}
			ptr = NULL;
			break;
		}
		else if (*buffer == '^' && *next == '(') // function call
		{
			char* end = BalanceParen(ptr+1); // function call args
			strncpy(out,ptr,end-ptr);
			out += end-ptr;
			*out = 0;
			ptr = end;
			break;
		}
		else if (*buffer == '$' || *buffer == '%' || (*buffer == '@' && IsDigit(buffer[1])) || (*buffer == '_' && IsDigit(buffer[1]))  ) // user or system variable or factset or match variable
		{
			if (*next != '=' && next[1] != '=') break; // not an assignment statement
			while (hold) // read op, value pairs
			{
				strcpy(out,next); // transfer assignment op or arithmetic op 
				out += strlen(out);
				strcpy(out," ");
				++out;
				ptr = ReadCompiledWord(hold,next); // read value
				strcpy(out,next); // transfer value
				out += strlen(out);
			
				// if value is a function call, get the whole call
				if (*next == '^' && *ptr == '(')
				{
					char* end = BalanceParen(ptr+1); // function call args
					strncpy(out,ptr,end-ptr);
					out += end-ptr;
					*out = 0;
					ptr = end;
				}

				strcpy(out," ");
				++out;
				if (*ptr != ENDUNIT) // more to rule
				{
					hold = ReadCompiledWord(ptr,next); // is there more to assign
					if (IsArithmeticOperator(next)) continue; // need to swallow op and value pair
				}
				break;
			}
			break;
		}
		else if (*buffer == '[') // choice area
		{
			//   find closing ]
			char* end = ptr-1;
			while (ALWAYS) 
			{
				end = strchr(end+1,']'); // find a closing ] 
				if (!end) break; // failed
				if (*(end-1) != '\\') break; // ignore literal \[
			}
			if (end) // found end of a [] pair
			{
				++end;
				strncpy(out,ptr,end-ptr);
				out += end-ptr;
				*out = 0;
				ptr = end + 1;
				if (*ptr != '[') break; // end of choice zone
			}
		}
		else break;
	}
	if (!stricmp(buffer,"^^loop ( -1 ) "))  strcpy(buffer,"^^loop ( ) ");	// shorter notation
	return ptr;
}

#ifndef WIN32
char getch() // direct character input read (no buffering)
{ 
	char buf = 0; 
	struct termios old = {0}; 
	if (tcgetattr(0, &old) < 0) perror("tcsetattr()"); 
	old.c_lflag &= ~ICANON; 
	old.c_lflag &= ~ECHO; 
	old.c_cc[VMIN] = 1; 
	old.c_cc[VTIME] = 0; 
	if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON"); 
	if (read(0, &buf, 1) < 0) perror ("read()"); 
	old.c_lflag |= ICANON; 
	old.c_lflag |= ECHO; 
	if (tcsetattr(0, TCSADRAIN, &old) < 0) perror ("tcsetattr ~ICANON"); 
	return (buf); 
}
#endif

static unsigned int GetDebuggerResumeCommand() // returns what to do to resume the level
{
	char word[MAX_WORD_SIZE];
	unsigned int command = 0;
	// interactive user control here
	char input[1000];
	*input = 0;
	char* ptr = input;
	Log(STDDEBUGLOG," ?: ");
	while (ALWAYS)
	{
		int x;
#ifdef WIN32
		if (!_kbhit()) continue;

		x = _getch();
		if (x == 0xE0) // arrow commands
		{
			x = _getch(); // assuming numlock off
			if (x == 0x50)  command = 'i'; // down  IN
			else if (x == 0x4b) command = 's'; // left  Go to completion
			else if (x == 0x4d)  command = 'o'; // right -- step over
			else if (x == 0x48) command = 'u';  // up -- step out
			return command;
		}
#else
		x = getch();
		if (x == 0x1b) // arrow commands via ESC
		{
			x = getch(); 
			if (x == 0x5b) x = getch(); 
			if (x == 0x42) command = 'i'; // down  IN
			else if (x == 0x44) command = 's'; // left  STOP
			else if (x == 0x43)  command = 'o'; // right -- step over
			else if (x == 0x41) command = 'u';  // up -- step out
			return command;
		}
#endif
		if (x == '\r' || x == '\n') // end of normal input
		{
			char* line = SkipWhitespace(input);
			if (*input == ':') Command(line,NULL); // command
			else if ((*input == 'b' || *input == 'g') && (input[1] == ' ' || !input[1])) // topic or rule breakpoint or transient breakpoint
			{
				line = input + 1;
				while (line && *line)
				{
					line = ReadCompiledWord(line,word);
					char* dot = strchr(word,'.');
					if (!dot && *word == '~') // topic
					{
						unsigned int topic = FindTopicIDByName(word);
						unsigned int i;
						for (i = 0; i < breakTopicIndex; ++i)
						{
							if (breakTopics[i] == topic) break;
						}
						if (!topic) Log(STDDEBUGLOG,"Topic %s not found\r\n",word);
						else if (i < breakTopicIndex){;} // already  in list
						else if (*input == 'g') 
						{
							transientBreakTopic = topic;
							return 'g';
						}
						else breakTopics[breakTopicIndex++] = topic;
						if (breakTopicIndex >= 10) --breakTopicIndex;
					}
					else
					{
						char* rule;
						int id;
						bool fulllabel = false;
						bool crosstopic = false;
						unsigned int topic;
						if (IsDigit(dot[1])) rule = GetRuleTag(topic,id,word);
						else rule = GetLabelledRule(topic,word,"",fulllabel,crosstopic,id);
						unsigned int i;
						for (i = 0; i < breakRuleIndex; ++i)
						{
							if (breakRules[i] == rule) break;
						}
						if (!rule) Log(STDDEBUGLOG,"Rule %s not found\r\n",word);
						else if (i < breakRuleIndex){;} // already  in list
						else if (*input == 'g') 
						{
							transientBreakRuleID = id;
							transientBreakRuleTopic = topic;
							transientBreakRule = rule;
							return 'g';
						}
						else 
						{
							breakRuleID[breakRuleIndex] = id;
							breakRuleTopic[breakRuleIndex] = topic;
							breakRules[breakRuleIndex] = rule;
							breakRuleIndex++;
						}
						if (breakRuleIndex >= 10) --breakRuleIndex;
					}
				}
				if (*input != 'g') x = 'l'; // list breakpoints
				else return 'g';
				Log(STDDEBUGLOG,"\r\n");
			}
			else if (*input == 'd' && input[1] == ' ') // delete topic breakpoint
			{
				line = input+2;
				while (line && *line)
				{
					line = ReadCompiledWord(line,word);
					line = ReadCompiledWord(line,word);
					char* dot = strchr(word,'.');
					if (!dot) // topic
					{
						unsigned int topic = FindTopicIDByName(word);
						if (!topic) Log(STDDEBUGLOG,"Topic %s not found\r\n",word);
						else 
						{
							for (unsigned int i = 0; i < breakTopicIndex; ++i)
							{
								if (breakTopics[i] == topic) 
								{
									memmove(breakTopics+i,breakTopics+i+1,sizeof(int) * (breakTopicIndex-i));
									--breakTopicIndex;
									break;
								}
							}
						}
					}
					else
					{
						char* rule;
						int id;
						bool fulllabel = false;
						bool crosstopic = false;
						unsigned int topic;
						if (IsDigit(dot[1])) rule = GetRuleTag(topic,id,word);
						else rule = GetLabelledRule(topic,word,"",fulllabel,crosstopic,id);
						unsigned int i;
						for (i = 0; i < breakRuleIndex; ++i)
						{
							if (breakRules[i] == rule) break;
						}
						if (!rule) Log(STDDEBUGLOG,"Rule %s not found\r\n",word);
						else 
						{
							for (unsigned int i = 0; i < breakRuleIndex; ++i)
							{
								if (breakRules[i] == rule) 
								{
									memmove(breakRules+i,breakRules+i+1,sizeof(char*) * (breakRuleIndex-i));
									memmove(breakRuleTopic+i,breakRuleTopic+i+1,sizeof(unsigned int) * (breakRuleIndex-i));
									memmove(breakRuleID+i,breakRuleID+i+1,sizeof(unsigned int) * (breakRuleIndex-i));
									--breakRuleIndex;
									break;
								}
							}
						}
 					}
				}
				x = 'l';
				Log(STDDEBUGLOG,"\r\n");
			}
			else x = 0;
			*input = 0;
		}

		if (!*input && x == 't')
		{
			debugTrace ^= TOPIC_TRACE;
			if (debugTrace & TOPIC_TRACE) Log(STDDEBUGLOG,"trace topics\r\n ?: ");
			else Log(STDDEBUGLOG,"untrace topics\r\n ?: ");
		}
		else if (!*input && x == 'r') 
		{
			debugTrace ^= RULE_TRACE;
			if (debugTrace & RULE_TRACE) Log(STDDEBUGLOG,"trace rules\r\n ?: ");
			else  Log(STDDEBUGLOG,"untrace rules\r\n ?: ");
		}
		else if (!*input && x == 'm') 
		{
			debugTrace |= RULE_MATCH;
			return 'm';
		}
		else if (!*input && x == 'x') 
		{
			return 'x';
		}	
		else if (!*input && x == 'h')
		{
			Log(STDDEBUGLOG,"Commands: \r\n");
			Log(STDDEBUGLOG,"  h (help) - list the commands\r\n");
			Log(STDDEBUGLOG," down-arrow (step in) - go logically deeper. Topic into rule, rule into output, output into subtopic\r\n");
			Log(STDDEBUGLOG," right-arrow (step over) - complete current unit (topic, rule, output fragment)\r\n");
			Log(STDDEBUGLOG," up-arrow (step out) - if topic or rule, finish topic. If output, finish rule.\r\n"); 
			Log(STDDEBUGLOG," left-arrow (run to completion) - run to completion or until stopped by a break.\r\n");
			Log(STDDEBUGLOG,"  b (set breaks) followed by list of topic names and/or rule tags or labels and ENTER\r\n");
			Log(STDDEBUGLOG,"  d (delete topic breaks) followed by list of topic names and/or rule tags or labels and ENTER\r\n");
			Log(STDDEBUGLOG,"  g (go til) followed by a topic names and/or rule tag or label and ENTER or just ENTER\r\n");
			Log(STDDEBUGLOG,"  m (run to match) execute rules til one matches\r\n");
			Log(STDDEBUGLOG,"  l (list breaks) - shows current topic breakpoints and rule breakpoints set\r\n");
			Log(STDDEBUGLOG,"  r (rule trace) - turn on/off rule entry trace\r\n");
			Log(STDDEBUGLOG,"  t (topic trace) - turn on/off topic entry/exit trace\r\n");
			Log(STDDEBUGLOG,"  w (where am I) - display state of topics within\r\n ?: ");
			Log(STDDEBUGLOG,"  x (exit debugging)\r\n");
		}
		else if (!*input && x == 'l') // list breakpoints
		{
			Log(STDDEBUGLOG,"\r\nBreak topics: " );
			for (unsigned int i = 0; i < breakTopicIndex; ++i) Log(STDDEBUGLOG,"%s ",GetTopicName(breakTopics[i]));
			Log(STDDEBUGLOG,"\r\n");
			Log(STDDEBUGLOG,"Break rules: " );
			for (unsigned int i = 0; i < breakRuleIndex; ++i) 
			{
				char label[MAX_WORD_SIZE];
				GetLabel(breakRules[i],label);
				Log(STDDEBUGLOG,"%s.%d.%d (%s) ",GetTopicName(breakRuleTopic[i]),TOPLEVELID(breakRuleID[i]),REJOINDERID(breakRuleID[i]));
			}
			Log(STDDEBUGLOG,"\r\n");
			Log(STDDEBUGLOG," ?: ");
			ptr = input;
			*ptr = 0;
		}
		else if (!*input && x == 'w') // show topic control stack
		{
			Log(STDDEBUGLOG,"topic nesting:\r\n");
			unsigned int count = 6;
			while (count--) Log(STDDEBUGLOG,"  ");	// indent for main topic
			Log(STDDEBUGLOG,"%s     <--\r\n",GetTopicName(currentTopicID));
			for (unsigned int i = topicIndex; i >= 2; --i)
			{
				unsigned int count = 6;
				while (count--) Log(STDDEBUGLOG,"  ");	// indent
				Log(STDDEBUGLOG,"%s\r\n",GetTopicName(topicStack[i]));
			}
			Log(STDDEBUGLOG," ?: ");
		}
		else if (x == 8) // do backspace
		{
			if (ptr != input)
			{
				*--ptr = 0;
				Log(STDDEBUGLOG,"\r\n ?: %s",input);
			}
		}
		else 
		{
			*ptr++ = (char) x;
			*ptr = 0;
			Log(STDDEBUGLOG,"%c",x);
		}
	}

	return command;
}

static void ExecuteResume(unsigned int command)
{
	lastCommand = command;
	if (command == 'o') // step over current thingy
	{
		Log(STDDEBUGLOG," step over\r\n");
		out2level = 0;
	}
	else if (command == 'x') // exit
	{
		Log(STDDEBUGLOG," exit debugger\r\n");
		debugger = false;
	}
	else if (command == 's') // run to completion
	{
		Log(STDDEBUGLOG," run\r\n");
		doing = lastCommand = OUT2TOPIC; 
		out2level = -1;
	}
	else if (command == 'g' || command == 'm')
	{
		if (command == 'm')  Log(STDDEBUGLOG," run til match\r\n");
		else  Log(STDDEBUGLOG," go\r\n");
		doing = TOPIC_LEVEL;
		out2level = -1;
	}
	else if (command == 'u') // up 
	{
		Log(STDDEBUGLOG," step out\r\n");
		if (doing == OUTPUT_LEVEL) // but traces need to be a separate flag
		{
			out2level = level - 1;
			if (kind == 'l') doing = lastCommand = OUT2LOOP; // complete the loop body
			else if (kind == 'i') doing =  lastCommand = OUT2IF; // complete the if body
			else if (kind == 'c') doing =  lastCommand = OUT2CALL; // complete the call
			else if (oldKind[oldIndex] == 'x') doing =  lastCommand = OUT2REUSE; // complete the reuse
			else if (kind == 'r') 
			{
				doing = lastCommand =  OUT2RULE; // complete the rule
				out2level = level;
			}
		}
		else if (doing == RULE_LEVEL) 
		{
			unsigned int i = oldIndex;
			doing = lastCommand = OUT2TOPIC; // run til this topic is exited
			if (kind == 't') out2level = level - 1;  // before his level
			else if (oldKind[oldIndex] == 'y')
			{
				doing =  lastCommand = OUT2REFINE;// complete the refine
				out2level = level - 1;
			}
			else
			{
				while (--i)
				{
					if (oldKind[i] == 't')
					{
						out2level = oldLevel[i+1] - 1; // before his level
						break;
					}
				}
			}
		}
		else if (doing == TOPIC_LEVEL)
		{
			out2level = level - 1;	 // before current level
		}
	}
	else if (command == 'i') // step in current thingy
	{
		Log(STDDEBUGLOG," step in\r\n");
		if (kind == 't') // a topic
		{
			lastRuleTopic = currentTopicID; // allow rules from what will come after this topic level (the rule level)
			doing = RULE_LEVEL;
		}
		else if (doing == RULE_LEVEL || doing == OUTPUT_LEVEL) doing = OUTPUT_LEVEL;
	}
}

#endif

void Debugger(int code,unsigned int result,char* at)
{
#ifndef DISCARDTESTING

	// reasons to do nothing
	if (blocked) return; // performing a pre-match on a rule or inside a match
	if (code == PARSING_OUTPUT && (lastCommand == OUT2RULE || doing != OUTPUT_LEVEL || (out2level >= -1 && globalDepth > out2level))) return;	// waiting til we get back to a topic
	if (code == ENTER_RULE_OUTPUT)  return;

	// simple endings of a debug level
	if (code == EXIT_REUSE_TOPIC || code == EXIT_REFINE || code == EXIT_LOOP || code == EXIT_RULE || code == EXIT_IF || code == EXIT_CALL) // simple terminations of scope
	{
		if (code == EXIT_RULE && lastCommand == OUT2RULE)
		{
			if (level == out2level)
			{
				lastCommand = 0;
				out2level = -2;
				doing = RULE_LEVEL;
			}
		}
		else if (code == EXIT_LOOP && lastCommand == OUT2LOOP)
		{
			if (level == out2level)
			{
				lastCommand = 0;
				out2level = -2;
				doing = OUTPUT_LEVEL;
			}
		}
		else if (code == EXIT_IF && lastCommand == OUT2IF)
		{
			if (level == out2level)
			{
				lastCommand = 0;
				out2level = -2;
				doing = OUTPUT_LEVEL;
			}
		}
		else if (code == EXIT_REFINE && lastCommand == OUT2REFINE)
		{
			if (level == out2level)
			{
				lastCommand = 0;
				out2level = -2;
				doing = OUTPUT_LEVEL;
			}
		}
		else if (code == EXIT_CALL && lastCommand == OUT2CALL)
		{
			if (out2level) call = 0;
			if (level == out2level)
			{
				lastCommand = 0;
				out2level = -2;
				doing = OUTPUT_LEVEL;
			}
		}

		PopDebugLevel();
		return;
	}
	else if (code == EXIT_TOPIC)
	{
		output = NULL;
		if (displayed[oldIndex]) // did we show entry?
		{
			IndentDebug();
			Log(STDDEBUGLOG,"exit %s result: %s responses: %d\r\n",GetTopicName(currentTopicID),ResultCode(result),responseIndex);
			if (!(debugTrace & (TOPIC_TRACE | RULE_MATCH)) && globalDepth > 1) refreshTopic = true; // top level topics dont trigger resume on their successors
		}
		PopDebugLevel();
		if (out2level >= 0 && level <= out2level) // wanting to exit this topic
		{
			out2level = -2;
			doing = RULE_LEVEL; // return to rule level that called us
		}
		return;
	}
	else if (code == EXIT_RULE_OUTPUT) 
	{
		if (doing == OUTPUT_LEVEL) doing = RULE_LEVEL; // drop out of this detail
		return;
	}

	// simple ENTERINGS of a debug level
	if (code == ENTER_DEBUGGER)
	{
		// clear the world in preparation for the 1st topic (the prepass)
		oldDoing[oldIndex] = 0;
		oldLevel[oldIndex] = 0;
		displayed[oldIndex] = 0;
		if (out2level != -1) 
		{
			out2level = -2;
			debugTrace = 0;
			transientBreakTopic = 0;
			transientBreakRuleID = 0;
			transientBreakRuleTopic = 0;
			transientBreakRule = NULL;
			lastCommand = 0;
			breakRuleIndex = 0;
			breakTopicIndex = 0;
		}
		refreshTopic = false;
		doing = START_LEVEL;
		level = globalDepth;
		oldIndex = 0;
		debugger = true;
		return;
	}
	else if (code == ENTER_TOPIC) // entering a topic is a major event
	{
		bool breakAtTopic = false;
		if (currentTopicID == transientBreakTopic)  // 'g' command
		{
			transientBreakTopic = 0;
			breakAtTopic = true;
		}
		for (unsigned int i = 0; i < breakTopicIndex; ++i) 
		{
			if (breakTopics[i] == currentTopicID) breakAtTopic = true;
		}
		if (breakAtTopic) 	out2level = -2;

		PushDebugLevel(doing,'t'); // note for when we leave this level

		// we should pass thru this blindly if executing RULE_LEVEL or RULE_TRACE or didnt enter via an "in" command (unless its a breakpoint)
		// we should show it if TOPIC_TRACE, breakpoint, executing "in" from an OUTPUT_LEVEL, doing a START_LEVEL, 
		if (debugTrace && !breakAtTopic) 
		{
			IndentDebug();
			Log(STDDEBUGLOG,"at %s\r\n",GetTopicName(currentTopicID));
			displayed[oldIndex] = true;
		}

		// things we can be doing: START_LEVEL  TOPIC_LEVEL  RULE_LEVEL  OUTPUT_LEVEL  TOPIC_TRACE  RULE_TRACE 5
		if (breakAtTopic) 
		{
			IndentDebug();
			Log(STDDEBUGLOG,"break at %s\r\n",GetTopicName(currentTopicID));
			displayed[oldIndex] = true;
			debugTrace |= TOPIC_TRACE; 
		}
		else if (out2level >= -1 && globalDepth > out2level) return;	// waiting til we get back to a topic
		else if (doing == START_LEVEL) // start to 1st topic
		{
			IndentDebug();
			Log(STDDEBUGLOG,"at %s\r\n",GetTopicName(currentTopicID));
			displayed[oldIndex] = true;
			doing = TOPIC_LEVEL;
		}
		else if (doing == TOPIC_LEVEL)
		{
			// you can be here by stepping in from an output_level or because you are just stepping over/out from a TOPIC_LEVEL
			if (lastCommand == 'i' && !displayed[oldIndex])
			{
				IndentDebug();
				Log(STDDEBUGLOG,"at %s\r\n",GetTopicName(currentTopicID));
				displayed[oldIndex] = true;
			}
		}
		else if (doing == OUTPUT_LEVEL) // if stepping over ignore, if stepping in then stop
		{
			if (lastCommand == 'i')
			{
				if (!displayed[oldIndex])
				{
					IndentDebug();
					Log(STDDEBUGLOG,"at %s\r\n",GetTopicName(currentTopicID));
					displayed[oldIndex] = true;
				}
				output = NULL;
				doing = TOPIC_LEVEL;
			}
		}
		else return;
	}
	else if (code == ENTER_REUSE_TOPIC) // special level for a reuse change of topic
	{
		PushDebugLevel(doing,'x');
		if (out2level >= -1 && globalDepth > out2level) return;	// waiting til we get back to a topic
		if (doing != OUTPUT_LEVEL && doing != RULE_LEVEL && !(debugTrace & RULE_TRACE)) return;	// output_level can end up here by doing ^refine or can step over or in a rule, ending at next rule 
		if (lastCommand != 'i') return;	// only visible on step in

		IndentDebug();
		Log(STDDEBUGLOG," ^reuse %s\r\n",GetTopicName(currentTopicID));
		displayed[oldIndex] = true;
		lastRuleTopic = currentTopicID;	// this is now the level that issued the rule, we continue in it for now.
		doing = RULE_LEVEL;
		return;
	}
	else if (code == ENTER_LOOP)
	{
		PushDebugLevel(doing,'l'); // note for when we leave this level
		if (debugTrace & (RULE_TRACE | TOPIC_TRACE)) return;
		if (out2level >= -1 && globalDepth > out2level) return;	// waiting til we get back to a topic
		if (lastCommand != 'i') return;	// only visible on step in
	
		if (doing != OUTPUT_LEVEL && doing != RULE_LEVEL && !(debugTrace & RULE_TRACE) ) return;	// output_level can end up here by doing ^refine or can step over or in a rule, ending at next rule 
		if (currentTopicID != lastRuleTopic) return;	// not in the topic issuing the command ... but REUSE can use any topic so it has special commands to set it up
		lastRuleTopic = currentTopicID;	// see rules of loop next
		action = 0;
		output = at;
		doing = OUTPUT_LEVEL;
		return;
	}
	else if (code == ENTER_IF)
	{
		PushDebugLevel(doing,'i'); // note for when we leave this level
		if (!(debugTrace & RULE_TRACE)) return;
		if (out2level >= -1 && globalDepth > out2level) return;	// waiting til we get back to a topic
		if (lastCommand != 'i') return;	// only visible on step in
	
		if (doing != OUTPUT_LEVEL && doing != RULE_LEVEL && !(debugTrace & RULE_TRACE)) return;	// output_level can end up here by doing ^refine or can step over or in a rule, ending at next rule 
		if (currentTopicID != lastRuleTopic) return;	// not in the topic issuing the command ... but REUSE can use any topic so it has special commands to set it up
		lastRuleTopic = currentTopicID;	// see rules of loop next
		action = 0;
		output = at; // show only rules AFTER this
		doing = OUTPUT_LEVEL;
		return;
	}
	else if (code == ENTER_CALL)
	{
		PushDebugLevel(doing,'c'); // note for when we leave this level
		if (debugTrace & (RULE_TRACE | TOPIC_TRACE)) return;
		if (out2level >= -1 && globalDepth > out2level) return;	// waiting til we get back to a topic
		if (lastCommand != 'i') return;	// only visible on step in
	
		if (doing != OUTPUT_LEVEL && doing != RULE_LEVEL && !(debugTrace & RULE_TRACE)) return;	// output_level can end up here by doing ^refine or can step over or in a rule, ending at next rule 
		if (currentTopicID != lastRuleTopic) return;	// not in the topic issuing the command ... but REUSE can use any topic so it has special commands to set it up
		lastRuleTopic = currentTopicID;	// see rules of loop next
		action = 0;
		call = lastOutput;
		output = at; // show only rules AFTER this
		doing = OUTPUT_LEVEL;
		return;
	}
	else if (code == ENTER_REFINE)
	{
		PushDebugLevel(doing,'y'); // note for when we leave this level
		if (debugTrace & (RULE_TRACE | TOPIC_TRACE)) return;
		if (out2level >= -1 && globalDepth > out2level) return;	// waiting til we get back to a topic
		if (lastCommand != 'i') return;	// only visible on step in

		if (doing != OUTPUT_LEVEL && doing != RULE_LEVEL && !(debugTrace & RULE_TRACE)) return;	// output_level can end up here by doing ^refine or can step over or in a rule, ending at next rule 
		if (currentTopicID != lastRuleTopic) return;	// not in the topic issuing the command ... but REUSE can use any topic so it has special commands to set it up
		lastRuleTopic = currentTopicID;
		doing = RULE_LEVEL;
		return;	// see rules of refine
	}
	else if (code == ENTER_RULE)
	{
		bool breakAtRule = false;
		if (transientBreakRule == currentRule) // 'g' transient breakpoint
		{
			breakAtRule = true;
			transientBreakRule = NULL;
		}
		if (code == ENTER_RULE) // BUG-- what about reuse access to rule output??
		{
			for (unsigned int i = 0; i < breakRuleIndex; ++i) 
			{
				if (breakRules[i] == currentRule) breakAtRule = true;
			}
		}
		if (breakAtRule) 	out2level = -2;

		PushDebugLevel(doing,'r'); // note for when we leave this level
		action = 0;
		// things we can be doing: START_LEVEL  TOPIC_LEVEL  RULE_LEVEL  OUTPUT_LEVEL  TOPIC_TRACE  5

		if (breakAtRule || debugTrace & (RULE_TRACE | RULE_MATCH)) {;}
		else if (out2level >= -1 && globalDepth > out2level) return;	// waiting til we get back to a topic
		else if (doing != OUTPUT_LEVEL && doing != RULE_LEVEL && !(debugTrace & RULE_TRACE)) return;	// output_level can end up here by doing ^refine or can step over or in a rule, ending at next rule 
		else if (lastRuleTopic && currentTopicID != lastRuleTopic) return;	// not in the topic issuing the command ... but REUSE can use any topic so it has special commands to set it up
		
		lastRuleTopic = currentTopicID;	// this level started a rule we cared about
		doing = RULE_LEVEL;
		if (output && *output == ENDUNIT && doing == OUTPUT_LEVEL) // was doing a prior output, which ended, close it out
		{
			output = NULL;
		}

		char pattern[MAX_WORD_SIZE];
		GetPattern(currentRule,NULL,pattern);
		unsigned int gap = 0;
		unsigned int wildcardSelector = 0;
		wildcardIndex = 0;  //   reset wildcard allocation on top-level pattern match
		unsigned int junk;
		blocked = true;
		bool match = (!*pattern) ? true : Match(pattern+2,0,0,'(',true,gap,wildcardSelector,junk,junk) != 0; 
		blocked = false;
		char matches = (match) ? '+' : '-';
		level = globalDepth;
		if (breakAtRule) 
		{
			IndentDebug();
			char label[MAX_WORD_SIZE];
			GetPattern(currentRule,label,NULL);
			Log(STDDEBUGLOG,"   break at %c %s.%d.%d (%s) %s\r\n",matches,GetTopicName(currentTopicID),TOPLEVELID(currentRuleID),REJOINDERID(currentRuleID),label,ShowRule(currentRule));
			doing = RULE_LEVEL;
		}
		else if (debugTrace &RULE_MATCH && !match && !(debugTrace & RULE_TRACE)) return;
		else 
		{
			IndentDebug();
			if (REJOINDERID(currentRuleID)) Log(STDDEBUGLOG,"   %c %s.%d.%d %s\r\n",matches,GetTopicName(currentTopicID),TOPLEVELID(currentRuleID),REJOINDERID(currentRuleID),ShowRule(currentRule));
			else Log(STDDEBUGLOG,"   %c %s.%d    %s\r\n",matches,GetTopicName(currentTopicID),TOPLEVELID(currentRuleID),ShowRule(currentRule));
		
			if (doing == OUTPUT_LEVEL) doing = RULE_LEVEL; // change mode on new rule
		}

		if (debugTrace & RULE_MATCH && match) debugTrace ^= RULE_MATCH;	 // stop on all matching rules
		else if (debugTrace & RULE_TRACE) return;
		else if (out2level >= -1 && globalDepth > out2level) return;	// waiting til we get back to a topic
		else if (doing != OUTPUT_LEVEL && doing != RULE_LEVEL && !(debugTrace & RULE_TRACE)) return;	// output_level can end up here by doing ^refine or can step over or in a rule, ending at next rule 
		else if (lastRuleTopic && currentTopicID != lastRuleTopic) return;	// not in the topic issuing the command ... but REUSE can use any topic so it has special commands to set it up

		output = 0; // no output yet
	}
	else if (code == PARSING_OUTPUT && doing == OUTPUT_LEVEL) // show the piece we are at IF its a call
	{
		if (!output) at = GetPattern(currentRule,NULL,NULL);
		else if (at < output) return;	// just execute it, we've displayed it
		char word[MAX_WORD_SIZE];
		output = ReadOutput(at,word);
		lastOutput = at;
		++action;
		IndentDebug();
		unsigned int n = strlen(GetTopicName(currentTopicID));
		while (--n) Log(STDDEBUGLOG," ");
		if (kind != 'i'  && kind != 'l' && kind != 'c') Log(STDDEBUGLOG,"      %d.%d act %d    %s\r\n",TOPLEVELID(currentRuleID),REJOINDERID(currentRuleID),action,word);
		else if (call)
		{
			char name[MAX_WORD_SIZE];
			ReadCompiledWord(call,name);
			Log(STDDEBUGLOG,"         %s act %d    %s\r\n",name,action,word);
		}
		else if (kind == 'l') Log(STDDEBUGLOG,"      LOOP act %d    %s\r\n",action,word);
		else if (kind == 'i') Log(STDDEBUGLOG,"      IF act %d    %s\r\n",action,word);
		else Log(STDDEBUGLOG,"         act %d    %s\r\n",action,word);
	}

	ExecuteResume(GetDebuggerResumeCommand());

#endif
}
