#include "common.h"

unsigned int currentOutputLimit = MAX_BUFFER_SIZE;	// max size of current output base
char* currentOutputBase = NULL;		// current base of buffer which must not overflow
char* currentRuleOutputBase = NULL;	// the partial buffer within outputbase started for current rule, whose output can be canceled.

#define MAX_OUTPUT_NEST 50
static char* oldOutputBase[MAX_OUTPUT_NEST];
static char* oldOutputRuleBase[MAX_OUTPUT_NEST];
static unsigned int oldOutputLimit[MAX_OUTPUT_NEST];
static int oldOutputIndex = 0;
unsigned int outputNest = 0;

#ifdef JUNK
Special strings:

When you put ^"xxxx" as a string in a normal output field, it is a format string. By definition you didnt need it compileable.
It will be treated...

When you use it as an argument to a function (top level), it is compiled.

When you put ^"xxxx" as a string in a table, it will be compiled and be either a pattern compile or an output compile.   
Internally that becomes "^xxx" which is fully executable.


#endif

void ResetOutput()
{
	outputNest = 0;
}

void PushOutputBuffers()
{
	oldOutputBase[oldOutputIndex] = currentOutputBase;
	oldOutputRuleBase[oldOutputIndex] = currentRuleOutputBase;
	oldOutputLimit[oldOutputIndex] = currentOutputLimit;
	++oldOutputIndex;
	if (oldOutputIndex == MAX_OUTPUT_NEST) --oldOutputIndex; // just fail it
}

void PopOutputBuffers()
{
	--oldOutputIndex;
	if (oldOutputIndex < 0) ++oldOutputIndex;
	currentOutputBase = oldOutputBase[oldOutputIndex];
	currentRuleOutputBase = oldOutputRuleBase[oldOutputIndex];
	currentOutputLimit = oldOutputLimit[oldOutputIndex];
}

void AllocateOutputBuffer()
{
	currentRuleOutputBase = currentOutputBase = AllocateBuffer();
	currentOutputLimit = maxBufferSize;
}

void FreeOutputBuffer()
{
	FreeBuffer(); // presumed the current buffer allocated via AllocateOutputBuffer
}

static int CountParens(char* start) 
{
	int paren = 0;
	start--;		//   back up so we can start at beginning
	while (*++start) if (*start == '"') ++paren; 
	return paren;
}

static bool IsAssignmentOperator(char* word)
{
	if ((*word == '<' || *word == '>') && word[1] == *word && word[2] == '=') return true;	 // shift operators
	return ((*word == '=' && word[1] != '=' && word[1] != '>') || (*word && *word != '!' && *word != '\\' && *word != '=' && word[1] == '='   )); // x = y, x *= y
}

char* ReadCommandArg(char* ptr, char* buffer,unsigned int& result,unsigned int control)
{
	if (control == 0) control |= OUTPUT_KEEPSET | OUTPUT_NOTREALBUFFER | OUTPUT_ONCE | OUTPUT_NOCOMMANUMBER;
	else control |= OUTPUT_ONCE | OUTPUT_NOCOMMANUMBER;
	return FreshOutput(ptr,buffer,result,control);
}

char* ReadShortCommandArg(char* ptr, char* buffer,unsigned int& result,unsigned int control)
{
	if (control == 0) control |= OUTPUT_KEEPSET | OUTPUT_NOTREALBUFFER | OUTPUT_ONCE | OUTPUT_NOCOMMANUMBER;
	else control |= OUTPUT_ONCE | OUTPUT_NOCOMMANUMBER;
	return FreshOutput(ptr,buffer,result,control,MAX_WORD_SIZE);
}

static char* AddFormatOutput(char* what, char* output)
{
	size_t len = strlen(what);
	if ((output - currentOutputBase + len) > (currentOutputLimit - 50)) 
		ReportBug("format string revision too big %s\r\n",output) // buffer overflow
	else
	{
		strcpy(output,what);
		output += len;
	}
	return output;
}

