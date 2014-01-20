#include "common.h"
char* version = "4.00";

clock_t startTimeInfo;							// start time of current volley

char revertBuffer[INPUT_BUFFER_SIZE];			// copy of user input so can :revert if desired
 
char* postProcessing = 0;						// copy of output generated during MAIN control. Postprocessing can prepend to it
unsigned int tokenCount;						// for document mode

char inBuffer[INPUT_BUFFER_SIZE];				// user input buffer - ptr to primaryInputBuffer
char outBuffer[MAX_BUFFER_SIZE];				// main user output buffer  BUG??? why not direct
char* readBuffer;								// main scratch reading buffer (files, etc)

unsigned int startSystem;						// time chatscript started

int always = 1;									// just something to stop the complaint about a loop based on a constant

#define RESPONSE_SEPARATOR '\''					// delineates each response item in log

// server data
#ifdef DISCARDSERVER
bool server = false;
#else
std::string interfaceKind = "0.0.0.0";
#ifdef WIN32
bool server = false;	// default is standalone on Windows
#elif IOS
bool server = true; // default is server on LINUX
#else
bool server = true; // default is server on LINUX
#endif
#endif
unsigned int port = 1024;						// server port

int prepareMode = NO_MODE;						// controls what processing is done in preparation NO_MODE, POS_MODE, PREPARE_MODE
bool noReact = false;

// :source:document data
bool documentMode = false;						// read input as a document not as chat
FILE* sourceFile = NULL;						// file to use for :source
unsigned int echoSource = 0;					// for :source, echo that input to nowhere, user, or log

// status of user input
unsigned int inputCount = 0;					// which user volley is this
bool moreToComeQuestion = false;				// is there a ? in later sentences
bool moreToCome = false;						// are there more sentences pending
uint64 tokenControl = 0;					// results from tokenization and prepare processing
char* nextInput;								// ptr to rest of users input after current sentence
char oldInputBuffer[INPUT_BUFFER_SIZE];			//  copy of the sentence we are processing
char currentInput[INPUT_BUFFER_SIZE];			// the sentence we are processing  BUG why both

// general display and flow controls
bool quitting = false;							// intending to exit chatbot
int systemReset = 0;							// intending to reload system - 1 (mild reset) 2 (full user reset)
bool autonumber = false;						// display number of volley to user
bool showWhy = false;							// show user rules generating his output
bool showTopic = false;							// show resulting topic on output
bool showTopics = false;						// show all relevant topics
bool showInput = false;							// Log internal input additions
bool all = false;								// generate all possible answers to input
bool regression = false;						// regression testing in progress
unsigned int trace = 0;							// current tracing flags
bool debugger = false;							// intercept things to the debugger
bool shortPos = false;							// display pos results as you go

int oktest = 0;									// auto response test

char respondLevel = 0;							// rejoinder level of a random choice block

int inputCounter = 0;							// protecting ^input from cycling
int totalCounter = 0;							// protecting ^input from cycling

static char userPrefix[MAX_WORD_SIZE];			// label prefix for user input
static char botPrefix[MAX_WORD_SIZE];			// label prefix for bot output

bool unusedRejoinder;							// inputRejoinder has been executed, blocking further calls to ^Rejoinder

// outputs generated
RESPONSE responseData[MAX_RESPONSE_SENTENCES+1];
unsigned char responseOrder[MAX_RESPONSE_SENTENCES+1];
unsigned int responseIndex;

unsigned int inputSentenceCount;				// which sentence of volley is this

///////////////////////////////////////////////
/// SYSTEM STARTUP AND SHUTDOWN
///////////////////////////////////////////////

void InitStandalone()
{
	startSystem =  clock()  / CLOCKS_PER_SEC;
	*currentFilename = 0;	//   no files are open (if logging bugs)
	tokenControl = 0;
	*computerID = 0; // default bot
}

void CreateSystem()
{
	char* os;

#ifdef WIN32
	os = "Windows";
#elif IOS
	os = "IOS";
#elif __MACH__
	os = "MACH";
#else
	os = "LINUX";
#endif
	printf("ChatScript Version %s  %ld bit %s  %s\r\n    ",version,(long int)(sizeof(char*) * 8),os,language);
#ifdef DISCARDSERVER 
	printf("Server disabled.  ");
#endif
#ifdef DISCARDSCRIPTCOMPILER
	printf("Script compiler disabled.  ");
#endif
#ifdef DISCARDTESTING
	printf("Testing disabled.  ");
#endif
#ifdef DISCARDDICTIONARYBUILD
	printf("Dictionary building disabled.  ");
#endif

	printf("\r\n");

	if (!stricmp(language,"ENGLISH"))
	{
		//  assign language code 
		P_GetAdjectiveBase = English_GetAdjectiveBase;   
		P_GetAdverbBase = English_GetAdverbBase;
		P_GetPastTense = English_GetPastTense;   
		P_GetPastParticiple = English_GetPastParticiple;   
		P_GetPresentParticiple = English_GetPresentParticiple;   
		P_GetThirdPerson = English_GetThirdPerson;   
		P_GetInfinitive = English_GetInfinitive;   
		P_GetSingularNoun = English_GetSingularNoun;   
		P_GetPluralNoun = English_GetPluralNoun;   
		P_TagIt = English_TagIt;
		P_SetSentenceTense = English_SetSentenceTense;
	}

	if (!buffers) // restart asking for new memory allocations
	{
		maxBufferSize = (maxBufferSize + 63);
		maxBufferSize &= 0xffffffC0; // force 64 bit align
		unsigned int total = maxBufferLimit * maxBufferSize;
		buffers = (char*) malloc(total); // have it around already for messages
		if (!buffers)
		{
			printf("cannot allocate buffer space");
			exit(1);
		}
		bufferIndex = 0;
		logmainbuffer = AllocateAlignedBuffer();
		readBuffer = AllocateBuffer();
		joinBuffer = AllocateBuffer();
		newBuffer = AllocateBuffer();
		baseBufferIndex = bufferIndex;
	}

	int old = trace; // in case trace turned on by default
	trace = 0;
	
	printf("Params:   dict:%ld fact:%ld text:%ldkb hash:%ld \r\n",(long int)maxDictEntries,(long int)maxFacts,(long int)(maxStringBytes/1000),(long int)maxHashBuckets);
	printf("          buffer:%dx%dkb cache:%dx%dkb userfacts:%d\r\n",(int)maxBufferLimit,(int)(maxBufferSize/1000),(int)userCacheCount,(int)(userCacheSize/1000),(int)userFactCount);

	InitScriptSystem();
	InitVariableSystem();
	ReloadSystem();			// builds layer1 facts and dictionary (from wordnet)
	LoadTopicSystem();		// dictionary reverts to wordnet zone
	InitSpellCheck();
	*currentFilename = 0;
	computerID[0] = 0;
	loginName[0] = loginID[0] = 0;
	*botPrefix = *userPrefix = 0;

	unsigned int factUsedMemKB = ( factFree-factBase) * sizeof(FACT) / 1000;
	unsigned int factFreeMemKB = ( factEnd-factFree) * sizeof(FACT) / 1000;
	unsigned int dictUsedMemKB = ( dictionaryFree-dictionaryBase) * sizeof(WORDENTRY) / 1000;
	// dictfree shares text space
	unsigned int textUsedMemKB = ( stringBase-stringFree)  / 1000;
	char* endDict = (char*)(dictionaryBase + maxDictEntries);
	unsigned int textFreeMemKB = ( stringFree- endDict) / 1000;
	unsigned int bufferMemKB = (maxBufferLimit * maxBufferSize) / 1000;
	
	unsigned int used =  factUsedMemKB + dictUsedMemKB + textUsedMemKB + bufferMemKB;
	used +=  (userTopicStoreSize + userTableSize) /1000;
	unsigned int free = factFreeMemKB + textFreeMemKB;

	unsigned int bytes = (tagRuleCount * MAX_TAG_FIELDS * sizeof(uint64)) / 1000;
	used += bytes;
	char buf2[MAX_WORD_SIZE];
	char buf1[MAX_WORD_SIZE];
	char buf[MAX_WORD_SIZE];
	strcpy(buf,StdIntOutput(factFree-factBase));
	strcpy(buf2,StdIntOutput(textFreeMemKB));
	unsigned int hole = 0;
	unsigned int maxdepth = 0;
	unsigned int count = 0;
	for (WORDP D = dictionaryBase+1; D < dictionaryFree; ++D) 
	{
		if (!D->word) ++hole; 
		else
		{
			unsigned int n = 1;
			WORDP X = D; 
			while (X != dictionaryBase)
			{
				++n;
				X = dictionaryBase + GETNEXTNODE(X);
			}  
			if (n > maxdepth) 
			{
				maxdepth = n;
				count = 1;
			}
			else if (n == maxdepth) ++count;
		}
	}

	printf("Used %dMB: dict %s (%dkb) hashdepth %d/%d fact %s (%dkb) text %dkb\r\n",
		used/1000,
		StdIntOutput(dictionaryFree-dictionaryBase), 
		dictUsedMemKB,maxdepth,count,
		buf,
		factUsedMemKB,
		textUsedMemKB);
	printf("           buffer (%dkb) cache (%dkb) POS: %d (%dkb)\r\n",
		bufferMemKB,
		(userTopicStoreSize + userTableSize)/1000,
		tagRuleCount,
		bytes);

	strcpy(buf,StdIntOutput(factEnd-factFree)); // unused facts
	strcpy(buf1,StdIntOutput(textFreeMemKB)); // unused text
	strcpy(buf2,StdIntOutput(free/1000));

	printf("Free %sMB: dict %s hash %d fact %s text %sKB \r\n",buf2,StdIntOutput(maxDictEntries-(dictionaryFree-dictionaryBase)),hole,buf,buf1);
	trace = old;
}

