#include "common.h"

#define OVERFLOW_SAFETY_MARGIN 800

#ifndef USERFACTS 
#define USERFACTS 100
#endif
unsigned int userFactCount = USERFACTS;			// how many facts user may save in topic file

//   replies we have tried already
char chatbotSaid[MAX_USED+1][SAID_LIMIT+3];  //   tracks last n messages sent to user
char humanSaid[MAX_USED+1][SAID_LIMIT+3]; //   tracks last n messages sent by user in parallel with userSaid
unsigned int humanSaidIndex;
unsigned int chatbotSaidIndex;

static char* saveVersion = "jan1013";	// format of save file

int userFirstLine = 0;	// start volley of current conversation
uint64 setControl = 0;	// which sets should be saved with user

static char* base;

char computerID[ID_SIZE];
char computerIDwSpace[ID_SIZE];
char loginID[ID_SIZE];    //   user FILE name (lower case)
char loginName[ID_SIZE];    //   user typed name
char callerIP[ID_SIZE];

char timeturn15[100];
char timeturn0[20];
char timePrior[20];

void StartConversation(char* buffer)
{
	*readBuffer = 0;
	nextInput = buffer;
	userFirstLine = inputCount+1;
	OnceCode("$control_pre");
    *currentInput = 0;
	responseIndex = 0;
	ResetSentence();
	Reply();
}

void PartialLogin(char* caller,char* ip)
{
    //   make user name safe for file system
	char*  id = loginID;
	char* at = caller-1;
	char c;
	while ((c = *++at)) 
	{
		if (IsAlphaOrDigit(c)) *id++ = c;
		else if (c == '_' || c == ' ') *id++ = '_';
	}
	*id = 0;

	sprintf(logFilename,"USERS/%slog-%s.txt",GetUserPath(loginID),loginID); // user log goes here

	if (ip) strcpy(callerIP,ip);
	else *callerIP = 0;
}

void Login(char* caller,char* usee,char* ip) //   select the participants
{
	if (!stricmp(usee,"trace")) // enable tracing during login
	{
		trace = (unsigned int) -1;
		echo = true;
		*usee = 0;
	}
    if (*usee) MakeLowerCopy(computerID,usee);
	if (!*computerID) ReadComputerID(); //   we are defaulting the chatee
	if (!*computerID) ReportBug("No default bot?\r\n")

	//   for topic access validation
	*computerIDwSpace = ' ';
	MakeLowerCopy(computerIDwSpace+1,computerID);
	strcat(computerIDwSpace," ");

	if (ip && *ip) // maybe use ip in generating unique login
	{
		if (!stricmp(caller,"guest")) sprintf(caller,"guest%s",ip);
		else if (*caller == '.') sprintf(caller,"%s",ip);
	}
	char* ptr = caller-1;
	while (*++ptr) 
	{
		if (!IsAlphaOrDigit(*ptr) && *ptr != '-' ) *ptr = '_'; // require simple file names
	}

    //   prepare for chat
    PartialLogin(caller,ip);
 }

void ReadComputerID()
{
	strcpy(computerID,"anonymous");
	WORDP D = FindWord("defaultbot",0); // do we have a FACT with the default bot in it as verb
	if (D)
	{
		FACT* F = GetVerbHead(D);
		if (F) MakeLowerCopy(computerID,Meaning2Word(F->subject)->word);
	}
}

void ResetUser()
{
 	chatbotSaidIndex = humanSaidIndex = 0;
	setControl = 0;
	for (unsigned int i = 1; i < MAX_FIND_SETS; ++i) SET_FACTSET_COUNT(i,0);
}

static char* SafeLine(char* line) // erase cr/nl to keep reads safe
{
	char* start = line;
	char c;
    while ((c = *++line))
    {
        if (c == '\r' || c == '\n') *line = ' ';  
    }
	return start;
}