void ReformatString(char* output, char* input,unsigned int controls) // take ^"xxx" format string and perform substitutions on variables within it
{
	size_t len = strlen(input) - 1;
	char c = input[len];
	input[len] = 0;	// remove closing "
	if (*input == ':') // has been compiled by script compiler. safe to execute fully. actual string is "^:xxxxx" 
	{
		++input;
		unsigned int result;
 		Output(input,output,result,controls|OUTPUT_EVALCODE); // directly execute the content
		input[len] = c;
		return;
	}

	char* start = output;
	*output = 0;
	char mainValue[3];
	mainValue[1] = 0;
	while (*input)
	{
		if (*input == '$' && !IsDigit(input[1])) // user variable
		{
			char* base = input;
			while (*++input && !IsWhiteSpace(*input) && *input != '.') // find end of variable name
			{
				if (*input != '$' && !IsLegalNameCharacter(*input)) 
				{
					char* at = IsUTF8(input);
					if (at) input = at-1;
					else break;
				}
			}
			c = *input;
			*input = 0;
			char* value = GetUserVariable(base);
			*input = c;
			output = AddFormatOutput(value, output); 
		}
		if (*input == '%' && IsAlpha(input[1])) // system variable
		{
			char* base = input;
			while (*++input && IsAlpha(*input) && *input != '.') {;} // find end of variable name
			c = *input;
			*input = 0;
			char* value = SystemVariable(base,NULL);
			if (*value) output = AddFormatOutput(value, output); 
			else if (!FindWord(base)) output = AddFormatOutput(base, output); // not a system variable
			*input = c;
		}
		else if (*input == '_' && IsDigit(input[1])) // canonical match variable
		{
			char* base = input++;
			if (IsDigit(*input)) ++input; // 2nd digit
			output = AddFormatOutput(GetwildcardText(GetWildcardID(base),true), output); 
		}
		else if (*input == '\'' && input[1] == '_' && IsDigit(input[2])) // quoted match variable
		{
			char* base = ++input;
			++input;
			if (IsDigit(*input)) ++input; // 2nd digit
			output = AddFormatOutput(GetwildcardText(GetWildcardID(base),false), output);
		}
		else if (*input == '@' && IsDigit(input[1])) // factset
		{
			// get end of reference
			char* base = input;
			input = GetSetType(input);
			size_t len = 0;
			if (!strnicmp(input,"subject",7)) len = 7;
			else if (!strnicmp(input,"verb",4)) len = 4;
			else if (!strnicmp(input,"object",6)) len = 6;
			input += len;
			
			// go get value of reference and copy over
			char c = *input;
			char* value = AllocateBuffer();
			unsigned int result;
			*input = 0;
			ReadCommandArg(base,value,result);
			*input = c;
			output = AddFormatOutput(value, output);
			FreeBuffer();
			if (result & ENDCODES) 
			{
				output = start + 1;  // null return
				break;
			}
		}
		else if (*input == '^' && IsDigit(input[1])) // function variable
		{
			char* base = input; 
			while (*++input && IsDigit(*input)){;} // find end of function variable name (expected is 1 digit)
			char* tmp = callArgumentList[atoi(base+1)+fnVarBase];
			// if tmp turns out to be $var or _var %var, need to recurse to get it

			if (*tmp == '$' && !IsDigit(tmp[1])) // user variable
			{
				char* value = GetUserVariable(tmp);
				output = AddFormatOutput(value, output); 
			}
			else if (*tmp == '_' && IsDigit(tmp[1])) // canonical match variable
			{
				char* base = tmp++;
				if (IsDigit(*tmp)) ++tmp; // 2nd digit
				output = AddFormatOutput(GetwildcardText(GetWildcardID(base),true), output); 
			}
			else if (*tmp == '\'' && tmp[1] == '_' && IsDigit(tmp[2])) // quoted match variable
			{
				char* base = ++tmp;
				++tmp;
				if (IsDigit(*tmp)) ++tmp; // 2nd digit
				output = AddFormatOutput(GetwildcardText(GetWildcardID(base),false), output);
			}
			else if (*tmp == '%' && IsAlpha(tmp[1])) // system variable
			{
				char* value = SystemVariable(tmp,NULL);
				if (*value) output = AddFormatOutput(value, output); 
				else if (!FindWord(tmp)) output = AddFormatOutput(tmp, output); // not a system variable
			}		
			else output = AddFormatOutput(tmp, output); 
		}
		else if (*input == '\\')
		{
			if (*++input == 'n') *output++ = '\n';
			else if (*input == 't') *output++ = '\t';
			else if (*input == 'r') *output++ = '\r';
			else *output++ = *input;
			++input;
		}
		else // ordinary character
		{
			mainValue[0] = *input++;
			output = AddFormatOutput(mainValue, output);
		}
	}
	*output = 0; // when failures, return the null string
}