void ReloadSystem()
{//   reset the basic system through wordnet but before topics
	InitFacts(); 
	InitDictionary();
	// make sets for the part of speech data
	LoadDictionary();
	InitFunctionSystem();
#ifndef DISCARDTESTING
	InitCommandSystem();
#endif
	ExtendDictionary(); // store labels of concepts onto variables.
	DefineSystemVariables();
	ClearUserVariables();

	if (!ReadBinaryFacts(FopenStaticReadOnly(UseDictionaryFile("facts.bin")))) 
	{
		WORDP safeDict = dictionaryFree;
		ReadFacts(UseDictionaryFile("facts.txt"),0);
		if ( safeDict != dictionaryFree) myexit("dict changed on read of facts");
		WriteBinaryFacts(fopen(UseDictionaryFile("facts.bin"),"wb"),factBase);
	}
	char name[MAX_WORD_SIZE];
	sprintf(name,"%s/systemfacts.txt",livedata);
	ReadFacts(name,0); // part of wordnet, not level 0 build 
	ReadLiveData();  // considered part of basic system before a build
	WordnetLockDictionary();
}

void ReadParams()
{
	char name[100];
	sprintf(name,"%s.txt",language);
	FILE* in = FopenStaticReadOnly(name); // is there a param file for this language
	if (in)
	{
		char word[MAX_WORD_SIZE];
		char buffer[MAX_WORD_SIZE];
		while (fgets(buffer,MAX_WORD_SIZE,in)) // using fgets doesnt require buffers be set up yet
		{
			char* eol = strchr(buffer,'\r');
			if (eol) *eol = 0;
			eol = strchr(buffer,'\n');
			if (eol) *eol = 0;
			eol = strchr(buffer,'=');
			if (eol) *eol = 0;		
			char* ptr = ReadCompiledWord(buffer,word);
			if (!stricmp(word,"hash")) maxHashBuckets = atoi(ptr);
			if (!stricmp(word,"dict")) maxDictEntries = atoi(ptr);
			if (!stricmp(word,"fact")) maxFacts = atoi(ptr);
			if (!stricmp(word,"text")) maxStringBytes = atoi(ptr) * 1000;
			if (!stricmp(word,"buffer"))
			{
				maxBufferLimit = atoi(ptr);
				eol = strchr(ptr,'x');
				if (eol) maxBufferSize = atoi(eol+1) * 1000;
			}
			if (!stricmp(word,"cache"))
			{
				userCacheCount = atoi(ptr);
				eol = strchr(ptr,'x');
				if (eol) userCacheSize = atoi(eol+1) * 1000;
			}
			if (!stricmp(word,"userfacts")) userFactCount = atoi(ptr);
		}
		fclose(in);
	}
}