static char* WriteUserFacts(char* ptr)
{
    //   write out fact sets first, before destroying any transient facts
	sprintf(ptr,"%x #set flags\r\n",(unsigned int) setControl);
	ptr += strlen(ptr);
	unsigned int i;
    unsigned int count;
	for (i = 1; i < MAX_FIND_SETS; ++i) 
    {
		if (!(setControl & (uint64) (1 << i))) continue; // purely transient stuff

		//   remove dead references
		FACT** set = factSet[i];
        count = (ulong_t) set[0];
		unsigned int j;
        for (j = 1; j <= count; ++j)
		{
			FACT* F = set[j];
			if (!F || F->flags & FACTDEAD)
			{
				memmove(&set[j],&set[j+1],sizeof(FACT*) * (count - j));
				--count;
				--j;
			}
		}
        if (!count) continue;

		// save this set
		sprintf(ptr,"#set %d\r\n",i); 
		ptr += strlen(ptr);
        for (j = 1; j <= count; ++j)
		{
			WriteFact(factSet[i][j],false,ptr,false,true);
			ptr += strlen(ptr);
			if ((size_t)(ptr-base) > (size_t)(userCacheSize-1000)) break; // close to overflowing buffer
		}
    }
	strcpy(ptr,"#`end fact sets\r\n");
	ptr += strlen(ptr);

	// most recent facts, in order
	FACT* F = factFree+1;
	count = userFactCount;
	while (count && --F > factLocked) // backwards down to base system facts
	{
		if (!(F->flags & (FACTDEAD|FACTTRANSIENT))) --count; // we will write this
	}

	--F;  
 	while (++F <= factFree)  
	{
		if (!(F->flags & (FACTDEAD|FACTTRANSIENT))) 
		{
			WriteFact(F,true,ptr,false,true);
			ptr += strlen(ptr);
		}
	}
	//ClearUserFacts();
	strcpy(ptr,"#`end user facts\r\n");
	ptr += strlen(ptr);

	return ptr;
}

static bool ReadUserFacts()
{	
    //   read in fact sets
    char word[MAX_WORD_SIZE];
    *word = 0;
    ReadALine(readBuffer, 0); //   setControl
	ReadHex(readBuffer,setControl);
    while (ReadALine(readBuffer, 0)) 
    {
		if (*readBuffer == '#' && readBuffer[1] == '`') break; // end of sets to read
		char* ptr = ReadCompiledWord(readBuffer,word);
        unsigned int setid;
        ptr = ReadInt(ptr,setid); 
		SET_FACTSET_COUNT(setid,0);
		if (trace & TRACE_USER) Log(STDUSERLOG,"Facts[%d]\r\n",setid);
	    while (ReadALine(readBuffer, 0)) 
		{
			if (*readBuffer == '#') break;
			char* ptr = readBuffer;
			FACT* F = ReadFact(ptr);
			if (F) AddFact(setid,F);
			if (trace & TRACE_USER) TraceFact(F);
        }
		if (*readBuffer == '#' && readBuffer[1] == '`') break;
	}
	if (strcmp(readBuffer,"#`end fact sets")) 
	{
		ReportBug("Bad fact sets alignment\r\n")
		return false;
	}

	// read long-term user facts
	while (ReadALine(readBuffer, 0)) 
	{
		if (*readBuffer == '#' && readBuffer[1] == '`') break;
		char* data = readBuffer;
		if (*data == '(' && strchr(data,')')) ReadFact(data);
		else 
		{
			ReportBug("Bad user fact %s\r\n",readBuffer)
			return false;
		}
	}	
    if (strcmp(readBuffer,"#`end user facts")) 
	{
		ReportBug("Bad user facts alignment\r\n")
		return false;
	}

	return true;
}

static char* WriteRecentMessages(char* ptr)
{
    //   recent human inputs
	int start = humanSaidIndex - 20; 
	if (start < 0) start = 0;
	unsigned int i;
    for (i = start; i < (unsigned int)humanSaidIndex; ++i)  
	{
		size_t len = strlen(humanSaid[i]);
		if (len == 0) continue;
		if ((unsigned int)(ptr - base) >= (userCacheSize - OVERFLOW_SAFETY_MARGIN - len)) break;
		sprintf(ptr,"%s\r\n",SafeLine(humanSaid[i]));
		ptr += strlen(ptr);
	}
	strcpy(ptr,"#`end user\r\n");
	ptr += strlen(ptr);
	
	// recent chatbot outputs
 	start = chatbotSaidIndex - 20;
	if (start < 0) start = 0;
    for (i = start; i < (int)chatbotSaidIndex; ++i) 
	{
		size_t len = strlen(chatbotSaid[i]);
		if (len == 0) continue;
		if ((unsigned int)(ptr - base) >= (userCacheSize - OVERFLOW_SAFETY_MARGIN - len)) break;
		sprintf(ptr,"%s\r\n",SafeLine(chatbotSaid[i]));
		ptr += strlen(ptr);
	}
	strcpy(ptr,"#`end chatbot\r\n");
	ptr += strlen(ptr);

	return ptr;
}