static void StdNumber(char* word,char* buffer,int controls) // text numbers may have sign and decimal
{
    size_t len = strlen(word);
    if (!IsDigitWord(word) || strchr(word,':')) // either its not a number or its a time - leave unchanged
    {
        strcpy(buffer,word);  
        return;
    }
    char* dot = strchr(word,'.'); // float?
    if (dot) 
    {
        *dot = 0; 
        len = dot-word; // integral prefix
    }

    if (len < 5 || controls & OUTPUT_NOCOMMANUMBER) // no comma with <= 4 digit, e.g., year numbers
    {
        if (dot) *dot = '.'; 
        strcpy(buffer,word);  
        return;
    }

	// add commas between number triples
    char* ptr = word;
    unsigned int offset = len % 3;
    len = (len + 2 - offset) / 3; 
    strncpy(buffer,ptr,offset); 
    buffer += offset;
    ptr += offset;
    if (offset && len) *buffer++ = ','; 
    while (len--)
    {
        *buffer++ = *ptr++;
        *buffer++ = *ptr++;
        *buffer++ = *ptr++;
        if (len) *buffer++ = ',';
    }
	if (dot) 
	{
		*buffer++ = '.';
		strcpy(buffer,dot+1);
	}
	else *buffer = 0;
}

char* StdIntOutput(int n)
{
	char buffer[50];
	static char answer[50];
	*answer = 0;
#ifdef WIN32
	sprintf(buffer,"%I64d",(long long int) n); 
#else
	sprintf(buffer,"%lld",(long long int) n); 
#endif

	StdNumber(buffer,answer,0);
	return answer;
}

char* StdFloatOutput(float n)
{
	char buffer[50];
	static char answer[50];
	*answer = 0;
	sprintf(buffer,"%1.2f",n);
	StdNumber(buffer,answer,0);
	return answer;
}

static char* ProcessChoice(char* ptr,char* buffer,unsigned int &result,int controls) //   given block of  [xx] [yy] [zz]  randomly pick one
{
	char* choice[CHOICE_LIMIT];
	char** choiceset = choice;
	int count = 0;
    char* endptr = 0;

	//   gather choices
	while (*ptr == '[') // get next choice for block
	{
		//   find closing ]
		endptr = ptr-1;
		while (ALWAYS) 
		{
			endptr = strchr(endptr+1,']'); // find a closing ] 
			if (!endptr) // failed
			{
				respondLevel = 0;
				return 0;
			}
			if (*(endptr-1) != '\\') break; // ignore literal \[
		}
        // choice can be simple: [ xxx ]  or conditional [ $var1 xxx] but avoid assignment [ $var1 = 10 xxx ] 
		char word[MAX_WORD_SIZE];
		char* base = ptr + 2; // start of 1st token within choice
		char* simpleOutput = ReadCompiledWord(base,word);
		char* var = word;
		bool notted = false;
		if (*word == '!')
		{
			notted = true;
			++var;
		}
		if (*var == '$' && IsAlpha(var[1])) // user variable given
		{
			ReadCompiledWord(simpleOutput,tmpWord);
			if (*tmpWord == '=' || tmpWord[1] == '=') choiceset[count++] = base; //  some kind of assignment, it's all simple output
			else if (!notted && !*GetUserVariable(var)) {;}	// user variable test fails
			else if (notted && *GetUserVariable(var)) {;}	// user variable test fails
			else choiceset[count++] = simpleOutput;
		}
		else choiceset[count++] = base;

		ptr = endptr + 1;   // past end of choice
		if (*ptr == ' ') ++ptr;	 // start of next token
	}

	//   pick a choice randomly
	respondLevel = 0;
	while (count > 0)
	{
		int r = random(count);
		char* ptr = choiceset[r];
		if (*ptr == ']') break; // choice does nothing by intention
		if (ptr[1] == ':' && ptr[2] == ' ') ptr += 3; // skip special rejoinder
		Output(ptr,buffer,result,controls);
		if (result & ENDCODES) break; // declared done
		
		// is choice a repeat of something already said... if so try again
		if (*buffer && HasAlreadySaid(buffer)) 
		{
			if (trace) Log(STDUSERLOG,"Choice %s already said\r\n",buffer);
			*buffer = 0;
		}
		else 
		{
			if (choiceset[r][1] == ':' && choiceset[r][2] == ' ') respondLevel = *choiceset[r];
			break; // if choice didnt fail, it has completed, even if it generates no output
		}
		choiceset[r] = choiceset[--count];
	}
	return endptr+2; //   skip to end of rand past the ] and space
}