unsigned int InitSystem(int argc, char * argv[],char* unchangedPath, char* readablePath, char* writeablePath)
{ // this work mostly only happens on first startup, not on a restart
	InitFileSystem(unchangedPath,readablePath,writeablePath);

	// default language English
	P_GetAdjectiveBase = English_GetAdjectiveBase;   
	P_GetAdverbBase = English_GetAdverbBase;
	P_GetPastTense = English_GetPastTense;   
	P_GetPastParticiple = English_GetPastParticiple;   
	P_GetPresentParticiple = English_GetPresentParticiple;   
	P_GetThirdPerson = English_GetThirdPerson;   
	P_GetInfinitive = English_GetInfinitive;   
	P_GetSingularNoun = English_GetSingularNoun;   
	P_GetPluralNoun = English_GetPluralNoun;   
	P_TagIt = English_TagIt;
	P_SetSentenceTense = English_SetSentenceTense;
	strcpy(language,"ENGLISH");

	strcpy(livedata,"LIVEDATA"); // default directory for dynamic stuff
	
	// set default parameters from file if there
	FILE* in = FopenStaticReadOnly("language.txt");
	if (in)
	{
		char buffer[MAX_WORD_SIZE];
		if (fgets(buffer,MAX_WORD_SIZE,in)) 
		{
			char* eol = strchr(buffer,'\r');
			if (eol) *eol = 0;
			eol = strchr(buffer,'\n');
			if (eol) *eol = 0;
			strcpy(language,buffer);
		}
		fclose(in);
	}
	ReadParams();	// default params associated with language form (kept outside of createsystem)
	
    char *evsrv_arg = 0;

	for (int i = 1; i < argc; ++i)
	{
		if (!strnicmp(argv[i],"buffer=",7))  // number of large buffers available  8x80000
		{
			maxBufferLimit = atoi(argv[i]+7); 
			char* size = strchr(argv[i]+7,'x');
			if (size) maxBufferSize = atoi(size+1) *1000;
			if (maxBufferSize < OUTPUT_BUFFER_SIZE)
			{
				printf("Buffer cannot be less than OUTPUT_BUFFER_SIZE of %d\r\n",OUTPUT_BUFFER_SIZE);
				myexit("buffer size less than output buffer size");
			}
		}
	}

	// need buffers for things that run ahead like servers and such.
	maxBufferSize = (maxBufferSize + 63);
	maxBufferSize &= 0xffffffc0; // force 64 bit align on size  
	unsigned int total = maxBufferLimit * maxBufferSize;
	buffers = (char*) malloc(total); // have it around already for messages
	if (!buffers)
	{
		printf("cannot allocate buffer space");
		return 1;
	}
	bufferIndex = 0;
	logmainbuffer = AllocateAlignedBuffer();
	readBuffer = AllocateBuffer();
	joinBuffer = AllocateBuffer();
	newBuffer = AllocateBuffer();
	baseBufferIndex = bufferIndex;

	InitTextUtilities();

    logFilename[0] = 0; 
    sprintf(logFilename,"serverlog%d.txt",port);
    echo = true;

	bool portGiven = false;
	for (int i = 1; i < argc; ++i)
	{
		if (!stricmp(argv[i],"trace")) trace = (unsigned int) -1; 
		else if (!strnicmp(argv[i],"dir=",4)) 
		{
#ifdef WIN32
			if (!SetCurrentDirectory(argv[i]+4))
			{
				printf("unable to change to %s\r\n",argv[i]+4);
			}
#else
			chdir(argv[i]+5);
#endif
		}
		else if (!strnicmp(argv[i],"save=",5)) volleyLimit = atoi(argv[i]+5);

		// memory sizings
		else if (!strnicmp(argv[i],"hash=",5)) 
		{
			maxHashBuckets = atoi(argv[i]+5); // size of hash
			setMaxHashBuckets = true;
		}
		else if (!strnicmp(argv[i],"dict=",5)) maxDictEntries = atoi(argv[i]+5); // how many dict words allowed
		else if (!strnicmp(argv[i],"mark=",5)) maxRefSentence = atoi(argv[i]+5) * 2; // how many xrefs 
		else if (!strnicmp(argv[i],"fact=",5)) maxFacts = atoi(argv[i]+5);  // fact entries
		else if (!strnicmp(argv[i],"text=",5)) maxStringBytes = atoi(argv[i]+5) * 1000; // string bytes in pages
		else if (!strnicmp(argv[i],"cache=",6)) // value of 10x0 means never save user data
		{
			userCacheSize = atoi(argv[i]+6) * 1000;
			char* number = strchr(argv[i]+6,'x');
			if (number) userCacheCount = atoi(number+1);
		}
		else if (!strnicmp(argv[i],"userfacts=",10)) userFactCount = atoi(argv[i]+10); // how many user facts allowed
		else if (!strnicmp(argv[i],"livedata=",9) ) strcpy(livedata,argv[i]+9);
		else if (!strnicmp(argv[i],"language=",9) )
		{
			MakeUpperCopy(language,argv[i]+9);
		}
#ifndef DISCARDCLIENT
		else if (!strnicmp(argv[i],"client=",7)) // client=1.2.3.4:1024  or  client=localhost:1024
		{
			server = false;
			char buffer[MAX_WORD_SIZE];
			strcpy(serverIP,argv[i]+7);
		
			char* portVal = strchr(serverIP,':');
			if ( portVal)
			{
				*portVal = 0;
				port = atoi(portVal+1);
			}

			printf("\r\nEnter client user name: ");
			ReadALine(buffer,stdin);
			printf("\r\n");
			Client(buffer);
			myexit("client ended");
		}  
#endif
#ifndef DISCARDSERVER
        else if (!strnicmp(argv[i], "evsrv:", 6))  evsrv_arg = argv[i];
		else if (!stricmp(argv[i],"local")) server = false; // local standalone
		else if (!stricmp(argv[i],"nouserlog")) userLog = 0;
		else if (!stricmp(argv[i],"userlog")) userLog = 1;
		else if (!stricmp(argv[i],"noserverlog")) serverLog = 0;
		else if (!stricmp(argv[i],"serverlog")) serverLog = 1;
		else if (!strnicmp(argv[i],"interface=",10)) interfaceKind = string(argv[i]+10); // specify interface
		else if (!strnicmp(argv[i],"port=",5))  // be a server
		{
#ifndef EVSERVER
			if (volleyLimit == -1) volleyLimit = DEFAULT_VOLLEY_LIMIT;
#else
            volleyLimit = 0;
#endif
            port = atoi(argv[i]+5); // accept a port=
 #ifndef EVSERVER
            portGiven = true;
            GrabPort(); 
#ifdef WIN32
            PrepareServer();
#endif
#endif
		}
#endif	
	}
#ifndef DISCARDSERVER
#ifdef EVSERVER
    if (server && evsrv_init(interfaceKind, port, evsrv_arg) < 0)  exit(4);
#else
#ifndef WIN32
    if (!portGiven && server) GrabPort(); 
#endif
#endif
#endif
	// defaults where not specified
	if (server)
	{
		if (userLog == LOGGING_NOT_SET) userLog = 0;	// default OFF for user if unspecified
		if (serverLog == LOGGING_NOT_SET) serverLog = 1; // default ON for server if unspecified
	}
	else
	{
		if (userLog == LOGGING_NOT_SET) userLog = 1;	// default ON for nonserver if unspecified
		if (serverLog == LOGGING_NOT_SET) serverLog = 1; // default on for nonserver 
	}
#ifndef EVSERVER
	if (volleyLimit == -1) volleyLimit = 1; // write every volley
#else
    volleyLimit = 0;
#endif

	CreateSystem();

	for (int i = 1; i < argc; ++i)
	{
#ifndef DISCARDSCRIPTCOMPILER
		if (!strnicmp(argv[i],"build0=",7))
		{
			sprintf(logFilename,"USERS/build0_log.txt");
			FILE* in = FopenUTF8Write(logFilename);
			if (in) fclose(in);
			ReadTopicFiles(argv[i]+7,BUILD0,NO_SPELL);
 			myexit("build0 complete");
		}  
		if (!strnicmp(argv[i],"build1=",7))
		{
			sprintf(logFilename,"USERS/build1_log.txt");
			FILE* in = FopenUTF8Write(logFilename);
			if (in) fclose(in);
			ReadTopicFiles(argv[i]+7,BUILD1,NO_SPELL);
 			myexit("build1 complete");
		}  
#endif
		if (!stricmp(argv[i],"trace")) trace = (unsigned int) -1; // make trace work on login
	}

#ifndef EVSERVER
	if (server)  Log(SERVERLOG, "\r\n\r\n======== Began server %s on port %d at %s\r\n",version,port,GetTimeInfo());
#else
	if (server) Log(SERVERLOG, "\r\n\r\n======== Began EV server %s on port %d at %s\r\n",version,port,GetTimeInfo());
#endif

	echo = false;

	InitStandalone();
	return 0;
}

void CloseSystem()
{
	FreeAllUserCaches(); // user system
    CloseDictionary();	// dictionary system
    CloseFacts();		// fact system
	CloseBuffers();		// memory system
}


////////////////////////////////////////////////////////
/// INPUT PROCESSING
////////////////////////////////////////////////////////

void ProcessInputFile()
{
	#ifdef WIN32
	DWORD time = 0;
#endif
	while (ALWAYS)
    {
		if (oktest) // self test using OK or WHY as input
		{
			printf("%s\r\n    ",UTF2ExtendedAscii(outBuffer));
			strcpy(inBuffer,(oktest == OKTEST) ? (char*) "OK" : (char*)"why?");
		}
		else if (quitting) return; 
		else if (systemReset) 
		{
			printf("%s\r\n",UTF2ExtendedAscii(outBuffer));
			*computerID = 0;	// default bot
			*inBuffer = 0;		// restart conversation
		}
		else 
		{
			if ((!documentMode || *outBuffer)  && !silent) // if not in doc mode OR we had some output to say - silent when no response
			{
				// output bot response
				if (*botPrefix) printf("%s ",botPrefix);
				if (autonumber) printf("%d: ",inputCount);
			}
			if (showTopic)
			{
				GetActiveTopicName(tmpWord); // will show currently the most interesting topic
				printf("(%s) ",tmpWord);
			}
			if (!silent) printf("%s",UTF2ExtendedAscii(outBuffer));
			if ((!documentMode || *outBuffer) && !silent) printf("\r\n");

			if (showWhy) printf("\r\n"); // line to separate each chunk

			//output user prompt
			if (documentMode || silent) {;} // no prompt in document mode
			else if (*userPrefix) printf("%s ",userPrefix);
			else printf("   >");

			*inBuffer = ' '; // distinguish EMPTY from conversation start 
			
			if (documentMode)
			{
				if (!ReadDocument(inBuffer,sourceFile)) break;
			}
			else if (!ReadALine(inBuffer+1,sourceFile,INPUT_BUFFER_SIZE-1)) break; // end of input

			// reading from file
			if (sourceFile != stdin)
			{
				char word[MAX_WORD_SIZE];
				ReadCompiledWord(inBuffer,word);
				if (!stricmp(word,"#exit") || !stricmp(word,"#stop") || !stricmp(word,"#end")|| !stricmp(word,":exit")  || !stricmp(word,":stop") || !stricmp(word,":end")) break;
				if ((!*word && !documentMode) || *word == '#') continue;
				if (echoSource == SOURCE_ECHO_USER) printf("< %s\r\n",inBuffer);
			}
		}
		PerformChat(loginID,computerID,inBuffer,NULL,outBuffer); // no ip
    }
	fclose(sourceFile);  // to get here, must have been a source file that ended
}