static bool ReadRecentMessages()
{
    for (humanSaidIndex = 0; humanSaidIndex <= MAX_USED; ++humanSaidIndex) 
    {
        ReadALine(humanSaid[humanSaidIndex], 0);
		if (*humanSaid[humanSaidIndex] == '#' && humanSaid[humanSaidIndex][1] == '`') break; // #end
    }
	if (humanSaidIndex > MAX_USED || strcmp(humanSaid[humanSaidIndex],"#`end user"))  // failure to end right
	{
		humanSaidIndex = 0;
		chatbotSaidIndex = 0;
		ReportBug("bad humansaid")
		return false;
	}
	else *humanSaid[humanSaidIndex] = 0;

	for (chatbotSaidIndex = 0; chatbotSaidIndex <= MAX_USED; ++chatbotSaidIndex) 
    {
        ReadALine(chatbotSaid[chatbotSaidIndex], 0);
		if (*chatbotSaid[chatbotSaidIndex] == '#' && chatbotSaid[chatbotSaidIndex][1] == '`') break; // #end
    }
	if (chatbotSaidIndex > MAX_USED || strcmp(chatbotSaid[chatbotSaidIndex],"#`end chatbot")) // failure to end right
	{
		chatbotSaidIndex = 0;
		ReportBug("Bad message alignment\r\n")
		return false;
	}
	else *chatbotSaid[chatbotSaidIndex] = 0;

	return true;
}

char* WriteVariables(char* ptr)
{
    while (userVariableIndex)
    {
        WORDP D = userVariableList[--userVariableIndex];
        if (!(D->internalBits & VAR_CHANGED) ) continue; 
		if (*D->word != '$') ReportBug("Bad user variable to save %s\r\n",D->word)
        else if (D->word[1] !=  '$' && D->w.userValue) // transients not dumped, nor are NULL values
		{
			char* val = D->w.userValue;
			while ((val = strchr(val,'\n'))) *val = ' '; //  clean out newlines
			sprintf(ptr,"%s=%s\r\n",D->word,SafeLine(D->w.userValue));
			ptr += strlen(ptr);
		}
        D->w.userValue = NULL;
		RemoveInternalFlag(D,VAR_CHANGED);
    }
	strcpy(ptr,"#`end variables\r\n");
	ptr += strlen(ptr);
	
	return ptr;
}

static bool ReadVariables()
{
	while (ReadALine(readBuffer, 0)) //   user variables
	{
		if (*readBuffer != '$') break; // end of variables
        char* ptr = strchr(readBuffer,'=');
        *ptr = 0; // isolate user var name from rest of buffer
        SetUserVariable(readBuffer,ptr+1);
    }

	if (strcmp(readBuffer,"#`end variables")) 
	{
		ReportBug("Bad variable alignment\r\n")
		return false;
	}
	return true;
}

static char* GatherUserData(char* ptr,time_t curr)
{
	if (!timeturn15[1] && inputCount >= 15 && responseIndex) sprintf(timeturn15,"%lu-%d%s",(unsigned long)curr,responseData[0].topic,responseData[0].id); // delimit time of turn 15 and location...
	sprintf(ptr,"%s %s %s %s |\n",saveVersion,timeturn0,timePrior,timeturn15); 
	ptr += strlen(ptr);
	ptr = WriteTopicData(ptr);
	ptr = WriteVariables(ptr);
	ptr = WriteUserFacts(ptr);
	ptr = WriteRecentMessages(ptr);
	*ptr++ = 0;
		
	if ((unsigned int) (ptr - base) >= (userCacheSize - OVERFLOW_SAFETY_MARGIN) ) ReportBug("User File too big for buffer %ld vs %ld\r\n",(long int)(ptr-base),(long int)userCacheSize) // too big

	return ptr;
}