char* FreshOutput(char* ptr,char* buffer,unsigned int &result,int controls,unsigned int limit)
{
	++outputNest;
	PushOutputBuffers();
	if (limit == MAX_WORD_SIZE) AllocateOutputBuffer(); // where he wants to put it is SMALL and we're not ready for that. allocate a big bufer can copy later
	else currentRuleOutputBase = currentOutputBase = buffer; // is a normal buffer
	ptr = Output(ptr,currentOutputBase,result,controls);
	if (limit == MAX_WORD_SIZE) // someone's small local buffer
	{
		if (strlen(currentOutputBase) >= limit) *buffer = 0;	// cannot accept this
		else strcpy(buffer,currentOutputBase);
		FreeOutputBuffer();
	}
	PopOutputBuffers();
	--outputNest;
	return ptr;
}

#define CONDITIONAL_SPACE() if (space) {*buffer++ = ' '; *buffer = 0;}

#ifdef INFORMATION
There are two kinds of output streams. The ONCE only stream expects to read an item and return.
If a fail code is hit when processing an item, then if the stream is ONCE only, it will be done
and return a ptr to the rest. If a general stream hits an error, it has to flush all the remaining
tokens and return a ptr to the end.
#endif

char* Output_Percent(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result,bool once)
{			
	// Handles system variables:  %date
	// Handles any other % item - %
	if (IsAlpha(word[1])) // must be a system variable
    {
		if (!once && IsAssignmentOperator(ptr)) return PerformAssignment(word,ptr,result); //   =  or *= kind of construction
		strcpy(word,SystemVariable(word,NULL));
	}
	if (*word) 
	{
 		CONDITIONAL_SPACE();
		strcpy(buffer,word); 
	}
	return ptr;
}

char* Output_Backslash(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result)
{
	// handles newline:  \n
	// handles backslashed strings: \"testing"  means 
	// handles backslashed standalone double quote - \"  - means treat as even/odd pair and on 2nd one (closing) do not space before it
	// handles any other backslashed item:  \help  means just put out the item without the backslash
	if (word[1] == 'r' && !word[2]) return ptr;		// ignore \r (built into \n)
	if (word[1] == 'n' && !word[2])  //   \n
	{
		CONDITIONAL_SPACE();
#ifdef WIN32
		strcpy(buffer,"\r\n");
#else
		strcpy(buffer,"\n");
#endif
	}
    else //   some other random backslashed content, including \" 
    {
 		if (word[1] != '"' || !(controls & OUTPUT_DQUOTE_FLIP)) CONDITIONAL_SPACE(); // no space before paired closing dquote 
		strcpy(buffer,word+1); 
    }
	return ptr;
}