void MainLoop() //   local machine loop
{
    printf("\r\nEnter user name: ");
    ReadALine(inBuffer,stdin);
	printf("\r\n");
	PerformChat(inBuffer,computerID,"",NULL,outBuffer); // unknown bot, no input,no ip
	
	sourceFile = stdin; // keep up the conversation indefinitely
retry:
	ProcessInputFile();
	sourceFile = stdin;
	*inBuffer = 0;
	inBuffer[1] = 0;
	if (!quitting) goto retry;
}

void ResetToPreUser() // prepare for multiple sentences being processed - data lingers over multiple sentences
{
	// limitation on how many sentences we can internally resupply
	globalDepth = 0;
	inputCounter = 0;
	totalCounter = 0;
	itAssigned = theyAssigned = 0;

	//  Revert to pre user-loaded state, fresh for a new user
	ReturnToFreeze();  // dict/fact/strings reverted
	ReestablishBotVariables();
	ResetTopicSystem();
	ResetUser();
	ResetFunctionSystem();
	ResetTokenSystem();

 	//   ordinary locals
	inputSentenceCount = 0;
	globalDepth = 0;
}

void ResetSentence() // read for next sentence to process from raw system level control only
{
	ResetFunctionSystem();
	respondLevel = 0; 
	currentRuleID = NO_REJOINDER;	//   current rule id
 	currentRule = 0;				//   current rule being procesed
	currentRuleTopic = -1;
	ruleErased = false;	
}

void FinishVolley(char* incoming,char* output,char* postvalue)
{
	// massage output going to user
	if (!documentMode)
	{
		strcpy(output,ConcatResult());
		postProcessing = output;
		++outputNest; // this is not generating new output
		OnceCode("$control_post",postvalue);
		--outputNest;
		postProcessing = 0;
		debugger = false; // must be over now

		// nothing he said generated a useful output
		if (!*output && !prepareMode && incoming && *incoming && *incoming != ':' && !regression && !all) 
		{
		}
	
		if (!server) // refresh prompts from a loaded bot since mainloop happens before user is loaded
		{
			WORDP dBotPrefix = FindWord("$botprompt");
			strcpy(botPrefix,(dBotPrefix && dBotPrefix->w.userValue) ? dBotPrefix->w.userValue : (char*)"");
			WORDP dUserPrefix = FindWord("$userprompt");
			strcpy(userPrefix,(dUserPrefix && dUserPrefix->w.userValue) ? dUserPrefix->w.userValue : (char*)"");
		}
		time_t curr = time(0);
		if (regression) curr = 44444444; 
		char* when = GetMyTime(curr);
		if (*incoming) strcpy(timePrior,GetMyTime(curr)); // when we did the last volley
		WriteUserData(curr); 
		// Log the results
		if (userLog)
		{
			GetActiveTopicName(tmpWord); // will show currently the most interesting topic
			if (*incoming && regression) Log(STDUSERLOG,"(%s) %s ==> %s ",tmpWord,TrimSpaces(incoming),TrimSpaces(output)); // simpler format for diff
			else if (!*incoming) Log(STDUSERLOG,"Start: user:%s bot:%s ip:%s rand:%d (%s) %d ==> %s  When:%s Version:%s Build0:%s Build1:%s 0:%s F:%s P:%s ",loginID,computerID,callerIP,randIndex,tmpWord,inputCount,output,when,version,timeStamp0,timeStamp1,timeturn0,timeturn15,timePrior); // conversation start
			else 
			{
				Log(STDUSERLOG,"Respond: user:%s bot:%s ip:%s (%s) %d  %s ==> %s  When:%s ",loginID,computerID,callerIP,tmpWord,inputCount,incoming,output,when);  // normal volley
				if (inputCount == 15 && timeturn15[1]) Log(STDUSERLOG," F:%s ",timeturn15);
			}
			if (!regression && responseIndex)
			{
				Log(STDUSERLOG,"Why: ");
				for (unsigned int i = 0; i < responseIndex; ++i) 
				{
					unsigned int order = responseOrder[i];
					if (!responseData[order].topic) continue;
					char label[MAX_WORD_SIZE];
					unsigned int id = atoi(responseData[order].id+1);
					char* dot = strchr(responseData[order].id+1,'.');
					id |= MAKE_REJOINDERID(atoi(dot+1));
					char* rule = GetRule(responseData[order].topic,id);
					GetLabel(rule,label);
					Log(STDUSERLOG,"%s%s %s ",GetTopicName(responseData[order].topic),responseData[order].id,label);
				}
			}
				Log(STDUSERLOG,"\r\n");
			if (shortPos) 
			{
				Log(STDUSERLOG,"%s",DumpAnalysis(1,wordCount,posValues,"Tagged POS",false,true));
				Log(STDUSERLOG,"\r\n");
			}
		}

		// now convert output separators between rule outputs to space from ' for user display result (log has ', user sees nothing extra) 
		char* sep = output;
		while ((sep = strchr(sep,RESPONSE_SEPARATOR))) 
		{
			if (*(sep-1) == ' ') memmove(sep,sep+1,strlen(sep));
			else *sep = ' ';
		}
	}
	else *output = 0;
	if (!documentMode) 
	{
		ShowStats(false);
		ResetToPreUser(); // back to empty state before any user
	}
}