void WriteUserData(time_t curr)
{ 
	if (!numberOfTopics)  return; //   no topics ever loaded or we are not responding
	if (!userCacheCount) return;	// never save users - no history
	char* buffer = GetCacheBuffer(currentCache);
	if (!buffer) return;
	base = buffer;	// global current buffer for saving
	sprintf(buffer,"USERS/%stopic_%s_%s.txt",GetUserPath(loginID),loginID,computerID);

#ifndef DISCARDTESTING
	if (!server && !documentMode) CopyFile2File("TMP/backup.txt",buffer,false);	// backup for debugging
#endif

	char* ptr = GatherUserData(base+strlen(base)+1,curr);
	Cache(base,ptr-base);
}

void ReadUserData(char* ptr) // passed  buffer with file content (where feasible)
{	
	tokenControl = 0;
	ResetUser();
	char* buffer = ptr;
	size_t len = 0;
	char junk[MAX_WORD_SIZE];
	*junk = 0;
	strcpy(timePrior,"0");
	strcpy(timeturn15,"0");
	strcpy(timeturn0,"0");
	if (buffer && *buffer != 0) // readable data
	{ 
		len = strlen(buffer);
		if (len > 100) // supposed to just be user filename info. compensate
		{
			char junk[MAX_WORD_SIZE];
			char* p = ReadCompiledWord(buffer,junk);
			len = p - buffer - 1; 
		}
		userRecordSourceBuffer = buffer + len + 1;
		ReadALine(readBuffer,0);
		char* x = ReadCompiledWord(readBuffer,junk);
		x = ReadCompiledWord(x,timeturn0); // the start stamp id if there
		x = ReadCompiledWord(x,timePrior); // the prior stamp id if there
		ReadCompiledWord(x,timeturn15); // the timeturn id if there
		if (stricmp(junk,saveVersion)) *buffer = 0;// obsolete format
	}
    if (!buffer || !*buffer) 
	{
		ReadNewUser();
		strcpy(timeturn0,GetMyTime(time(0))); // startup time
	}
	else
	{
		if (!ReadTopicData()) return;
		if (!ReadVariables()) return;
		if (!ReadUserFacts()) return;
		if (!ReadRecentMessages()) return;
		randIndex = atoi(GetUserVariable("$randindex")) + (inputCount % MAXRAND);	// rand base assigned to user
	}
}

void ReadNewUser()
{
	ResetUser();
	ClearUserVariables();
	ClearUserFacts();
	ResetTopicSystem();
	userFirstLine = 1;
	tokenControl = 0;
	inputCount = 0;

	//   set his random seed
	unsigned int rand = (unsigned int) Hashit((unsigned char *) loginID,strlen(loginID));
	char word[MAX_WORD_SIZE];
	randIndex = rand & 4095;
    sprintf(word,"%d",randIndex);
	SetUserVariable("$randindex",word ); 
	strcpy(word,computerID);
	*word = GetUppercaseData(*word);
	SetUserVariable("$bot",word ); 
	SetUserVariable("$login",loginName);

	sprintf(readBuffer,"^%s",computerID);
	WORDP D = FindWord(readBuffer,0,LOWERCASE_LOOKUP);
	if (!D) return;

	char* buffer = AllocateBuffer();
	*buffer = 0;
	PushOutputBuffers();
	currentRuleOutputBase = currentOutputBase = buffer;
	ChangeDepth(2,"ReadNewUser");
	unsigned int result;
	DoFunction(D->word,buffer,buffer,result);
	PopOutputBuffers();
	ChangeDepth(-2,"ReadNewUser");
	FreeBuffer();

	char* value = GetUserVariable("$token");
	int64 v;
	ReadInt64(value,v);
	tokenControl = (*value) ? v : (DO_SUBSTITUTE_SYSTEM | DO_INTERJECTION_SPLITTING | DO_PROPERNAME_MERGE | DO_NUMBER_MERGE | DO_SPELLCHECK | DO_PARSE );
	inputRejoinderTopic = inputRejoinderRuleID = NO_REJOINDER; 
}