char* Output_Function(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result,bool once)
{
	if (IsDigit(word[1]))  //   function variable
	{
		if (!once && IsAssignmentOperator(ptr))  ptr = PerformAssignment(word,ptr,result); //   =  or *= kind of construction
		else //  replace function variable with its content, treating it as though the original content had been supplied
		{
			size_t len = strlen(callArgumentList[atoi(word+1)+fnVarBase]);
			size_t size = (buffer - currentOutputBase);
			if ((size + len) >= (currentOutputLimit-50) ) 
			{
				result = FAILRULE_BIT;
				return ptr;
			}

			strcpy(buffer,callArgumentList[atoi(word+1)+fnVarBase]);
			*word = '`';	// marker for retry
			word[1] = '^';	// additional marker for function variables
		}
	}
	else if (word[1] == '"') // functional string, uncompiled.  DO NOT USE function calls within it
	{
		CONDITIONAL_SPACE();
		ReformatString(buffer,word+2);
	}
	else  if (word[1] == '$' || word[1] == '_' || word[1] == '\'') // ^$$1 = null or ^_1 = null or ^'_1 = null is indirect user assignment
	{
		Output(word+1,buffer,result,controls|OUTPUT_NOTREALBUFFER); // no leading space
		if (!once && IsAssignmentOperator(ptr)) 
		{
			strcpy(word,buffer);
			*buffer = 0;
			return PerformAssignment(word,ptr,result); //   =  or *= kind of construction
		}
		*word = '`';	// marker for retry
	}
	else if (word[1] == '^') // if and loop
	{
		if (!strcmp(word,"^^if")) ptr = HandleIf(ptr,buffer,result);  
		else if (!strcmp(word,"^^loop")) ptr = HandleLoop(ptr,buffer,result); 
		else result = FAILRULE_BIT;
	}
	else // functions or ordinary words
	{
		if (*ptr != '(' || !word[1]) // a non function
		{
			CONDITIONAL_SPACE();
			strcpy(buffer,word);
		}
		else // ordinary function
		{
			ptr =  DoFunction(word,ptr,buffer,result); 
			if (result == UNDEFINED_FUNCTION) {;} 
			else if (space && *buffer && *buffer != ' ') // we need to add a space
			{
				memmove(buffer+1,buffer,strlen(buffer) + 1);
				*buffer = ' ';
			}
		}
	}
	return ptr;
}

char* Output_AttachedPunctuation(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result)
{
	// Handles spacing after a number:  2 .  
	// Handles not spacing before common punctuation:   . ? !  ,  :  ; 
	if (*word == '.' && controls & OUTPUT_ISOLATED_PERIOD) // if period after a number, always space after it (to be clear its not part of the number)
	{
		if (IsDigit(*(buffer-1))) *buffer++ = ' ';
	}
	strcpy(buffer,word); 
	return ptr;
}

char* Output_Text(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result)
{
	// handles text or script
	if (*ptr != '(' || controls & OUTPUT_FACTREAD || IsDigit(*word)) //   SIMPLE word  - paren if any is a nested fact read, or number before ( which cant be ^number
	{
		CONDITIONAL_SPACE();
		StdNumber(word,buffer,controls);
	}
	else  //   function call missing ^
	{
		memmove(word+1,word,strlen(word)+1);
		*word = '^';  // supply ^
		ptr = Output_Function(ptr,word,space,buffer, controls,result,false);
		if (result == UNDEFINED_FUNCTION) // wasnt a function after all.
		{
			CONDITIONAL_SPACE();
			StdNumber(word+1,buffer,controls);			
		}
	}
	return ptr;
}

char* Output_AtSign(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result,bool once)
{
	// handles factset assignement: @3 = @2
	// handles factset field: @3object
	if (!once && IsAssignmentOperator(ptr)) ptr = PerformAssignment(word,ptr,result);
	else if (impliedSet != ALREADY_HANDLED)
	{
		CONDITIONAL_SPACE();
		strcpy(buffer,word);
	}
    else if (IsDigit(word[1]) && IsAlpha(*GetSetType(word)) && !(controls & OUTPUT_KEEPQUERYSET)) //   fact set reference
    {
		unsigned int store = GetSetID(word);
		unsigned int count = FACTSET_COUNT(store);
		if (!count || count >= MAX_FIND) return ptr;
		FACT* F = factSet[store][1]; // use but don't use up most recent fact
		MEANING T;
		uint64 flags;
		char type = *GetSetType(word);
		if (type  == 's' ) 
		{
			T = F->subject;
			flags = F->flags & FACTSUBJECT;
		}
		else if (type== 'v')
		{
			T = F->verb;
			flags = F->flags & FACTVERB;
		}
		else if (type == 'a' && impliedWild != ALREADY_HANDLED)
		{
			strcpy(ARGUMENT(1), word);
			result = FLR(buffer,'l');
			return ptr;
		}
		else 
		{
			T = F->object;
			flags = F->flags & FACTOBJECT;
		}
		char* answer;
		char buf[100];
		if (flags) 
		{
			sprintf(buf,"%d",T);
			answer = buf;
		}
		else  answer = Meaning2Word(T)->word;
		CONDITIONAL_SPACE();
 		Output(answer,buffer,result,controls|OUTPUT_NOTREALBUFFER|OUTPUT_EVALCODE);
	}
	else 
	{
		CONDITIONAL_SPACE();
		strcpy(buffer,word);
	}
    return ptr;
 }