void PerformChat(char* user, char* usee, char* incoming,char* ip,char* output)
{ //   primary entrypoint for chatbot -- null incoming treated as conversation start.
	if (!documentMode)
	{
		docTime = ElapsedMilliseconds();
		tokenCount = 0;
	}

    output[0] = 0;
	output[1] = 0;
	*currentFilename = 0;
	bufferIndex = baseBufferIndex; // return to default basic buffers used so far, in case of crash and recovery

    //   case insensitive logins
    static char caller[MAX_WORD_SIZE];
	static char callee[MAX_WORD_SIZE];
	callee[0] = 0;
    caller[0] = 0;
	MakeLowerCopy(callee, usee);
    if (user) 
	{
		strcpy(caller,user);
		//   allowed to name callee as part of caller name
		char* separator = strchr(caller,':');
		if (separator) *separator = 0;
		if (separator && !*usee) MakeLowerCopy(callee,separator+1); // override the bot
		strcpy(loginName,caller); // original name as he typed it

		MakeLowerCopy(caller,caller);
	}
	bool hadIncoming = *incoming != 0 || documentMode;
	while (incoming && *incoming == ' ') ++incoming;	// skip opening blanks

	if (incoming[0] && incoming[1] == '#' && incoming[2] == '!') // naming bot to talk to- and whether to initiate or not - e.g. #!Rosette#my message
	{
		char* next = strchr(incoming+3,'#');
		if (next)
		{
			*next = 0;
			MakeLowerCopy(callee,incoming+3); // override the bot name (including future defaults if such)
			strcpy(incoming+1,next+1);	// the actual message.
			if (!*incoming) incoming = 0;	// login message
		}
	}

    if (trace & TRACE_MATCH) Log(STDUSERLOG,"Incoming data- %s | %s | %s\r\n",caller, (*callee) ? callee : " ", (incoming) ? incoming : "");
 
	bool fakeContinue = false;
	if (callee[0] == '&') // allow to hook onto existing conversation w/o new start
	{
		*callee = 0;
		fakeContinue = true;
	}
    Login(caller,callee,ip); //   get the participants names

	if (systemReset) // drop old user
	{
		if (systemReset == 2) ReadNewUser(); 
		else
		{
			char* ptr = GetFileRead(loginID,computerID); // read user file, if any, or get it from cache
			ReadUserData(ptr);		//   now bring in user state
		}
		systemReset = 0;
	}
	else if (!documentMode) 
	{
		// preserve file status reference across this read use of ReadALine
		int BOMvalue = -1; // get prior value
		char oldc;
		int oldCurrentLine;	
		BOMAccess(BOMvalue, oldc,oldCurrentLine); // copy out prior file access and reinit user file access

		char* ptr = GetFileRead(loginID,computerID); // read user file, if any, or get it from cache
		ReadUserData(ptr);		//   now bring in user state
		BOMAccess(BOMvalue, oldc,oldCurrentLine); // restore old BOM values
	}
	// else documentMode
	if (fakeContinue) return;

	if (!ip) ip = "";

	unsigned int ok = true;
    if (!*incoming && !hadIncoming) StartConversation(output); //   begin a conversation
    else  
	{
		if (*incoming == '\r' || *incoming == '\n' || !*incoming) // incoming is blank, make it so
		{
			*incoming = ' ';
			incoming[1] = 0;
		}

		static char copy[INPUT_BUFFER_SIZE];
		strcpy(copy,incoming); // so input trace not contaminated by input revisions
		ok = ProcessInput(copy,output);
	}
	if (!ok) return; // command processed

	FinishVolley(incoming,output,NULL);
}

int Reply() 
{
	withinLoop = 0;
	ResetOutput();
	ResetTopicReply();

	if (trace) 
	{
		Log(STDUSERLOG,"\r\n\r\nReply input: ");
		for (unsigned int i = 1; i <= wordCount; ++i) Log(STDUSERLOG,"%s ",wordStarts[i]);
		Log(STDUSERLOG,"\r\n  Pending topics: %s\r\n",ShowPendingTopics());
	}
	unsigned int result = 0;
	int pushed = PushTopic(FindTopicIDByName(GetUserVariable("$control_main")));
	if (pushed < 0) return FAILRULE_BIT;
	if (debugger) DebugHeader("Main");
	AllocateOutputBuffer();
	result = PerformTopic(0,currentOutputBase); //   allow control to vary
	FreeOutputBuffer();
	currentRuleOutputBase = currentOutputBase = NULL;
	if (pushed) PopTopic();
	if (globalDepth) ReportBug("Main code global depth not 0");
	return result;
}

unsigned int ProcessInput(char* input,char* output)
{
	startTimeInfo =  ElapsedMilliseconds();
	lastInputSubstitution[0] = 0;

	//   precautionary adjustments
	char* p = input;
	while ((p = strchr(p,ENDUNIT))) *p = '\'';
  
	char *buffer = TrimSpaces(input);
	size_t len = strlen(buffer);
	if (!len)
	{
		*buffer = ' ';
		buffer[1] = 0;
	}

	if (len >= MAX_MESSAGE) buffer[MAX_MESSAGE-1] = 0; 
#ifndef DISCARDTESTING
	if (*buffer == ':' && IsAlpha(buffer[1]) && len > 4) // avoid reacting to :P and other texting idioms
	{
		int commanded = COMMANDED;
		Log(STDUSERLOG,"Command: %s\r\n",buffer);
		commanded = DoCommand(buffer,output);

		if (!strnicmp(buffer,":debug",6))
		{
			memmove(input,buffer+7,strlen(buffer));
			if (!stricmp(input,":revert") || !stricmp(input,"revert")) strcpy(input,revertBuffer); 
			buffer = SkipWhitespace(input);
		}
		else if (!strnicmp(buffer,":retry",6) )
		{
			memmove(buffer,buffer+6,strlen(buffer+6) + 1);
			char* more = SkipWhitespace(buffer);
			if (!*more)
			{
				strcpy(input,revertBuffer); 
				buffer = SkipWhitespace(input);
			}
		}
		else if (commanded == BEGINANEW)  
		{ 
			ResetToPreUser();	// back to empty state before user
			StartConversation(buffer);
			return 2; 
		}
		else if (commanded == COMMANDED ) 
		{
			return false; 
		}
		else if (commanded == OUTPUTASGIVEN) return true; 
	}
#endif
	++inputCount;
	if (trace) Log(STDUSERLOG,"\r\n\r\nInput: %d to %s: %s \r\n",inputCount,computerID,input);
	strcpy(currentInput,input);	//   this is what we respond to, literally.

	if (!strncmp(buffer,"... ",4)) buffer += 4;	// a marker from ^input
	else if (!strncmp(buffer,". ",2)) buffer += 2;	//   maybe separator after ? or !

	//   process input now
	char prepassTopic[MAX_WORD_SIZE];
	strcpy(prepassTopic,GetUserVariable("$prepass"));
	nextInput = buffer;

	if (!documentMode) 
	{
		responseIndex = 0;	// clear out data (having left time for :why to work)
		AddHumanUsed(buffer);
		OnceCode("$control_pre");
		if (responseIndex != 0) ReportBug("Not expecting PRE to generate a response")
	}

 	int loopcount = 0;
	while (nextInput && *nextInput && loopcount < 50) // loop on user input sentences
	{
		topicIndex = currentTopicID = 0; // precaution
		DoSentence(prepassTopic); // sets nextInput to next piece
		++inputSentenceCount;
	}
	if (++loopcount > 50) ReportBug("loopcount excess %d %s",loopcount,nextInput)
  
#ifndef DISCARDTESTING
	if (!server) strcpy(revertBuffer,currentInput); // for a possible revert
#endif
	return true;
}

bool PrepassSentence(char* prepassTopic)
{
	if (prepassTopic && *prepassTopic)
	{
		unsigned int topic = FindTopicIDByName(prepassTopic);
		if (topic && !(GetTopicFlags(topic) & TOPIC_BLOCKED))  
		{
			int pushed =  PushTopic(topic); 
			if (pushed < 0) return false;
			ChangeDepth(1,"PrepassSentence");
			PushOutputBuffers();
			if (debugger) DebugHeader("SentencePrePass");
			AllocateOutputBuffer();
			unsigned int result = PerformTopic(0,currentOutputBase); 
			FreeOutputBuffer();
			PopOutputBuffers();
			ChangeDepth(-1,"PrepassSentence");
			if (pushed) PopTopic();
			//   subtopic ending is not a failure.
			if (result & (ENDSENTENCE_BIT | FAILSENTENCE_BIT| ENDINPUT_BIT)) 
			{
				if (result & ENDINPUT_BIT) nextInput = "";
				--inputSentenceCount; // abort this input
				return true; 
			}
			if (prepareMode == PREPARE_MODE || trace || prepareMode == POS_MODE || (prepareMode == PENN_MODE && trace & TRACE_POS)) DumpTokenFlags(); // show revised from prepass
		}
	}
	return false;
}

