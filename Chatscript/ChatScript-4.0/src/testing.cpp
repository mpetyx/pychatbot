#include "common.h"

#ifndef DISCARDTESTING

static unsigned int lineLimit = 0; // in abstract report lines that are longer than this...
static WORDP topLevel = 0;
static unsigned int err = 0;
static unsigned int filesSeen; 
static 	char directory[MAX_WORD_SIZE];
static int itemcount = 0;
static char* abstractBuffer;
static char summary[MAX_WORD_SIZE];

#define ABSTRACT_SPELL 1
#define ABSTRACT_SET_MEMBER 2
#define ABSTRACT_CANONICAL 4
#define ABSTRACT_PRETTY 8
#define ABSTRACT_VP 16
#define ABSTRACT_NOCODE 32
#define ABSTRACT_RESTRICTIONS (ABSTRACT_SPELL|ABSTRACT_SET_MEMBER|ABSTRACT_CANONICAL|ABSTRACT_PRETTY|ABSTRACT_VP )

#define RECORD_SIZE 4000

// prototypes
static bool DumpOne(WORDP S,int all,int depth,bool shown);
static int CountDown(MEANING T,int all,int depth,unsigned int baseStamp);
static void C_Retry(char* input);

static MEANING* meaningList; // list of meanings from :concepts
static MEANING* meaningLimit; // end of meaninglistp

////////////////////////////////////////////////////////
/// UTILITY ROUTINES
////////////////////////////////////////////////////////

int CountSet(WORDP D,unsigned int baseStamp) //   full recursive referencing
{
	if (!D) return 0;

	int count = 0;
	FACT* F = GetObjectHead(D);
	FACT* G;
	while (F) //   do all atomic members of it
	{
		G = F;
		F = GetObjectNext(F);
		WORDP S = Meaning2Word(G->subject);
		if (!(G->verb == Mmember)  || G->flags & FACTDEAD) continue;
		if (*S->word == '~' ) continue;
		else if (Meaning2Index(G->subject)) count += CountDown(GetMaster(G->subject),-1,0,baseStamp); //   word~2 reference is a synset header -- follow IS_A submembers
		else //   simple atomic member -- or POS specificiation
		{
			if (S->inferMark <= baseStamp) //   count once
			{
				S->inferMark = inferMark;
				++count;
			}
		}
	}
	F = GetObjectHead(D);
	while (F) //   do all set members of it
	{
		G = F;
		F = GetObjectNext(F);
		WORDP S = Meaning2Word(G->subject);
		if (!(G->verb == Mmember)  || G->flags & FACTDEAD) continue;
		if (*S->word == '~')  count += CountSet(S,baseStamp);
	}
	return count;
}

static int CountDown(MEANING T,int all,int depth,unsigned int baseStamp)
{ //  T is a synset header
	T &= -1 ^ SYNSET_MARKER;

	if (all == 5) return 0;
	int count = 0;

	//   show each word in synset
    WORDP D = Meaning2Word(T);
	WORDP baseWord = D;
	unsigned int index = Meaning2Index(T);
	unsigned int baseIndex = index;

	// walk the master list of synonyms at this level
	bool shown = false;
	while (ALWAYS) 
    {
		MEANING next = GetMeaning(D,index);
		if (D->inferMark != inferMark) 
		{
			if (D->inferMark <= baseStamp) ++count;
			D->inferMark = inferMark;	
			if (depth != -1) shown |= DumpOne(D,all,depth,shown); //   display it
		}
		D = Meaning2Word(next);
		index = Meaning2Index(next);
		if (D == baseWord && index == baseIndex) break; // back at start of loop
    }

	//   down go down to next level synset from this one
	FACT* F = GetObjectHead(T); 
	while (F)
	{
		if (F->verb ==  Mis && F->object == T) count += CountDown(F->subject,all,depth+1,baseStamp);
		F = GetObjectNext(F);
	}
	return count;
}

static void Indent(int count,bool nonumber)
{
	if (!nonumber) Log(STDUSERLOG,"%d.",count);
	while (count--) Log(STDUSERLOG,"    ");
}

static bool DumpOne(WORDP S,int all,int depth,bool shown)
{
	bool did = false;
	if (all) 
	{
			if ( all == 3) return false;
			if (itemcount == 0 && all != 2) Indent(depth,shown);
			unsigned char* data = GetWhereInSentence(S);
			if (all == 1) 
			{
				if (!data)
				{
					data = AllocateWhereInSentence(S);
					if (!data) return false;
					*data = 0;
					data[1] = 0;
				}
				if (++data[1] == 0) ++data[0];
			}
			if (all == 1 && *data && (data[0] || data[1] > 1)) Log(STDUSERLOG,"+%s  ",S->word); //   multiple occurences
			else  //   first occurence of word
			{
				if (all == 1 && !(S->systemFlags & VERB_DIRECTOBJECT)) //   generate a list of intransitive verbs
				{
					FILE* out = fopen("intransitive.txt","ab");
					fprintf(out,"%s 1\r\n",S->word);
					fclose(out);
				}
				if (all == 1 && (S->systemFlags & VERB_INDIRECTOBJECT)) //   generate a list of dual transitive verbs
				{
					FILE* out = fopen("intransitive.txt","ab");
					fprintf(out,"%s 2\r\n",S->word);
					fclose(out);
				}
				Log(STDUSERLOG,"%s  ",S->word);
			}
			++itemcount;
			if (itemcount == 10 && all != 2)
			{
				Log(STDUSERLOG,"\r\n");
				itemcount = 0;
			}
			did = true;
	}
	return did;
}

static void MarkExclude(WORDP D)
{
	FACT* F = GetObjectHead(D);
	while (F)
	{
		if (F->verb == Mexclude) Meaning2Word(F->subject)->inferMark = inferMark;
		F = GetObjectNext(F);
	}
}

/////////////////////////////////////////////
/// TESTING
/////////////////////////////////////////////

static void C_AutoReply(char* input)
{
	regression = 1;
	if  (*input == '1' || *input == 'O' || *input == 'o') oktest = OKTEST;
	else if  (*input == '2' || *input == 'W' || *input == 'w') oktest = WHYTEST;
	else 
	{
		regression =  false;
		oktest = 0;
	}
	if (oktest) Log(STDUSERLOG,"Auto input set to %s\r\n",(oktest == OKTEST) ? "ok" : "why");
}  

static void C_NoReact(char* input)
{
	noReact = !noReact;
	Log(STDUSERLOG,"Noreact = %d\r\n",noReact);
} 

static void C_POS(char* input)
{
	if (!*input) prepareMode = (prepareMode == POS_MODE) ? NO_MODE : POS_MODE;
	else 
	{
		unsigned int oldtrace = trace;
		uint64 oldTokenControl = tokenControl;

		char word[MAX_WORD_SIZE];
		char* at = ReadCompiledWord(input,word);
		if (!stricmp(word,"PENN"))
		{
			input = at;
			tokenControl = STRICT_CASING | DO_ESSENTIALS| DO_PARSE | DO_CONTRACTIONS| NO_HYPHEN_END | NO_COLON_END | NO_SEMICOLON_END | TOKEN_AS_IS;
		}
		else 
		{
			char* token = GetUserVariable("$token");
			int64 f;
			ReadInt64(token,f);
			tokenControl = f;
		}
		trace = (unsigned int) -1;
		prepareMode = POS_MODE;
		quotationInProgress = 0;	
		PrepareSentence(input,true,true);	
		prepareMode = NO_MODE;
		tokenControl = oldTokenControl;
		trace = oldtrace;
	}
}

static void C_Prepare(char* input)
{
	uint64 oldToken = tokenControl;
	if (!*input) prepareMode = (prepareMode == PREPARE_MODE) ? NO_MODE : PREPARE_MODE;
	else 
	{
		char prepassTopic[MAX_WORD_SIZE];
		strcpy(prepassTopic, GetUserVariable("$prepass"));
		unsigned int oldtrace = trace;
		nextInput = input;
		while (ALWAYS)
		{
			prepareMode = PREPARE_MODE;
			PrepareSentence(nextInput,true,true);	
			prepareMode = NO_MODE;
			if (!trace) trace = TRACE_BASIC | TRACE_MATCH;
			if (PrepassSentence(prepassTopic)) PrepareSentence(nextInput,true,true); // user input revise and resubmit?  -- COULD generate output and set rejoinders
			else if (!*nextInput) break;
		}
		trace = oldtrace;
	}
	tokenControl = oldToken;
}

static void C_Source(char* input)
{
	char word[MAX_WORD_SIZE];
	char* ptr = ReadCompiledWord(input,word);
	FILE* in = FopenReadNormal(word); // source
	if (in) sourceFile = in;
	else Log(STDUSERLOG,"No such source file: %s\r\n",word);
	SetUserVariable("$$document",word);
	ReadCompiledWord(ptr,word);
	echoSource = 0;
	if (!stricmp(word,"echo")) echoSource = SOURCE_ECHO_USER;
	else if (!stricmp(word,"internal"))  echoSource = SOURCE_ECHO_LOG;
} 

static void ReadNextDocument(char* name,uint64 value)
{
	FILE* in = FopenReadNormal(name); // source
	if (in) sourceFile = in;
	else 
	{
		Log(STDUSERLOG,"No such source file: %s\r\n",name);
		return;
	}
	inputSentenceCount = 0;
	docTime = ElapsedMilliseconds();
	tokenCount = 0;
	documentMode = true;
	ShowStats(true);
	SetUserVariable("$$document",name);
	echo = false;
	*outBuffer = 0;
	OnceCode("$control_pre"); // just once per document
	ProcessInputFile();
	postProcessing = AllocateBuffer();
	documentMode = false; 
	FinishVolley(" ",outBuffer,summary);
	FreeBuffer();
	postProcessing = 0;
	documentMode = true;
	char* ptr = GetFileRead(loginID,computerID); // read user file, if any, or get it from cache
	ReadUserData(ptr);		//   now bring in user state anew
}

static void C_Document(char* input)
{
	FILE* pub = FopenUTF8Write("LOGS/pub.txt");
	fclose(pub);

	documentBuffer = AllocateBuffer() + 1; // hide a char before edge for backward testing
	*documentBuffer = 0;
	++baseBufferIndex; // system will reset  buffers each sentence to include ours
	char name[MAX_WORD_SIZE];
	char* ptr = ReadCompiledWord(input,name);
	char attrib[MAX_WORD_SIZE];
	*summary = 0;
	echoSource = 0;

	while (ptr)
	{
		ptr = ReadCompiledWord(ptr,attrib);
		if (!*attrib) break;
		if (!stricmp(attrib,"single")) singleSource = true;
		else if (!stricmp(attrib,"echo")) echoDocument = true;
		else if (!stricmp(attrib,"internal"))  echoSource = SOURCE_ECHO_LOG;
		else if (*attrib == '~') strcpy(summary,attrib); // final postprocess
	}
	
	size_t len = strlen(name);
	if (name[len-1] == '/') WalkDirectory(name,ReadNextDocument, 0);
	else ReadNextDocument(name,0);
	echo = false;
	documentBuffer = 0;
	FreeBuffer();
	documentMode = false;
	--baseBufferIndex;
} 

static void DoAssigns(char* ptr)  // find variable assignments
{
	char var[MAX_WORD_SIZE];
	char* dollar;
	char* percent;
	char* underscore;
	char* at;
	char* first = 0;
	while (ptr) // do all user variables
	{
		at = NULL;
		char* spot = ptr;
		dollar = strchr(ptr,'$');  // find a potential variable
		percent = strchr(ptr,'%');
		underscore = strchr(ptr,'_');
		if (dollar) at = dollar;
		else if (percent) at = percent;
		else while ((underscore = strchr(spot,'_')))  // may not be real yet, might be like New_year's_eve 
		{
			if (IsDigit(underscore[1])) 
			{
				at = underscore;
				break;
			}
			else spot = underscore + 1;
		}
		if (!at) break;
		if (percent && percent < at) at = percent;
		if (underscore && underscore < at) at = underscore;
		
		// at is the soonest assignment
		char* eq = strchr(at,'='); // has an assignment
		if (!eq) break; // no assignment
		char c = *eq;
		*eq-- = 0; // break off lhs
		ReadCompiledWord(at,var);
		*eq = c;  // change  $current=1  into $curren= 1 moving operator and giving space after
		if (eq[2] == '=') 
		{
			eq[1] = eq[2];
			eq[2] = ' '; // 2 char operator
		}
		else eq[1] = ' '; // single char operator

		if (!first) first = at; // start of all
		unsigned int result;
		ptr = PerformAssignment(var,eq,result);
		if (ptr) memset(at,' ',ptr-at);
	}
	if (first) 
		*first = 0; // remove all assignments
}

static void C_TestPattern(char* input)
{ // pattern, input, optional var assigns  -  (drink)  Do you like drink? %date = 1
#ifndef DISCARDSCRIPTCOMPILER
	if (*input != '(') 
	{
		Log(STDUSERLOG,"Bad test pattern");
		return;
	}

	char data[MAX_WORD_SIZE];
	char* pack = data;
	strcpy(readBuffer,input);
	if (setjmp(scriptJump[++jumpIndex])) // return on script compiler error
	{
		--jumpIndex;
		return;
	}
	ReadNextSystemToken(NULL,NULL,data,false,false); // flush cache
	char* ptr = ReadPattern(readBuffer, NULL, pack,false); // swallows the pattern

	//   var assign?
	DoAssigns(ptr);
	if (!*ptr) return; //   forget example sentence

	char prepassTopic[MAX_WORD_SIZE];
	strcpy(prepassTopic,GetUserVariable("$prepass"));
	PrepareSentence(ptr,true,true);	
	
	unsigned int gap = 0;
	unsigned int wildcardSelector = 0;
	wildcardIndex = 0;
	unsigned int junk1;
	blocked = true;
	bool result =  Match(data+2,0,0,'(',true,gap,wildcardSelector,junk1,junk1);
	blocked = false;
	if (result) Log(STDUSERLOG," Matched\r\n");
	else Log(STDUSERLOG," Failed\r\n");
	--jumpIndex;
#endif
}

static void GambitTestTopic(char* topic)
{
	int topicID = FindTopicIDByName(topic);
	if (!topicID) 
	{
		Log(STDUSERLOG,"topic not found %s\r\n",topic);
		return;
	}
	if (GetTopicFlags(topicID) & TOPIC_NOGAMBITS) return;
	bool  oldRegression = regression;
	regression = true;

	char* data = GetTopicData(topicID);
	char* output = AllocateBuffer();
	int ruleID = -1;
	while (data)
	{
		char* rule = data;
		++ruleID;
		int id = 0;
		data = FindNextRule(NEXTTOPLEVEL,data,id);
		if (*rule != GAMBIT && *rule != RANDOM_GAMBIT) continue; // not a gambit

		// get the output
		rule  = GetPattern(rule,NULL,NULL);
		char* end = strchr(rule,ENDUNIT);  // will not be a useful output as blanks will become underscores, but can do ^reuse() to execute it
		*end = 0;
		strcpy(output,rule);
		*end = ENDUNIT;
		char* q = strchr(output,'?');
		if (!q) continue;	 // not a question
		q[1] = 0; // ignore any following.
		char* at = q;
		while (--at > output) // is there a question before this
		{
			if ((*at == '.' || *at == '!') && at[1] == ' ') break;// end of a sentence?
		}
		if (at != output) output = at+1;

		globalDepth = 0;

		//  perform varible setup, do assigns, and prepare matching context
		ResetToPreUser();
		ReadNewUser();   
		char prepassTopic[MAX_WORD_SIZE];
		strcpy(prepassTopic,GetUserVariable("$prepass"));
		inputCount = 1;
		strcpy(currentInput,output);	//   this is what we respond to, literally.
		nextInput = output;
		OnceCode("$control_pre");
		PrepareSentence(output,true,true);
		if (PrepassSentence(prepassTopic)) PrepareSentence(nextInput,true,true); // user input revise and resubmit?  -- COULD generate output and set rejoinders
		AddPendingTopic(topicID); // ResetToPreUser clears pendingTopicIndex
		responseIndex = 0;
		Reply();
		if (pendingTopicIndex && pendingTopicList[pendingTopicIndex-1] == (unsigned int) topicID){;}
		else if (!responseIndex || responseData[0].topic != (unsigned int)topicID )
		{
			Log(STDUSERTABLOG,"Not answering own question in topic %d %s.%d.%d: %s => %s %s \r\n\r\n",++err,topic,TOPLEVELID(ruleID),REJOINDERID(ruleID),output,GetTopicName(responseData[0].topic),responseData[0].response);
		}
	}
	FreeBuffer();
	regression = oldRegression;
}

static void C_TestTopic(char* input)
{
	char word[MAX_WORD_SIZE];
	input = ReadCompiledWord(input,word);
	char prepassTopic[MAX_WORD_SIZE];
	strcpy(prepassTopic,GetUserVariable("$prepass"));
	PrepareSentence(input,true,true);	
	if (PrepassSentence(prepassTopic)) PrepareSentence(nextInput,true,true); // user input revise and resubmit?  -- COULD generate output and set rejoinders
	unsigned int topic = FindTopicIDByName(word);
	if (!topic)  return;
	int pushed =  PushTopic(topic); 
	if (pushed < 0) return;
	char* buffer = AllocateBuffer();
	ClearUserVariableSetFlags();
	AllocateOutputBuffer();
	PerformTopic(0,buffer); //   ACTIVE handle - 0 is good result
	FreeOutputBuffer();
	FreeBuffer();
	for (unsigned int i = 0; i < responseIndex; ++i) Log(STDUSERLOG,"%s\r\n", responseData[responseOrder[i]].response);
	ShowChangedVariables();
}