char* Output_Bracket(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result)
{
	// handles normal token: [ice 
	// handles choice: [ this is data ]
	if (word[1] || controls & OUTPUT_NOTREALBUFFER || !*ptr) StdNumber(word,buffer,controls); // normal token
	else ptr = ProcessChoice(ptr-2,buffer,result,controls);      
	return ptr;
}

char* Output_Quote(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result)
{
	// handles possessive: 's
	// handles original wildcard: '_2
	// handles quoted variable: '$hello
	if (word[1] == 's' && !word[2])	strcpy(buffer,"'s");	// possessive
	else if (word[1] == '_')			// original wildcard
	{
		int index = GetWildcardID(word+1); //   which one
		char* at = wildcardOriginalText[index];
		if (*at)  CONDITIONAL_SPACE();
		StdNumber(at,buffer,controls);
	}
	else if (word[1] == '$')  //    variable quoted, means dont reeval its content
	{
		strcpy(buffer,word+1);
	}
	else if (word[1] == '^' && IsDigit(word[2]))  //   function variable quoted, means dont reeval its content
	{
		size_t len = strlen(callArgumentList[atoi(word+2)+fnVarBase]);
		size_t size = (buffer - currentOutputBase);
		if ((size + len) >= (currentOutputLimit-50) ) 
		{
			result = FAILRULE_BIT;
			return ptr;
		}

		strcpy(buffer,callArgumentList[atoi(word+2)+fnVarBase]);
	}
	else 
	{
		CONDITIONAL_SPACE();
		StdNumber(word,buffer,controls);
	}
	return ptr;
}


char* Output_String(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result)
{
	// handles function string: "^ .... "  which means go eval the contents of the string
	// handles simple string: "this is a string"  which means just put it out (with or without quotes depending on controls)
	size_t len;
	CONDITIONAL_SPACE();
	if (controls & OUTPUT_UNTOUCHEDSTRING) strcpy(buffer,word);
	else if (word[1] == FUNCTIONSTRING && controls & OUTPUT_EVALCODE) // treat as format string
	{
		ReformatString(buffer,word+2,controls);
	}
	else if (controls & OUTPUT_NOQUOTES)
	{
		strcpy(buffer,word+1);
		len = strlen(buffer);
		if  (buffer[len-1] == '"') buffer[len-1] = 0; // remove end quote
	}
	else strcpy(buffer,word);
	return ptr;
}
	
char* Output_Underscore(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result,bool once)
{
	// handles wildcard assigment: _10 = hello
	// handles wildcard: _19
	// handles simple _ or _xxxx 
	if (!once && IsAssignmentOperator(ptr)) ptr = PerformAssignment(word,ptr,result); 
	else if (IsDigit(word[1])) // wildcard 
	{
		int id = GetWildcardID(word);
		if (id >= 0)
		{
			if (*wildcardCanonicalText[id]) { CONDITIONAL_SPACE();}
			StdNumber(wildcardCanonicalText[id],buffer,controls);
		}
	}
	else // stand-alone _ or some non wildcard
	{
		CONDITIONAL_SPACE();
		strcpy(buffer,word);
	}
	return ptr;
}