void DoSentence(char* prepassTopic)
{
	char input[INPUT_BUFFER_SIZE];  // complete input we received
	strcpy(input,nextInput);

	if (all) Log(STDUSERLOG,"\r\n\r\nInput: %s\r\n",input);
	unsigned int oldtrace = trace;
	bool oldecho = echo;
	if (prepareMode == PREPARE_MODE)  echo = true;

    //    generate reply by lookup first
	bool retried = false;
retry:  
	char* start = nextInput; // where we read from
	ResetSentence();			//   ready to accept interjection data from raw system control level
 	PrepareSentence(nextInput,true,true); // user input.. sets nextinput up to continue
	if (PrepassSentence(prepassTopic))  // user input revise and resubmit?  -- COULD generate output and set rejoinders
	{
		trace = oldtrace;
		echo = oldecho;
		return;
	}
	if (prepareMode == PREPARE_MODE) // just do prep work, no actual reply
	{
		trace = oldtrace;
		echo = oldecho;
		return; 
	}
	 tokenCount += wordCount;

    if (!wordCount && responseIndex != 0) return; // nothing here and have an answer already. ignore this

	if (showTopics)
	{
		bool oldEcho = echo;
		echo = true;
		impliedSet = 0;
		KeywordTopicsCode(NULL);
		for (unsigned int i = 1; i <=  FACTSET_COUNT(0); ++i)
		{
			FACT* F = factSet[0][i];
			WORDP D = Meaning2Word(F->subject);
			WORDP N = Meaning2Word(F->object);
			unsigned int topic = FindTopicIDByName(D->word);
			char* name = GetTopicName(topic);
			Log(STDUSERLOG,"%s (%s) : ",name,N->word);
			//   look at references for this topic
			int start = -1;
			while (GetIthSpot(D,++start)) // find matches in sentence
			{
				// value of match of this topic in this sentence
				for (unsigned int k = positionStart; k <= positionEnd; ++k) 
				{
					if (k != positionStart) Log(STDUSERLOG,"_");
					Log(STDUSERLOG,"%s",wordStarts[k]);
				}
				Log(STDUSERLOG," ");
			}
			Log(STDUSERLOG,"\r\n");
		}
		impliedSet = ALREADY_HANDLED;
		echo = oldEcho;
	}

	if (noReact) return;
	int result =  Reply();
	if (result & RETRYSENTENCE_BIT && !retried) 
	{
		retried = true;	 // allow ONE retry -- issues with this
		--inputSentenceCount;
		char* buf = AllocateBuffer();
		strcpy(buf,nextInput);	// protect future input
		strcpy(start,oldInputBuffer); // copy back current input
		strcat(start," "); 
		strcat(start,buf); // add back future input
		nextInput = start; // retry old input
		FreeBuffer();
		goto retry; // try input again -- maybe we changed token controls...
	}
	if (result & FAILSENTENCE_BIT)  --inputSentenceCount;
	if (result == ENDINPUT_BIT) nextInput = ""; // end future input
}

void OnceCode(const char* which,char* function) //   run before doing any of his input
{
	withinLoop = 0;
	topicIndex = currentTopicID = 0; 
	char* name = (!function || !*function) ? GetUserVariable(which) : function;
	int topic = FindTopicIDByName(name);
	if (!topic) return;
	if (trace & TRACE_MATCH) 
	{
		if (!stricmp(which,"$control_pre")) 
		{
			Log(STDUSERLOG,"\r\nPrePass\r\n");
			if (trace & TRACE_VARIABLE) DumpVariables();
		}
		else 
		{
			Log(STDUSERLOG,"\r\n\r\nPostPass\r\n");
			Log(STDUSERLOG,"Pending topics: %s\r\n",ShowPendingTopics());
		}
	}

	int pushed = PushTopic(topic);
	if (pushed < 0) return;
	if (debugger) DebugHeader(!stricmp(which,"$control_pre") ? (char*) "PrePass" : (char*)"PostPass");
	ruleErased = false;	
	AllocateOutputBuffer();
	PerformTopic(GAMBIT,currentOutputBase);
	FreeOutputBuffer();
	currentRuleOutputBase = currentOutputBase =  NULL;

	if (pushed) PopTopic();
	if (topicIndex) ReportBug("topics still stacked")
	if (globalDepth) ReportBug("Once code %s global depth not 0",name);
	topicIndex = currentTopicID = 0; // precaution
}
		
void AddHumanUsed(const char* reply)
{
	if (humanSaidIndex >= MAX_USED) humanSaidIndex = 0; // chop back //  overflow is unlikely but if he inputs >10 sentences at once, could
    unsigned int i = humanSaidIndex++;
    *humanSaid[i] = ' ';
	size_t len = strlen(reply);
	if (len >= SAID_LIMIT) // too long to save in user file
	{
		strncpy(humanSaid[i]+1,reply,SAID_LIMIT); 
		humanSaid[i][SAID_LIMIT] = 0; 
	}
	else strcpy(humanSaid[i]+1,reply); 
}

void AddBotUsed(const char* reply,unsigned int len)
{
	if (chatbotSaidIndex >= MAX_USED) chatbotSaidIndex = 0; // overflow is unlikely but if he could  input >10 sentences at once
	unsigned int i = chatbotSaidIndex++;
    *chatbotSaid[i] = ' ';
	if (len >= SAID_LIMIT) // too long to save in user file
	{
		strncpy(chatbotSaid[i]+1,reply,SAID_LIMIT); 
		chatbotSaid[i][SAID_LIMIT] = 0; 
	}
	else strcpy(chatbotSaid[i]+1,reply); 
}

bool HasAlreadySaid(char* msg)
{
    if (!*msg) return true; 
    if (Repeatable(currentRule) || GetTopicFlags(currentTopicID) & TOPIC_REPEAT) return false;
	msg = TrimSpaces(msg);
	size_t actual = strlen(msg);
    for (unsigned int i = 0; i < chatbotSaidIndex; ++i) // said in previous recent  volleys
    {
		size_t len = strlen(chatbotSaid[i]+1);
		if (actual > (SAID_LIMIT-3)) actual = len;
        if (!strnicmp(msg,chatbotSaid[i]+1,actual+1)) return true; // actual sentence is indented one (flag for end reads in column 0)
    }
	for (unsigned int i = 0; i  < responseIndex; ++i) // already said this turn?
	{
		size_t len = strlen(responseData[i].response);
		if (actual > (SAID_LIMIT-3)) actual = len;
         if (!strnicmp(msg,responseData[i].response,actual+1)) return true; 
	}
    return false;
}

static void SaveResponse(char* msg)
{
    strcpy(responseData[responseIndex].response,msg); // the response
    responseData[responseIndex].responseInputIndex = inputSentenceCount; // which sentence of the input was this in response to
	responseData[responseIndex].topic = currentTopicID; // what topic wrote this
	sprintf(responseData[responseIndex].id,".%d.%d",TOPLEVELID(currentRuleID),REJOINDERID(currentRuleID));
	if (currentReuseID != -1)
	{
		size_t len = strlen(responseData[responseIndex].id);
		sprintf(responseData[responseIndex].id + len,".%d.%d",TOPLEVELID(currentReuseID),REJOINDERID(currentReuseID));
	}
	responseOrder[responseIndex] = (unsigned char)responseIndex;
    
	unsigned int id = atoi(responseData[responseIndex].id+1);
	char* dot = strchr(responseData[responseIndex].id+1,'.');
	id |= MAKE_REJOINDERID(atoi(dot+1));
		
	responseIndex++;
	if (responseIndex == MAX_RESPONSE_SENTENCES) --responseIndex;

	// now mark rule as used up if we can since it generated text
	SetErase(true); // top level rules can erase whenever they say something
	
	if (showWhy)
	{
		bool oldecho = echo;
		echo = true;
		Log(STDUSERLOG,"=> %s %s%d.%d  %s\r\n",(!UsableRule(currentTopicID,currentRuleID)) ? "-" : "", GetTopicName(currentTopicID,false),TOPLEVELID(currentRuleID),REJOINDERID(currentRuleID),ShowRule(currentRule));
		echo = oldecho;
	}
}