static void VerifyAccess(char* topic,char kind,char* prepassTopic) // prove patterns match comment example, kind is o for outside, r for rule, t for topic, s for samples
{
	bool testKeyword = kind == 'k';
	bool testPattern = kind == 'p' ;
	bool testBlocking = kind == 'b';
	bool testSample = kind == 's' || kind == 'S' ;
	if (kind == 'a' || !kind) testKeyword = testPattern = testBlocking =  true;
 	unsigned int topicIndex = FindTopicIDByName(topic);
	if (!topicIndex) 
	{
		printf("%s not found\r\n",topic);
		return;
	}
	WORDP topicWord = FindWord(GetTopicName(topicIndex)); // must find it
	topic = topicWord->word;
	
	int flags = GetTopicFlags(topicIndex);
	if (flags & TOPIC_BLOCKED) return;

	if (testKeyword) 	// has no keyword into here so dont test keyword access
	{
		FACT* F = GetObjectHead(topicWord);
		while (F)
		{
			if (F->verb == Mmember) break; 
			F = GetObjectNext(F);
		}
		if (!F)  testKeyword = false;
	}

	if (GetTopicFlags(topicIndex) & (TOPIC_RANDOM|TOPIC_NOBLOCKING)) testBlocking = false;	
	if (GetTopicFlags(topicIndex) & (TOPIC_RANDOM|TOPIC_NOKEYS)) testKeyword = false;	
	if (GetTopicFlags(topicIndex) & TOPIC_NOPATTERNS) testPattern = false;	
	if (GetTopicFlags(topicIndex) & TOPIC_NOSAMPLES) testSample = false;	
	
	char name[100];
	char* tname = (*topic == '~') ? (topic + 1) : topic;
	if (duplicateCount) sprintf(name,"VERIFY/%s.%d.txt",tname,duplicateCount);
	else sprintf(name,"VERIFY/%s.txt",tname);
	FILE* in = fopen(name,"rb");
	if (!in) 
	{
		printf("%s verification data not found\r\n",name);
		return;
	}

	unsigned int oldtrace = trace;
	trace = 0;
	Log(STDUSERLOG,"VERIFYING %s ......\r\n",topic);
	char* copyBuffer = AllocateBuffer();
	char junk[MAX_WORD_SIZE];
	// process verification data
	while (ReadALine(readBuffer,in))
	{
		if (bufferIndex > 6) return;

		if (!strnicmp(readBuffer,":trace",10))
		{
			trace = atoi(readBuffer+11);
			continue;
		}
		if (!strnicmp(readBuffer,":exit",5)) myexit(":exit requested");
		bool failTest = false;
	
		// read tag of rule to apply input to
		char* dot = strchr(readBuffer,'.');
		if (!dot) continue;
		int ruleID = atoi(dot+1);
		dot = strchr(dot+1,'.');
		if (dot) ruleID |= atoi(dot+1) * ONE_REJOINDER;
		char* rule = GetRule(topicIndex,ruleID);						// the rule we want to test
		char* topLevelRule = GetRule(topicIndex,TOPLEVELID(ruleID));	// the top level rule (if a rejoinder)
		char* rejoinderTop = NULL;
		int rejoinderTopID = 0;
		if (rule != topLevelRule) // its a rejoinder, find the start of the rejoinder area
		{
			rejoinderTop = topLevelRule;  // the limit of backward checking
			char* at = rule; // start at the given rejoinder
			rejoinderTopID = ruleID;
			while (*at >= *rule && REJOINDERID(rejoinderTopID)) // stop if drop below our level or go to top level rule
			{
				at = RuleBefore(topicIndex,at);
				rejoinderTopID -= ONE_REJOINDER;
			}
			rejoinderTopID += ONE_REJOINDER; // now move back down to 1st of our level
			rejoinderTop = FindNextRule(NEXTRULE,at,rejoinderTopID);
		}

		// the comment headers are:
		// #!x  - description header for :abstract
		// #!!R  - expect to fail RULE
		// #!!T - expect to fail TOPIC (be masked by earlier rule)
		// #!!K - expect to fail keywords
		// #!!S - dont run sample
		*junk = junk[1] = junk[2] = 0;
		char* test = strchr(readBuffer,'!')+1;	// the input sentence (skipping offset and #! marker)
		if (*test != ' ') test = ReadCompiledWord(test,junk); // things to not test
		MakeLowerCase(junk);
		if (*junk == 'x') continue;  // only used for :abstract

		bool wantFailBlocking = false;
		bool wantFailKeyword = false;
		bool wantFailPattern = false;
		bool wantNoSample = false;
		if (strchr(junk,'b') || strchr(junk,'B')) wantNoSample = wantFailBlocking = true;
		if (strchr(junk,'k') || strchr(junk,'K')) wantNoSample = wantFailKeyword = true;
		if (strchr(junk,'p') || strchr(junk,'P')) wantFailPattern = true;
		if (strchr(junk,'s') || strchr(junk,'S')) wantNoSample = true;

		//   test pattern on THIS rule

		globalDepth = 0;

		//  perform varible setup, do assigns, and prepare matching context
		ResetToPreUser();
		ReadNewUser();   
		inputCount = 1;
		if (testSample) OnceCode("$control_pre");

		DefineSystemVariables(); // clear system variables to default
		DoAssigns(test); // kills test start where any defines are
		strcpy(copyBuffer,test);
		strcpy(currentInput,test);	//   this is what we respond to, literally.
		nextInput = test;
		PrepareSentence(test,true,true);
		if (PrepassSentence(prepassTopic)) PrepareSentence(nextInput,true,true); // user input revise and resubmit?  -- COULD generate output and set rejoinders
		currentTopicID = topicIndex;
		strcpy(test,copyBuffer); // sentence prep may have altered test data and we might want to redo it
		AddPendingTopic(topicIndex); // ResetToPreUser clears pendingTopicIndex

		char label[MAX_WORD_SIZE];
		char pattern[MAX_WORD_SIZE];
		char* topLevelOutput = GetOutputCopy(topLevelRule); 
		GetPattern(rule,label,pattern);
		if (!*pattern) 
		{
			ReportBug("No pattern here? %s %s\r\n",topic,rule)
			continue;
		}

		bool result;
		if (testKeyword && !wantFailKeyword &&  !TopLevelGambit(rule) &&  TopLevelRule(rule) )  // perform keyword test on sample input sentence
		{
			unsigned int pStart = 0;
			unsigned int pEnd = 0;
			if (!GetNextSpot(topicWord,0,pStart,pEnd)) // not findable topic
			{
				Query("direct_v","?","idiom",topic,(unsigned int)-1,"?","?","?","?");  // get all idioms that can trigger this topic
				unsigned int i = FACTSET_COUNT(0);
				while (i > 0)
				{
					FACT* F = factSet[0][i];
					WORDP pattern = Meaning2Word(F->subject);
					strcpy(callArgumentList[callArgumentBase+1],pattern->word);
					*callArgumentList[callArgumentBase+2] = 0; // dumy argument 1
					if (MatchCode(junk) == 0) break;
					--i;
				}

				if ( i == 0) 
				{
					Log(STDUSERTABLOG,"%d Missing keyword %s.%d.%d <= %s\r\n",++err,topic,TOPLEVELID(ruleID),REJOINDERID(ruleID),test);
					failTest = true;
				}
			}
		}

		//   inside the pattern, test this rule
		if (testPattern && !wantFailPattern)
		{
			result = RuleTest(rule);
			if ((!result && !wantFailPattern) || (result && wantFailPattern) ) //   didnt do what we expected
			{
				Log(STDUSERTABLOG,"Bad Pattern %d %s.%d.%d: %s => %s\r\n\r\n",++err,topic,TOPLEVELID(ruleID),REJOINDERID(ruleID),test,pattern);
				failTest = true;

				// redo with tracing on if selected so we can watch it fail
				if (oldtrace)
				{
					trace = oldtrace;
					PrepareSentence(test,true,true);	
					if (PrepassSentence(prepassTopic)) PrepareSentence(nextInput,true,true); // user input revise and resubmit?  -- COULD generate output and set rejoinders
					strcpy(test,copyBuffer); // sentence prep may have altered test data and we might want to redo it
					RuleTest(rule);
					trace = 0;
				}
				continue;
			}
		}
		
		if (testBlocking && !wantFailBlocking  && !TopLevelGambit(rule)) // check for blocking
		{
			char* data;
			char* output = NULL;
			int id = 0;
			if (TopLevelRule(rule)) // test all top level rules in topic BEFORE this one
			{
				data = GetTopicData(topicIndex);
				char label[MAX_WORD_SIZE];
				char pattern[MAX_WORD_SIZE];
				while (data && data < rule)
				{
					if (*data == GAMBIT || *data == RANDOM_GAMBIT); // no data gambits
					else if (*rule == STATEMENT && *data == QUESTION); // no mismatch type
					else if (*rule == QUESTION && *data == STATEMENT); // no mismatch type
					else 
					{
						output = GetPattern(data,label,pattern);
						if (!*pattern) break; 
						unsigned int result;
						if (pattern[2] == ')' || pattern[2] == '*') result =  0; // universal match patterns are PRESUMED not to be blocking. they obviously obscure anything
						else result = RuleTest(data);// past the paren
						if (result)	break; // he matched, blocking us
					}
					data = FindNextRule(NEXTTOPLEVEL,data,id);
				}
			}
			else  // rejoinder matching 
			{
				data = rejoinderTop;
				id = rejoinderTopID;
				while (data < rule)
				{
					if (*data == *rule)// all rules of this same level and before us
					{
						unsigned int result = RuleTest(data); // past the paren
						if (result)	break; // he matched, blocking us
					}
					data = FindNextRule(NEXTRULE,data,id);
				}
			}

			if (data && data < rule) // earlier rule matches same pattern
			{
				// prove not a simple () (*) (!?)  (?) etc
				char* t = pattern+2; // start AFTER the ( 
				while (ALWAYS)
				{
					t = ReadCompiledWord(t,junk);
					if (!stricmp(junk,"!") || !stricmp(junk,"*") || !stricmp(junk,"?")) continue;
					break;
				}
				if (*junk == 0 || *junk == ')') continue;	// presumed end of pattern

				// prove it may output something - all matching rejoinders automatically mask if occur sooner
				if (!Rejoinder(data)) // top level units that dont generate output dont actually mask.
				{
					char word[MAX_WORD_SIZE];
					while (*output && *output != ENDUNIT)
					{
						output = ReadCompiledWord(output,word);
						if (IsAlpha(*word) && FindWord(word)) 
							break; // possible problem
					}
				}
				if (!output || *output == ENDUNIT) continue;	// no text output found

				if (REJOINDERID(id)) Log(STDUSERTABLOG,"Blocking %d Rejoinder %d.%d %s\r\n",++err,TOPLEVELID(id),REJOINDERID(id),ShowRule(data));
				else Log(STDUSERTABLOG,"Blocking %d TopLevel %d.%d %s\r\n",++err,TOPLEVELID(id),REJOINDERID(id),ShowRule(data));
				Log(STDUSERTABLOG,"    blocks %d.%d %s\r\n    given: %s\r\n\r\n",TOPLEVELID(ruleID),REJOINDERID(ruleID),ShowRule(rule),test);
				failTest = true;
			}
		}
		if (testSample && !wantNoSample && !failTest  && TopLevelRule(rule)) // check for sample
		{
			ClearPendingTopics();
			responseIndex = 0;
			currentTopicID = 0;
			if (oldtrace) trace = oldtrace;
			Reply();
			trace = 0;			
			if (!responseIndex || responseData[0].topic != topicIndex)
			{
				Log(STDUSERTABLOG,"Bad sample topic %d %s.%d.%d: %s => %s %s \r\n\r\n",++err,topic,TOPLEVELID(ruleID),REJOINDERID(ruleID),test,GetTopicName(responseData[0].topic),responseData[0].response);
			}
			else if (kind != 'S') // also check rule bad
			{
				int id = atoi(responseData[0].id+1);
				char* dot = strchr(responseData[0].id+2,'.');
				id |= MAKE_REJOINDERID(atoi(dot+1));

				int reuseid = -1;
				dot = strchr(dot+1,'.');
				if (dot)
				{
					reuseid = atoi(dot+1);
					dot = strchr(dot+2,'.');
					reuseid |= MAKE_REJOINDERID(atoi(dot+1));
				}
				if (id == ruleID || (reuseid >= 0 && TOPLEVELID(reuseid) == (unsigned int) ruleID)) {;} // we match
				else if (TOPLEVELID(id) == (unsigned int) ruleID && !strstr(topLevelOutput,"refine")) {;} // we matched top level and are not looking for refinement
				else
					Log(STDUSERTABLOG,"Bad sample rule %d %s  For: %s \r\n   want- %d.%d\n   got - %s => %s\r\n\r\n",++err,topic,test,
						TOPLEVELID(ruleID),REJOINDERID(ruleID),
						responseData[0].id+1,ShowRule(GetRule(topicIndex,id)));
			}
		}
	}
	fclose(in);
	RemovePendingTopic(topicIndex);
	FreeBuffer(); // copyBuffer
	trace = oldtrace;
}

static void VerifyAllTopics(char kind,char* prepassTopic,char* topic)
{
	size_t len = 0;
	char* x = strchr(topic,'*');
	if (x) len = x - topic ;
	for (unsigned int i = 1; i <= lastTopic; ++i) 
	{
		if (len && strnicmp(GetTopicName(i),topic,len)) continue;
		VerifyAccess(GetTopicName(i),kind,prepassTopic);
	}
}

static void AllGambitTests(char* topic)
{
	size_t len = 0;
	char* x = strchr(topic,'*');
	if (x) len = x - topic;
	for (unsigned int i = 1; i <= lastTopic; ++i) 
	{
		if (len && strnicmp(GetTopicName(i),topic,len)) continue;
		GambitTestTopic(GetTopicName(i));
	}
}

static void C_Verify(char* input)
{
	char word[MAX_WORD_SIZE];
	char topic[MAX_WORD_SIZE];
	*topic = 0;
	err = 0;
	char* ptr = SkipWhitespace(input);
	// :verify    or    :verify blocking   or  :verify blocking ~family   or  :verify ~family or :verify sample
	if (*ptr == '~') ptr = ReadCompiledWord(ptr,topic);  // topic specifier given
	char type = 0;
	while (ptr && *ptr)
	{
		ptr = ReadCompiledWord(ptr,word);
		if (!strnicmp(word,"pattern",7)) type = 'p';
		else if (!stricmp(word,"all")) type = 'a';
		else if (!strnicmp(word,"gambit",6)) type = 'g';
		else if (!strnicmp(word,"block",5))  type = 'b';
		else if (!strnicmp(word,"keyword",7)) type = 'k';
		else if (!strnicmp(word,"sample",6))
		{
			if (!strnicmp(word,"sampletopic",11)) type = 'S'; // bad topic only
			else type = 's'; // bad topics and rules
		}
		else if (!*topic) // topic name given without ~
		{
			topic[0] = '~';
			strcpy(topic+1,word);
		}
	}
	
	if (type != 'g')
	{
		char prepassTopic[MAX_WORD_SIZE];
		strcpy(prepassTopic,GetUserVariable("$prepass"));
		if (*topic == '~' && !strchr(topic,'*')) VerifyAccess(topic,type,prepassTopic);
		else VerifyAllTopics(type,prepassTopic,topic);
	}

	// now do gambit tests
	if (type == 'g' || type == 'a')
	{
		if (*topic == '~'  && !strchr(topic,'*')) GambitTestTopic(topic);
		else AllGambitTests(topic);
	}
	Log(STDUSERLOG,"%d verify findings.\r\n",err);
}

bool stanfordParser = false;

static void PennWrite(char* name,uint64 flags)
{
	FILE* out = (FILE*)flags;
	FILE* in = fopen(name,"rb");
	if (!in) 
	{
		printf("missing %s\r\n",name);
		return;
	}
	bool content = false;
	char* buffer = AllocateBuffer();
	*buffer = 0;
	char* ptr = buffer;
	bool pendingDone = false;
	bool openQuote = false;
	while (ReadALine(readBuffer,in,maxBufferSize,true)) // read lines, returning empties as well
	{
		char word[MAX_WORD_SIZE];
		ReadCompiledWord(readBuffer,word);
		if (!*word && !stanfordParser) // empty line always separates sentences from Pennbank
		{
			if (content)
			{
				*ptr = 0;
				fprintf(out,"%s\r\n",buffer);
				ptr = buffer;
				*ptr = 0;
				content = false;
			}
			continue;
		}
		char* at = readBuffer;
		while (at && *at)
		{
			at = ReadCompiledWord(at,word);
			if (pendingDone) // saw a closing, aim to close it if not quote close
			{
				if (*word == '\'' && word[1] == '\'' && word[2] == '/' && openQuote) // close quote 
				{
					strcpy(word,"\"/\"");
					openQuote = false;
					strcat(ptr,word);
					ptr += strlen(ptr);
					strcat(ptr," ");
					++ptr;
					*word = 0;
				}	

				*ptr = 0;
				fprintf(out,"%s\r\n",buffer);
				ptr = buffer;
				*ptr = 0;
				content = false;
				pendingDone = false;
				if (!*word) continue; // closed quote around this
			}

			if (*word == '`' && word[1] == '`' && word[2] == '/') // open quote 
			{
				strcpy(word,"\"/\"");
				openQuote = true;
			}
			if (*word == '\'' && word[1] == '\'' && word[2] == '/') // close quote 
			{
				strcpy(word,"\"/\"");
				openQuote = false;
			}			
			if (*word == '[' || *word == ']') continue;	// ignore this
			if (*word == '=' && word[1] == '=') // ignore ======================================
			{
				if (content)
				{
					*ptr = 0;
					fprintf(out,"%s\r\n",buffer);
					ptr = buffer;
					*ptr = 0;
					content = false;
				}
				continue; 
			}
			strcat(ptr,word);
			ptr += strlen(ptr);
			strcat(ptr," ");
			++ptr;

			if (!content && !stanfordParser)
			{
				if (IsLowerCase(*word)) Log(STDUSERLOG,"LOWER START? %s in %s \r\n",readBuffer,name);
			}
			content = true;

			if (stanfordParser && (*word == '.' || *word == '?' || *word == '!')) // sentences using stanford parser will end with punctuation UNLESS have quote after that
			{
				pendingDone = true;
			}
		}
	}
	if (content)
	{
		*ptr = 0;
		fprintf(out,"%s\r\n",buffer);
	}
	fclose(in);
	FreeBuffer();
}

static void C_PennFormat(char* file)
{
	char indir[MAX_WORD_SIZE];
	file = ReadCompiledWord(file,indir); // where source is
	char word[MAX_WORD_SIZE];
	file = ReadCompiledWord(file,word); // where source is
	char outfile[MAX_WORD_SIZE];
	sprintf(outfile,"REGRESS/PENNTAGS/%s.txt",word); // where output is

	if (!strnicmp(file,"stanford",4)) stanfordParser = true; // sentences end with . or ! or ?
	FILE* out = fopen(outfile,"wb");
	if (!out) return;
	WalkDirectory(indir,PennWrite,(uint64)out);
	fclose(out);
}