char* Output_Dollar(char* ptr, char* word, bool space,char* buffer, unsigned int controls,unsigned int& result,bool once)
{
	// handles user variable assignment: $myvar = 4
	// handles user variables:  $myvar
	// handles US money: $1000.00
	if (word[1] && !IsDigit(word[1])) // variable
    {
		if (controls & OUTPUT_EVALCODE && !(controls & OUTPUT_KEEPVAR)) 
		{
			char* answer = GetUserVariable(word);
			if (*answer == '$') strcpy(word,answer); // force nested indirect on var of a var value
		}

		if (!once && IsAssignmentOperator(ptr)) ptr = PerformAssignment(word,ptr,result); 
		else
		{
			char* value = GetUserVariable(word);
			if (value && *value) {CONDITIONAL_SPACE();} 
			StdNumber(value,buffer,controls);
		}
	}	
    else // money or simple $
    {
		CONDITIONAL_SPACE();
		StdNumber(word,buffer,controls);
	}
	return ptr;
}

char* Output(char* ptr,char* buffer,unsigned int &result,int controls)
{ 
	//   an output stream consists of words, special words, [] random zones, commands, C-style script. It autoformats whitespace.
	*buffer = 0; 
    result = 0;
	if (!*ptr) return NULL;	
	bool once = false;
	if (controls & OUTPUT_ONCE) // do one token
	{
		once = true;
		controls ^= OUTPUT_ONCE;
	}
	if (buffer < currentOutputBase || (size_t)(buffer - currentOutputBase) >= (size_t)(currentOutputLimit-200)) // output is wrong or in danger already?
	{
		result =  (unsigned int) (buffer - currentOutputBase); // debug info
		result = FAILRULE_BIT;
		return ptr;
	}

	char* start = buffer;
    bool quoted = false;
    char word[MAX_WORD_SIZE];
	int paren = 0;

    while (ptr)
    {
		if (!*ptr || *ptr == ENDUNIT) break; // out of data
		char* hold = ptr;
        ptr = ReadCompiledWord(ptr,word); 
 		if ((!*word || (*word == ')' || *word == ']' || *word == '}'))  && !paren  ) break; // end of data or choice or body
		if (debugger) Debugger(PARSING_OUTPUT,0,hold); // a real thing to print gets debugged, not just a closer
	
		//   determine whether to space before item or not. Don't space at start but normally space after existing output tokens with exceptions.
		bool space = false;
		if (!once)
		{
			char before;
			if (start == buffer && controls & OUTPUT_NOTREALBUFFER) {;} // there is no before in this buffer and we are at start
			else
			{
				before = *(buffer-1);
				// dont space after $  or # or [ or ( or " or / or newline
				space = before && before != '(' && before != '[' && before != '{'  && before != '$' && before != '#' && before != '"' && before != '/' && before != '\n';
				if (before == '"') space = !(CountParens(currentOutputBase) & 1); //   if parens not balanced, add space before opening doublequote
			}
		}
retry:
		if (!outputNest && !*currentOutputBase && buffer != currentOutputBase) start = buffer = currentOutputBase;	// buffer may have been flushed and needs reset
		switch (*word)
        {
		// groupings
		case ')':  case ']': case '}':  // ordinary output closers, never space before them
			*buffer++ = *word;
			*buffer = 0;
            break;
		case '(': case '{':
			CONDITIONAL_SPACE();
			if (word[1]) StdNumber(word,buffer,controls); // its a full token like (ice)_ is_what
			else 
			{
				++paren;
				*buffer++ = *word;
				*buffer = 0;
			}
            break;
 		case '[':  //   random choice. process the choice area, then resume in this loop after the choice is made
			ptr = Output_Bracket(ptr, word, space, buffer, controls,result);
			break;

		// variables of various flavors
        case '$': //   user variable or money
			ptr = Output_Dollar(ptr, word, space, buffer, controls,result,once);
            break;
 		case '_': //   wildcard or standalone _ OR just an ordinary token
			ptr = Output_Underscore(ptr, word, space, buffer, controls,result,once);
			break;
        case '%':  //   system variable
	 		ptr = Output_Percent(ptr, word, space, buffer, (quoted) ? (controls | OUTPUT_DQUOTE_FLIP)  : controls,result,once);
			break;
		case '@': //   a fact set reference like @9 @1subject @2object or something else
			ptr = Output_AtSign(ptr, word, space, buffer, controls,result,once);
			break;

		// sets, functions, strings
		case '~':	//concept set 
			CONDITIONAL_SPACE();
			strcpy(buffer,word);
            break;
 		case '^': //   function call or function variable or FORMAT string (by definition uncompiled)
			if (*currentRuleOutputBase && (!strcmp(word,"^gambit") || !strcmp(word,"^respond") || !strcmp(word,"^reuse") )) // leaving current rule
			{
				AddResponse(currentRuleOutputBase);
				buffer = currentRuleOutputBase;	
				*buffer = 0;
			}

			ptr = Output_Function(ptr, word, space, buffer, controls,result,once);
			if (*word == '`') // retry marker from ^$var substitution loop back and handle it
			{
				if (!*buffer) result = FAILRULE_BIT;
				else if (once && word[1] != '^') break;	// have our answer (unless it was a function variable substitution)
				else
				{
					strcpy(word,buffer);
					*buffer = 0;
					goto retry; 
				}
			}
			break;
        case '"': // string of some kind
			ptr = Output_String(ptr, word, space, buffer, controls,result);
			break;

		// prefixes:  quote, backslash
		case '\\':  //   backslash needed for new line  () [ ]   
  			ptr = Output_Backslash(ptr, word, space, buffer, (quoted) ? (controls | OUTPUT_DQUOTE_FLIP)  : controls,result);
			if (word[1] == '"') quoted = !quoted;
            break;
		case '\'': //   quoted item
			ptr = Output_Quote(ptr, word, space, buffer, controls,result);
			break;

		// punctuation which should not be spaced   .  ,  :  ;  !  ?  
		case '.': case '?': case '!': case ',':  case ';':  //   various punctuation wont want a space before
			ptr = Output_AttachedPunctuation(ptr, word, space, buffer, (buffer >= start) ? (controls | OUTPUT_ISOLATED_PERIOD)  : controls,result);
			break;
		case ':': // a debug command?
		{
#ifndef DISCARDTESTING
			WORDP D = FindWord(word);
			if (D && D->x.debugIndex) 
			{
				unsigned int oldtopicid = currentTopicID;
				char* oldrule = currentRule;
				int oldruleid = currentRuleID;
				int oldruletopic = currentRuleTopic;
				Command(ptr - strlen(word) - 1,NULL);
				currentTopicID = oldtopicid;
				currentRule = oldrule;
				currentRuleID = oldruleid;
				currentRuleTopic = oldruletopic;
				ptr = NULL;
				break; // just abort flow after this
			}
#endif
			// ordinary :
			ptr = Output_AttachedPunctuation(ptr, word, space, buffer, (buffer >= start) ? (controls | OUTPUT_ISOLATED_PERIOD)  : controls,result);
			break;
		}
		// DROP THRU if not debug command
        default: //  text or C-Script
			ptr = Output_Text(ptr, word, space, buffer, controls,result);
        }

		if (!outputNest && *buffer) // generated top level output
		{
			if (buffer == start) // this is our FIRST output
			{
				buffer = start;  // debug stop
			}
			if (trace & (TRACE_OUTPUT|TRACE_MATCH) &&  !(controls &OUTPUT_SILENT)) Log(STDUSERLOG," =:: %s ",buffer);
		}
		//   update location and check for overflow
		buffer += strlen(buffer);
		unsigned int size = (buffer - currentOutputBase);
        if (size >= (currentOutputLimit-200) && !(result  & FAILCODES)) 
		{
			result = FAILRULE_BIT;
		}

		if (result & (RETRYRULE_BIT|RETRYTOPRULE_BIT|ENDCODES))
		{
			if (result & FAILCODES && !(controls & OUTPUT_LOOP)) *start = 0; //  kill output
			if (!once) ptr = BalanceParen(ptr,true); // swallow excess input BUG - does anyone actually care
			break;
		}
		if (once) break;    
	}

    return ptr;
}