bool AddResponse(char* msg)
{
	if (!msg || !*msg) return false;
	char* buffer = AllocateBuffer();
    size_t len = strlen(msg);
 	if (len > OUTPUT_BUFFER_SIZE)
	{
		ReportBug("response too big %s",msg)
		strcpy(msg+OUTPUT_BUFFER_SIZE-5,"..."); //   prevent trouble
		len = strlen(msg);
	}

    strcpy(buffer,msg);
	Convert2Underscores(buffer,false); // leave new lines alone
	Convert2Blanks(buffer);	// dont keep underscores in output regardless
	*buffer = GetUppercaseData(*buffer); 

	//   remove spaces before commas (geofacts often have them in city_,_state)
	char* ptr = buffer;
	while (ptr && *ptr)
	{
		char* comma = strchr(ptr,',');
		if (comma && comma != buffer )
		{
			if (*--comma == ' ') memmove(comma,comma+1,strlen(comma));
			ptr = comma+2;
		}
		else if (comma) ptr = comma+1;
		else ptr = 0;
	}

    if (all || HasAlreadySaid(buffer) ) // dont really do this, either because it is a repeat or because we want to see all possible answers
    {
		if (all)
		{
			bool oldecho = echo;
			echo = true;
			Log(STDUSERLOG,"Choice: %s (%s %s)\r\n",buffer,GetTopicName(currentTopicID,false),ShowRule(currentRule));
			echo = oldecho;
		}
        else if (trace) Log(STDUSERLOG,"Rejected: %s already said\r\n",buffer);
        memset(msg,0,len+1); //   kill partial output
		FreeBuffer();
        return false;
    }
    if (trace & TRACE_OUTPUT) Log(STDUSERTABLOG,"Message: %s\r\n",buffer);
	if (debugger) 
	{
		int count = globalDepth;
		while (count--) printf(" ");
		printf("Message: %s\r\n",buffer);
	}

    SaveResponse(buffer);
    memset(msg,0,len+1); // erase all of original message, +  1 extra as leading space
	FreeBuffer();
    return true;
}

char* ConcatResult()
{
    static char  result[OUTPUT_BUFFER_SIZE];
    result[0] = 0;
	for (unsigned int i = 0; i < responseIndex; ++i) 
    {
		unsigned int order = responseOrder[i];
        if (responseData[order].response[0]) 
		{
			char* reply = responseData[order].response;
			size_t len = strlen(reply);
			if (len >= OUTPUT_BUFFER_SIZE)
			{
				ReportBug("overly long reply %s",reply)
				reply[OUTPUT_BUFFER_SIZE-50] = 0;
			}
			AddBotUsed(reply,len);
		}
    }

	//   now join up all the responses as one output into result
	unsigned int size = 0;
	char* starts = AllocateBuffer();
	char* copy = AllocateBuffer();
	for (unsigned int i = 0; i < responseIndex; ++i) 
    {
		unsigned int order = responseOrder[i];
        if (!responseData[order].response[0]) continue;
		char* piece = responseData[order].response;
		size_t len = strlen(piece);
		if ((len + size) >= OUTPUT_BUFFER_SIZE) break;
		if (*result) 
		{
			result[size++] = RESPONSE_SEPARATOR; // add separating item from last unit for log detection
			result[size] = 0;
		}

		// each sentence becomes a transient fact
		char* start = piece;
		char* ptr = piece;

		while (ptr && *ptr) // find sentences of response
		{
			start = ptr;
			char* old = ptr;
			unsigned int count;
			ptr = Tokenize(ptr,count,(char**) starts,false,true);   //   only used to locate end of sentence but can also affect tokenFlags (no longer care)
			char c = *ptr; // is there another sentence after this?
			char d = 0;
			if (c) 
			{
				d = *(ptr-1); // save to restore later
				*(ptr-1) = 0; // kill the separator temporarily
			}

			//   save sentences as facts
			char* out = copy;
			char* at = old-1;
			while (*++at) // copy message and alter some stuff like space or cr lf
			{
				if (*at == '\r' || *at == '\n') {;}
				else *out++ = (*at == ' ') ? '_' : *at;  // fact words want no spaces
			}
			*out = 0;
			if (*copy) // we did copy something, make a fact of it
			{
				char name[MAX_WORD_SIZE];
				sprintf(name,"%s.%s",GetTopicName(responseData[order].topic),responseData[order].id);
				CreateFact(MakeMeaning(StoreWord(copy)),Mchatoutput,MakeMeaning(StoreWord(name)),FACTTRANSIENT);
			}

			// now add data to result to user
			if (c) *(ptr-1) = d; // restore
			len = ptr - start;
			strncpy(result+size,start,len); // no leading or trailing blanks except between sentences.
			size += len;
			result[size] = 0;
		}	
	}

	FreeBuffer();
	FreeBuffer();
    return result;
}