static void C_PennMatch(char* file)
{
	char word[MAX_WORD_SIZE];
	file = ReadCompiledWord(file,word);
	bool raw = false;
	bool basic = false;
	char filename[MAX_WORD_SIZE];
	if (*word) sprintf(filename,"REGRESS/PENNTAGS/%s.txt",word);
	else strcpy(filename,"REGRESS/PENNTAGS/penn.txt");
	FILE* in = FopenReadOnly(filename);
	if (!in) 
	{
		Log(STDUSERLOG,"No such file %s\r\n",filename);
		return;
	}
	
	file = ReadCompiledWord(file,word);
	if (!stricmp(word,"raw")) raw = true; // original rule-based pos results not lost anything?
	if (!stricmp(word,"basic")) basic = true;

	char* buffer = AllocateBuffer();
	char tags[MAX_SENTENCE_LENGTH][20];
	char tokens[MAX_SENTENCE_LENGTH][100];
	char mytags[MAX_SENTENCE_LENGTH][200];
	char prior[MAX_WORD_SIZE];
	unsigned int len;
	unsigned int right = 0;
	unsigned int total = 0;
	unsigned int sentences = 0;
	quotationInProgress = 0;	
	prepareMode = PENN_MODE;
	unsigned int totalAmbigs = 0;
	unsigned int ambigItems = 0;
	while (ReadALine(readBuffer,in))
	{
		char* at = buffer;
		*at = 0;
		char word[MAX_WORD_SIZE];
		char* ptr = SkipWhitespace(readBuffer);
		if (!*ptr || *ptr == '#') continue;
		if (!strnicmp(ptr,":exit",5)) break;
		if (*ptr == ':') 
		{
			char output[MAX_WORD_SIZE];
			DoCommand(ptr,output);
			continue;
		}
		len = 0;
		while (ptr && *ptr)
		{
			ptr = ReadCompiledWord(ptr,word);
			if (!*word) break;
			char* sep = strrchr(word,'/'); // find last one (there might be \/  when they actually want token
			if (!sep)
			{
				printf("Failed %s\r\n",readBuffer);
				break;
			}
			*sep = 0;
			++len;

			// recode \/ and its ilk
			char word1[MAX_WORD_SIZE];
			strcpy(word1,word);
			char* sep1;
			while ((sep1 = strchr(word1,'\\'))) memmove(sep1,sep1+1,strlen(sep1)+1);

			strcpy(tokens[len],word1);

			if (!stricmp(word1,"-LRB-")) strcat(at,"(");
			else if (!stricmp(word1,"-RRB-")) strcat(at,")");
			else if (!stricmp(word1,"-LSB-")) strcat(at,"[");
			else if (!stricmp(word1,"-RSB-")) strcat(at,"]");
			else if (!stricmp(word1,"-LCB-")) strcat(at,"{");
			else if (!stricmp(word1,"-RCB-")) strcat(at,"}");
			else if (*word1 == '`' && word1[1] == '`') strcat(at,"\"");  // open quote
			else if (*word1 == '\'' && word1[1] == '\'') strcat(at,"\""); // close quote
			else strcat(at,word1);

			at += strlen(at);
			strcat(at++," ");
			strcpy(tags[len],sep+1); // what we expect
		}
		if (len == 0) continue; // on to next

		*at = 0;
		total += len;
		if (*tags[len] == '.' || *tags[len] == '?' || *tags[len] == '!') 
		{
			++right;	 // end punctuation is always right
			--len;
		}
		// test this sentence
		char* answer1;
		tokenControl = STRICT_CASING | DO_ESSENTIALS | DO_POSTAG | DO_CONTRACTIONS | NO_HYPHEN_END | NO_COLON_END | NO_SEMICOLON_END | TOKEN_AS_IS | NO_MARK;
		if (!raw && !basic) tokenControl |= DO_PARSE;
		ReturnToFreeze();
		PrepareSentence(buffer,true,true);	
		++sentences;
		answer1 = DumpAnalysis(1,wordCount,posValues,"Tagged POS",false,true); // to debug at
		if (basic) 
		{
			bool bad = false;
			for (unsigned int i = 1; i <= wordCount; ++i) if (bitCounts[i] != 1) bad = true;
			if (bad) Log(STDUSERLOG,"%s\r\n",answer1);
			continue;
		}
		char* xxhold = answer1; // for debugging
		answer1 = strchr(answer1,':') + 1;
		unsigned int a = 0;
		while (answer1 && *answer1)
		{
			char* close = strstr(answer1,")  ");
			if (!close) break;
			close[1] = 0;
			strcpy(mytags[++a],answer1);
			close[1] =  ' ';
			answer1 = close+3;
		}
		if (answer1) strcpy(mytags[++a],answer1); // any remnant
		unsigned int oldRight = right;
		if ((a-1) != wordCount)
		{
			Log(STDUSERLOG,"Tag MisCount: %d instead of %d %s \r\n",a,wordCount,buffer);
			while (++a <= wordCount) *mytags[a] = 0;
		}

		if (len != wordCount) 
		{
			Log(STDUSERLOG,"MisCount: %d %s \r\n",currentFileLine,buffer);
		}
		strcpy(prior,buffer);
		for (unsigned int i = 1; i <= wordCount; ++i) // match off the pos values we understand. all others are wrong by definition
		{
			unsigned int ok = right;
retry:
			char* sep = strchr(tags[i],'|');
			if (sep) *sep = 0;

			if (bitCounts[i] != 1)
			{
				totalAmbigs += bitCounts[i];
				++ambigItems;
			}

			if (bitCounts[i] != 1 && (tokenControl & DO_PARSE) == DO_PARSE  ) // did not solve - we normally do all
			{
				Log(STDUSERLOG,"Ambiguous at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			} 
			else if (!stricmp(tags[i],"-LRB-"))
			{
				if (*wordStarts[i] == '(') ++right;
				else Log(STDUSERLOG,"** Bad left paren at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"-RRB-"))
			{
				if (*wordStarts[i] == ')') ++right;
				else Log(STDUSERLOG,"** Bad right paren at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"-LSB-"))
			{
				if (*wordStarts[i] == '[') ++right;
				else Log(STDUSERLOG,"** Bad left square bracket at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"-RSB-"))
			{
				if (*wordStarts[i] == ']') ++right;
				else Log(STDUSERLOG,"** Bad right square bracket at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"-LCB-"))
			{
				if (*wordStarts[i] == '{') ++right;
				else Log(STDUSERLOG,"** Bad left curly bracket at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"-RCB-"))
			{
				if (*wordStarts[i] == '}') ++right;
				else Log(STDUSERLOG,"** Bad right curly bracket at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (posValues[i] & IDIOM) ++right;
			else if (!stricmp(tags[i],"TO")) ++right;	// always correct
			else if (!stricmp(tags[i],"NN")) 
			{
				if (posValues[i] & (NOUN_SINGULAR | ADJECTIVE_NOUN | NOUN_NUMBER)  && allOriginalWordBits[i] & (NOUN_SINGULAR|NOUN_NUMBER)) ++right;
				else if (posValues[i] & ADJECTIVE_PARTICIPLE && allOriginalWordBits[i] & NOUN_GERUND) ++right; // *drinking straws
				else if (posValues[i] & NOUN_SINGULAR) ++right; // they doubtless dont know it should be lower case
				else if (posValues[i] & ADJECTIVE_NORMAL && allOriginalWordBits[i] & NOUN_SINGULAR) ++right;  //"*expert aim"
				else if (posValues[i] & NOUN_PROPER_SINGULAR) ++right; // "*Pill bugs are good"
				else if (posValues[i] & NOUN_GERUND && allOriginalWordBits[i] & NOUN_SINGULAR) ++right; // "*spitting is good"
				else if (posValues[i] & PRONOUN_BITS) ++right; // someone, anyone, etc
				else Log(STDUSERLOG,"** Bad NN (singular) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"NNS")) 
			{
				if (posValues[i] & (NOUN_PLURAL| ADJECTIVE_NOUN)  && allOriginalWordBits[i] & NOUN_PLURAL) ++right;
				else if (posValues[i] & NOUN_NUMBER && canSysFlags[i] & MODEL_NUMBER) ++right; // model numbers
				else if (posValues[i] & NOUN_PROPER_PLURAL) ++right; // they get it wrong
				else if (posValues[i] & PRONOUN_BITS) ++right; // others
				else if (posValues[i] & NOUN_SINGULAR && allOriginalWordBits[i] & (NOUN_MASS | NOUN_PLURAL)) ++right;
				else Log(STDUSERLOG,"** Bad NNS (plural) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"NNP")) // proper singular
			{
				uint64 val;
				if (posValues[i] & (NOUN_PROPER_SINGULAR | ADJECTIVE_NOUN) && allOriginalWordBits[i] & (NOUN_PROPER_SINGULAR | NOUN_SINGULAR)) ++right;
				else if (posValues[i] & NOUN_PROPER_PLURAL && allOriginalWordBits[i] & NOUN_PROPER_SINGULAR) ++right; // we just picked the other side
				else if (posValues[i] & NOUN_NUMBER && canSysFlags[i] & MODEL_NUMBER) ++right; // model numbers
				else if (posValues[i] & NOUN_NUMBER && IsRomanNumeral(wordStarts[i],val)) ++right; //  roman numerals
				else if (posValues[i] & ADJECTIVE_NORMAL && IsUpperCase(*wordStarts[i])) ++right; // things like French can be adjective or noun, we often call them adjectives instead of adjective_noun
				else if (posValues[i] & ADJECTIVE_NORMAL && allOriginalWordBits[i] & NOUN_PROPER_SINGULAR) ++right; // things like French can be adjective or noun, we often call them adjectives instead of adjective_noun
				else if (posValues[i] & NOUN_PROPER_PLURAL) ++right; // if it ended in s like Atomos.
				else if (posValues[i] & NOUN_SINGULAR) ++right; // "Bear had to eat a lot in raw mode
				else Log(STDUSERLOG,"** Bad NNP (propersingular) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"NNPS"))  // proper plural
			{
				if (posValues[i] & (NOUN_PROPER_PLURAL| ADJECTIVE_NOUN) && allOriginalWordBits[i] & NOUN_PROPER_PLURAL) ++right;
				else if (posValues[i] & NOUN_PROPER_SINGULAR && allOriginalWordBits[i] & NOUN_PROPER_PLURAL) ++right; // we just picked the other side
				else if (posValues[i] & NOUN_PLURAL) ++right;
				else Log(STDUSERLOG,"** Bad NNPS (properplural) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"IN")) 
			{
				if (posValues[i] & (CONJUNCTION_SUBORDINATE|PREPOSITION)) ++right;
				else if (posValues[i] & PARTICLE && allOriginalWordBits[i] & PREPOSITION) ++right; 
				else Log(STDUSERLOG,"** Bad IN at %d: %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"PDT")) 
			{
				if (posValues[i] & PREDETERMINER) ++right;
				else Log(STDUSERLOG,"** Bad PDT at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"POS")) 
			{
				if (posValues[i] & POSSESSIVE) ++right;
				else Log(STDUSERLOG,"** Bad POS at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"LS")) // bullet point
			{
				Log(STDUSERLOG,"** Bad LS at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"CC")) 
			{
				if (posValues[i] & CONJUNCTION_COORDINATE) ++right;
				else Log(STDUSERLOG,"** Bad CC at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"JJ")) 
			{
				if (posValues[i] & (ADJECTIVE_NORMAL|NOUN_NUMBER)) ++right;
				else if (posValues[i] & ADJECTIVE_NUMBER && !(lcSysFlags[i] & (MORE_FORM|MOST_FORM))) ++right;
				else if (posValues[i] & NOUN_SINGULAR && allOriginalWordBits[i] & NOUN_GERUND  && allOriginalWordBits[i] & ADJECTIVE_NORMAL) ++right; // " *melting point" 
				else if (posValues[i] & NOUN_PROPER_SINGULAR) ++right; // " *Western boots" 
				else if (posValues[i] & (ADJECTIVE_PARTICIPLE | ADJECTIVE_NOUN)) ++right; // "I am *tired"  "*pill bugs eat"
				else if (posValues[i] & NOUN_GERUND ) ++right; // "he got me *moving"
				else if (posValues[i] & NOUN_SINGULAR && posValues[i+1] & (ADJECTIVE_BITS|NOUN_BITS) && (tokenControl & DO_PARSE) != DO_PARSE ) ++right; // "the *bank teller" when using RAW mode or "*money_market mutual funds"
				else if (posValues[i] & DETERMINER) ++right; // "*Other people"
				else if (posValues[i] & PARTICLE) ++right; // "take it for *granted"
				else if (posValues[i] & VERB_PRESENT_PARTICIPLE && allOriginalWordBits[i] & ADJECTIVE_BITS) ++right;	// she is *willing to go"
				else Log(STDUSERLOG,"** Bad JJ at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"JJR")) 
			{
				if (originalLower[i]  && posValues[i] & ADJECTIVE_NORMAL && lcSysFlags[i] & MORE_FORM) ++right;
				else if ( posValues[i] & DETERMINER && lcSysFlags[i] & MORE_FORM) ++right;
				else Log(STDUSERLOG,"** Bad JJR at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"JJS")) 
			{
				if (originalLower[i] && posValues[i] & ADJECTIVE_NORMAL && lcSysFlags[i] & MOST_FORM) ++right;
				else if ( posValues[i] & DETERMINER && lcSysFlags[i] & MOST_FORM) ++right;
				else Log(STDUSERLOG,"** Bad JJS at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"RB")) 
			{
				if (posValues[i] & ADVERB_NORMAL) ++right;
				else  if (posValues[i] & PARTICLE && allOriginalWordBits[i] & ADVERB_BITS)  ++right;
				else Log(STDUSERLOG,"** Bad RB at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"RBR")) 
			{
				if (originalLower[i]  && posValues[i] & ADVERB_NORMAL && lcSysFlags[i] & MORE_FORM) ++right;
				else Log(STDUSERLOG,"** Bad RBR at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"RBS")) 
			{
				if (originalLower[i]  && posValues[i] & ADVERB_NORMAL && lcSysFlags[i] & MOST_FORM) ++right;
				else Log(STDUSERLOG,"** Bad RBS at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
		    }
			else if (!stricmp(tags[i],"UH")) 
			{
				if (posValues[i] & INTERJECTION) ++right;
				else if (wordCount == 1) ++right;	// anything COULD be...
				else Log(STDUSERLOG,"** Bad UH at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
		    }
			else if (!stricmp(tags[i],"MD")) 
			{
				if (posValues[i] & AUX_VERB_BITS) ++right;
				else Log(STDUSERLOG,"** Bad MD at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"RP")) 
			{
				if (posValues[i] & PARTICLE) ++right;
				else if (posValues[i] & ADVERB_NORMAL) ++right; // who can say if ideomatic particle verb or adverb.... 
				else Log(STDUSERLOG,"** Bad RP at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"DT")) 
			{
				if (posValues[i] & DETERMINER_BITS) ++right; // a an the // my her their our your
				else if (posValues[i] & ADJECTIVE_NUMBER) ++right;	// all numbers to us as adjectives might be considered determiners ?????
				else if (!stricmp(wordStarts[i],"this") && posValues[i] & PRONOUN_BITS) ++right; 
				else if (!stricmp(wordStarts[i],"that") && posValues[i] & PRONOUN_BITS) ++right;
				else if (!stricmp(wordStarts[i],"those") && posValues[i] & PRONOUN_BITS) ++right;
				else if (posValues[i] & ADVERB_BITS && posValues[i+1] & PREPOSITION) ++right; // "he walked *all by himself"
				//else if (!stricmp(wordStarts[i],"every") || !stricmp(wordStarts[i],"no") || !stricmp(wordStarts[i],"another")
				//	 || !stricmp(wordStarts[i],"any") || !stricmp(wordStarts[i],"some")
				else  Log(STDUSERLOG,"** Bad DT at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"PRP$")) 
			{
				if (posValues[i] & PRONOUN_POSSESSIVE) ++right;
				else Log(STDUSERLOG,"** Bad PRP$ at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"PRP")) 
			{
				if (posValues[i] & (PRONOUN_BITS)) ++right;
				else Log(STDUSERLOG,"** Bad PRP at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"VB")) // infinitive
			{
				if (posValues[i] & (NOUN_INFINITIVE|VERB_INFINITIVE)) ++right;  
				else if (posValues[i] & AUX_VERB_BITS && allOriginalWordBits[i] &  VERB_INFINITIVE) ++right;  // includes our modals 
				else Log(STDUSERLOG,"** Bad VB (infinitive) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"VBD")) // past
			{
				if (posValues[i] & VERB_PAST || (posValues[i] & AUX_VERB_BITS &&  allOriginalWordBits[i] &  VERB_PAST) ) ++right;  // includes our modals that can have this tense as verbs
				else Log(STDUSERLOG,"** Bad VBD (past) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"VBG"))  // gerund present participle
			{
				if (allOriginalWordBits[i] & (VERB_PRESENT_PARTICIPLE|NOUN_GERUND)) ++right;  // includes our modals that can have this tense as verbs
				else  Log(STDUSERLOG,"** Bad VBG (present participle) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
		    }
			else if (!stricmp(tags[i],"VBN")) // past particple
			{
				if (posValues[i] & VERB_PAST_PARTICIPLE || ( posValues[i] & AUX_VERB_BITS && allOriginalWordBits[i] & VERB_PAST_PARTICIPLE)) ++right;  // includes our modals that can have this tense as verbs
				else if (posValues[i] & ADJECTIVE_PARTICIPLE && allOriginalWordBits[i] & VERB_PAST_PARTICIPLE) ++right;
				else  Log(STDUSERLOG,"** Bad VBN (past participle) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"VBP")) // present
			{
				if (posValues[i] & VERB_PRESENT || (posValues[i] & AUX_VERB_BITS && allOriginalWordBits[i]  &  VERB_PRESENT)) ++right;  // includes our modals that can have this tense as verbs
				else Log(STDUSERLOG,"** Bad VBP (present) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"VBZ")) // 3ps
			{
				if (posValues[i] & VERB_PRESENT_3PS || (posValues[i] & AUX_VERB_BITS && allOriginalWordBits[i]  &  VERB_PRESENT_3PS)) ++right; // includes our modals that can have this tense as verbs
				else Log(STDUSERLOG,"** Bad VBZ (present 3ps) at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"WDT")) 
			{
				if (!stricmp(wordStarts[i],"that") || !stricmp(wordStarts[i],"what") ||!stricmp(wordStarts[i],"whatever") ||!stricmp(wordStarts[i],"which") ||!stricmp(wordStarts[i],"whichever"))
				{ 
					if (posValues[i] & (DETERMINER|PRONOUN_BITS|CONJUNCTION_SUBORDINATE)) ++right; // what dog is that
					else Log(STDUSERLOG,"** Bad WDT at %d: %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
				}
				else Log(STDUSERLOG,"** Bad WDT at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"WP")) 
			{
				// that may be WDT
				if ( !stricmp(wordStarts[i],"what") || !stricmp(wordStarts[i],"who") || !stricmp(wordStarts[i],"whom"))
				{ // that whatever which WDT - whatsoever RB -  whosoever NN
					if (posValues[i] & (PRONOUN_BITS|CONJUNCTION_SUBORDINATE | DETERMINER | PREDETERMINER)) ++right; // what is that
					else Log(STDUSERLOG,"** Bad WP at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
				}
				else Log(STDUSERLOG,"** Bad WP at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"WP$")) 
			{
				if (!stricmp(wordStarts[i],"whose"))
				{
					if (posValues[i] & (PRONOUN_POSSESSIVE | DETERMINER)) ++right; // whose dog is that -- do we do both? or only one?
					else Log(STDUSERLOG,"** Bad WP$ at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
				}
				else Log(STDUSERLOG,"** Bad WP$ at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"WRB")) 
			{
				if (!stricmp(wordStarts[i],"how")  ||!stricmp(wordStarts[i],"whenever") ||!stricmp(wordStarts[i],"when") ||!stricmp(wordStarts[i],"where")
					||!stricmp(wordStarts[i],"whereby")||!stricmp(wordStarts[i],"wherein")||!stricmp(wordStarts[i],"why"))
				{
					// the ONLY exception is  "when" meaning "if" should be IN.  
					++right; 
					// however, whence, wherever, whereof are NOT wrb?
				}
				else Log(STDUSERLOG,"** Bad WRB at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!IsAlpha(*tags[i])) 
			{
				++right;	// all punctuation must be right
			}
			else if (!stricmp(tags[i],"CD")) 
			{
				if (posValues[i] & (NOUN_NUMBER | ADJECTIVE_NUMBER)) ++right;
				else Log(STDUSERLOG,"** Bad CD at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"EX")) 
			{
				if (posValues[i]  &  THERE_EXISTENTIAL) ++right;
				else Log(STDUSERLOG,"** Bad EX at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!stricmp(tags[i],"FW")) 
			{
				if (strstr(mytags[i],"unknown-word") || allOriginalWordBits[i] & FOREIGN_WORD) ++right;
				else Log(STDUSERLOG,"** Bad FW at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);
			}
			else if (!sep) Log(STDUSERLOG,"** Bad Unknown tag at %d: line %d %s %s %s\r\n",i,currentFileLine,mytags[i],tags[i],buffer);

			// composite choices
			if (sep && right == ok) // didn't match it yet
			{
				memmove(tags[i],sep+1,strlen(sep+1) + 1);
				goto retry;
			}
		}
		if ((tokenControl & DO_PARSE) == DO_PARSE ) 
		{
			if ((right-oldRight) != wordCount){;} // pos is bad so parse is by definition bad
			else if (tokenFlags & FAULTY_PARSE && !(tokenFlags & NOT_SENTENCE)) Log(STDUSERLOG,"     ** bad parse at %d: %s\r\n",currentFileLine,buffer);

			// verify plurality and determined
			unsigned int subject = 0;
			for (unsigned int i = startSentence; i <= endSentence; ++i)
			{
				if (roles[i] & (SUBJECT2|MAINSUBJECT)) subject = i;
				if (roles[i] & (VERB2|MAINVERB))
				{
					if (subject && posValues[subject] == NOUN_SINGULAR && posValues[i] == VERB_PRESENT && !(originalLower[i]->properties & NOUN_MASS)) Log(STDUSERLOG,"*** Warning singular noun %s to plural verb %s in %s\r\n",wordStarts[subject],wordStarts[i],buffer);
					if (subject && posValues[subject] == NOUN_PLURAL && posValues[i] == VERB_PRESENT_3PS) Log(STDUSERLOG,"*** Warning plural noun %s to singular verb %s in %s\r\n",wordStarts[subject],wordStarts[i],buffer);
					if (subject && posValues[subject] == PRONOUN_BITS && posValues[i] == VERB_PRESENT && !(lcSysFlags[i] & PRONOUN_SINGULAR)) Log(STDUSERLOG,"*** Warning singular pronoun %s to plural verb %s in %s\r\n",wordStarts[subject],wordStarts[i],buffer);
					if (subject && posValues[subject] == PRONOUN_BITS && posValues[i] == VERB_PRESENT_3PS && lcSysFlags[i] & PRONOUN_SINGULAR) Log(STDUSERLOG,"*** Warning plural pronoun %s to singular verb %s in %s\r\n",wordStarts[subject],wordStarts[i],buffer);
					subject = 0;
				}
				if (roles[i] & (SUBJECT2|MAINSUBJECT|OBJECT2|MAINOBJECT|INDIRECTOBJECT2|MAININDIRECTOBJECT) && posValues[i] & NOUN_SINGULAR && originalLower[i] && !(originalLower[i]->properties & (NOUN_MASS|PRONOUN_BITS)) )
				{
					unsigned int det;
					if (!IsDeterminedNoun(i,det)) Log(STDUSERLOG,"*** Warning undetermined noun %s in %s\r\n",wordStarts[i],buffer);
				}
			}
		}
	}
	fclose(in);
	FreeBuffer();
	float percent = ((float)right * 100) /total;
	int val = (int)percent;
	float percent1 = ((float)ambigItems * 100) /total;
	int val1 = (int)percent1;
	Log(STDUSERLOG,"right:%d wrong:%d total:%d percent:%d sentences:%d  ambig:%d percent:%d counts:%d\r\n",right,total-right,total,val,sentences,ambigItems, val1, totalAmbigs);
}

static void C_PennNoun(char* file)
{
	char word[MAX_WORD_SIZE];
	file = ReadCompiledWord(file,word);
	char filename[MAX_WORD_SIZE];
	if (*word) sprintf(filename,"REGRESS/PENNTAGS/%s.txt",word);
	else strcpy(filename,"REGRESS/PENNTAGS/penn.txt");
	FILE* in = FopenReadOnly(filename);
	if (!in) return;
	char* buffer = AllocateBuffer();
	char tags[MAX_SENTENCE_LENGTH][20];
	char tokens[MAX_SENTENCE_LENGTH][100];
	unsigned int len;
	while (ReadALine(readBuffer,in))
	{
		char* at = buffer;
		*at = 0;
		char word[MAX_WORD_SIZE];
		char* ptr = SkipWhitespace(readBuffer);
		if (!*ptr || *ptr == '#') continue;
		len = 0;
		while (ptr && *ptr)
		{
			ptr = ReadCompiledWord(ptr,word);
			if (!*word) break;
			char* sep = strrchr(word,'/'); // find last one (there might be \/  when they actually want token
			if (!sep)
			{
				printf("Failed %s\r\n",readBuffer);
				break;
			}
			*sep = 0;
			++len;

			// recode quotes (opening and closing)
			if (*word == '`' && word[1] == '`') strcpy(word,"\"");
			if (*word == '\'' && word[1] == '\'') strcpy(word,"\"");
			// recode \/ and its ilk
			char word1[MAX_WORD_SIZE];
			strcpy(word1,word);
			char* sep1;
			while ((sep1 = strchr(word1,'\\'))) memmove(sep1,sep1+1,strlen(sep1)+1);

			strcpy(tokens[len],word1);
			strcat(at,word1);
			at += strlen(at);
			strcat(at++," ");
			strcpy(tags[len],sep+1); // what we expect
		}
		if (len == 0) continue; // on to next

		*at = 0;
		for (unsigned int i = 1; i <= len; ++i) // match off the pos values we understand. all others are wrong by definition
		{
			char* sep = strchr(tags[i],'|');
			if (sep) *sep = 0;

			if (!stricmp(tags[i],"NN")) // found a noun, look backwards...
			{
				if (!strnicmp(tags[i+1],"NN",2)) continue;	 // noun follows us. he must be determined instead
				for (unsigned int x = i-1; x >= 1; --x)
				{
					if (!stricmp(tokens[x],",")) break;	 // immediately after comma may be appositive "Bob, dog of my dreams
					if (!stricmp(tokens[x],"of")) break;	 // can say of xxx always as in type of dog
					if (!stricmp(tags[x],"CC")) break;	// assume guy before is determeined
					if (!stricmp(tags[x],"DT")) break;	// it is determined
					if (!stricmp(tags[x],"POS")) break;	// is owned
					if (tags[x][0] == 'N') continue;	// it is joined noun.
					if (*tags[x] == 'J') continue; // adj
					if (!stricmp(tags[x],"PRP$")) break; // word after conjunct
					WORDP D = FindWord(tokens[i]);
					if (D && D->properties & NOUN_MASS)
						break;
					if (IsUpperCase(*tokens[i])) break; // actually not NN
					if (D && !IsAlpha(*D->word)) break;	 // not a normal word

					if (D && D->properties & NOUN_NODETERMINER)
						break;
					Log(STDUSERLOG,"%s: %s %s  %s\r\n",tokens[i],tags[x],tokens[x], buffer); // unxpected
					break;
	
				}
			}
		}
	}
	fclose(in);
	FreeBuffer();
}

static void C_VerifyPos(char* file)
{
	if (!*file) file = "REGRESS/postest.txt";
	FILE* in = FopenReadOnly(file);
	if (!in) return;

#ifdef WIN32
	unsigned int start = GetTickCount();
#endif
	
	prepareMode = POSVERIFY_MODE;
	uint64 oldtokencontrol = tokenControl;
	tokenControl =  DO_PARSE | DO_ESSENTIALS| DO_CONTRACTIONS | DO_BRITISH  | STRICT_CASING | DO_NUMBER_MERGE | DO_PROPERNAME_MERGE; 
	unsigned int tokens = 0;
	unsigned int count = 0;
	unsigned int fail = 0;
	char sentence[MAX_WORD_SIZE];
	while (ReadALine(readBuffer,in))
	{
		char* ptr =  SkipWhitespace(readBuffer);
		if (!strnicmp(ptr,"#END",3)) break;
		if (!*ptr || *ptr == '#') continue;
		// debug command
		if (*ptr == ':' && IsAlpha(ptr[1]))
		{
			char output[MAX_WORD_SIZE];
			DoCommand(ptr,output);
			continue;
		}
		ReturnToFreeze(); // dont let dictionary tamper affect this. A problem with ANY multiple sentence input...
	
		++count;
		strcpy(sentence,ptr);
		PrepareSentence(sentence,true,true);
		tokens += wordCount;
		char parseForm[MAX_WORD_SIZE * 5];
		*parseForm = 0;
		char liveParse[MAX_WORD_SIZE * 5];
		*liveParse = 0;
		strcpy(liveParse,DumpAnalysis(1,wordCount,posValues,"Parsed POS",false,true));
		TrimSpaces(liveParse,false);
		while (ReadALine(readBuffer,in))
		{
			char* start = SkipWhitespace(readBuffer);
			if (!*start || *start == '#') continue;
			if (!strnicmp(start,"Parsed",6)) 
			{
				strcpy(parseForm,TrimSpaces(start,false)); 
				break;
			}
		}

		if (strcmp(parseForm,liveParse))
		{
			size_t i;
			for (i = 0; i < strlen(parseForm); ++i)
			{
				if (parseForm[i] != liveParse[i]) break;
				if (!parseForm[i] || !liveParse[i]) break;
			}
			while (i && parseForm[--i] != '(');	// find start backwards
			if (i) --i;
			while (i && parseForm[--i] != ' ');	
			char hold[BIG_WORD_SIZE];
			strcpy(hold,parseForm+i);
			parseForm[i] = 0;
			strcat(parseForm,"\r\n--> ");
			strcat(parseForm,hold);
			char hold1[BIG_WORD_SIZE];
			*hold1 = 0;
			size_t len = strlen(liveParse);
			if ( len > i) strcpy(hold1,liveParse+i);
			liveParse[i] = 0;
			strcat(liveParse,"\r\n--> ");
			strcat(liveParse,hold1);
			Log(STDUSERLOG,"\r\nMismatch at %d: %s\r\n",count,sentence);
			Log(STDUSERLOG,"          got: %s\r\n",liveParse);
			Log(STDUSERLOG,"         want: %s\r\n",parseForm);
			int old = trace;
			trace |= TRACE_POS;
			PrepareSentence(sentence,true,true);
			trace = old;
			++fail;
		}
	}

	fclose(in);

#ifdef WIN32
	Log(STDUSERLOG,"%d sentences tested, %d failed doing %d tokens in %d ms\r\n",count,fail,tokens, GetTickCount() - start);
#else
	Log(STDUSERLOG,"%d sentences tested, %d failed.\r\n",count,fail);
#endif
	tokenControl = oldtokencontrol;
	prepareMode = 0; 
}

static void C_TimePos(char* file) // how many wps for pos tagging
{
	if (!*file) file = "RAWDICT/postiming.txt";
	FILE* in = fopen(file,"rb");
	if (!in) return;
	prepareMode = POSTIME_MODE;
	uint64 oldtokencontrol = tokenControl;
	tokenControl = DO_PARSE | DO_SUBSTITUTE_SYSTEM  | DO_NUMBER_MERGE | DO_PROPERNAME_MERGE ;
	posTiming = 0;
	unsigned int words = 0;
	while (ReadALine(readBuffer,in))
	{
		char* ptr =  SkipWhitespace(readBuffer);
		if (!*ptr || *ptr == '#') continue;
		if (!strnicmp(ptr,"Tagged",6)) continue; 
		PrepareSentence(ptr,true,true);
		words += wordCount;
	}

	fclose(in);
	float wps = (float)words / ((float)posTiming/(float)1000.0);
	Log(STDUSERLOG,"%d words tagged in %d ms wps: %d.\r\n",words,posTiming, (unsigned int) wps);
	tokenControl = oldtokencontrol;
	prepareMode = 0; 
}

static void C_VerifySpell(char* file) // test spell checker against a file of entries  wrong-spell rightspell like livedata/spellfix.txt
{ 
	FILE* in = fopen(file,"rb");
	if (!in) return;
	unsigned int right = 0;
	unsigned int wrong = 0;
	while (ReadALine(readBuffer,in))
	{
		// pull out the wrong and right words
		char wrongWord[MAX_WORD_SIZE];
		char rightWord[MAX_WORD_SIZE];
		char* ptr = SkipWhitespace(readBuffer);
		if (*ptr == 0 || *ptr == '#' || *ptr == '<' || *ptr == '\'' || IsDigit(*ptr)) continue; // unusual stuff
		ptr = ReadCompiledWord(ptr,wrongWord);
		if (strchr(wrongWord,'>') || strchr(wrongWord,'.') || strchr(wrongWord,',')) continue;  // unusual stuff
		ReadCompiledWord(ptr,rightWord);
		if (!*rightWord || strchr(rightWord,'+') || *rightWord == '~'  || *rightWord == '%') continue;  // unusual stuff
		
		WORDP D = FindWord(wrongWord);
		if (D && D->properties & (PART_OF_SPEECH|FOREIGN_WORD)) // already has a meaning
		{
			Log(STDUSERLOG,"%s already in dictionary\r\n",wrongWord);
			continue;
		}

		char* fix = SpellFix(wrongWord,1,PART_OF_SPEECH); 
		if (fix && !strcmp(fix,rightWord)) ++right;
		else
		{
			Log(STDUSERLOG,"%s wanted %s but got %s\r\n",wrongWord,rightWord,fix);
			++wrong;
		}
	}

	fclose(in);
	Log(STDUSERLOG,"Right:%d  Wrong:%d\r\n",right,wrong);
}

static void VerifySubstitutes1(WORDP D, uint64 unused)
{
	if (!(D->internalBits & HAS_SUBSTITUTE)) return;

	char expectedText[MAX_WORD_SIZE];
	char resultText[MAX_WORD_SIZE];
	*readBuffer = 0;
	unsigned int n;

	//   see if word has start or end markers. Remove them.
	bool start = false;
	if (*D->word == '<')
	{
		start = true;
		n = BurstWord(D->word+1);
	}
	else n = BurstWord(D->word);
	bool end = false;
	char* last = GetBurstWord(n-1);
	size_t len = strlen(last);
	if (last[len-1] == '>')
	{
		end = true;
		last[len-1] = 0;
	}

	//   now composite an example, taking into account start and end markers
	unsigned int i;
	if (!start) strcat(readBuffer,"x ");	//   so match is not at start
	for (i = 0; i < n; ++i)
	{
		strcat(readBuffer,GetBurstWord(i));
		strcat(readBuffer," ");
	}
	if (!end) strcat(readBuffer,"x "); //   so match is not at end

	//   generate what it results in
	PrepareSentence(readBuffer,true,true);

	*resultText = 0;
	if (!end) --wordCount;	//   remove the trailing x
	for (i = 1; i <= wordCount; ++i) //   recompose what tokenize got
	{
		if (!start && i == 1) continue;	//   remove the leading x
		strcat(resultText,wordStarts[i]);
		strcat(resultText," ");
	}

	WORDP S = GetSubstitute(D);
	if (!S && wordCount == 0) return;	//   erased just fine
	if (!S) Log(STDUSERLOG,"Substitute failed: %s didn't erase itself, got %s\r\n",D->word,resultText);
	else
	{
		strcpy(expectedText,S->word);
		strcat(expectedText," ");	//   add the trailing blank we get from concats above
		char* at;
		while ((at = strchr(expectedText,'+'))) *at = ' '; //   break up answer
		if (!stricmp(resultText,expectedText)) return;	//   got what was expected
		Log(STDUSERLOG,"Substitute failed: %s got %s not %s\r\n",D->word,resultText,expectedText);
	}
}

static void C_VerifySubstitutes(char* ptr) //   see if substitutes work...
{
	WalkDictionary(VerifySubstitutes1);
}

/////////////////////////////////////////////////////
/// SYSTEM CONTROL COMMANDS
/////////////////////////////////////////////////////

static void C_Bot(char* name)
{
	MakeLowerCopy(computerID,name);
	computerIDwSpace[0] = ' ';
	MakeLowerCopy(computerIDwSpace+1,computerID);
	strcat(computerIDwSpace," "); // trailing space
	int BOMvalue = -1; // get prior value
	char oldc;
	int oldCurrentLine;	
	BOMAccess(BOMvalue, oldc,oldCurrentLine); // copy out prior file access and reinit user file access
	ReadUserData(GetFileRead(loginID,computerID));
	BOMAccess(BOMvalue, oldc,oldCurrentLine); // restore old BOM values
	wasCommand = BEGINANEW;	// make system save revised user file
}


static void C_Build(char* input)
{
#ifndef DISCARDSCRIPTCOMPILER
	char oldlogin[MAX_WORD_SIZE];
	char oldbot[MAX_WORD_SIZE];
	char oldbotspace[MAX_WORD_SIZE];
	char oldloginname[MAX_WORD_SIZE];
	strcpy(oldlogin,loginID);
	strcpy(oldbot,computerID);
	strcpy(oldbotspace,computerIDwSpace);
	strcpy(oldloginname,loginName);
	char file[MAX_WORD_SIZE];
	char control[MAX_WORD_SIZE];
	input = ReadCompiledWord(input,file);
	input = SkipWhitespace(input);
	int spell = PATTERN_SPELL;
	bool reset = false;
	while (*input) 
	{
		input = ReadCompiledWord(input,control);
		if (!stricmp(control,"nospell")) spell = NO_SPELL;
		else if (!stricmp(control,"outputspell")) spell = OUTPUT_SPELL;
		else if (!stricmp(control,"reset")) reset = true;
	}
	size_t len = strlen(file);
	if (!*file) Log(STDUSERLOG,"missing build label");
	else
	{
		sprintf(logFilename,"USERS/build%s_log.txt",file); //   all data logged here by default
		FILE* in = FopenUTF8Write(logFilename);
		if (in) fclose(in);
		char word[MAX_WORD_SIZE];
		sprintf(word,"files%s.txt",file);
		if (ReadTopicFiles(word,(file[len-1] == '0') ? BUILD0 : BUILD1,spell)) 
		{
			if (!stricmp(computerID,"anonymous")) *computerID = 0;	// use default
			CreateSystem();
			systemReset = (reset) ? 2 : 1;
		}
	}
	// refresh current user data lost when we rebooted the system
	strcpy(loginID,oldlogin);
	strcpy(computerID,oldbot);
	strcpy(computerIDwSpace,oldbotspace);
	strcpy(loginName,oldloginname);
#endif
}  

static void C_Quit(char* input)
{
	Log(STDUSERLOG,"Exiting ChatScript via Quit\r\n");
	quitting = true;
}

static void C_Restart(char* input)
{
	trace = 0;
	if (*input) 
	{
		MakeUpperCopy(language,input);
		NoteLanguage(); // set default is most recent persona dictionary
	}
	ClearUserVariables();
	CloseSystem();
	ReadParams();	// default params associated with language form
	CreateSystem();
	InitStandalone();
	if (!server)
	{
		printf("\r\nEnter user name: ");
		ReadALine(inBuffer,stdin);
		printf("\r\n");
		echo = false;
		PerformChat(inBuffer,computerID,"",callerIP,outBuffer);
	}
	else Log(STDUSERLOG,"System restarted\r\n");
}

static void C_User(char* username)
{
	printf("\r\nEnter input: ");
	ReadALine(inBuffer,stdin);
	PerformChat(SkipWhitespace(username),computerID,inBuffer,callerIP,outBuffer);
}

///////////////////////////////////////////////
/// SERVER COMMANDS
///////////////////////////////////////////////

static void C_Flush(char* x)
{
	FlushCache();
}


///////////////////////////////////////////////////
/// WORD INFORMATION
///////////////////////////////////////////////////

static MEANING FindChild(MEANING who,int n)
{ // GIVEN SYNSET
    FACT* F = GetObjectHead(who);
	unsigned int index = Meaning2Index(who);
    while (F)
    {
        FACT* at = F;
        F = GetObjectNext(F);
        if (at->verb != Mis) continue;
		if (index && at->object  != who) continue;	// not us
        if (--n == 0)   return at->subject;
    }
    return 0;
} 

static void DrawDownHierarchy(MEANING T,unsigned int depth,unsigned int limit,bool sets)
{
	if (sets) limit = 1000;
    if (depth >= limit || !T) return;
    WORDP D = Meaning2Word(T);
	if (D->inferMark == inferMark) return;	

	D->inferMark = inferMark;
    unsigned int index = Meaning2Index(T);
    unsigned int size = GetMeaningCount(D);
    if (!size) size = 1; 

	if (*D->word == '~') // show set members
	{
		if (D->internalBits & HAS_EXCLUDE) MarkExclude(D);

		FACT* F = GetObjectHead(D);
		unsigned int i = 0;
		while (F)
		{
			if (F->verb == Mmember)
			{
				MEANING M = F->subject;
				WORDP S = Meaning2Word(M);
				if (S->inferMark != inferMark)
				{
					if (*S->word == '~' && (depth + 1) < limit) // expand to lower level
					{
						Log(STDUSERLOG,"\r\n");
						for (unsigned int j = 0; j < (depth*2); ++j) Log(STDUSERLOG," "); // depth inclusive because tabbing for next level
						Log(STDUSERLOG,"%s ",WriteMeaning(M)); // simple member
						DrawDownHierarchy(M,depth+1,limit,sets);
						Log(STDUSERLOG,"\r\n");
						for (unsigned int j = 0; j < (depth*2); ++j) Log(STDUSERLOG," ");
					}
					else Log(STDUSERLOG,"%s ",WriteMeaning(M)); // simple member
					if ( ++i >= 10)
					{
						Log(STDUSERLOG,"\r\n");
						for (unsigned int j = 0; j < (depth*2); ++j) Log(STDUSERLOG," ");
						i = 0;
					}
				}
			}
			F = GetObjectNext(F);
		}
		return;
	}

    for (unsigned int k = 1; k <= size; ++k) //   for each meaning of this dictionary word
    {
        if (index)
		{
			if (k != index) continue; //   not all, just one specific meaning
			T = GetMaster(GetMeaning(D,k)); 
		}
		else 
		{
			if (GetMeaningCount(D)) T = GetMaster(GetMeaning(D,k));
			else T = MakeMeaning(D); //   did not request a specific meaning, look at each in turn
		}

        //   for the current T meaning
		char* gloss = GetGloss(Meaning2Word(T),Meaning2Index(T));
		if (!gloss) gloss = "";
        if (depth++ == 0 && size)  Log(STDUSERLOG,"\r\n<%s.%d => %s %s\r\n",D->word,k,WriteMeaning(T),gloss); //   header for this top level meaning is OUR entry and MASTER
        int l = 0;
        while (++l) //   find the children of the meaning of T
        {
			MEANING child = (limit >= 1) ? FindChild(T,l) : 0; //   only headers sought
            if (!child) break;
			if (sets) //   no normal words, just a set hierarchy
			{
				WORDP D = Meaning2Word(child);
				if (*D->word != '~') continue;
			}

			 //   child and all syn names of child
            for (unsigned int j = 0; j <= (depth*2); ++j) Log(STDUSERLOG," "); 
   			gloss = GetGloss(Meaning2Word(child),Meaning2Index(child));
			if (!gloss) gloss = "";
			Log(STDUSERLOG,"%d. %sz %s\r\n",depth,WriteMeaning(child),gloss);
			DrawDownHierarchy(child,depth,limit,sets);
        } //   end of children for this value
        --depth;
    }
}

static void DumpConceptPath(MEANING T) // once you are IN a set, the path can be this
{
	int k = 0;
	while (++k)
	{
		MEANING parent = FindSetParent(T,k); //   next set we are member of
		if (!parent)  break;

		WORDP D = Meaning2Word(parent);	// topic or concept
		if (D->internalBits & HAS_EXCLUDE) // prove no violation
		{
			FACT* F = GetObjectHead(D);
			while (F)
			{
				if (F->verb == Mexclude)
				{
					WORDP E = Meaning2Word(F->subject);
					if (E->inferMark == inferMark) break;
				}
				F = GetObjectNext(F);
			}
			if (F) continue;	// exclusion in effect
		}
		WORDP E = Meaning2Word(parent);
		if (E->inferMark != inferMark) 
		{
			E->inferMark = inferMark;
			*meaningLimit++ = parent;
		}
	}
}

static void ShowConcepts(MEANING T)
{
 	MEANING parent;
	unsigned int count;
	WORDP E = Meaning2Word(T);
	unsigned int index = Meaning2Index(T);
    if (*E->word != '~' && index == 0)  // at a base word
	{
		DumpConceptPath(T); // what is it a member of direclty

		//   then do concepts based on this word...
		unsigned int size = GetMeaningCount(E);
		if (!size) size = 1;	//   always at least 1, itself
		//   immediate sets of this base
		for  (unsigned int k = 1; k <= size; ++k)
		{
			if (index && k != index) continue; //   not all, just correct meaning

			//   get meaningptr spot facts are stored (synset head)
			if (!GetMeaningCount(E) ) T = MakeMeaning(E);	//   a generic since we have no meanings
			else 
			{
				if (GetMeaning(E,k) & SYNSET_MARKER) T = MakeMeaning(E,k); // we are master
				else T = GetMaster(GetMeaning(E,k)) | (GetMeaning(E,k) & TYPE_RESTRICTION); 
			}
			DumpConceptPath(T); 
		}

		//   up one wordnet hierarchy based on each meaning
		for  (unsigned int k = 1; k <= size; ++k)
		{
			if (index && k != index) continue; //   not all, just correct meaning

			//   get meaningptr spot facts are stored (synset head)
			if (!GetMeaningCount(E) ) T = MakeMeaning(E);	//   a generic since we have no meanings
			else 
			{
				if (GetMeaning(E,k) & SYNSET_MARKER) T = MakeMeaning(E,k); // we are master
				else T = GetMaster(GetMeaning(E,k)) | (GetMeaning(E,k) & TYPE_RESTRICTION); 
			}
			count = 0;
			while ((parent =  FindSynsetParent(T,count++))) ShowConcepts(parent); // immediate wordnet hierarchy
		}
	}
	else if (index != 0) //    always synset nodes above the base
	{
		count = 0;
		while ((parent =  FindSynsetParent(T,count++))) DumpConceptPath(parent); // sets of next parent level up
		count = 0;
		while ((parent =  FindSynsetParent(T,count++))) ShowConcepts(parent); // and follow next parent level up
	}
	else  DumpConceptPath(T); // track this synset to the next level
}

static void C_Concepts(char* input)
{
	char word[MAX_WORD_SIZE];
	ReadCompiledWord(input,word);
	MEANING M = ReadMeaning(word,false);
	if (!M) return;
	M = GetMaster(M);
	Log(STDUSERLOG,"%s: ",word);
	NextinferMark();

	meaningList = (MEANING*) AllocateBuffer();
	meaningLimit = meaningList;

	// check substitutes
	WORDP D = Meaning2Word(M);
	if (D->internalBits & HAS_SUBSTITUTE)
	{
		D = GetSubstitute(D);
		if (*D->word == '~')  *meaningLimit++ = MakeMeaning(D); 
	}

	char alter[MAX_WORD_SIZE];
	sprintf(alter,"<%s",word);
	D = FindWord(alter);
	if (D && D->internalBits & HAS_SUBSTITUTE)
	{
		D = GetSubstitute(D);
		if (*D->word == '~')   *meaningLimit++ = MakeMeaning(D); 
	}
	
	sprintf(alter,"<%s>",word);
	D = FindWord(alter);
	if (D && D->internalBits & HAS_SUBSTITUTE)
	{
		D = GetSubstitute(D);
		if (*D->word == '~')   *meaningLimit++ = MakeMeaning(D); 
	}

	 *meaningLimit++ = M;

	// check concepts and topics
	while (meaningList < meaningLimit) 
	{
		WORDP E = Meaning2Word(*meaningList);
		if (*E->word == '~') Log(STDUSERLOG,(E->systemFlags & TOPIC) ? (char*) "T%s " : (char*) "%s ",E->word);
		ShowConcepts(*meaningList++);
	}
	Log(STDUSERLOG,"\n");

	FreeBuffer();
}

static void C_Down(char* input)
{
	char word[MAX_WORD_SIZE];
	input = ReadCompiledWord(input,word);
	input = SkipWhitespace(input);
    int limit = atoi(input);
    if (!limit) limit = 1; //   top 2 level only (so we can see if it has a hierarchy)
	input = SkipWhitespace(input);
	NextinferMark();
	MEANING M = ReadMeaning(word,false);
	M = GetMaster(M);
    DrawDownHierarchy(M,1,limit+1,!stricmp(input,"sets"));
	Log(STDUSERLOG,"\r\n");
}

static void FindXWord(WORDP D, uint64 pattern)
{
	if (D->word && MatchesPattern(D->word,(char*) pattern)) Log(STDUSERLOG,"%s\r\n",D->word);
}

static void C_FindWords(char* input)
{
	WalkDictionary(FindXWord,(uint64) input);
}
static bool TestSetPath(MEANING T,unsigned int depth) // once you are IN a set, the path can be this
{
	WORDP D = Meaning2Word(T);
	if (D->inferMark == inferMark || depth > 100) return false;
	D->inferMark = inferMark;
	int k = 0;
	while (++k)
	{
		MEANING parent = FindSetParent(T,k); //   next set we are member of
		if (!parent)  break;
		WORDP D = Meaning2Word(parent);	// topic or concept
		if (D == topLevel) return true;
		if (TestSetPath(parent,depth+1)) return true; // follow up depth first
	}
	return false;
}

static bool TestUpHierarchy(MEANING T,int depth)
{
    if (!T) return false;

    WORDP E = Meaning2Word(T);
	if (E == topLevel) return true;
	unsigned int index = Meaning2Index(T);
    if (depth == 0)  
	{
		if (TestSetPath(T,depth)) return true;	
		if (*E->word == '~') return false;	// not a word

		//   then do concepts based on this word...
		unsigned int size = GetMeaningCount(E);
		if (!size) size = 1;	//   always at least 1, itself

		//   draw wordnet hierarchy based on each meaning
		for  (unsigned int k = 1; k <= size; ++k)
		{
			if (index && k != index) continue; //   not all, just correct meaning

			//   get meaningptr spot facts are stored (synset head)
			if (!GetMeaningCount(E) ) T = MakeMeaning(E);	//   a generic since we have no meanings
			else 
			{
				if (GetMeaning(E,k) & SYNSET_MARKER) T = MakeMeaning(E,k); // we are master
				else T = GetMaster(GetMeaning(E,k)); 
			}
			if (TestSetPath(T,depth)) return true;
			unsigned int count = 0;
			MEANING parent;
			while ((parent = FindSynsetParent(T,count++)))
			{
				//   walk wordnet hierarchy
				if (TestSetPath(parent,depth)) return true;
				TestUpHierarchy(parent,depth+1); //   we find out what sets PARENT is in (will be none- bug)
			}
		}
	}
	else //    always synset nodes
	{
		E->inferMark = inferMark; // came this way
		unsigned int count = 0;
		MEANING parent;
		while ((parent = FindSynsetParent(T,count++)))
		{
			//   walk wordnet hierarchy
			if (TestSetPath(parent,depth)) return true;
			TestUpHierarchy(parent,depth+1); //   we find out what sets PARENT is in (will be none- bug)
		}
	}
	return false;
}

static void TestSet(WORDP D,uint64 flags)
{
	if (!(D->properties & flags) || !(D->systemFlags & AGE_LEARNED)) return; // only want simple words to be tested
	MEANING M = MakeMeaning(D);
	NextinferMark();
	if (TestUpHierarchy(M,0)) return;
	Log(STDUSERLOG,"%s\r\n",D->word);
}

static void C_Nonset(char* buffer)
{
	char type[MAX_WORD_SIZE];
	buffer = ReadCompiledWord(buffer,type);
	uint64 kind  = FindValueByName(type);
	if (!kind) return;
	WORDP D = FindWord(buffer);
	topLevel = D;
	WalkDictionary(TestSet,kind);
}

static void C_HasFlag(char* buffer)
{
	bool notflag = false;
	char type[MAX_WORD_SIZE];
	buffer = ReadCompiledWord(buffer,type);
	WORDP D = FindWord(type); // name of set
	buffer = SkipWhitespace(buffer);
	if (*buffer == '!')
	{
		notflag = true;
		++buffer;
	}
	buffer = ReadCompiledWord(buffer,type);
	uint64 flag  = FindValue2ByName(type); // flag to find or !find
	FACT* F = GetObjectHead(D);
	while (F)
	{
		if (F->verb == Mmember)
		{
			WORDP S = Meaning2Word(F->subject);
			if (S->systemFlags & flag)
			{
				if (!notflag) Log(STDUSERLOG,"%s has %s\r\n",S->word,type);
			}
			else
			{
				if (notflag) Log(STDUSERLOG,"%s lacks %s\r\n",S->word,type);
			}
		}
		F = GetObjectNext(F);
	}
}

static bool HitTest(WORDP D, WORDP set) // can we hit this
{
	if (D->inferMark == inferMark) return false;	// been here already
	D->inferMark = inferMark;
	FACT* F = GetSubjectHead(D);
	while (F)
	{
		if (F->verb == Mmember)
		{
			WORDP E = Meaning2Word(F->object);
			if (E == set) return true;
			if (*E->word == '~') 
			{
				if (HitTest(E,set)) return true;
			}
		}
		F = GetSubjectNext(F);
	}

	return false;
}

static void C_Overlap(char* buffer)
{
	char set1[MAX_WORD_SIZE];
	char set2[MAX_WORD_SIZE];
	buffer = ReadCompiledWord(buffer,set1);
	WORDP E = FindWord(set1);
	if (!E || E->word[0] != '~')
	{
		printf("no such set %s\r\n",set1);
		return;
	}
	buffer = ReadCompiledWord(buffer,set2);
	WORDP D = FindWord(set2);
	if (!E || E->word[0] != '~')
	{
		printf("no such set %s\r\n",set2);
		return;
	}
	Log(STDUSERLOG,"These members of %s are also in %s:\r\n",set1,set2);

	// walk members of set1, seeing if they intersect set2
	FACT* F = GetObjectHead(E);
	while (F)
	{
		E = Meaning2Word(F->subject);
		if (F->verb == Mmember && *E->word != '~') // see if word is member of set2
		{
			NextinferMark();
			if (HitTest(E,D)) Log(STDUSERLOG,"%s\r\n",E->word);
		}
		F = GetObjectNext(F);
	}

}

static bool DumpSetPath(MEANING T,unsigned int depth) // once you are IN a set, the path can be this
{
	int k = 0;
	if (depth > 20)
	{
		printf("Hierarchy too deep-- recursive?");
		return false;
	}	
	while (++k)
	{
		MEANING parent = FindSetParent(T,k); //   next set we are member of
		if (!parent)  break;

		WORDP D = Meaning2Word(parent);	// topic or concept
		if (D->internalBits & HAS_EXCLUDE) // prove no violation
		{
			FACT* F = GetObjectHead(D);
			while (F)
			{
				if (F->verb == Mexclude)
				{
					WORDP E = Meaning2Word(F->subject);
					if (E->inferMark == inferMark) break;
				}
				F = GetObjectNext(F);
			}
			if (F) continue;	// exclusion in effect
		}

        Log(STDUSERLOG,"    ");
		for (unsigned int j = 0; j < depth; ++j) Log(STDUSERLOG,"   "); 
		WORDP E = Meaning2Word(parent);
		if (E->systemFlags & TOPIC) Log(STDUSERLOG,"T%s \r\n",WriteMeaning(parent)); 
		else Log(STDUSERLOG,"%s \r\n",WriteMeaning(parent)); 
		if (!DumpSetPath(parent,depth+1)) return false; // follow up depth first
	}
	return true;
}

static bool DumpUpHierarchy(MEANING T,int depth)
{
    if (!T) return true;
	if (depth > 20)
	{
		printf("Hierarchy too deep-- recursive?");
		return false;
	}

    WORDP E = Meaning2Word(T);
	E->inferMark = inferMark; // came this way
	unsigned int index = Meaning2Index(T);
    if (depth == 0)  
	{
		Log(STDUSERLOG,"\r\nFor %s:\r\n",E->word); 
		Log(STDUSERLOG," Set hierarchy:\r\n"); 

		if (!DumpSetPath(T,depth)) return false;	
		if (*E->word == '~') return true;	// we are done, it is not a word

		//   then do concepts based on this word...
		unsigned int size = GetMeaningCount(E);
		if (!size) size = 1;	//   always at least 1, itself
		Log(STDUSERLOG," Wordnet hierarchy:\r\n"); 

		//   draw wordnet hierarchy based on each meaning
		for  (unsigned int k = 1; k <= size; ++k)
		{
			if (index && k != index) continue; //   not all, just correct meaning

			//   get meaningptr spot facts are stored (synset head)
			if (!GetMeaningCount(E) ) T = MakeMeaning(E);	//   a generic since we have no meanings
			else 
			{
				if (GetMeaning(E,k) & SYNSET_MARKER) T = MakeMeaning(E,k) | (GetMeaning(E,k) & TYPE_RESTRICTION); // we are master
				else T = GetMaster(GetMeaning(E,k)) | (GetMeaning(E,k) & TYPE_RESTRICTION); 
			}
			WORDP D1 = Meaning2Word(T);
			Log(STDUSERLOG,"  ");
			Log(STDUSERLOG,"%s~%d:",E->word,k);
			if (T & NOUN) Log(STDUSERLOG,"N   ");
			else if (T & VERB) Log(STDUSERLOG,"V   ");
			else if (T & ADJECTIVE) Log(STDUSERLOG,"Adj ");
			else if (T & ADVERB) Log(STDUSERLOG,"Adv ");
			else if (T & PREPOSITION) Log(STDUSERLOG,"Prep ");
			char* gloss = GetGloss(D1,Meaning2Index(T));
			if (gloss) Log(STDUSERLOG," %s ",gloss);
			Log(STDUSERLOG,"\r\n"); 
		
			if (!DumpSetPath(T,depth)) return false;
			unsigned int count = 0;
			MEANING parent;
			while ((parent =  FindSynsetParent(T,count++)))
			{
				//   walk wordnet hierarchy
				WORDP P = Meaning2Word(parent);
				Log(STDUSERLOG,"   ");
				for (int j = 0; j < depth; ++j) Log(STDUSERLOG,"   "); 
				Log(STDUSERLOG," is %s ",WriteMeaning(parent)); //   we show the immediate parent
				char* gloss = GetGloss(P,Meaning2Index(parent));
				if (gloss) Log(STDUSERLOG," %s ",gloss);
				Log(STDUSERLOG,"\r\n"); 
				if (!DumpSetPath(parent,depth)) return false;
				if (!DumpUpHierarchy(parent,depth+1)) return false; //   we find out what sets PARENT is in (will be none- bug)
			}
		}
	}
	else //    always synset nodes
	{
		unsigned int count = 0;
		MEANING parent;
		while ((parent =  FindSynsetParent(T,count++)))
		{
			//   walk wordnet hierarchy
			WORDP P = Meaning2Word(parent);
			unsigned int index = Meaning2Index(parent);
			Log(STDUSERLOG,"   ");
			for (int j = 0; j < depth; ++j) Log(STDUSERLOG,"   "); 
			Log(STDUSERLOG," is %s",WriteMeaning(parent)); //   we show the immediate parent
			char* gloss = GetGloss(P,index);
			if (gloss) Log(STDUSERLOG," %s ",gloss);
			Log(STDUSERLOG,"\r\n");
			if (!DumpSetPath(parent,depth)) return false;
			if (!DumpUpHierarchy(parent,depth+1)) return false; //   we find out what sets PARENT is in (will be none- bug)
		}
	}
	return true;
}

static void C_Up(char* input)
{
 	char word[MAX_WORD_SIZE];
	NextinferMark();
	ReadCompiledWord(input,word);
	MEANING M = ReadMeaning(word,false);
	M = GetMaster(M);
	DumpUpHierarchy(M,0);
}

static void C_Word(char* input)
{
	char word[MAX_WORD_SIZE];
	char junk[MAX_WORD_SIZE];
	while(ALWAYS)
	{
		input = ReadCompiledWord(input,word);
		if (!*word) break;
		input = SkipWhitespace(input);
		int limit= 0;
		if (IsDigit(*input))
		{
			input = ReadCompiledWord(input,word);
			limit = atoi(junk);
		}
		DumpDictionaryEntry(word,limit);  
	}
} 	

static void WordDump(WORDP D,uint64 flags)
{
	if (!(D->properties & NOUN_SINGULAR)) return;
	if (!(D->properties & NOUN_PLURAL)) return;
	//unsigned int len = strlen(D->word);
	//if (D->word[len-1] == 's')
	{
		//WORDP X = FindWord(D->word,len-1);
		//if (X && X->properties & NOUN_SINGULAR)
			Log(STDUSERLOG,"%s \r\n",D->word);

	}
}

static void C_WordDump(char* input)
{
	WalkDictionary(WordDump,0);

#ifdef JUNK
	WORDP D = FindWord(input);
	if (!D) 
	{
		Log(STDUSERLOG,"No such set %s\r\n",input);
		return;
	}
	FACT* F = GetObjectHead(D);
	while (F)
	{
		if (F->verb == Mmember)
		{
			if (D->systemFlags & VERB_TAKES_VERBINFINITIVE)
				Log(STDUSERLOG,"redundant %s\r\n",D->word);
		}
		F = GetObjectNext(F);
	}
#endif
} 	

//////////////////////////////////////////////////
/// SYSTEM INFO
/////////////////////////////////////////////////

static void C_Commands(char* x)
{
	int i = 0;
	CommandInfo *routine;
	while ((routine = &commandSet[++i]) && routine->word) Log(STDUSERLOG,"%s - %s\r\n",routine->word,routine->comment); // linear search
}

static void C_Definition(char* x)
{
	char name[MAX_WORD_SIZE];
	ReadCompiledWord(x,name);
	WORDP D = FindWord(name);
	if (!D || !(D->systemFlags & FUNCTION_NAME)) Log(STDUSERLOG,"No such name\r\n");
	else if ((D->systemFlags & FUNCTION_BITS) == IS_PLAN_MACRO) Log(STDUSERLOG,"Plan macro\r\n");
	else if (D->x.codeIndex && (D->systemFlags & FUNCTION_BITS) != IS_TABLE_MACRO) Log(STDUSERLOG,"Engine API function\r\n");
	else if ((D->systemFlags & FUNCTION_BITS) == IS_OUTPUT_MACRO) Log(STDUSERLOG,"output macro: %s\r\n",D->w.fndefinition+1); // skip arg count
	else Log(STDUSERLOG,"pattern macro: %s\r\n",D->w.fndefinition+1); // skip arg count
}

static void C_Variables(char* input)
{
	if (!stricmp(input,"system")) DumpSystemVariables();
	else if (!stricmp(input,"user")) DumpVariables(); 
	else // all
	{
		DumpVariables();
		DumpSystemVariables();
		Log(STDUSERLOG,"Max Buffers used %d\r\n",maxBufferUsed);
		Log(STDUSERLOG,"%s\r\n",ShowPendingTopics());
	}
} 	

static void C_Functions(char* input)
{
	DumpFunctions();
}

static void ShowMacro(WORDP D,uint64 junk)
{
	if (!(D->systemFlags & FUNCTION_NAME)) {;} // not a function or plan
	else if ((D->systemFlags & FUNCTION_BITS) == IS_PLAN_MACRO) Log(STDUSERLOG,"plan: %s (%d)\r\n",D->word,D->w.planArgCount);
	else if (D->x.codeIndex) {;} //is system function (when not plan)
	else if (D->systemFlags & IS_PATTERN_MACRO && D->systemFlags & IS_OUTPUT_MACRO) Log(STDUSERLOG,"dualmacro: %s (%d)\r\n",D->word,MACRO_ARGUMENT_COUNT(D));
	else if (D->systemFlags & IS_PATTERN_MACRO) Log(STDUSERLOG,"patternmacro: %s (%d)\r\n",D->word,MACRO_ARGUMENT_COUNT(D));
	else if (D->systemFlags & IS_OUTPUT_MACRO) 	Log(STDUSERLOG,"outputmacro: %s (%d)\r\n",D->word,MACRO_ARGUMENT_COUNT(D));
	else if (D->systemFlags & IS_PLAN_MACRO) Log(STDUSERLOG,"tablemacro: %s (%d)\r\n",D->word,MACRO_ARGUMENT_COUNT(D));
}

static void C_Macros(char* input)
{
	WalkDictionary(ShowMacro,0);
}

static void C_MemStats(char* input)
{
	unsigned int factUsedMemKB = ( factFree-factBase) * sizeof(FACT) / 1000;
	unsigned int dictUsedMemKB = ( dictionaryFree-dictionaryBase) * sizeof(WORDENTRY) / 1000;
	// dictfree shares text space
	unsigned int textUsedMemKB = ( stringBase-stringFree)  / 1000;
	char* endDict = (char*)(dictionaryBase + maxDictEntries);
	unsigned int textFreeMemKB = ( stringFree- endDict) / 1000;
	unsigned int bufferMemKB = (maxBufferLimit * maxBufferSize) / 1000;
	
	unsigned int used =  factUsedMemKB + dictUsedMemKB + textUsedMemKB + bufferMemKB;
	used +=  (userTopicStoreSize + userTableSize) /1000;

	char buf2[MAX_WORD_SIZE];
	char buf[MAX_WORD_SIZE];
	strcpy(buf,StdIntOutput(factFree-factBase));
	strcpy(buf2,StdIntOutput(textFreeMemKB));
	Log(STDUSERLOG,"Used: words %s (%dkb) facts %s (%dkb) text %dkb buffers %d overflowBuffers %d\r\n",
		StdIntOutput(dictionaryFree-dictionaryBase), 
		dictUsedMemKB,
		buf,
		factUsedMemKB,
		textUsedMemKB,
		bufferIndex,overflowIndex);
}

static void C_Who(char*input)
{
	Log(STDUSERLOG,"%s talking to %s\r\n",loginID,computerID);
}

//////////////////////////////////////////////////////////
//// COMMAND SYSTEM
//////////////////////////////////////////////////////////

void InitCommandSystem() // set dictionary to match builtin functions
{
	unsigned int k = 0;
	CommandInfo *routine;
	while ((routine = &commandSet[++k]) && routine->word )
	{
		if (*routine->word == ':') StoreWord((char*) routine->word)->x.debugIndex  = (unsigned short)k; // a command, not a display label
	}
}

int Command(char* input,char* output)
{
	char word[MAX_WORD_SIZE];
	input = ReadCompiledWord(input,word);
	WORDP D = FindWord(word);
	bool oldecho = echo;
	echo = true;	// see outputs sent to log file on console also
	if (D) 
	{
		CommandInfo* info;
		if (D->x.debugIndex) 
		{
			info = &commandSet[D->x.debugIndex];
			if (debugger && !info->debugger) // not usable from inside debugger
			{
				Log(STDUSERLOG,"Cannot use %s from inside debugger\r\n",word);
				echo = oldecho;
				return 0;
			}
			input = SkipWhitespace(input);
			char data[MAX_WORD_SIZE];
			strcpy(data,input);
			TrimSpaces(data,false);
			wasCommand = COMMANDED;
			testOutput = output;
			if (output) *output = 0;
			(*info->fn)(data);
			testOutput = NULL;
			if (strcmp(info->word,":trace") && strcmp(info->word,":echo")) echo = oldecho;
			return wasCommand;
		}
	}
	Log(STDUSERLOG,"Unknown command %s\r\n",word);
	echo = oldecho;
	return COMMANDED; 
}

//////////////////////////////////////////////////////////
//// TOPIC INFO
//////////////////////////////////////////////////////////

void C_Gambits(char* buffer)
{
	buffer = SkipWhitespace(buffer);
	unsigned int topic = FindTopicIDByName(buffer);
	if (!topic) 
	{
		Log(STDUSERLOG,"No such topic %s\r\n",buffer);
		return;
	}
	
	char* base = GetTopicData(topic);  
	int ruleID = 0;
	unsigned int* map = gambitTagMap[topic];
	ruleID = *map;
	unsigned int* indices =  ruleOffsetMap[topic];
	unsigned int n = 0;
	while (ruleID != NOMORERULES)
	{
		char* ptr = base + indices[ruleID]; // the gambit 
		char* end = strchr(ptr,ENDUNIT);
		*end = 0;
		++n;
		char label[MAX_WORD_SIZE];
		char pattern[MAX_WORD_SIZE];
		char* output = GetPattern( ptr,label,pattern);
		if (strlen(pattern) == 4) *pattern = 0;
		if (*label) strcat(label,":");
		if (!UsableRule(topic,ruleID)) Log(STDUSERLOG,"- %d %s %s    %s\r\n",n,label,output,pattern);
		else Log(STDUSERLOG,"%d  %s %s    %s\r\n",n,label,output,pattern);
		*end = ENDUNIT;
		ruleID = *++map;
	}
}

void C_Pending(char* buffer)
{
	Log(STDUSERLOG,"Pending topics: %s\r\n", ShowPendingTopics());
}

static void CountConcept(WORDP D, uint64 count)
{
	if (D->systemFlags & CONCEPT && !(D->systemFlags & TOPIC))
	{
		unsigned int* ctr = (unsigned int*) count;
		++*ctr;
	}
}

static void C_TopicStats(char* input)
{
	unsigned int totalgambits = 0;
	unsigned int totalresponders = 0;
	unsigned int totalrejoinders = 0;
	unsigned int totalquestions = 0;
	unsigned int totalstatements = 0;
	unsigned int totaldual = 0;
	unsigned int conceptCount = 0;
	bool normal = false;
	if (!stricmp(input,"normal")) // show only normal topics
	{
		normal = true;
		*input = 0;
	}
	WalkDictionary(CountConcept,(uint64) &conceptCount);
	unsigned int topicCount = 0;

	size_t len = 0;
	char* x = strchr(input,'*');
	if (x) len = x - input;
	else if (*input == '~') len = strlen(input);

	for (unsigned int i = 1; i <= lastTopic; ++i) 
	{
		if (len && strnicmp(GetTopicName(i),input,len)) continue;
		char* name = GetTopicName(i);
		char* data = GetTopicData(i);
		unsigned int flags = GetTopicFlags(i);
		if (flags & TOPIC_SYSTEM && normal) continue;
		++topicCount;
		unsigned int gambits = 0;
		unsigned int responders = 0;
		unsigned int rejoinders = 0;
		int id = 0;
		while (data && *data)
		{
			if (TopLevelGambit(data)) ++gambits;
			else if (TopLevelRule(data)) ++responders;
			else ++rejoinders;

			if (*data == QUESTION) ++totalquestions;
			else if (*data == STATEMENT) ++totalstatements;
			else if (*data == STATEMENT_QUESTION) ++totaldual;

			data = FindNextRule(NEXTRULE,data,id);
		}
		totalgambits += gambits;
		totalresponders += responders;
		totalrejoinders += rejoinders;
		Log(STDUSERLOG,"    %s     gambits %d responders %d rejoinders %d\r\n", name,gambits,responders,rejoinders);
	}
	unsigned int totalrules = totalgambits + totalresponders + totalrejoinders;
	Log(STDUSERLOG,"Concepts %d Topics %d rules %d \r\n  gambits %d  responders %d (?: %d s: %d  u: %d) rejoinders %d\r\n",conceptCount,topicCount,totalrules,totalgambits,totalresponders,totalquestions,totalstatements,totaldual,totalrejoinders);
}

static void TrackFactsUp(MEANING T,FACT* G,WORDP base) //   show what matches up in unmarked topics
{ 
    if (!T) return;
	WORDP D = Meaning2Word(T);
	unsigned int index = Meaning2Index(T);
	unsigned int flags = (T & TYPE_RESTRICTION);
	if (!flags) flags = ESSENTIAL_FLAGS;
	if (D->systemFlags & TOPIC) 
	{
		if (D->inferMark == inferMark) return;
		D->inferMark = inferMark;
		unsigned int flags = GetTopicFlags(FindTopicIDByName(D->word));
		if (flags & TOPIC_SYSTEM) return;	// dont report system intersects
		char word[MAX_WORD_SIZE];
		if (Meaning2Word(G->subject) == base) sprintf(word,"%s(%s)",D->word,base->word);
		else sprintf(word,"%s(%s=>%s)",D->word,base->word,WriteMeaning(G->subject));
		Log(STDUSERLOG,"%s\r\n",word);
		return;	
	}
	FACT* F = GetSubjectHead(D); 
	while (F) 
	{
		WORDP object = Meaning2Word(F->object);
		if ((F->verb == Mmember || F->verb == Mis) && object->inferMark != inferMark) 
		{
			unsigned int restrict = F->subject & TYPE_RESTRICTION;
			if (restrict) // type restricted member
			{
				if (!( restrict & flags ))
				{
					F = GetSubjectNext(F);
					continue;
				}
			}

			//  meaning restriction 
			if (index == Meaning2Index(F->subject)) // match generic or specific 
			{
				WORDP E = Meaning2Word(F->subject);
				if (*E->word == '~') TrackFactsUp(F->object,G,base);
				else TrackFactsUp(F->object,F,base);
			}
		}
		F = GetSubjectNext(F);
	}
}

static void TabInset(unsigned int depth,bool eol)
{
	if (eol) Log(STDUSERLOG,"\r\n");
	for (unsigned int i = 0; i < depth; ++i) Log(STDUSERLOG,"  ");
}

static void TrackFactsDown(MEANING M,FACT* F,unsigned int depth,size_t& length,bool display)
{
	WORDP D = Meaning2Word(M);
	if (D->inferMark == inferMark) return;	// already marked
	D->inferMark = inferMark;
	if (*D->word == '~')  // its a set or topic-- nest and do the set
	{
		if (display)
		{
			if ( length != depth)  TabInset(depth,true);
			// header
			Log(STDUSERLOG,"%s\r\n",D->word);
			// indent 
			TabInset(depth+2,true);
			length = depth + 2;
		}
		else TrackFactsUp(M,F,D);
	    // concept keywords
		FACT* F = GetObjectHead(D);
		while (F)
		{
			TrackFactsDown(F->subject,F,depth+2,length,display);
			F = GetObjectNext(F);
		}
		if (display)
		{
			TabInset(depth,true); // end of concept keywords // restore indent 
			length = depth * 2;
		}
	}
	else // displaying a word of a set
	{
		unsigned int index = Meaning2Index(M);
		if (display)
		{
			char word[MAX_WORD_SIZE];
			if (!index)	sprintf(word,"%s ",D->word);
			else sprintf(word,"%s~%d ",D->word,index);
			Log(STDUSERLOG,"%s",word);
			size_t wlen = strlen(word)  + 1;
			length += wlen;
			while (wlen < 20) // force each word to be 20 wide
			{
				Log(STDUSERLOG," ");
				++wlen;
				++length;
			}
			if (length > 120) // avoid long lines
			{
				TabInset(depth,true);
				length = depth * 2;
			}
		}
		else if (index) // need to propogate down - but might be huge-- dont display
		{
			int l = 0;
			M = GetMaster(M); // master meaning
			while (++l) //   find the children of the meaning of T
			{
				MEANING child = FindChild(M,l);
				if (!child) break;
				TrackFactsDown(child,F,depth+2,length,false);
			} //   end of children for this value
		}
		else
		{
			FACT* F = GetSubjectHead(D); // who comes from this word
			while (F)
			{
				if (F->verb == Mmember)	TrackFactsUp(F->object,F,D); 
				F = GetSubjectNext(F);
			}
			unsigned int size = GetMeaningCount(D); // all meanings up
			for  (unsigned int k = 1; k <= size; ++k)
			{
				MEANING M = GetMeaning(D,k);
				TrackFactsUp(M,F,D); // anyone else refers to this meaning?
				MEANING parent = FindSetParent(M,0); //   next set we are member of
				TrackFactsUp(parent,F,D);
			}
		}
	}
}

static void C_Topics(char* input)
{
	PrepareSentence(input,true,true);	
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
	
}

static void C_TopicInfo(char* input)
{
	char word[MAX_WORD_SIZE];
	char* ptr = ReadCompiledWord(input,word);
	if (*word == '~' && word[1] == 0) 
	{
		if (inputRejoinderTopic == NO_REJOINDER) return;
		strcpy(word,GetTopicName(inputRejoinderTopic));
		input = ptr;
	}
	else if (*word == '~')  input = ptr;
	
	size_t len = 0;
	char* x = strchr(word,'*');
	if (x) len = x - word;
	else if (*word == '~') len = strlen(word);

	for (unsigned int topicid = 1; topicid <= lastTopic; ++topicid) 
	{
		if (len && strnicmp(GetTopicName(topicid),word,len)) continue;

		WORDP D = FindWord(GetTopicName(topicid));
		int rejoinderOffset = -1;
		if ((int)topicid == inputRejoinderTopic) rejoinderOffset = inputRejoinderRuleID;
		bool used = true;
		bool available = true;
		bool rejoinder = false;
		bool gambit = false;
		bool responder = false;
		bool keys = false;
		bool overlap = false;
		bool all = false;
		if (!*input) all = keys = overlap = gambit = responder = rejoinder = true; // show it all
		char* ptr = input;
		while (*ptr)
		{
			ptr = ReadCompiledWord(ptr,word); // restriction
			if (!*word) break;
			if (!stricmp(word,"used")) available = false;
			else if (!stricmp(word,"available")) used = false;

			else if (!stricmp(word,"rejoinder")) rejoinder = true;
			else if (!stricmp(word,"gambit")) gambit = true;
			else if (!stricmp(word,"responder")) responder = true;
			else if (!stricmp(word,"all")) rejoinder = gambit = responder = true;

			else if (!stricmp(word,"keys")) keys = true;
			else if (!stricmp(word,"overlap")) overlap = true;
		}
		if (!gambit && !responder && !rejoinder) used = available = false;
		if (all) DisplayTopicFlags(topicid);

		if (keys) // display all keys (execpt recursive wordnet)
		{
			Log(STDUSERLOG,"\r\nTopic Keys: %s\r\n",D->word);
			NextinferMark();
			if (D->internalBits & HAS_EXCLUDE) MarkExclude(D);
			FACT* F = GetObjectHead(D);
			size_t length = 2;
			Log(STDUSERLOG,"  ");
			while (F)
			{
				TrackFactsDown(F->subject,F,1,length,true); 
				F = GetObjectNext(F);
			}
			Log(STDUSERLOG,"\r\n");
		}

		if (overlap)
		{
			FACT* F = GetObjectHead(D);
			NextinferMark();
			D->inferMark = inferMark;
			if (D->internalBits & HAS_EXCLUDE) MarkExclude(D);
			size_t length = 2;
			bool started = false;
			while (F)
			{
				if (F->verb == Mmember)
				{
					if (!started)
					{
						Log(STDUSERLOG,"\r\nTopic Key Overlap: %s\r\n",D->word);
						Log(STDUSERLOG,"  ");
						started = true;
					}
					TrackFactsDown(F->subject,F,1,length,false); 
				}
				F = GetObjectNext(F);
			}
			Log(STDUSERLOG,"\r\n");
		}

		if ((used || available) && !gambit && !rejoinder && !responder) rejoinder = gambit = responder = true;

		unsigned int gambits = 0;
		unsigned int statements = 0;
		unsigned int questions = 0;
		unsigned int dual = 0;
		unsigned int rejoinders = 0;

		int id = 0;
		char* name = GetTopicName(topicid);
		char* data = GetTopicData(topicid);
		bool access = true;
		while (data && *data) // walk data
		{
			char* rule = ShowRule(data);
			if (*data == GAMBIT || *data == RANDOM_GAMBIT) ++gambits;
			else if (*data == QUESTION) ++questions;
			else if (*data == STATEMENT) ++statements;
			else if (*data == STATEMENT_QUESTION) ++dual;
			else  ++rejoinders;
			if (TopLevelRule(data))
			{
				access = UsableRule(topicid,id);
				if ((*data == GAMBIT || *data == RANDOM_GAMBIT) && !gambit) access = false;
				else if ((*data == QUESTION || *data == STATEMENT_QUESTION || *data == STATEMENT) && !responder) access = false;
				else if (!access) // no access exists
				{
					if (used) 
					{
						Log(STDUSERLOG,"  - %d(%d) %s\r\n",id,ruleOffsetMap[topicid][id],rule);
						access = true;
					}
				}
				else // rule is accessible
				{
					if (available) Log(STDUSERLOG,"    %d(%d) %s\r\n",id,ruleOffsetMap[topicid][id],rule);
					else access = false;
				}
			}
			else if (rejoinder) // inherits prior access
			{
				if (access)
				{
					unsigned int depth = *rule - 'a';
					while (depth--) Log(STDUSERLOG,"    "); // indent appropriately
					if (id == rejoinderOffset) Log(STDUSERLOG,"  ***  (%d) %s\r\n",REJOINDERID(id),rule); // current rejoinder
					else Log(STDUSERLOG,"       (%d) %s\r\n",REJOINDERID(id),rule);
				}
			}
			data = FindNextRule(NEXTRULE,data,id);
		}
		if (all) Log(STDUSERLOG,"%s(%d)  gambits: %d  responders: %d (?:%d s:%d u:%d)  rejoinders: %d\r\n", name,topicid,gambits,statements+questions+dual,statements, questions, dual,rejoinders);
	}
}

static void C_Where(char* input)
{
	unsigned int topic = FindTopicIDByName(input);
	if (topic)	Log(STDUSERLOG,"%s is from %s\r\n",input,GetTopicFile(topic));
}

//////////////////////////////////////////////////////////
//// FACT INFO
//////////////////////////////////////////////////////////

static void C_AllFacts(char* input)
{
	WriteFacts(FopenUTF8Write("TMP/facts.txt"),factBase);
}

static void C_Facts(char* input)
{
	char word[MAX_WORD_SIZE];
	char* ptr = ReadCompiledWord(input,word);
	FACT* G = NULL;
	WORDP D = NULL;
	unsigned int index = 0;
	FACT* F;
	if (*word == '(') // actual fact
	{
		char arg1[MAX_WORD_SIZE];
		char arg2[MAX_WORD_SIZE];
		char arg3[MAX_WORD_SIZE];
		ptr -= (strlen(word)-1) + 1;
		ptr = ReadCompiledWord(ptr,arg1);
		ptr = ReadCompiledWord(ptr,arg2);
		ptr = ReadCompiledWord(ptr,arg3);
		size_t len = strlen(arg3);
		if (arg3[len-1] == ')') arg3[len-1] = 0;	// remove trailing )
		G = FindFact(ReadMeaning(arg1,false),ReadMeaning(arg2,false),ReadMeaning(arg3,false),0); 
		if (!G) 
		{
			Log(STDUSERLOG,"No such facts\r\n");
			return;
		}
	}
	else
	{
		MEANING M = ReadMeaning(word,false);
		index = Meaning2Index(M);
		if (!M)
		{
			Log(STDUSERLOG,"No such meaning exists\r\n");
			return;
		}
		D = Meaning2Word(M);

	}
	F = (G) ? GetSubjectHead(G) :  GetSubjectHead(D);
	while (F)
	{
		if (index && F->subject != index) {;}
		else TraceFact(F);
		F = GetSubjectNext(F);
	}	
	F = (G) ? GetVerbHead(G) :  GetVerbHead(D);
	while (F)
	{
		if (index && F->verb != index)  {;}
		else TraceFact(F);
		F = GetVerbNext(F);
	}
	F = (G) ? GetObjectHead(G) :  GetObjectHead(D);
	while (F)
	{
		if (index && F->object != index)  {;}
		else TraceFact(F);
		F = GetObjectNext(F);
	}
}

static void C_UserFacts(char* input)
{
	FACT* F = factLocked;
	while (++F <= factFree)
	{
		char word[MAX_WORD_SIZE];
		Log(STDUSERLOG,"%s",WriteFact(F,false,word,false,true));
	}
}

//////////////////////////////////////////////////////////
//// DEBUGGING COMMANDS
//////////////////////////////////////////////////////////

static void C_Debug(char* input)
{
	if (!strnicmp(input,":retry",6) || !strnicmp(input,"revert",6) )
	{
		C_Retry(input);
		printf("debugging input: %s\r\n",revertBuffer);
	}

	if (!server) 
	{
		Log(STDDEBUGLOG,"Debugger started - for help type 'h'\r\n");
		out2level = -2;
		Debugger(ENTER_DEBUGGER);
	}
}

static void C_Do(char* input)
{
	SAVEOLDCONTEXT()
	if (!debugger)
	{
		++inputCount;
		responseIndex = 0;	// clear out data (having left time for :why to work)
		AddHumanUsed(":do");
		AddRepeatable(0);
		OnceCode("$control_pre");
	}
	currentRule = 0;
	currentRuleID = 0;
	currentRuleTopic =  currentTopicID = 0;
	char* data = AllocateBuffer();
	char* out = data;
	char* answer = AllocateBuffer();
#ifndef DISCARDSCRIPTCOMPILER
	hasErrors = 0;
	ReadOutput(input, NULL,out,NULL);
	if (hasErrors) Log(STDUSERLOG,"\r\nScript errors prevent execution.");
	else 
	{
		unsigned int result;
		FreshOutput(data,answer,result);
		Log(STDUSERLOG,"   result: %s  output: %s\r\n",ResultCode(result),answer);
		if (!debugger) AddResponse(answer);
	}
#else
	Log(STDUSERLOG,"Script compiler not installed.");
#endif
	FreeBuffer();
	FreeBuffer();
	RESTOREOLDCONTEXT()
	wasCommand = OUTPUTASGIVEN; // save results to user file
}

static void C_Silent(char* input)
{
	silent = !silent;
}

static void C_Retry(char* input)
{
	ResetToPreUser();
	ResetSentence();
	char file[MAX_WORD_SIZE];
	sprintf(file,"USERS/topic_%s_%s.txt",loginID,computerID);
	CopyFile2File(file,"TMP/backup.txt",false);	
	char* buffer = FindUserCache(file); // set currentCache
	if (buffer) FreeUserCache(); // erase cache of user so it reads revised disk file
	int BOMvalue = -1; // get prior value
	char oldc;
	int oldCurrentLine;	
	BOMAccess(BOMvalue, oldc,oldCurrentLine); // copy out prior file access and reinit user file access
	ReadUserData(GetFileRead(loginID,computerID));
	BOMAccess(BOMvalue, oldc,oldCurrentLine); 
}

static void C_Log(char* input)
{
	Log(STDUSERLOG,"Log: %s\r\n",input);
}

static void C_Skip(char* buffer)
{
	unsigned int topic = GetPendingTopicUnchanged();
	if (!topic) 
	{
		Log(STDUSERLOG,"No pending topic\r\n");
		return;
	}
	unsigned int* offsets = ruleOffsetMap[topic];
	int n = atoi(SkipWhitespace(buffer));
	unsigned int* map = gambitTagMap[topic];
	unsigned int ruleID = *map;
	char * rule = NULL;
	char* data = GetTopicData(topic);  
	while (ruleID != NOMORERULES)
	{
		rule = data + offsets[ruleID];
		if (TopLevelGambit(rule) && UsableRule(topic,ruleID) && --n == 0) SetRuleDisableMark(topic, ruleID);
		ruleID = *++map;
	}
	if (ruleID != NOMORERULES) Log(STDUSERLOG,"Next gambit of %s is: %s...\r\n",GetTopicName(topic),ShowRule(GetRule(topic,ruleID)));
	WriteUserData(0);
}

static void C_Show(char* input)
{
	char word[MAX_WORD_SIZE];
	ReadCompiledWord(input,word);
	if (!stricmp(word,"all"))
	{
		all = !all;
		Log(STDUSERLOG,"All set to %d\n",all);
	}
	else if (!stricmp(word,"echo"))
	{
		echo = !echo;
		Log(STDUSERLOG," echo set to %d\n",echo);
	}
	else if (!stricmp(word,"echoserver"))
	{
		echoServer = !echoServer;
		Log(STDUSERLOG," echoServer set to %d\n",echoServer);
	}
	else if (!stricmp(word,"input"))
	{
		showInput = !showInput;
		Log(STDUSERLOG," input set to %d\n",showInput);
	}
	else if (!stricmp(word,"mark"))
	{
		showMark = !showMark;
		Log(STDUSERLOG," showMark set to %d\n",showMark);
	}
	else if (!stricmp(word,"number"))
	{
		autonumber = !autonumber;
		Log(STDUSERLOG," autonumber set to %d\n",autonumber);
	}
	else if (!stricmp(word,"pos"))
	{
		shortPos = !shortPos;
		Log(STDUSERLOG," Pos set to %d\n",shortPos);
	}
	else if (!stricmp(word,"serverLog"))
	{
		serverLog = !serverLog;
		Log(STDUSERLOG," serverLog set to %d\n",serverLog);
	}
	else if (!stricmp(word,"stats"))
	{
		ruleCount = 0;
		stats = !stats;
		Log(STDUSERLOG," stats set to %d\n",stats);
	}
	else if (!stricmp(word,"topic"))
	{
		showTopic = !showTopic;
		Log(STDUSERLOG," topic set to %d\n",showTopic);
	}
	else if (!stricmp(word,"topics"))
	{
		showTopics = !showTopics;
		Log(STDUSERLOG," topics set to %d\n",showTopics);
	}
	else if (!stricmp(word,"why"))
	{
		showWhy = !showWhy;
		Log(STDUSERLOG," why set to %d\n",showWhy);
	}
} 

static void ShowTrace()
{
	// general
	if (trace & (TRACE_VARIABLE|TRACE_MATCH|TRACE_BASIC)) 
	{
		Log(STDUSERLOG,"Enabled simple: ");
		if (trace & TRACE_BASIC) Log(STDUSERLOG,"basic ");
		if (trace & TRACE_MATCH) Log(STDUSERLOG,"match ");
		if (trace & TRACE_VARIABLE) Log(STDUSERLOG,"variables ");
		Log(STDUSERLOG,"\r\n");
	}

	// mild detail
	if (trace & (TRACE_OUTPUT|TRACE_PREPARE|TRACE_PATTERN)) 
	{
		Log(STDUSERLOG,"Enabled mild detail: ");
		if (trace & TRACE_OUTPUT) Log(STDUSERLOG,"output ");
		if (trace & TRACE_PREPARE) Log(STDUSERLOG,"prepare ");
		if (trace & TRACE_PATTERN) Log(STDUSERLOG,"pattern ");
		Log(STDUSERLOG,"\r\n");
	}
	// deep detail
	if (trace & (TRACE_FACTCREATE|TRACE_INFER|TRACE_HIERARCHY|TRACE_SUBSTITUTE|TRACE_VARIABLESET|TRACE_QUERY|TRACE_USER|TRACE_POS)) 
	{
		Log(STDUSERLOG,"Enabled deep detail: ");
		if (trace & TRACE_FACTCREATE) Log(STDUSERLOG,"fact ");
		if (trace & TRACE_INFER) Log(STDUSERLOG,"infer ");
		if (trace & TRACE_HIERARCHY) Log(STDUSERLOG,"hierarchy ");
		if (trace & TRACE_SUBSTITUTE) Log(STDUSERLOG,"substitute ");
		if (trace & TRACE_VARIABLESET) Log(STDUSERLOG,"varassign ");
		if (trace & TRACE_QUERY) Log(STDUSERLOG,"query ");
		if (trace & TRACE_USER) Log(STDUSERLOG,"user ");
		if (trace & TRACE_POS) Log(STDUSERLOG,"pos ");
		Log(STDUSERLOG,"\r\n");
	}

	// general
	if ((trace & (TRACE_BASIC|TRACE_MATCH|TRACE_VARIABLE)) != (TRACE_BASIC|TRACE_MATCH|TRACE_VARIABLE)) 
	{
		Log(STDUSERLOG,"Disabled simple: ");
		if (!(trace & TRACE_BASIC)) Log(STDUSERLOG,"basic ");
		if (!(trace & TRACE_MATCH)) Log(STDUSERLOG,"match ");
		if (!(trace & TRACE_VARIABLE)) Log(STDUSERLOG,"variables ");
		Log(STDUSERLOG,"\r\n");
	}

	// mild detail
	if ((trace & (TRACE_OUTPUT|TRACE_PREPARE|TRACE_PATTERN)) != (TRACE_OUTPUT|TRACE_PREPARE|TRACE_PATTERN)) 
	{
		Log(STDUSERLOG,"Disabled mild detail: ");
		if (!(trace & TRACE_OUTPUT)) Log(STDUSERLOG,"output ");
		if (!(trace & TRACE_PREPARE)) Log(STDUSERLOG,"prepare ");
		if (!(trace & TRACE_PATTERN)) Log(STDUSERLOG,"pattern ");
		Log(STDUSERLOG,"\r\n");
	}

	// deep detail
	if ((trace & (TRACE_FACTCREATE|TRACE_INFER|TRACE_HIERARCHY|TRACE_SUBSTITUTE|TRACE_VARIABLESET|TRACE_QUERY|TRACE_USER|TRACE_POS)) != (TRACE_FACTCREATE|TRACE_INFER|TRACE_HIERARCHY|TRACE_SUBSTITUTE|TRACE_VARIABLESET|TRACE_QUERY|TRACE_USER|TRACE_POS) )
	{
		Log(STDUSERLOG,"Disabled deep detail: ");
		if (!(trace & TRACE_FACTCREATE)) Log(STDUSERLOG,"fact ");
		if (!(trace & TRACE_INFER)) Log(STDUSERLOG,"infer ");
		if (!(trace & TRACE_HIERARCHY)) Log(STDUSERLOG,"hierarchy ");
		if (!(trace & TRACE_SUBSTITUTE)) Log(STDUSERLOG,"substitute ");
		if (!(trace & TRACE_VARIABLESET)) Log(STDUSERLOG,"varassign ");
		if (!(trace & TRACE_QUERY)) Log(STDUSERLOG,"query ");
		if (!(trace & TRACE_USER)) Log(STDUSERLOG,"user ");
		if (!(trace & TRACE_POS)) Log(STDUSERLOG,"pos ");
		Log(STDUSERLOG,"\r\n");
	}
}

static void C_Say(char* input)
{
	AddResponse(input);
	wasCommand = OUTPUTASGIVEN;
}

static void C_Trace(char* input)
{
	char word[MAX_WORD_SIZE];
	unsigned int flags = trace;
	if (!*input) ShowTrace();
	if (!*input) return;

	while (input) 
	{
		input = ReadCompiledWord(input,word); // if using trace in a table, use closer "end" if you are using named flags
		if (!*word) break;
		input = SkipWhitespace(input);
		if (!stricmp(word,"all")) flags = (unsigned int)-1;
		else if (!stricmp(word,"none")) flags = 0;
		else if (*word == '-' && !word[1]) // remove this flag
		{
			input = ReadCompiledWord(input,word);
			if (!stricmp(word,"basic")) flags &= -1 ^ TRACE_BASIC;
			else if (!stricmp(word,"match")) flags &= -1 ^ TRACE_MATCH;
			else if (!stricmp(word,"variables")) flags &= -1 ^ TRACE_VARIABLE; 
			else if (!stricmp(word,"simple")) flags &= -1 ^ (TRACE_BASIC|TRACE_MATCH|TRACE_VARIABLE); 

			else if (!stricmp(word,"prepare")) flags &= -1 ^ TRACE_PREPARE; 
			else if (!stricmp(word,"output")) flags &= -1 ^ TRACE_OUTPUT;
			else if (!stricmp(word,"pattern")) flags &= -1 ^ TRACE_PATTERN;
			else if (!stricmp(word,"mild")) flags &= -1 ^ (TRACE_PREPARE|TRACE_OUTPUT|TRACE_PATTERN); 

			else if (!stricmp(word,"infer")) flags &= -1 ^ TRACE_INFER;
			else if (!stricmp(word,"substitute")) flags &= -1 ^ TRACE_SUBSTITUTE;
			else if (!stricmp(word,"hierarchy")) flags &= -1 ^ TRACE_HIERARCHY;
			else if (!stricmp(word,"fact")) flags &= -1 ^  TRACE_FACTCREATE;
			else if (!stricmp(word,"varassign")) flags &= -1 ^  TRACE_VARIABLESET;
			else if (!stricmp(word,"query")) flags &= -1 ^  TRACE_QUERY;
			else if (!stricmp(word,"user")) flags &= -1 ^  TRACE_USER;
			else if (!stricmp(word,"pos")) flags &= -1 ^  TRACE_POS;
			else if (!stricmp(word,"tcp")) flags &= -1 ^  TRACE_TCP;
			else if (!stricmp(word,"deep")) flags &= -1 ^ (TRACE_INFER|TRACE_SUBSTITUTE|TRACE_HIERARCHY| TRACE_FACTCREATE| TRACE_VARIABLESET| TRACE_QUERY| TRACE_USER|TRACE_POS|TRACE_TCP); 

		}
		else if (*word == '+' && !word[1]) // add this flag
		{
			input = ReadCompiledWord(input,word);
			if (!stricmp(word,"basic")) flags |= TRACE_BASIC;
			else if (!stricmp(word,"match")) flags  |= TRACE_MATCH;
			else if (!stricmp(word,"variables")) flags  |= TRACE_VARIABLE; 
			else if (!stricmp(word,"simple")) flags |= (TRACE_BASIC|TRACE_MATCH|TRACE_VARIABLE); 

			else if (!stricmp(word,"prepare")) flags |= TRACE_PREPARE; 
			else if (!stricmp(word,"output")) flags  |= TRACE_OUTPUT;
			else if (!stricmp(word,"pattern")) flags  |= TRACE_PATTERN;
			else if (!stricmp(word,"mild")) flags |= (TRACE_PREPARE|TRACE_OUTPUT|TRACE_PATTERN); 

			else if (!stricmp(word,"infer")) flags  |= TRACE_INFER;
			else if (!stricmp(word,"substitute")) flags  |= TRACE_SUBSTITUTE;
			else if (!stricmp(word,"hierarchy")) flags |= TRACE_HIERARCHY;
			else if (!stricmp(word,"fact")) flags  |=  TRACE_FACTCREATE;
			else if (!stricmp(word,"varassign")) flags  |=  TRACE_VARIABLESET;
			else if (!stricmp(word,"query")) flags  |=  TRACE_QUERY;
			else if (!stricmp(word,"user")) flags  |=  TRACE_USER;
			else if (!stricmp(word,"pos")) flags  |= TRACE_POS;
			else if (!stricmp(word,"tcp")) flags  |= TRACE_TCP;
			else if (!stricmp(word,"deep")) flags |= (TRACE_INFER|TRACE_SUBSTITUTE|TRACE_HIERARCHY| TRACE_FACTCREATE| TRACE_VARIABLESET| TRACE_QUERY| TRACE_USER|TRACE_POS|TRACE_TCP); 
		}
		else if (IsNumberStarter(*word)) 
		{
			ReadInt(word,flags);
			break; // there wont be more flags -- want :trace -1 in a table to be safe from reading the rest
		}
		else if (!stricmp(word,"basic")) flags |= TRACE_BASIC;
		else if (!stricmp(word,"match")) flags |= TRACE_MATCH;
		else if (!stricmp(word,"variables")) flags |= TRACE_VARIABLE; 
		else if (!stricmp(word,"simple")) flags |= (TRACE_BASIC|TRACE_MATCH|TRACE_VARIABLE); 

		else if (!stricmp(word,"prepare")) flags |= TRACE_PREPARE; 
		else if (!stricmp(word,"output")) flags |= TRACE_OUTPUT;
		else if (!stricmp(word,"pattern")) flags |= TRACE_PATTERN;
		else if (!stricmp(word,"mild")) flags |= (TRACE_PREPARE|TRACE_OUTPUT|TRACE_PATTERN); 

		else if (!stricmp(word,"infer")) flags |= TRACE_INFER;
		else if (!stricmp(word,"substitute")) flags |= TRACE_SUBSTITUTE;
		else if (!stricmp(word,"hierarchy")) flags |= TRACE_HIERARCHY;
		else if (!stricmp(word,"fact")) flags |= TRACE_FACTCREATE;
		else if (!stricmp(word,"varassign")) flags |= TRACE_VARIABLESET;
		else if (!stricmp(word,"query")) flags |= TRACE_QUERY;
		else if (!stricmp(word,"user")) flags |= TRACE_USER;
		else if (!stricmp(word,"pos")) flags |= TRACE_POS;
		else if (!stricmp(word,"tcp")) flags |= TRACE_TCP;
		else if (!stricmp(word,"deep")) flags |= (TRACE_INFER|TRACE_SUBSTITUTE|TRACE_HIERARCHY| TRACE_FACTCREATE| TRACE_VARIABLESET| TRACE_QUERY| TRACE_USER|TRACE_POS|TRACE_TCP); 

		else if (!stricmp(word,"0") || !stricmp(word,"clear")) trace = 0;
		else if (!stricmp(word,"end")) break; // safe end
		else if (*word == '!') // NOT tracing a topic 
		{
			if (word[1]) // ! jammed against topic, separate it
			{
				input -= strlen(word+1); 
				word[1] = 0;
			}
			input = ReadCompiledWord(input,word);
			SetTopicDebugMark(FindTopicIDByName(word),0);
			return;
		}
		else if (*word == '^')
		{
			WORDP FN = FindWord(word);
			if (FN) 
			{
				FN->internalBits ^= TRACE_MACRO;
				Log(STDUSERLOG,"Tracing function %s = %d\r\n",word, (FN->internalBits & TRACE_MACRO) ? 1 : 0);
			}
		}
		else if (*word == '~') // tracing a topic or rule by label
		{
			char* period = strchr(word,'.');
			if (period) *period = 0;
			unsigned int topic = FindTopicIDByName(word);
			if (!period) SetTopicDebugMark(topic,!topicDebugMap[topic]);
			else if (IsAlpha(period[1])) // find ALL labelled statement and mark them
			{
				int id = 0;
				char* which = GetTopicData(topic);
				bool found = false;
				while (which && *which && (which = FindNextLabel(topic,period+1,which,id,true)))
				{
					SetDebugRuleMark(topic,id);
					found = true;
					which = FindNextRule(NEXTRULE,which,id);
				}
				if (!found)  Log(STDUSERLOG,"cannot find %s.%s\r\n",word,period+1);
			}
			else if (IsDigit(period[1]))// did he use number notation?
			{
				int id = 0;
				*period = '.';
				char* rule = GetRuleTag(topic,id,word);
				if (rule) SetDebugRuleMark(topic,id);
				else Log(STDUSERLOG,"cannot find %s.%s\r\n",word,period+1);
			}
			return;
		}
	}
	trace = flags;
	echo = (trace) ? true : false;
    Log(STDUSERLOG," trace set to %d\n",trace);
	if (trace && !*input) ShowTrace();
} 

void C_Why(char* buffer)
{
	for (unsigned int i = 0;  i < responseIndex; ++i)
	{
		unsigned int order = responseOrder[i];
		unsigned int topic = responseData[order].topic;
		unsigned int id = atoi(responseData[order].id+1);
		char* dot = strchr(responseData[order].id+1,'.');
		id |= MAKE_REJOINDERID(atoi(dot+1));
		Log(STDUSERLOG,"%s%s  %s\r\n",GetTopicName(topic),responseData[order].id,ShowRule(GetRule(topic,id)));
	}
}

//////////////////////////////////////////////////////////
//// MISC COMMANDS
//////////////////////////////////////////////////////////

static void CleanIt(char* word,uint64 junk) // remove cr from source lines for Linux
{
	FILE* in = fopen(word,"rb");
	if (!in) 
	{
		printf("missing %s\r\n",word);
		return;
	}
	fseek (in, 0, SEEK_END);
    size_t size = ftell(in);
	char* buf = (char*) malloc(size+2); // enough to hold the file

	fseek (in, 0, SEEK_SET);
	unsigned int val = (unsigned int) fread(buf,1,size,in);
	fclose(in);
	if ( val != size) return;
	buf[size] = 0;	// force an end

	// now overwrite file with proper trimming
	FILE* out = fopen(word,"wb");
	for (unsigned int i = 0; i < size; ++i)
	{
		if (buf[i] != '\r' && buf[i] != 26) fwrite(buf+i,1,1,out);	// remove cr and ^Z
	}
	if (buf[size-1] != '\n') fwrite("\n",1,1,out); // force ending line feed
	fclose(out);
	free(buf);
}

static void C_Clean(char* word) // remove CR for LINUX
{
	WalkDirectory("src",CleanIt,0);
}

static void BuildDummyConcept(WORDP D,uint64 junk)
{
	if ((D->internalBits & BUILD0) && *D->word == '~') CreateFact(MakeMeaning(D),Mmember,MakeMeaning(FindWord("~a_dummy")));
}

static void SortConcept(WORDP D,uint64 junk)
{
	if ((D->internalBits & BUILD0) && *D->word == '~') Sortit(D->word,(int)junk); // will be 0 for no input, some char value otherwise
}

static void C_SortConcept(char* input)
{
#ifdef INFORMATION
To get concepts in a file sorted alphabetically (both by concept and within)l, do 
	0. :build 0 when only the file involved is the only active one in files0.txt
	1. :sortconcept x		-- builds one concept per line and sorts the file by concept name
	2. take the contents of concept.top and replace the original file
	3. :build 0
	4. :sortconcept			-- maps concepts neatly onto multiple lines
	5. take the contents of x.txt and replace the original file
#endif
	WORDP D = StoreWord("~a_dummy",AS_IS);
	if (*input) 
	{
		WalkDictionary(BuildDummyConcept,0); // stores names of concepts on dummy concept, to lock position in dictionary. later not, will be read in
		AddSystemFlag(D,CONCEPT);
		AddInternalFlag(D,BUILD0);
	}

	fclose(fopen("cset.txt","wb"));
	if (!*input) // hide this on second pass
	{
		WORDP D = FindWord("~a_dummy");
		RemoveInternalFlag(D,BUILD0);
	}
	WalkDictionary(SortConcept,(uint64)input[0]);
	if (*input) system("sort /rec 63000 c:/chatscript/cset.txt >concepts.top");
}

//////////////////////////////////////////////////////////
//// ANALYTICS
//////////////////////////////////////////////////////////

static void DisplayTables(char* topic)
{
	char args[MAX_WORD_SIZE];
	sprintf(args,"( %s )",topic);
	Callback(FindWord(GetUserVariable("$abstract")),args);
}

static void DoHeader(int count,char* basic,FILE* in,int id,unsigned int spelling)
{
	if (*abstractBuffer == 0) 	// no more verification data for this topic
	{
		// display header
		if (!lineLimit)	
		{
			TabInset(count,false);
			Log(STDUSERLOG,"%s",basic); 
		}
		return;
	}

	// get verification matching input -- ~abortion.0.0 #! I am against abortion.
	static int readID = 0;
	static char* test = NULL;
	if ((unsigned char)*abstractBuffer == 1) readID = -1; // read 1st line of topic data
retry:
	char type = 0;
	while (readID == -1 || TOPLEVELID(id) > TOPLEVELID(readID) ||  ( TOPLEVELID(id) == TOPLEVELID(readID) && REJOINDERID(id) > REJOINDERID(readID)  )) // flush reads until get 1st good one
	{
		if (!ReadALine(abstractBuffer,in)) break;	// no more verifcation data
		char* dot = strchr(abstractBuffer,'.');
		char* dot1 = strchr(dot+1,'.');
		readID = MAKE_REJOINDERID(atoi(dot1+1)) + atoi(dot+1); // the id pending
		test = strchr(abstractBuffer,'#');
		type = test[2];
		if (!(spelling & ABSTRACT_PRETTY)) test += 2;
		if ((type == 'x' || type == 'X') && *test != ' ' && ((TOPLEVELID(id) > TOPLEVELID(readID)) ||  (TOPLEVELID(id) == TOPLEVELID(readID) && REJOINDERID(id) > REJOINDERID(readID)) )) // global topic comment, dump it immediately and keep going
		{
			Log(STDUSERLOG,"\r\n%s\r\n\r\n",test+1); 
			readID = -1;
		}
	}

	if (test && (type == 'x' || type == 'X') && *test != ' ' && readID == id) // global topic comment for current match
	{
		Log(STDUSERLOG,"\r\n%s\r\n\r\n",test+1); 
		readID = -1;
		goto retry;
	}

	// since we have sample input, kill pattern
	if (id == readID && *basic != ' ' && !(spelling & ABSTRACT_PRETTY)) 
	{
		unsigned int offset = 2;
		while (basic[offset] != ' ') ++offset; // find end of blank space before pattern.
		basic[offset] = 0;  
	}

	// display header
	if (spelling & ABSTRACT_PRETTY && id == readID)  
	{
		TabInset(count,false);
		Log(STDUSERLOG,"%s\r\n",test);
	}
	if (!lineLimit)	
	{
		TabInset(count,false);
		Log(STDUSERLOG,"%s",basic); 
	}

	// display verify as pattern
	if (id == readID && !lineLimit && !(spelling & ABSTRACT_PRETTY)) 
	{
		Log(STDUSERLOG," %s =>   ",test);
	}
}

static void DisplayTopic(char* name,int spelling)
{
	int topicID = FindTopicIDByName(name);
	if (!topicID || BlockedBotAccess(topicID)) return;
	char* rule = GetTopicData(topicID); 
	if (!rule) return;
	*abstractBuffer = 1;	// buffer started for new topic
	if (spelling & ABSTRACT_PRETTY)
	{
		unsigned int lineSize = 0;
		Log(STDUSERLOG,"\r\nTOPIC: %s",name);
		unsigned int flags = GetTopicFlags(topicID);
		if (flags & TOPIC_SYSTEM) Log(STDUSERLOG," SYSTEM");
		if (flags & TOPIC_KEEP) Log(STDUSERLOG," KEEP");
		if (flags & TOPIC_REPEAT) Log(STDUSERLOG," REPEAT");
		if (flags & TOPIC_RANDOM) Log(STDUSERLOG," RANDOM");
		if (flags & TOPIC_NOSTAY) Log(STDUSERLOG," NOSTAY");
		if (flags & TOPIC_PRIORITY) Log(STDUSERLOG," PRIORITY");
		if (flags & TOPIC_LOWPRIORITY) Log(STDUSERLOG," DEPRIORITIZE");
		if (flags & TOPIC_NOBLOCKING) Log(STDUSERLOG," NOBLOCKING");
		if (flags & TOPIC_NOPATTERNS) Log(STDUSERLOG," NOPATTERNS");
		if (flags & TOPIC_NOGAMBITS) Log(STDUSERLOG," NOGAMBITS");
		if (flags & TOPIC_NOSAMPLES) Log(STDUSERLOG," NOSAMPLES");
		if (flags & TOPIC_NOKEYS) Log(STDUSERLOG," NOKEYS");
		if (flags & TOPIC_SAFE) Log(STDUSERLOG," SAFE");
		Log(STDUSERLOG," (");
		WORDP D = FindWord(name);
		FACT* F = GetObjectHead(D);
		while (F) 
		{
			if (F->verb == Mmember|| F->verb == Mexclude)
			{
				char word[MAX_WORD_SIZE];
				if (F->flags & ORIGINAL_ONLY) sprintf(word,"'%s ",WriteMeaning(F->subject));
				else sprintf(word,"%s ",WriteMeaning(F->subject));
				if (F->verb == Mexclude) Log(STDUSERLOG,"!");
				size_t wlen = strlen(word);
				lineSize += wlen;
				Log(STDUSERLOG,"%s",word);
				if (lineSize > 500) // avoid long lines
				{
					Log(STDUSERLOG,"\r\n     ");
					lineSize = 0;
				}
			}
			F = GetObjectNext(F);
		}
		Log(STDUSERLOG,")\r\n\r\n");
	}
	else Log(STDUSERLOG,"\r\n****** TOPIC: %s\r\n",name);

	char word[MAX_WORD_SIZE];
	char fname[MAX_WORD_SIZE];
	sprintf(fname,"VERIFY/%s.txt",name+1);
	FILE* in = fopen(fname,"rb");

	bool preprint;
	char* old = NULL;
	char* buffer = AllocateBuffer();
	char* tmpBuffer = AllocateBuffer();
	char label[MAX_WORD_SIZE];
	char pattern[MAX_BUFFER_SIZE];
	char basic[MAX_BUFFER_SIZE];
	int id = 0;
	char bodyKind[100];

	while (rule && *rule) // for each rule
	{
		preprint = false;
		char* output = GetPattern(rule,label,pattern);
		if (spelling & ABSTRACT_VP)
		{
			char* end = strchr(output,ENDUNIT);
			*end = 0;
			if (*rule == QUESTION || *rule == STATEMENT_QUESTION)
			{
				if (!*label && strstr(output,"factanswer")) Log(STDUSERLOG,"No label for: %s %s\r\n",pattern,output);
			}
			*end = ENDUNIT;
			rule = FindNextRule(NEXTRULE,rule,id);
			continue;
		}
		if (spelling & ABSTRACT_PRETTY) // revise pattern for cannonical
		{
			*tmpBuffer = 0;
			char word[MAX_WORD_SIZE];
			char* pbase = pattern;
			if (*label) 
			{
				strcat(tmpBuffer,label);
				strcat(tmpBuffer," ");
			}
			while (pbase && *pbase)
			{
				pbase = ReadCompiledWord(pbase,word);
				if (IsAlpha(word[0]) && strchr(word,'_') && spelling & ABSTRACT_PRETTY ) // is it a word or a phrase
				{
					WORDP X = FindWord(word);
					if (X && X->properties & PART_OF_SPEECH) {;} // known word
					else // make it a phrase
					{
						Convert2Blanks(word);
						strcat(word,"\"");	// closing quote
						memmove(word+1,word,strlen(word)+1);
						*word = '"';
					}
				}
				if (IsAlpha(word[0]) && spelling & ABSTRACT_CANONICAL) // could be made canonical
				{
					WORDP entry, canonical;
					uint64 sysflags = 0;
					uint64 cansysflags = 0;
					GetPosData(0,word,entry,canonical,sysflags,cansysflags);
					if (canonical)
					{
						// if canonical is upper and entry is lower, dont show canonical
						if (entry && canonical && IsUpperCase(*canonical->word) && !IsUpperCase(*entry->word)) {;}
						else if (!stricmp(canonical->word,"unknown-word")) {;}
						else strcpy(word,canonical->word);
					}
				}
				strcat(tmpBuffer,word);
				strcat(tmpBuffer," ");
			}
			strcpy(pattern,tmpBuffer);
		}

		// std rule header
		unsigned int kind = *rule;
		basic[0] = (unsigned char)kind;
		basic[1] = ':';
		basic[2] = ' ';
		basic[3] = 0;
		int choiceNest = 0;
		char* choiceStart = NULL;
		unsigned int choiceCharacters = 0;

		// revise comparison patterns
		if (*pattern)
		{
			char* compare = pattern;
			while (ALWAYS)
			{
				char* compare1 = strstr(compare," !="); // hunt for comparison operators
				compare = strstr(compare," =");
				if (compare1)
				{
					if (compare1 < compare || !compare) compare = compare1;
				}
				if (!compare) break;
				if (*++compare != '=') ++compare; // negated compare
				if (compare[1] != ' ') memmove(compare,compare+2,strlen(compare+1));// remove header and accelerator of comparison
			}
			if ((kind == 't' || kind == 'r') && *pattern == '(' && pattern[1] == ' ' && pattern[2] == ')') *pattern = 0;	// there is no pattern really for this gambit
			else
			{
				strcat(basic,pattern);
				if (!(spelling & ABSTRACT_PRETTY)) strcat(basic," => ");
			}
		}
		
		// now determine the output
		unsigned int indent = Rejoinder(rule) ? ((*rule - 'a' + 1) * 2) : 0; 
		char* outputPtr = buffer;
		*outputPtr = 0;
		bool badspell = false;
		int hasBody = 0;
		char* end = strchr(output,ENDUNIT);
		*end = 0;
		bool badWord = false;
		bool multipleOutput = false;
		int level = 0;
		char levelMark[1000];
		levelMark[0] = 0;
		char* prior = "";
		char* prior2 = "";
		while (output && *output && *output != ' ') // read output until end of rule
		{
			if (spelling & ABSTRACT_PRETTY) // line by line neatened output
			{
				prior2 = prior;
				prior = output;
				output = ReadOutput(output,word);
				if (!*word) break;	// nothing left
				if (*word == '}') 
				{
					--level;
					if (level < 0) 
						level = 0;
				}
				// for ^if testing zone, remove accelerator
				if (word[3] == '{' && word[4] == ' ' && !word[5]) strcpy(word,"{ ");
				if (word[0] && word[1] && word[2] && word[3] == ' ' && !word[4]) // possible accelerator
				{
					if (!strnicmp(prior2,"^^if",4)) continue;	// ignore accelerator after iftest to skip to next test
					if (!strnicmp(prior2,"^^loop",6)) continue;	// ignore accelerator at start of loop to skip
					if (!strnicmp(prior2,"} ",2) && levelMark[level+1] == 'i') continue;	 // ignore jump after if branch to end of whole if
				}
				
				if (multipleOutput) for (unsigned int j = 0; j < (indent + (level * 2) + 4); ++j) 
				{
					sprintf(outputPtr,"  ");
					outputPtr += 2;
				}
				if (*word == '^' && word[1] == '^') memmove(word,word+1,strlen(word));	// ^^if and ^^loop make normal user written
				sprintf(outputPtr,"%s\r\n",word); // abstract puts lf after EACH item
				outputPtr += strlen(outputPtr);
				multipleOutput = true;
				if (*word == '{' ) 
				{
					++level;
					levelMark[level] = 0;
					if (!strnicmp(prior2,"^^if",4)) levelMark[level] = 'i'; // is an if level
				}
				continue;
			}

			output = ReadCompiledWord(output,word);
			if (!*word) break; 
			if (*word == '+') break;	// skip assignment
			switch(*word)
			{
			case '[': // choice area, with optional label
				++choiceNest;
				choiceStart = outputPtr;
				sprintf(outputPtr,"%s ",word);
				outputPtr += strlen(outputPtr);
				output = ReadCompiledWord(output,word);
				if (word[1] == ':' && !word[2]) // jump label
				{
					sprintf(outputPtr,"%s ",word); 
					outputPtr += strlen(outputPtr);
					output = ReadCompiledWord(output,word);
				}
				break;
			case ']':
				if (--choiceNest == 0)
				{ 
					unsigned int len =  outputPtr - choiceStart; // size of [] 
					strcpy(outputPtr++,"]");
					if (!spelling && len >= lineLimit && len && lineLimit) Log(STDUSERLOG,"(%d) %s\r\n",len,choiceStart);
					choiceCharacters += len; 
				}
				break;
			case ')':
				if (preprint) //   closing preprint call
				{
					preprint = false;
					continue;
				}
				break;
			case '}':
				if (hasBody)
				{
					if (bodyKind[hasBody] == 'i') // if
					{
						output = strchr(output,' ');
						if (output) ++output; // skip end sizing jump rule
					}
					--hasBody;
					continue;
				}
				break;
			case '$':
				if (IsDigit(word[1])) break; // money $
				// flow into these other variables
			case '%': case '_': case '@': // match variable or set variable
				if (*output == '=' || output[1] == '=') // assignment
				{
					output = ReadCompiledWord(output,word); // assign op
					output = ReadCompiledWord(output,word); // rhs item
					if (*word == '^' && *output == '(') output = BalanceParen(output+1); // rhs function call
					while (IsArithmeticOperator(output)) // arithmetic with assignment
					{
						output = ReadCompiledWord(output,word); // op
						output = ReadCompiledWord(output,word);  // next rhs item
						if (*word == '^' && *output == '(') output = BalanceParen(output+1); // rhs function call
					}
					continue;
				}
				break;
			case '^': // function call or argument
				if (!stricmp(word,"^preprint") || !stricmp(word,"^print") || !stricmp(word,"^insertprint") || !stricmp(word,"^postprint")) // show content
				{
					output = ReadCompiledWord(output,word);
					preprint = true;
					continue;
				}
				else if ((!stricmp(word,"^^if") || !stricmp(word,"^^loop")) && *output == '(') 
				{
					++hasBody;
					bodyKind[hasBody] = word[2]; // i or l
					output = strchr(output,'{') + 2;
					continue;
				}
				else if (*output == '(') output = BalanceParen(output+1); //  end call
				break;
			case ':':  // shouldnt be label inside []
				break;
			case '=': // assignment
				old = outputPtr;
				while (*old && *--old && *old != ' '); // find LHS of assignment
				if (*old == ' ') // erase left hand of assignment
				{
					outputPtr = old + 1;
					if (*outputPtr == '$' || *outputPtr == '_' || *outputPtr == '@' || *outputPtr == '%') *outputPtr = 0;
				}
				if (*output != '^') output = ReadCompiledWord(output,word);	// swallow next when not a function call
				break;
			case '~': 
				break;
			case '\\':
				if (word[1] == '"')
				{
					sprintf(outputPtr,"%s ",word+1);
					outputPtr += strlen(outputPtr);
				}
				break;
			default: // ordinary words usually
				if (!stricmp(word,"else") && (*output == '(' || *output =='{')) //  else {}  OR else if () {}
				{
					++hasBody;
					bodyKind[hasBody] = 'i'; // if
					if (*output != '{') output = strchr(output,'{');
					output += 2;
					continue;
				}
				else 
				{
					bool wrong = false;
					if (spelling == ABSTRACT_SET_MEMBER && word[1])
					{
						WORDP D = FindWord(word,0,LOWERCASE_LOOKUP);
						if (!D || D->inferMark != inferMark) D = FindWord(word,0,UPPERCASE_LOOKUP);
						if (D && D->inferMark == inferMark) badWord = true;
					}
					char copy[MAX_WORD_SIZE];
					MakeLowerCopy(copy,(*word == '\'') ? (word+1) : word);
					if (spelling & ABSTRACT_SPELL && word[1]) // ignore punctuation
					{
						if (*word == '*' || IsDigit(*word)) continue;	 // ignore wildcards, numbers, or jump zones
						size_t len = strlen(copy);
						if (copy[len-1] == '{') continue;  // an IF jump zone
						while (len-- && IsPunctuation(copy[len])) copy[len] = word[len] = 0;  // remove trailing punctuation.
						char* apostrophe = strchr(copy,'\'');
						WORDP D = FindWord(copy);
						if (!D || !(D->properties & PART_OF_SPEECH)) 
						{
							if (!D || !(D->internalBits & HAS_SUBSTITUTE)) // not known, try for sentence head one
							{
								char word[MAX_WORD_SIZE];
								sprintf(word,"<%s",copy);
								D = FindWord(word);
							}
							if (!D) D = FindWord(copy,0,UPPERCASE_LOOKUP);
						}
						if (D && (D->properties & PART_OF_SPEECH || D->internalBits & HAS_SUBSTITUTE)){;} //  we know this word
						else if (D && D->internalBits & QUERY_KIND) {;} // a query
						else if (IsUrl(copy,0) || apostrophe || copy[0] == '_' || copy[0] == '$' || copy[0] == '%' || copy[0] == '@' || copy[0] == '"') {;} 
						else if (!FindCanonical( copy, 1,true)) wrong = badspell = true;
					}
					if (wrong) 
						Log(STDUSERLOG,"%s\r\n",word);
					else sprintf(outputPtr,"%s ",word);
					outputPtr += strlen(outputPtr);
				}
			}
		} 
		
		*end = ENDUNIT; // restore data

		// we have the output, what to do with it

		if (spelling & ABSTRACT_SET_MEMBER && !badWord) *buffer = 0; // only do lines with censored words, showing context
		if (spelling & ABSTRACT_SPELL) *buffer = 0; // only do lines with bad spelling
		size_t len = strlen(buffer);
		if (choiceCharacters) len -= choiceCharacters - 1; // dont zero out len
		bool headit = false;
 		if (len > lineLimit && len) headit = true; // if line is long enough to report
		else if (!*buffer && !lineLimit && !(spelling & ABSTRACT_SET_MEMBER) && !(spelling & ABSTRACT_SPELL) && !(spelling & ABSTRACT_PRETTY)) // nothing to show but we want to see anything
		{
			if (!(spelling & ABSTRACT_NOCODE) )
			{
				strcpy(buffer," { code }");
				headit = true;
			}
		}
		if (headit && !(spelling & ABSTRACT_RESTRICTIONS)) 
		{
			DoHeader(indent,basic,in,id,spelling); 
			if (lineLimit) Log(STDUSERLOG,"(%d) ",len);
			Log(STDUSERLOG,"%s\r\n",buffer);
		}
		*end = ENDUNIT;
		rule = FindNextRule(NEXTRULE,rule,id);
	}
	if (in) fclose(in);
	FreeBuffer();
	FreeBuffer();
}

static void MarkDownHierarchy(MEANING T)
{
    if (!T) return;
    WORDP D = Meaning2Word(T);
	if (D->inferMark == inferMark) return;	
	D->inferMark = inferMark;

	if (*D->word == '~') // follow members of set
	{
		FACT* F = GetObjectHead(D);
		while (F)
		{
			if (F->verb == Mmember)
			{
				MEANING M = F->subject;
				WORDP S = Meaning2Word(M);
				if (S->inferMark != inferMark) MarkDownHierarchy(M);
			}
			F = GetObjectNext(F);
		}
	}
}

void CopyFile2File(const char* newname,const char* oldname, bool automaticNumber)
{
	char name[MAX_WORD_SIZE];
	FILE* out;
	if (automaticNumber) // get next number
	{
		const char* at = strchr(newname,'.');	//   get suffix
		int len = at - newname;
		strncpy(name,newname,len);
		strcpy(name,newname); //   base part
		char* endbase = name + len;
		int j = 0;
		while (++j)
		{
			sprintf(endbase,"%d.%s",j,at+1);
			out = fopen(name,"rb");
			if (out) fclose(out);
			else break;
		}
	}
	else strcpy(name,newname);

	FILE* in = fopen(oldname,"rb");
	if (!in) 
	{
		unlink(name); // kill any old one
		return;	
	}
	out = fopen(name,"wb");
	if (!out) // cannot create 
	{
		fclose(out);
		return;
	}
	fseek (in, 0, SEEK_END);
	unsigned long size = ftell(in);
	fseek (in, 0, SEEK_SET);

	char buffer[RECORD_SIZE];
	while (size >= RECORD_SIZE)
	{
		fread(buffer,1,RECORD_SIZE,in);
		fwrite(buffer,1,RECORD_SIZE,out);
		size -= RECORD_SIZE;
	}
	if (size > 0)
	{
		fread(buffer,1,size,in);
		fwrite(buffer,1,size,out);
	}

	fclose(out);
	fclose(in);
}

static void C_Abstract(char* input)
{
	int spelling = 0;
	if (IsDigit(*input)) input = SkipWhitespace(ReadInt(input,lineLimit)); // line length limit
	char word[MAX_WORD_SIZE];
	if (!strnicmp(input,"spell",5))
	{
		spelling |= ABSTRACT_SPELL;
		input = ReadCompiledWord(input,word);
	}
	if (!strnicmp(input,"censor",6))
	{
		spelling |= ABSTRACT_SET_MEMBER;
		input = ReadCompiledWord(input+6,word);
		NextinferMark();
		MarkDownHierarchy(MakeMeaning(StoreWord(word)));
	}
	if (!strnicmp(input,"canon",5))
	{
		spelling |= ABSTRACT_CANONICAL | ABSTRACT_PRETTY;
		input = ReadCompiledWord(input,word);
	}
	if (!strnicmp(input,"pretty",5))
	{
		spelling |= ABSTRACT_PRETTY;
		input = ReadCompiledWord(input,word);
	}
	if (!strnicmp(input,"vp",2))
	{
		spelling |= ABSTRACT_VP;
		input = ReadCompiledWord(input,word);
	}
	if (!strnicmp(input,"nocode",6))
	{
		spelling |= ABSTRACT_NOCODE;
		input = ReadCompiledWord(input,word);
	}
	input = SkipWhitespace(input);

	abstractBuffer = AllocateBuffer();

	size_t len = 0;
	char* x = strchr(input,'*');
	if (x) len = x - input;
	else if (*input == '~') len = strlen(input);

	// get topic if given
	if (*input == '~' && !x)
	{
		DisplayTopic(input,spelling);
		DisplayTables(input);
	}
	else if (*input && *input != '~') // from topic file
	{
		char filename[MAX_WORD_SIZE];
		ReadCompiledWord(input,filename);
		for (unsigned int i = 1; i <= lastTopic; ++i) 
		{
			if (!stricmp(GetTopicFile(i),filename)) DisplayTopic(GetTopicName(i),spelling);
		}
	}
	// otherwise do all
	else
	{
		for (unsigned int i = 1; i <= numberOfTopics; ++i) 
		{
			if (len && strnicmp(GetTopicName(i),input,len)) continue;
			DisplayTopic(GetTopicName(i),spelling);
		}
		DisplayTables("*");
	}
	FreeBuffer();
}

static void C_Diff(char* input)
{
	char file1[MAX_WORD_SIZE];
	char file2[MAX_WORD_SIZE];
	input = ReadCompiledWord(input,file1);
	input = ReadCompiledWord(input,file2);
	input = SkipWhitespace(input);
	char separator = *input;
	FILE* in1 = fopen(file1,"rb");
	if (!in1) 
	{
		Log(STDUSERLOG,"%s does not exist\r\n",file1);
		return;
	}
	FILE* in2 = fopen(file2,"rb");
	if (!in2) 
	{
		Log(STDUSERLOG,"%s does not exist\r\n",file2);
		fclose(in1);
		return;
	}
	char* buf1 = AllocateBuffer();
	char* buf2 = AllocateBuffer();
	unsigned int n = 0;
	unsigned int err = 0;
	while (ALWAYS)
	{
		++n;
		if (!fgets(buf1,maxBufferSize,in1)) 
		{
			if (fgets(buf2,maxBufferSize,in2)) 
			{
				Log(STDUSERLOG,"2nd file has more at line %d: %s\r\n",n,buf2);
				++err;
			}
			break;
		}
		if (!fgets(buf2,maxBufferSize,in2)) 
		{
			++err;
			Log(STDUSERLOG,"1st file has more at line %d: %s\r\n",n,buf1);
			break;
		}
		// dont show the input after this
		char* sep1 = strchr(buf1,separator);
		if (sep1) *sep1 = 0;
		char* sep2 = strchr(buf2,separator);
		if (sep2) *sep2 = 0;

		// clean up white space
		char* start1 = buf1;
		while (*start1 == '\r' || *start1 == '\n' || *start1 == ' ' || *start1 == '\t') ++start1;
		size_t len1 = strlen(start1);
		while (len1 && (start1[len1-1] == '\r' || start1[len1-1] == '\n' || start1[len1-1] == ' ' || start1[len1-1] == '\t')) --len1;

		char* start2 = buf2;
		while (*start2 == '\r' || *start2 == '\n' || *start2 == ' ' || *start2 == '\t') ++start2;
		size_t len2 = strlen(start2);
		while (len2 && (start2[len2-1] == '\r' || start2[len2-1] == '\n' || start2[len2-1] == ' ' || start2[len2-1] == '\t')) --len2;
	
		if (len1 != len2 || strncmp(start1,start2,len1)) 
		{
			if (sep1) *sep1 = ':';
			if (sep2) *sep2 = ':';
			Log(STDUSERLOG,"%5d<<    %s\r\n",n,start1);
			Log(STDUSERLOG,"     >>    %s\r\n",start2);
			++err;
		}
		// else
		// {
			// Log(STDUSERLOG,"     ++    %s\r\n",start1);
			// Log(STDUSERLOG,"     --    %s\r\n",start2);
		// }
	}
	FreeBuffer();
	FreeBuffer();
	fclose(in2);
	fclose(in1);
	Log(STDUSERLOG,"%d lines differ.\r\n",err);
}

static void TrimIt(char* name,uint64 flag) 
{
	//  0 = user->bot
	//  1 = bot->user
	//  2 = topic user-bot
	//  3 = topic bot->user last sentence
	//  4 = indent bot
	//  5 = indent human
	//  6 = user only
	//  7 = tags verify user-bot
	//  8 = topic indent bot5 
	
	char prior[MAX_BUFFER_SIZE];
	FILE* in = FopenReadWritten(name);
	if (!in) return;
	if ((++filesSeen % 1000) == 0) printf( ((filesSeen % 100000) == 0) ? (char*)"+\r\n" : (char*) "."); // mark progress thru files

	bool header = false;
	FILE* out = fopen("TMP/tmp.txt","ab");

	char file[MAX_WORD_SIZE];
	*file = 0;
	*prior = 0;
	char* at;
	while (ReadALine(readBuffer,in) ) 
	{
		size_t len = strlen(readBuffer);
   		if (!len) continue;

		// fields are: type, user, bot, ip, resulting topic, current volley id, user input, bot output, dateinfo, possible f:,  followed by rule tags for each issued output.
		
		if (strncmp(readBuffer,"Respond:",8)) // normal line?
		{
			if (strncmp(readBuffer,"Start:",6)) continue;	// junk line
			// other things you could do with start line here
			continue;
		}

		// normal volley

		char user[MAX_WORD_SIZE];
		char* ptr = strstr(readBuffer,"user:") + 5;
		ptr = ReadCompiledWord(ptr,user);

		char bot[MAX_WORD_SIZE];
		ptr = ReadCompiledWord(ptr+4,bot); // skip "bot:"

		char ip[MAX_WORD_SIZE];
		ptr += 3; // skip ip:
		char* outp = ip;
		while (*ptr != ' ') *outp++ = *ptr++;
		*outp = 0;

		char topic[MAX_WORD_SIZE];
		at = strchr(readBuffer,'(') + 1;
		char* end = strchr(at,')');
		*end = 0;
		at = ReadCompiledWord(at,topic);	
		
		char volley[MAX_WORD_SIZE];
		at = ReadCompiledWord(end+1,volley); 
		char* input = SkipWhitespace(at); // now points to user input start

		char* output = strstr(at," ==> ");
		if (!output) continue;
		*output = 0;	// end of input
		output += 5;
		output = SkipWhitespace(output);  // start of output

		char* when = strstr(output,"When:");
		if (!when)  continue;
		*when = 0;	// end of output
		when += 5; // start of datestamp

		char* why = strstr(when,"Why:");
		if (why) // why may not exist (like postprocess output and gesture output)
		{
			*why = 0;	// end of datestamp
			why += 5;  // beginnings of whys
		}
		else why = "";
		
		// remove our internal reply markers
		char* x = output;
		while ((x = strchr(x,'`'))) *x++ = ' ';

		// now show the data
		char display[MAX_BUFFER_SIZE];
		display[0] = 0;

		if (flag == 0) sprintf(display,"\r\n%s   =>   %s\r\n",input,output); //  showing both as pair, user first
		else if (flag == 1)  sprintf(display,"\r\n%s   =>   %s\r\n",prior,input);  // pair, bot first
		else if ( flag == 2) sprintf(display,"\r\n%s %s   =>   %s\r\n",topic,input,output); //  showing both as pair, user first, with topic of response
		else if ( flag == 3) sprintf(display,"%s %s   =>   %s\r\n",topic,prior,input); // topic bot user
 		else if ( flag == 4) sprintf(display,"%s\r\n\t%s\r\n",input,output); // 2liner, indented computer
		else if ( flag == 8) sprintf(display,"%s\r\n\t(%s) %s\r\n",input,topic,output); // 2liner, indented computer   + topic
  		else if ( flag == 5) sprintf(display,"%s\r\n\t%s\r\n",output,input); // 2liner, indented user 
 		else if ( flag == 6) sprintf(display,"%s\r\n",input); // user input only
		else if ( flag == 7) // figure out matching verify
		{
			char tag[MAX_WORD_SIZE];
			char file[MAX_WORD_SIZE];
			char* whyTag = why;
			bool start = true;
			char* atOutput = output;
			if (*whyTag != '~') // had no main motivation
			{
				fprintf(out,"?-?   %s => %s\r\n",input,atOutput); //  showing both as pair, user first, with tag of response and verify input reference
			}
			while (*whyTag == '~') // do each tag
			{
				whyTag = ReadCompiledWord(whyTag,tag); // get tag  which is topic.x.y or topic.x.y.a.b (reuse) and optional label which is whytag
				char* separation = strchr(atOutput,'|');
				if (separation) *separation = 0; // block out rest of output for a moment
				char* dot;
				dot = strchr(tag,'.'); // split of topic from rest of tag
				if (dot)
				{
					*dot = 0;
					strcpy(topic,tag); // get the topic of the tag
					*dot = '.';
				}

				char reuseTag[MAX_WORD_SIZE];
				strcpy(reuseTag,tag);
				unsigned int reuseTopLevel = atoi(dot+1);  

				// see if reuse tag involved
				char* dot1 = strchr(dot+1,'.');	// find rejoinder ptr 
				dot1 = strchr(dot1+1,'.');	// is there a reuse ptr  topic.x.y with .a.b
				if (dot1) 
				{
					*dot1 = 0;
					strcpy(reuseTag,tag);		// the reused tag value
					memmove(dot+1,dot1+1,15);	// skip over normal tag and show tag that called the reused rule
				}
				
				unsigned int wantedToplevel = atoi(dot+1);

				// get verify file
				sprintf(file,"VERIFY/%s.txt",topic+1);
				FILE* in = FopenReadWritten(file);

				char verify[MAX_WORD_SIZE];
				*verify = 0;
				if (in)
				{
					while (ReadALine(display,in)) // find matching verify
					{
						char word[MAX_WORD_SIZE];
						char* ptr = ReadCompiledWord(display,word);
						if (!stricmp(word,tag))
						{
							char junk[MAX_WORD_SIZE];
							ptr = ReadCompiledWord(ptr,junk); // skip the marker
							strcpy(verify,ptr);
							break;
						}
						if (!stricmp(word,reuseTag) && !*verify) // the reuse rule does not have priority, the original rule does
						{
							char junk[MAX_WORD_SIZE];
							ptr = ReadCompiledWord(ptr,junk); // skip the marker
							strcpy(verify,ptr);
						}
						dot = strchr(word,'.');
						unsigned int toplevel = atoi(dot+1);
						if (toplevel > wantedToplevel && toplevel > reuseTopLevel) break; // too far.
					}

					fclose(in);
				}
				char pattern[MAX_WORD_SIZE];
				*pattern = 0;
				unsigned int topicid;
				int id;
				char* rule = GetRuleTag(topicid,id,tag);
				if (rule) GetPattern(rule,NULL,pattern);
				else rule = "-";
				if (!*pattern) strcpy(pattern,"()"); // gambits for example
				if (start) start = false;
				else fprintf(out,"    ");
				fprintf(out,"%s|\"%s\"|%c: %s|%s|%s\r\n",tag,verify,*rule,pattern,input,atOutput); //  showing both as pair, user first, with tag of response and verify input reference
				if (separation) atOutput = separation + 1; // next output
			}
			*display = 0;
		}
		if (*display) 
		{
			if (!header) 
			{
				header = true;
				char* type = " ";
				if ( flag == 0) type = "user->bot";
				else if (flag == 1) type = "bot->user";
				else if ( flag == 2) type = "topic user->bot";
				else if ( flag == 3) type = "topic bot->user";
				else if ( flag == 4) type = "indent bot";
				else if ( flag == 5) type = "indent human";
				else if ( flag == 6) type = "user only";
				else if ( flag == 7) type = "tags verify user->bot";
				else if ( flag == 8) type = "indent bot + topic";
				char* last = strrchr(name,'/');
				if (last) name = last;
				fprintf(out,"\r\n# ----------------- %s   %s\r\n",name,type);
			}
			fprintf(out,"%s",display);
		}

		strcpy(prior,output); // what bot said previously
	}
    fclose(in);
    if (out) fclose(out);
	Log(STDUSERLOG,"Trim %s complete\r\n",name);
}

static void C_Trim(char* input) // create simple file of user chat from directory
{   
 	char word[MAX_WORD_SIZE];
	char file[MAX_WORD_SIZE];
	char* original = input;
	*file = 0;
	input = ReadCompiledWord(input,word);
	filesSeen = 0;

	if (!strnicmp("log-",word,4)) // is just a file name
	{
		*directory = 0;	
		if (strstr(word,"txt")) sprintf(file,"USERS/%s",word);
		else sprintf(file,"USERS/%s.txt",word);
		input = ReadCompiledWord(input,word);
	}
	else if (IsAlpha(*word)) // directory name or simple user name
	{
		*directory = 0;	
		sprintf(file,"USERS/log-%s.txt",word);
		FILE* x = fopen(file,"rb");
		if (x) fclose(x); // see if file exists. if not, then its a directory name
		else 
		{
			strcpy(directory,word);
			*file = 0;
		}
		input = ReadCompiledWord(input,word);

	}
	else strcpy(directory,"LOGS");

	unsigned int flag = atoi(word); 
	FILE* out = fopen("TMP/tmp.txt","wb");
	fprintf(out,"# %s\r\n",original);
	Log(STDUSERLOG,"# %s\r\n",input);
	fclose(out);

	if (!*file) WalkDirectory(directory,TrimIt,flag);
	else TrimIt(file,flag);
	printf("\r\n");
}

void ReadBNC(char* buffer);
#ifdef DISCARDDICTIONARYBUILD
void ReadBNC(char* buffer){;}
#endif

CommandInfo commandSet[] = // NEW
{//  actual command names must be lower case 
	{"",0,""},
	
	{"\r\n---- Debugging commands",0,""}, 
	{":debug",C_Debug,"run debugger on the given line of input",false},
	{":do",C_Do,"Execute the arguments as an output stream, e.g., invoke a function, set variables, etc",true},  
	{":silent",C_Silent,"toggle silent - dont show outputs",false}, 
	{":log",C_Log,"dump message into log file",false}, 
	{":noreact",C_NoReact,"Disable replying to input",false}, 
	{":retry",C_Retry,"Back up and try replacement user input or just redo last sentence",false}, 
	{":say",C_Say,"Make chatbot say this line",false}, 
	{":skip",C_Skip,"Erase next n gambits",false}, 
	{":show",C_Show,"All, Input, Mark, Number, Pos, Stats, Topic, Topics, Why",true},
	{":trace",C_Trace,"Set trace variable (all none basic prepare match output pattern infer query substitute hierarchy fact control topic pos)",true},
	{":why",C_Why,"Show rules causing most recent output",false}, 
	
	{"\r\n---- Fact info",0,""}, 
	{":allfacts",C_AllFacts,"Write all facts to TMP/facts.tmp",true}, 
	{":facts",C_Facts,"Display all facts with given word or meaning",true}, 
	{":userfacts",C_UserFacts,"Display current user facts",true}, 

	{"\r\n---- Topic info",0,""}, 
	{":gambits",C_Gambits,"Show gambit sequence of topic",true}, 
	{":pending",C_Pending,"Show current pending topics list",true}, 
	{":topicstats",C_TopicStats,"Show stats on all topics or named topic or NORMAL for non-system topics",true},
	{":topicinfo",C_TopicInfo,"Show information on a topic",true}, 
	{":topics",C_Topics,"Show topics that given input resonates with",true}, 
	{":where",C_Where,"What file is the given topic in",true}, 

	{"\r\n---- System info",0,""},  
	{":commands",C_Commands,"Show all :commands",true}, 
	{":definition",C_Definition,"Show code of macro named",true},
	{":functions",C_Functions,"List all defined system ^functions",true},
	{":macros",C_Macros,"List all user-defined ^macros and plans",true},
	{":memstats",C_MemStats,"Show memory allocations",true},
	{":variables",C_Variables,"Display current user/sysytem/all variables",true}, 
	{":who",C_Who,"show current login/computer pair",true}, 
		
	{"\r\n---- Word information",0,""}, 
	{":down",C_Down,"Show wordnet items inheriting from here or concept members",false},  
	{":concepts",C_Concepts,"Show concepts triggered by this word",false},  
	{":findwords",C_FindWords,"show words matching pattern of letters and *",true},
	{":hasflag",C_HasFlag,"List words of given set having or !having some system flag",false}, 
	{":nonset",C_Nonset,"List words of POS type not encompassed by given set",false}, 
	{":overlap",C_Overlap,"Direct members of set x that are also in set y somehow",false}, 
	{":up",C_Up,"Display concept structure above a word",false}, 
	{":word",C_Word,"Display information about given word",true}, 
	{":worddump",C_WordDump,"show words via hardcoded test",true}, 

	{"\r\n---- System Control commands",0,""}, 
	{":bot",C_Bot,"Change to this bot",false},  
	{":quit",C_Quit,"Exit ChatScript",true}, 
	{":reset",ResetUser,"Start user all over again, flushing his history",false}, 
	{":restart",C_Restart,"Restart Chatscript, optionally naming a language",false}, 
	{":user",C_User,"Change to named user, not new conversation",false}, 
	
	{"\r\n---- Build commands",0,""}, 
	{":build",C_Build,"Compile a script - filename {nospell,outputspell,reset}",false}, 
	{":builddict",BuildDictionary," short, short init, or wordnet are options instead of default full",false}, 
	{":clean",C_Clean,"Convert source files to NL instead of CR/LF for unix",false}, 

	{"\r\n---- Script Testing",0,""},  
	{":autoreply",C_AutoReply,"[OK,Why] use one of those as all input.",false}, 
	{":document",C_Document,"Switch input to named file/directory as a document {single, echo}",false}, 
	{":prepare",C_Prepare,"Show results of tokenization, tagging, and marking on a sentence",false},  
	{":source",C_Source,"Switch input to named file",false}, 
	{":testpattern",C_TestPattern,"See if a pattern works with an input.",false}, 
	{":testtopic",C_TestTopic,"Try named topic responders on input",false}, 
	{":verify",C_Verify,"Given test type & topic, test that rules are accessible. Tests: pattern (default), blocking(default), keyword(default), sample, gambit, all.",false},  

	{"\r\n---- Server commands",0,""}, 
	{":crash",0,"Simulate a crash",false}, 
	{":flush",C_Flush,"Flush server cached user data to files",false}, 
	
	{"\r\n---- Analytics",0,""}, 
	{":abstract",C_Abstract,"Display overview of ChatScript topics",false}, 
	{":diff",C_Diff,"match 2 files and report lines that differ",false}, 
	{":trim",C_Trim,"Strip excess off chatlog file to make simple file TMP/tmp.txt",true}, 
	
	{"\r\n---- internal testing support",0,""}, 
	{":pennformat",C_PennFormat,"rewrite penn tagfile (eg as output from stanford) as one liners",true}, 
	{":pennmatch",C_PennMatch,"FILE {raw basic} compare penn file against internal result",true}, 
	{":pennnoun",C_PennNoun,"locate mass nouns in pennbank",true}, 
	{":pos",C_POS,"Show results of tokenization and tagging",false},  
	{":sortconcept",C_SortConcept,"Prepare concept file alphabetically",false}, 
	{":timepos",C_TimePos,"compute wps average to prepare inputs",false},
	{":verifypos",C_VerifyPos,"Regress pos-tagging using default REGRESS/postest.txt file or named file",false},
	{":verifyspell",C_VerifySpell,"Regress spell checker against file",false}, 
	{":verifysubstitutes",C_VerifySubstitutes,"Regress test substitutes of all kinds",false}, 
#ifndef WIN32
	{":bnc",ReadBNC,"BNC POS stats",false}, 
#endif
	{0,0,""},	
};

static int VerifyAuthorization(FILE* in) //   is he allowed to use :commands
{
	char buffer[MAX_WORD_SIZE];
	if (!in) return 1;			//   no restriction file
	unsigned int result = 0;
	while (ReadALine(buffer,in) )
    {
		if (!stricmp(buffer,"all") || !stricmp(buffer,callerIP) || (*buffer == 'L' && buffer[1] == '_' && !stricmp(buffer+1,loginID))) //   allowed by IP or L_loginname
		{ 
			result = 1;
			break;
		}
	}
	fclose(in);
	return result;
}

void SortTopic(WORDP D,uint64* junk)
{
	if (!(D->internalBits & (BUILD0|BUILD1))) return; // ignore internal system topics (of which there are none)
	if (D->systemFlags & TOPIC) Sortit(D->word,(int)(long long)junk);
}

void SortTopic0(WORDP D,uint64 junk)
{
	if (!(D->internalBits & (BUILD0|BUILD1))) return; // ignore internal system concepts
	if (!(D->systemFlags & TOPIC)) return;
	CreateFact(MakeMeaning(D),Mmember,MakeMeaning(StoreWord("~_dummy",AS_IS)));
}

typedef std::vector<char*>::const_iterator  citer;

static bool myFunction (char* i,char* j) 
{ 
	if (*i == '~' && *j != '~') return true; // all concepts come first
	if (*i != '~' && *j == '~') return false;
	return stricmp(i,j) < 0; 
}

static bool myInverseFunction (char* i,char* j) 
{ 
	if (*i == '~' && *j != '~') return true; // all concepts come first
	if (*i != '~' && *j == '~') return false;
	return stricmp(i,j) > 0; 
}

void Sortit(char* name,int oneline)   
{
	FILE* out = fopen("cset.txt","ab");
	WORDP D = FindWord(name,0);
	if (!D) return;

	char word[MAX_WORD_SIZE];
	MakeUpperCopy(word,name);
	int cat = FindTopicIDByName(name); // note if its a category, need to dump its flags also

	// get the concept members 
	std::vector<char*> mylist;
	FACT* F = GetObjectHead(D);
	while (F)
	{
        if (F->verb == Mmember || F->verb == Mexclude)
		{
			strcpy(word,WriteMeaning(F->subject));
			if (*word == '~') MakeUpperCase(word); // cap all concepts and topics
			WORDP E = StoreWord(word);
			if (F->verb == Mexclude) AddInternalFlag(E,BEEN_HERE); // exclude notation
			mylist.push_back(E->word);
		}
		F = GetObjectNext(F);
	}

	// sort the member list, but do special concept reversed so comes in proper to flood dictionary in correct order
	std::sort(mylist.begin(), mylist.end(),!stricmp(name,"~a_dummy") ? myInverseFunction : myFunction);

	// dump the concept lists
	bool drop = false;
	bool close = false;
	char* buffer = AllocateBuffer();
	*buffer = 0;
	for (citer it = mylist.begin(), end = mylist.end(); it < end; ++it)   
	{	  
		if (close) 
		{
			fprintf(out,"%s\r\n",buffer);
			*buffer = 0;
			close = false;
			for (unsigned int j = 1; j <= 10; ++j) strcat(buffer," ");
		}
		if (!drop) // put out the header
		{
			sprintf(buffer, (D->systemFlags & TOPIC) ? (char*) "topic: %s " : (char*) "concept: %s ",name);
			drop = true;
			if (cat)
			{
				int flags = GetTopicFlags(cat);
                if (flags & TOPIC_LOWPRIORITY) strcat(buffer,"deprioritize ");
                if (flags & TOPIC_NOBLOCKING) strcat(buffer,"noblocking ");
				if (flags & TOPIC_NOKEYS) strcat(buffer,"nokeys ");
				if (flags & TOPIC_NOPATTERNS) strcat(buffer,"nopatterns ");
				if (flags & TOPIC_NOSAMPLES) strcat(buffer,"nosamples ");
				if (flags & TOPIC_NOGAMBITS) strcat(buffer,"nogambits ");
	            if (flags & TOPIC_KEEP) strcat(buffer,"keep ");
                if (flags & TOPIC_NOSTAY) strcat(buffer,"nostay ");
                if (flags & TOPIC_PRIORITY) strcat(buffer,"priority ");
                if (flags & TOPIC_RANDOM) strcat(buffer,"random ");
                if (flags & TOPIC_REPEAT) strcat(buffer,"repeat ");
				if (flags & TOPIC_SAFE) strcat(buffer,"safe ");
                if (flags & TOPIC_SYSTEM) strcat(buffer,"system ");
			}
			else
			{
				uint64 properties = D->properties;
				uint64 bit = START_BIT;	
				while (bit)
				{
					if (properties & bit ) sprintf(buffer + strlen(buffer),"%s ",FindNameByValue(bit));
					bit >>= 1;
				}
				properties = D->systemFlags;
				bit = START_BIT;
				while (bit)
				{
					if (properties & bit  && !(bit & (CONCEPT|TOPIC))) sprintf(buffer + strlen(buffer),"%s ",FindName2ByValue(bit));
					bit >>= 1;
				}
			}
			strcat(buffer,"(");
		}
		char* b = buffer + strlen(buffer);
		WORDP G = FindWord(*it);
		if (G->internalBits & BEEN_HERE) // marked for exclude
		{
			G->internalBits ^= BEEN_HERE;
			sprintf(b,"!%s ",*it ); 
		}
		else sprintf(b,"%s ",*it ); 
		if (strlen(buffer) > 180 && !oneline) close = true;
	}
	if (drop) fprintf(out,"%s)\r\n",buffer);
	FreeBuffer();
	fclose(out);
}

#endif

///// ALWAYS AVAILABLE

int DoCommand(char* input,char* output,bool authorize)
{
#ifndef DISCARDTESTING
	if (server && authorize && !VerifyAuthorization(FopenReadOnly("authorizedIP.txt"))) 
	{
		Log(SERVERLOG,"Command %s issued but not authorized\r\n",input);
		return true;
	}
	*currentFilename = 0;
	char* ptr = NULL;
	ReadNextSystemToken(NULL,ptr,NULL,false,false);		// flush any pending data in input cache
	if (stricmp(input,":why")) responseIndex = 0;
	return Command(input,output); 
#else
	if (server) Log(SERVERLOG,"Command %s issued but testing is discarded\r\n",input);
	return COMMANDED;
#endif
}