void PrepareSentence(char* input,bool mark,bool user) // set currentInput and nextInput
{
	char* original[MAX_SENTENCE_LENGTH];
	unsigned int mytrace = trace;
	if (prepareMode == PREPARE_MODE) mytrace = 0;
  	ClearWhereInSentence(false);
	memset(unmarked,0,MAX_SENTENCE_LENGTH);
	ResetTokenSystem();

	char* ptr = input;
	tokenFlags |= (user) ? USERINPUT : 0; // remove any question mark

	// skip the ... from ^input() joining
	ptr = SkipWhitespace(ptr);
	if (!strncmp(ptr,"... ",4)) ptr += 4;  
    ptr = Tokenize(ptr,wordCount,wordStarts); 

 	if (tokenFlags & ONLY_LOWERCASE) // force lower case
	{
		for (unsigned int i = 1; i <= wordCount; ++i) 
		{
			if (wordStarts[i][0] != 'I' || wordStarts[i][1]) MakeLowerCase(wordStarts[i]);
		}
	}
	
	// this is the input we currently are processing.
	*oldInputBuffer = 0;
	char* at = oldInputBuffer;
	for (unsigned int i = 1; i <= wordCount; ++i)
	{
		strcpy(at,wordStarts[i]);
		at += strlen(at);
		*at++ = ' ';
	}
	*at = 0;

	// force Lower case on plural start word which has singular meaning (but for substitutes
	if (wordCount)
	{
		char word[MAX_WORD_SIZE];
		MakeLowerCopy(word,wordStarts[1]);
		size_t len = strlen(word);
		if (strcmp(word,wordStarts[1]) && word[1] && word[len-1] == 's') // is a different case and seemingly plural
		{
			WORDP O = FindWord(word,len,UPPERCASE_LOOKUP);
			WORDP D = FindWord(word,len,LOWERCASE_LOOKUP);
			if (D && D->properties & PRONOUN_BITS) {;} // dont consider hers and his as plurals of some noun
			else if (O && O->properties & NOUN) {;}// we know this noun (like name James)
			else
			{
				char* singular = GetSingularNoun(word,true,false);
				D = FindWord(singular);
				if (D && stricmp(singular,word)) // singular exists different from plural, use lower case form
				{
					D = StoreWord(word); // lower case plural form
					if (D->internalBits & UPPERCASE_HASH) AddProperty(D,NOUN_PROPER_PLURAL|NOUN);
					else AddProperty(D,NOUN_PLURAL|NOUN);
					wordStarts[1] = D->word;
				}
			}
		}
	}
 	if (mytrace || prepareMode == PREPARE_MODE)
	{
		if (tokenFlags & USERINPUT) Log(STDUSERLOG,"\r\nOriginal User Input: %s\r\n",input);
		else Log(STDUSERLOG,"\r\nOriginal Chatbot Output: %s\r\n",input);
		Log(STDUSERLOG,"Tokenized: ");
		for (unsigned int i = 1; i <= wordCount; ++i) Log(STDUSERLOG,"%s  ",wordStarts[i]);
		Log(STDUSERLOG,"\r\nTokenControl: ");
		DumpTokenControls(tokenControl);
		Log(STDUSERLOG,"\r\n");
	}
	unsigned int originalCount = wordCount;
	if (mytrace || prepareMode) memcpy(original+1,wordStarts+1,wordCount * sizeof(char*));	// replicate for test

	if (tokenControl & (DO_SUBSTITUTE_SYSTEM|DO_PRIVATE))  
	{
		ProcessSubstitutes();
 		if (mytrace || prepareMode == PREPARE_MODE)
		{
			int changed = 0;
			if (wordCount != originalCount) changed = true;
			for (unsigned int i = 1; i <= wordCount; ++i) if (original[i] != wordStarts[i]) changed = i;
			if (changed)
			{
				Log(STDUSERLOG,"Substituted: ");
				for (unsigned int i = 1; i <= wordCount; ++i) Log(STDUSERLOG,"%s  ",wordStarts[i]);
				Log(STDUSERLOG,"\r\n");
				memcpy(original+1,wordStarts+1,wordCount * sizeof(char*));	// replicate for test
			}
			originalCount = wordCount;
		}
	}
	// test for punctuation not done by substitutes
	char c = (wordCount) ? *wordStarts[wordCount] : 0;
	if (c == '?' || c == '!') 
	{
		tokenFlags |= (c == '?') ? QUESTIONMARK : EXCLAMATIONMARK;
		--wordCount;
	}  
	
	// if 1st token is an interjection DO NOT allow this to be a question
	if (wordCount && wordStarts[1] && *wordStarts[1] == '~' && !(tokenControl & NO_INFER_QUESTION)) 
		tokenFlags &= -1 ^ QUESTIONMARK;

	// special lowercasing of 1st word if it COULD be AUXVERB and is followed by pronoun - avoid DO and Will and other confusions
	if (wordCount > 1 && IsUpperCase(*wordStarts[1]))
	{
		WORDP X = FindWord(wordStarts[1],0,LOWERCASE_LOOKUP);
		if (X && X->properties & AUX_VERB)
		{
			WORDP Y = FindWord(wordStarts[2]);
			if (Y && Y->properties & PRONOUN_BITS) wordStarts[1] = X->word;
		}
	}

	if (tokenControl & DO_PROPERNAME_MERGE && wordCount)  ProcessCompositeName();   
 	if (tokenControl & DO_NUMBER_MERGE && wordCount)  ProcessCompositeNumber(); //   numbers AFTER titles, so they dont change a title
 	if (mytrace || prepareMode == PREPARE_MODE) 
	{
		int changed = 0;
		if (wordCount != originalCount) changed = true;
		for (unsigned int i = 1; i <= wordCount; ++i) if (original[i] != wordStarts[i]) changed = i;
		if (changed)
		{
			if ((tokenControl & (DO_PROPERNAME_MERGE | DO_NUMBER_MERGE)) == (DO_PROPERNAME_MERGE | DO_NUMBER_MERGE)) 
				Log(STDUSERLOG,"Name/Number-merged Input: ");
			else if (tokenControl & DO_PROPERNAME_MERGE) Log(STDUSERLOG,"Name-merged: ");
			else Log(STDUSERLOG,"Number-merged Input: ");
			for (unsigned int i = 1; i <= wordCount; ++i) Log(STDUSERLOG,"%s  ",wordStarts[i]);
			Log(STDUSERLOG,"\r\n");
			memcpy(original+1,wordStarts+1,wordCount * sizeof(char*));	// replicate for test
			originalCount = wordCount;
		}
	}

	// spell check unless 1st word is already a known interjection. Will become standalone sentence
	if (tokenControl & DO_SPELLCHECK && wordCount && *wordStarts[1] != '~')  
	{
		if (SpellCheckSentence())
		{
			tokenFlags |= DO_SPELLCHECK;
			if (tokenControl & (DO_SUBSTITUTE_SYSTEM|DO_PRIVATE))  ProcessSubstitutes();
		}
 		if (mytrace || prepareMode == PREPARE_MODE)
		{
 			int changed = 0;
			if (wordCount != originalCount) changed = true;
			for (unsigned int i = 1; i <= wordCount; ++i) if (original[i] != wordStarts[i]) changed = i;
			if (changed)
			{
				Log(STDUSERLOG,"Spelling fixed: ");
				for (unsigned int i = 1; i <= wordCount; ++i) Log(STDUSERLOG,"%s  ",wordStarts[i]);
				Log(STDUSERLOG,"\r\n");
			}
		}
	}

	if (echoSource == SOURCE_ECHO_LOG) 
	{
		bool oldecho = echo;
		echo = true;
		Log(STDUSERLOG,"  => ");
		for (unsigned int i = 1; i <= wordCount; ++i) Log(STDUSERLOG,"%s  ",wordStarts[i]);
		Log(STDUSERLOG,"\r\n");
		echo = oldecho;
	}
	nextInput = ptr;	//   allow system to overwrite input here
 
	if (tokenControl & DO_INTERJECTION_SPLITTING && wordCount > 1 && *wordStarts[1] == '~') // interjection. handle as own sentence
	{
		// formulate an input insertion
		char buffer[BIG_WORD_SIZE];
		*buffer = 0;
		for (unsigned int i = 2; i <= wordCount; ++i)
		{
			strcat(buffer,wordStarts[i]);
			strcat(buffer," ");
		}
		if (tokenFlags & QUESTIONMARK) strcat(buffer,"? ");
		else if (tokenFlags & EXCLAMATIONMARK) strcat(buffer,"! ");
		else strcat(buffer,". ");
		char* end = buffer + strlen(buffer);
		strcpy(end,nextInput); // a copy of rest of input
		strcpy(nextInput,buffer); // unprocessed user input is here
		ptr = nextInput;
		wordCount = 1;
		tokenFlags |= DO_INTERJECTION_SPLITTING;
	}

	wordStarts[wordCount+1] = 0; // visible end of data in debug display
	wordStarts[wordCount+2] = 0;

    if (mark && wordCount) MarkAllImpliedWords();

	nextInput = SkipWhitespace(nextInput);
    moreToCome = strlen(nextInput) > 0;	   
	moreToComeQuestion = (strchr(nextInput,'?') != 0);
	char nextWord[MAX_WORD_SIZE];
	ReadCompiledWord(nextInput,nextWord);
	WORDP next = FindWord(nextWord);
	if (next && next->properties & QWORD) moreToComeQuestion = true; // assume it will be a question (misses later ones in same input)
	if (prepareMode == PREPARE_MODE || trace || prepareMode == POS_MODE || (prepareMode == PENN_MODE && trace & TRACE_POS)) DumpTokenFlags();
}

#ifndef NOMAIN
int main(int argc, char * argv[]) 
{
	if (InitSystem(argc,argv)) myexit("failed to load memory\r\n");
    if (!server) MainLoop();
#ifndef DISCARDSERVER
    else
    {
#ifdef EVSERVER
        if (evsrv_run() == -1) Log(SERVERLOG, "evsrv_run() returned -1");
#else
        InternetServer();
#endif
    }
#endif
	CloseSystem();
	myexit("shutdown complete");
}
#endif
