// markSystem.cpp - annotates the dictionary with what words/concepts are active in the current sentence

#include "common.h"

#ifdef INFORMATION

For every word in a sentence, the word knows it can be found somewhere in the sentence, and there is a 64-bit field of where it can be found in that sentence.
The field is in a hashmap and NOT in the dictionary word, where it would take up excessive memory.

Adjectives occur before nouns EXCEPT:
	1. object complement (with some special verbs)
	2. adjective participle (sometimes before and sometimes after)

In a pattern, an author can request:
	1. a simple word like bottle
	2. a form of a simple word non-canonicalized like bottled or apostrophe bottle
	3. a WordNet concept like bottle~1 
	4. a set like ~dead or :dead

For #1 "bottle", the system should chase all upward all sets of the word itself, and all
WordNet parents of the synset it belongs to and all sets those are in. 

Marking should be done for the original and canonical forms of the word.

For #2 "bottled", the system should only chase the original form.

For #3 "bottle~1", this means all words BELOW this in the wordnet hierarchy not including the word
"bottle" itself. This, in turn, means all words below the particular synset head it corresponds to
and so instead becomes a reference to the synset head: "0173335n" or some such.

For #4 "~dead", this means all words encompassed by the set ~dead, not including the word ~dead.

So each word in an input sentence is scanned for marking. 
the actual word gets to see what sets it is in directly. 
Thereafter the system chases up the synset hierarchy fanning out to sets marked from synset nodes.

#endif
static unsigned int whereFreeList = 0;
static unsigned int tempList = 0;
static unsigned int triedFreeList = 0;

unsigned int maxRefSentence = MAX_XREF_SENTENCE  * 2;

// mark debug tracing
bool showMark = false;
static unsigned int markLength = 0; // prevent long lines in mark listing trace
#define MARK_LINE_LIMIT 80

char unmarked[MAX_SENTENCE_LENGTH]; // can completely disable a word from mark recognition
unsigned int tempc = 0;
MEANING* GetTemps(WORDP D)
{
	if (D->temps) return (MEANING*) Index2String(D->temps); // we have an active temp (valid for duration of VOLLEY)

	unsigned char* data = (unsigned char*) AllocateString(NULL,5 * sizeof(MEANING),false,true); // factback ptr, whereinfo for words, linked list of temps allocated
	if (!data) return NULL;
	++tempc;
	MEANING* means = (MEANING*)data;
	means[USEDTEMPSLIST] = tempList;		// link back - we have a list threaded thru the temps triples
	tempList = MakeMeaning(D);  
	D->temps = String2Index((char*)data);

	return means;
}

void FreeTemps() // release the temps list ptrs
{
	while (tempList) // list of words that have templist attached
	{
		WORDP D = Meaning2Word(tempList);
		unsigned int mean = D->temps;
		if (!mean)
		{
			tempList = 0;
			break;
		}
		MEANING* means = (MEANING*) Index2String(mean); // the temp set
		tempList = means[USEDTEMPSLIST];
		D->temps = 0;
		--tempc;
	}
	tempc =  0;
}

unsigned char* AllocateWhereInSentence(WORDP D)
{
	MEANING* set = GetTemps(D);
	if (!set) return NULL;
		
	// get where chunk
	unsigned char* data;
	if (whereFreeList)
	{
		data = (unsigned char*) Index2String(whereFreeList);
		unsigned int* tmp = (unsigned int*) data;
		whereFreeList = *tmp;
	}
	else 
	{
		data = (unsigned char*) AllocateString(NULL,maxRefSentence,0,false);
		if (!data) return NULL;
		++xrefCount;
	}
	memset(data,0xff,maxRefSentence);

	// store where in the temps data
	set[WHEREINSENTENCE] = String2Index((char*) data);

	return data; // returns past hidden field
}

unsigned char* GetWhereInSentence(WORDP D) 
{
	if (!D->temps) return NULL; // where cache is not up to date
	MEANING* set = GetTemps(D);
	if (!set) return NULL;
	unsigned int index = set[WHEREINSENTENCE];
	if (!index) return NULL;
	return (unsigned char*) Index2String(index); 
}
static void ClearTemps(WORDP D, uint64 junk)
{
	D->temps = 0;
}

void ClearWhereInSentence(bool fullClear) // erases both the WHEREINSENTENCE and the TRIEDBITS forms and the tally
{
	unsigned int xlist = tempList;
	while (xlist) // list of words that have templist attached (some have where info and some may have factback info)
	{
		WORDP D = Meaning2Word(xlist);
		unsigned int mean = D->temps;
		if (!mean)
		{
			static int did = false;
			if (!did) ReportBug("Bad freeing of xlist\r\n");
			did = true;
			triedFreeList = whereFreeList = 0;
			WalkDictionary(ClearTemps,0); // drop all transient data completely
			break;
		}
		MEANING* tempset = (MEANING*) Index2String(mean); // the temp set
		xlist = tempset[USEDTEMPSLIST];

		// do the WHEREINSENTENCE chunks
		MEANING where = tempset[WHEREINSENTENCE];
		if (where) // clear them and hook them up
		{
			tempset[WHEREINSENTENCE] = 0; // cleared
			MEANING* means = (MEANING*) Index2String(where);
			*means = whereFreeList;
			whereFreeList = where;
		}

		// do the TRIEDBITS chunks
		where = tempset[TRIEDBITS];
		if (where) // clear them and hook them up
		{
			tempset[TRIEDBITS] = 0; // cleared
			MEANING* means = (MEANING*) Index2String(where);
			*means = triedFreeList;
			triedFreeList = where;
		}
	}

	if (fullClear) triedFreeList = whereFreeList = 0;
}

void SetTried(WORDP D,uint64 bits)
{
	unsigned char* data;
	MEANING* list = GetTemps(D);
	if (!list) return;
	if (list[TRIEDBITS]) // have one, just update it 
	{
		data = (unsigned char*) Index2String(list[TRIEDBITS]);
		uint64* data1 = (uint64*) data;
		*data1 = bits;
		return;
	}
	
	// allocate a new one
	if (triedFreeList) // reusable ones
	{
		data = (unsigned char*) Index2String(triedFreeList);
		unsigned int* tmp = (unsigned int*) data;
		triedFreeList = *tmp;
	}
	else  data = (unsigned char*) AllocateString(NULL,64,0,true);
	if (!data) return;

	// store where in the temps data
	list[TRIEDBITS] = String2Index((char*) data);

	uint64* data1 = (uint64*) data;
	*data1 = bits;
}

uint64 GetTried(WORDP D)
{
	if (!D->temps) return 0; // where cache is not up to date
	MEANING* set = GetTemps(D);
	if (!set) return 0;
	unsigned int index = set[TRIEDBITS];
	if (!index) return 0;
	unsigned char* data = (unsigned char*) Index2String(index); 
	uint64* bits = (uint64*) data;
	return *bits;
}

void RemoveMatchValue(WORDP D, unsigned int position)
{
	unsigned char* data = GetWhereInSentence(D);
	if (!data) return;
	for (unsigned int i = 0; i < maxRefSentence; i += 2)
	{
		if (data[i] == position) 
		{
			if (trace) Log(STDUSERLOG,"unmark %s @word %d  ",D->word,position);
			memmove(data+i,data+i+2,(maxRefSentence - i - 2)); 
			break;
		}
	}
}

void MarkWordHit(WORDP D, unsigned int start,unsigned int end)
{	//   keep closest to start at bottom, when run out, drop later ones 
    if (!D || !D->word) return;
	if (end > wordCount) end = wordCount;    
	if (start > wordCount) 
	{
		ReportBug("save position is too big")
		return;
	}
	// diff < 0 means peering INSIDE a multiword token before last word
	// we label END as the word before it (so we can still see next word) and START as the actual multiword token
 	unsigned char* data = GetWhereInSentence(D);
    if (!data) data = AllocateWhereInSentence(D);
	if (!data) return;

	bool added = false;
	for (unsigned int i = 0; i < maxRefSentence; i += 2)
	{
		if (data[i] == 0) // CANNOT BE TRUE
		{
			static bool did = false;
			if (!did) ReportBug("illegal whereref for %s at %d\r\n",D->word,inputCount);
			did = true;
		}
		if (data[i] == start) 
		{
			if (end > data[i+1])
			{
				data[i+1] = (unsigned char)end; 
				added = true;
			}
			break; // we are already here
		}
		else if (data[i] > start) 
		{
			memmove(data+i+2,data+i,maxRefSentence - i - 2);
			data[i] = (unsigned char)start;
			data[i+1] = (unsigned char)end;
			added = true;
			break; // data inserted here
		}
	}

	if (( trace & (TRACE_PREPARE|TRACE_HIERARCHY)  || prepareMode == PREPARE_MODE ) && added)  
	{
		markLength += D->length;
		if (markLength > MARK_LINE_LIMIT)
		{
			markLength = 0;
			Log(STDUSERLOG,"\r\n");
			Log(STDUSERTABLOG,"");
		}
		Log(STDUSERLOG,(D->systemFlags & TOPIC) ? " +T%s " : " +%s",D->word);
		Log(STDUSERLOG,(start != end) ? "(%d-%d)": "(%d) ",start,end);
	}
}

unsigned int GetIthSpot(WORDP D,unsigned int i)
{
    if (!D) return 0; //   not in sentence
	unsigned char* data = GetWhereInSentence(D);
	if (!data) return 0;
	i *= 2;
	if (i >= maxRefSentence) return 0; // at end
	positionStart = data[i];
	if (positionStart == 0xff) return 0;
	positionEnd = data[i+1];
	if (positionEnd > wordCount)
	{
		static bool did = false;
		if (!did) ReportBug("Getith out of range %s at %d\r\n",D->word,inputCount);
		did = true;
	}
    return positionStart;
}

unsigned int GetNextSpot(WORDP D,int start,unsigned int &positionStart,unsigned int& positionEnd, bool reverse)
{//   spot can be 1-31,  range can be 0-7 -- 7 means its a string, set last marker back before start so can rescan
	//   BUG - we should note if match is literal or canonical, so can handle that easily during match eg
	//   '~shapes matches square but not squares (whereas currently literal fails because it is not ~shapes
    if (!D) return 0; //   not in sentence
	unsigned char* data = GetWhereInSentence(D);
	if (!data) return 0;
	
	unsigned int i;
	positionStart = 0;
	for (i = 0; i < maxRefSentence; i += 2)
	{
		unsigned char at = data[i];
		if (unmarked[at]){;}
		else if (reverse)
		{
			if (at < start) 
			{
				positionStart = at;
				positionEnd = data[i+1];
				if (positionEnd > wordCount)
				{
					static bool did = false;
					if (!did) ReportBug("Getith out of range %s at %d\r\n",D->word,inputCount);
					did = true;
				}
			}
			else return positionStart;
		}
		else if (at > start)
		{
			if (at == 0xff) return 0; // end of data going forward
			positionStart = at;
			positionEnd = data[i+1];
			if (positionEnd > wordCount)
			{
				static bool did = false;
				if (!did) ReportBug("Getith out of range %s at %d\r\n",D->word,inputCount);
				did = true;
			}
			return positionStart;
		}
	}
    return 0;
}

static int MarkSetPath(MEANING M, unsigned int start, unsigned  int end, unsigned int depth, bool canonical) //   walks set hierarchy
{//   travels up concept/class sets only, though might start out on a synset node or a regular word
	unsigned int flags = M & ESSENTIAL_FLAGS;
	if (!flags) flags = ESSENTIAL_FLAGS;
	WORDP D = Meaning2Word(M);
	unsigned int index = Meaning2Index(M); // always 0 for a synset or set
	
	// check for any repeated accesses of this synset or set or word
	uint64 offset = 1 << index;
	uint64 tried = GetTried(D);
 	if (D->inferMark == inferMark) // been thru this word recently
	{
		if (*D->word == '~') return -1;	// branch is marked
		if (tried & offset)	return -1;	// word synset done this branch already
	}
	else //   first time accessing, note recency and clear tried bits
	{
		D->inferMark = inferMark;
		if (*D->word != '~') 
		{
			SetTried(D,0);
			tried = 0;
		}
	}
 	if (*D->word != '~') SetTried(D,tried |offset);

	int result = 0;
	FACT* F = GetSubjectHead(D); 
	while (F)
	{
		if (F->verb == Mmember) // ~concept members and word equivalent
		{
			char word[MAX_WORD_SIZE];
			char* fact;
			if (trace == TRACE_HIERARCHY)  
			{
				fact = WriteFact(F,false,word); // just so we can see it
				unsigned int hold = globalDepth;
				globalDepth = depth;
				Log(STDUSERTABLOG,"%s   ",fact); // \r\n
				globalDepth = hold;
			}
			unsigned int restrict = F->subject & TYPE_RESTRICTION;
			if (restrict && !(restrict & flags)) {;} // type restriction in effect for this concept member
			else if (canonical && F->flags & ORIGINAL_ONLY) {;} // incoming is not original words and must be

			//   index meaning restriction (0 means all)
			else if (index == Meaning2Index(F->subject)) // match generic or exact subject 
			{
				// test for word not included in set
				WORDP E = Meaning2Word(F->object); // this is a topic or concept
				bool mark = true;
				if (E->internalBits & HAS_EXCLUDE) // set has some members it does not want
				{
					FACT* G = GetObjectHead(E);
					while (G)
					{
						if (G->verb == Mexclude) // see if this is marked for this position, if so, DONT trigger topic
						{
							WORDP S = Meaning2Word(G->subject);
							unsigned int positionStart,positionEnd;
							if (GetNextSpot(S,start-1,positionStart,positionEnd) && positionStart == start && positionEnd == end)
							{
								mark = false;
								break;
							}
						}
						G = GetObjectNext(G);
					}
				}

				if (mark)
				{
					MarkWordHit(E,start,end);
					if (MarkSetPath(F->object,start,end,depth+1,canonical) != -1) result = 1; // someone marked
				}
			}
		}
		F = GetSubjectNext(F);
	}
	return result;
}

static void RiseUp(MEANING M,unsigned int start, unsigned int end,unsigned int depth,bool canonical) //   walk wordnet hierarchy above a synset node
{	// M is always a synset head 
	M &= -1 ^ SYNSET_MARKER;
	unsigned int index = Meaning2Index(M);
	WORDP D = Meaning2Word(M);
	WORDP X;
	char word[MAX_WORD_SIZE];
	sprintf(word,"%s~%d",D->word,index);
	X = FindWord(word,0,PRIMARY_CASE_ALLOWED);
	if (X) 	MarkWordHit(X,start,end); // direct reference in a pattern

	// now spread and rise up
	if (MarkSetPath(M,start,end,depth,canonical) == -1) return; // did the path
	FACT* F = GetSubjectHead(D); 
	while (F)
	{
		if (F->verb == Mis && (index == 0 || F->subject == M)) RiseUp(F->object,start,end,depth+1,canonical); // allowed up
		F = GetSubjectNext(F);
	}
}

static void MarkSequenceTitleFacts(MEANING M, unsigned int start, unsigned int end,bool canonical) // title phrases in sentence
{
    if (!M) return;
	WORDP D = Meaning2Word(M);
	if (D->properties & NOUN_TITLE_OF_WORK && canonical) return; // accidental canonical match. not intended

	if (D->properties & PART_OF_SPEECH) // mark pos data
	{
		uint64 bit = START_BIT;
		for (int j = 63; j >= 0; --j)
		{
			if (D->properties & bit) MarkFacts(posMeanings[j],start,end,canonical,(D->properties & NOUN_TITLE_OF_WORK && !canonical) ? false : true); // treat original title as a full normal word
			bit >>= 1;
		}
	}

	MarkFacts(M,start,end,canonical,true);
}

void MarkFacts(MEANING M,unsigned int start, unsigned int end,bool canonical,bool sequence) 
{ // M is always a word or sequence from a sentence

    if (!M) return;
	WORDP D = Meaning2Word(M);
	if (!sequence || D->properties & (PART_OF_SPEECH|NOUN_TITLE_OF_WORK|NOUN_HUMAN) || D->systemFlags & PATTERN_WORD) MarkWordHit(D,start,end); // if we want the synset marked, RiseUp will do it.
	int result = MarkSetPath(M,start,end,0,canonical); // generic membership of this word all the way to top
	if (sequence && result == 1) MarkWordHit(D,start,end); // if we want the synset marked, RiseUp will do it.
	WORDP X;
	char word[MAX_WORD_SIZE];
	if (M & NOUN) // BUG- this wont work up the ontology, only at the root of what the script requests
	{
		sprintf(word,"%s~n",D->word);
		X = FindWord(word,0,PRIMARY_CASE_ALLOWED);
		if (X) 	MarkWordHit(X,start,end); // direct reference in a pattern
	}
	if (M & VERB)
	{
		sprintf(word,"%s~v",D->word);
		X = FindWord(word,0,PRIMARY_CASE_ALLOWED);
		if (X) 	MarkWordHit(X,start,end); // direct reference in a pattern
	}
	if (M & ADJECTIVE)
	{
		sprintf(word,"%s~a",D->word);
		X = FindWord(word,0,PRIMARY_CASE_ALLOWED);
		if (X) 	MarkWordHit(X,start,end); // direct reference in a pattern
	}
	if (M & ADVERB)
	{
		sprintf(word,"%s~b",D->word);
		X = FindWord(word,0,PRIMARY_CASE_ALLOWED);
		if (X) 	MarkWordHit(X,start,end); // direct reference in a pattern
	}
	if (M & PREPOSITION)
	{
		sprintf(word,"%s~p",D->word);
		X = FindWord(word,0,PRIMARY_CASE_ALLOWED);
		if (X) 	MarkWordHit(X,start,end); // direct reference in a pattern
	}

	//   now follow out the allowed synset hierarchies 
	unsigned int index = Meaning2Index(M);
	unsigned int size = GetMeaningCount(D);
	uint64 flags = M & ESSENTIAL_FLAGS;
	if (!flags) flags = ESSENTIAL_FLAGS; // unmarked ptrs can rise all branches
	for  (unsigned int k = 1; k <= size; ++k) 
	{
		M = GetMeaning(D,k); // it is a flagged meaning unless it self points

		// walk the synset words and see if any want vague concept matching like dog~~
		MEANING T = M; // starts with basic meaning
		unsigned int n = (index && k != index) ? 80 : 0;	// only on this meaning or all synset meanings 
		while (n < 50) // insure not infinite loop
		{
			WORDP X = Meaning2Word(T);
			unsigned int ind = Meaning2Index(T);
			sprintf(word,"%s~~",X->word);
			WORDP V = FindWord(word,0,PRIMARY_CASE_ALLOWED);
			if (V) 	MarkWordHit(V,start,end); // direct reference in a pattern
			if (!ind) break;	// has no meaning index
			T = GetMeanings(X)[ind];
			if (!T)
				break;
			if ((T & MEANING_BASE) == (M & MEANING_BASE)) break; // end of loop
			++n;
		}

		M = (M & SYNSET_MARKER) ? MakeMeaning(D,k) : GetMaster(M); // we are the master itself or we go get the master
		RiseUp(M,start,end,0,canonical); // allowed meaning pos (self ptrs need not rise up)
	}
}

static void SetSequenceStamp() //   mark words in sequence, original and canonical (but not mixed) - detects proper name potential up to 5 words
{
	char* rawbuffer = AllocateBuffer();
	char* canonbuffer1 = AllocateBuffer();
	unsigned int oldtrace = trace;
	unsigned int usetrace = trace;
	char* buffer2 = AllocateBuffer();
	if (trace & TRACE_PREPARE || prepareMode == PREPARE_MODE) 
	{
		Log(STDUSERLOG,"\r\n    sequences=\r\n");
		usetrace = (unsigned int) -1;
	}

	//   consider all sets of up to 3-in-a-row 
	int limit = ((int)endSentence) - 1;
	for (int i = startSentence; i <= limit; ++i)
	{
		if (!IsAlphaOrDigit(*wordStarts[i])) continue; // we only composite words, not punctuation or quoted stuff

		//   set base phrase
		*rawbuffer = 0;
		canonbuffer1[0] = 0;
		strcat(rawbuffer,wordStarts[i]);
		strcat(canonbuffer1,wordCanonical[i]);
       
		//   fan out for addon pieces
		unsigned int k = 0;
		int index = 0;
		uint64 logbase = logCount; // see if we logged anything
		while ((++k + i) <= endSentence)
		{
	
			strcat(rawbuffer,"_");
			strcat(canonbuffer1,"_");
			strcat(rawbuffer,wordStarts[i+k]);
			strcat(canonbuffer1,wordCanonical[i+k]);

			if (!IsAlphaOrDigit(*wordStarts[i+k])) break; // we only composite words, not punctuation or quoted stuff except we can include them as part of something
			NextinferMark();

			// for now, accept upper and lower case forms of the decomposed words for matching
			// storeword instead of findword because we normally dont store keyword phrases in dictionary
			MakeLowerCopy(buffer2,rawbuffer);
			WORDP D = FindWord(buffer2,0,LOWERCASE_LOOKUP); 
			if (D)
			{
				trace = (D->subjectHead || D->systemFlags & PATTERN_WORD || D->properties & PART_OF_SPEECH)  ? usetrace : 0; // being a subject head means belongs to some set. being a marked word means used as a keyword
				MarkFacts(MakeMeaning(D),i,i+k,false,true); 
			}
			MakeUpperCopy(buffer2,rawbuffer);
			D = FindWord(buffer2,0,UPPERCASE_LOOKUP);
			if (D)
			{
				trace = (D->subjectHead || D->systemFlags & PATTERN_WORD || D->properties & PART_OF_SPEECH)  ? usetrace : 0;
				MarkSequenceTitleFacts(MakeMeaning(D),i,i+k,false);
			}
			MakeLowerCopy(buffer2,canonbuffer1);
			D = FindWord(buffer2,0,LOWERCASE_LOOKUP);
			if (D) 
			{
				trace = (D->subjectHead  || D->systemFlags & PATTERN_WORD || D->properties & PART_OF_SPEECH)  ? usetrace : 0;
				MarkFacts(MakeMeaning(D),i,i+k,true,true); 
			}
			MakeUpperCopy(buffer2,canonbuffer1);
			D = FindWord(buffer2,0,UPPERCASE_LOOKUP);
			if (D)
			{
				trace = (D->subjectHead  || D->systemFlags & PATTERN_WORD || D->properties & PART_OF_SPEECH)  ? usetrace : 0;
				MarkSequenceTitleFacts(MakeMeaning(D),i,i+k,true); 
			}
			if (logCount != logbase && usetrace)  Log(STDUSERLOG,"\r\n"); // if we logged something, separate
			
			if (++index >= SEQUENCE_LIMIT) break; //   up thru 5 words in a phrase
		}
	}
	trace = oldtrace;
	FreeBuffer();
	FreeBuffer();
	FreeBuffer();
}

static void StdMark(MEANING M, unsigned int start, unsigned int end) 
{
	if (!M) return;
	MarkFacts(M,start,end);		//   the basic word
	WORDP D = Meaning2Word(M);
	if (D->systemFlags & TIMEWORD && !(D->properties & PREPOSITION)) MarkFacts(MakeMeaning(Dtime),start,end);
}

void MarkAllImpliedWords()
{
	ChangeDepth(1,"MarkAllImpliedWords");
	unsigned int i;
 	for (i = 1; i <= wordCount; ++i)  capState[i] = IsUpperCase(*wordStarts[i]); // note cap state
	TagIt(); // pos tag and maybe parse

	if ( prepareMode == POS_MODE || prepareMode == PENN_MODE || prepareMode == POSVERIFY_MODE  || prepareMode == POSTIME_MODE || tokenControl & NO_MARK) 
	{
		ChangeDepth(-1,"MarkAllImpliedWords");
		return;
	}

    if (trace & TRACE_PREPARE || prepareMode == PREPARE_MODE) Log(STDUSERLOG,"\r\nConcepts: \r\n");
 	if (showMark) Log(STDUSERLOG,"----------------\r\n");
	markLength = 0;
	
	//   now mark every word in sentence
    for (i = startSentence; i <= endSentence; ++i) //   mark that we have found this word, either in original or canonical form
    {
		if (ignoreWord[i]) continue;
		char* original =  wordStarts[i];

		if (showMark) Log(STDUSERLOG,"\r\n");
		NextinferMark(); // blocks circular fact marking.
 		if (trace  & (TRACE_HIERARCHY | TRACE_PREPARE) || prepareMode == PREPARE_MODE) Log(STDUSERLOG,"\r\n%d: %s raw= ",i,original);
	
		uint64 flags = posValues[i];
		if (flags & ADJECTIVE_NOUN) // transcribe back to noun
		{
			MarkFacts(MadjectiveNoun,i,i); 
			flags &= -1 ^ ADJECTIVE_NOUN;
			if (originalLower[i]) flags |= originalLower[i]->properties & (NOUN_SINGULAR|NOUN_PLURAL|NOUN_PROPER_SINGULAR|NOUN_PROPER_PLURAL);
		}
		finalPosValues[i] = flags; // these are what we finally decided were correct pos flags from tagger
		// put back non-tagger generalized forms of bits
		if (flags & NOUN_BITS) finalPosValues[i] |= NOUN;
		if (flags & (VERB_TENSES | NOUN_INFINITIVE| NOUN_GERUND)) finalPosValues[i] |= VERB;
		if (flags & AUX_VERB_BITS) finalPosValues[i] |= AUX_VERB;
		if (flags & ADJECTIVE_BITS) finalPosValues[i] |= ADJECTIVE;
		if (flags & ADVERB_BITS) finalPosValues[i] |= ADVERB;

		MarkTags(i);
#ifndef DISCARDPARSER
		MarkRoles(i);
#endif
	
		// mark general number property
		if (finalPosValues[i] & ( NOUN_NUMBER | ADJECTIVE_NUMBER))  
		{
			MarkFacts(Mnumber,i,i); 
			//   handle finding fractions as 3 token sequence  mark as placenumber 
			if (*wordStarts[i+1] == '/' && wordStarts[i+1][1] == 0 && finalPosValues[i+2] & (NOUN_NUMBER | ADJECTIVE_NUMBER))
			{
				MarkFacts(MakeMeaning(Dplacenumber),i,i);  
				if (trace & TRACE_PREPARE || prepareMode == PREPARE_MODE) Log(STDUSERLOG,"=%s/%s \r\n",wordStarts[i],wordStarts[i+2]);
			}
			else if (finalPosValues[i] & (NOUN_NUMBER | ADJECTIVE_NUMBER) && IsPlaceNumber(wordStarts[i]))
			{
				MarkFacts(MakeMeaning(Dplacenumber),i,i);  
			}
			// special temperature property
			char c = GetTemperatureLetter(original);
			if (c)
			{
				if (c == 'F') MarkFacts(MakeMeaning(StoreWord("~fahrenheit")),i,i);
				else if (c == 'C') MarkFacts(MakeMeaning(StoreWord("~celsius")),i,i);
				else if (c == 'K')  MarkFacts(MakeMeaning(StoreWord("~kelvin")),i,i);
				MarkFacts(Mnumber,i,i);
				char number[MAX_WORD_SIZE];
				sprintf(number,"%d",atoi(original));
				canonicalLower[i] =  StoreWord(number,(NOUN_NUMBER | ADJECTIVE_NUMBER));
				if (canonicalLower[i]) wordCanonical[i] = canonicalLower[i]->word;
			}

			// special currency property
			char* number;
			char* currency = GetCurrency(wordStarts[i],number); 
			if (currency) 
			{
				char tmp[MAX_WORD_SIZE];
				strcpy(tmp,currency);
				MarkFacts(Mmoney,i,i); 
				if (*currency == '$') tmp[1] = 0;
				else if (*currency == 0xe2 && currency[1] == 0x82 && currency[2] == 0xac) tmp[3] = 0;
				else if (*currency == 0xc2 && currency[1] == 0xa5 ) tmp[2] = 0;
				else if (*currency == 0xc2 && currency[1] == 0xa3 ) tmp[2] = 0;
				MarkFacts(MakeMeaning(StoreWord(tmp)),i,i);
			}
		}
	
        WORDP OL = originalLower[i];
		WORDP CL = canonicalLower[i];
 		WORDP OU = originalUpper[i]; 
        WORDP CU = canonicalUpper[i]; 
		if (!OU && !OL) OU = FindWord(original,0,UPPERCASE_LOOKUP);	 // can this ever work?
		if (!CU && original[1]) // dont convert single letters to upper case "a" if it hasnt already decided its not a determiner
		{
			CU = FindWord(original,0,UPPERCASE_LOOKUP);	// try to find an upper to go with it, in case we can use that, but not as a human name
			if (OU); // it was originally uppercase or there is no lower case meaning
			else if (CU && CU->properties & (NOUN_FIRSTNAME|NOUN_HUMAN)) CU = NULL;	// remove accidental names 
		}
	
		if (!OL && (!OU || !OU->properties) && !CL && (!CU || !CU->properties)) // allow unknown proper names to be marked unknown
		{
			MarkFacts(MakeMeaning(Dunknown),i,i); // unknown word
			MarkFacts(MakeMeaning(StoreWord(original)),i,i);		// allowed word
		}
		
		StdMark(MakeTypedMeaning(OL,0,(unsigned int)(finalPosValues[i] & TYPE_RESTRICTION)), i, i);
        if (trace & TRACE_PREPARE || prepareMode == PREPARE_MODE) Log(STDUSERLOG," // "); //   close original meanings lowercase

		markLength = 0;
		StdMark(MakeTypedMeaning(OU,0,(unsigned int)(finalPosValues[i] & TYPE_RESTRICTION)), i, i);
		
		if (trace & TRACE_PREPARE || prepareMode == PREPARE_MODE) 
		{
			if (CL) Log(STDUSERLOG,"\r\n%d: %s canonical= ", i,CL->word ); //    original meanings lowercase
			else Log(STDUSERLOG,"\r\n%d: %s canonical= ", i,(CU) ? CU->word : "" ); //    original meanings uppercase
		}

		//   canonical word
  		StdMark(MakeTypedMeaning(CL,0, (unsigned int)(finalPosValues[i] & TYPE_RESTRICTION)), i, i);

 		markLength = 0;
	    if (trace & TRACE_PREPARE || prepareMode == PREPARE_MODE) Log(STDUSERLOG," // "); //   close canonical form lowercase
 		
		StdMark(MakeTypedMeaning(CU,0, (unsigned int)(finalPosValues[i] & TYPE_RESTRICTION)), i, i);

		// canonical word is a number (maybe we didn't register original right) eg. "how much is 24 and *seven"
		if (canonicalLower[i] && IsDigit(*canonicalLower[i]->word) && IsNumber(canonicalLower[i]->word)) MarkFacts(Mnumber,i,i);  

		if (trace & TRACE_PREPARE || prepareMode == PREPARE_MODE) Log(STDUSERLOG," "); //   close canonical form uppercase
		markLength = 0;
	
        //   peer into multiword expressions  (noncanonical), in case user is emphasizing something so we dont lose the basic match on words
        //   accept both upper and lower case forms . 
		// But DONT peer into something proper like "Moby Dick"
		unsigned int  n = BurstWord(wordStarts[i]); // peering INSIDE a single token....
		WORDP D,E;
		if (finalPosValues[i] & (NOUN_PROPER_SINGULAR|NOUN_PROPER_PLURAL)) n = 1;
		if (tokenControl & NO_WITHIN);  // peek within hypenated words
        else if (n >= 2 && n <= 4) //   longer than 4 is not emphasis, its a sentence - we do not peer into titles
        {
			static char words[5][MAX_WORD_SIZE];
			unsigned int k;
			for (k = 0; k < n; ++k) strcpy(words[k],GetBurstWord(k)); // need local copy since burstwords might be called again..

            for (unsigned int k = 0; k < n; ++k)
            {
  				unsigned int prior = (k == (n-1)) ? i : (i-1); //   -1  marks its word match INSIDE a string before the last word, allow it to see last word still
                E = FindWord(words[k],0,PRIMARY_CASE_ALLOWED); 
                if (E)
				{
					if (!(E->properties & (NOUN_PROPER_SINGULAR|NOUN_PROPER_PLURAL))) StdMark(MakeMeaning(E),i,prior);
					else MarkFacts(MakeMeaning(E),i,prior);
				}
                E = FindWord(words[k],0,SECONDARY_CASE_ALLOWED); 
				if (E)
				{
					if (!(E->properties & (NOUN_PROPER_SINGULAR|NOUN_PROPER_PLURAL))) StdMark(MakeMeaning(E),i,prior);
					else MarkFacts(MakeMeaning(E),i,prior);
				}
           }
        }

		// now look on either side of a hypenated word
		char* hypen = strchr(wordStarts[i],'-');
		if (hypen) 
		{
			MarkFacts(MakeMeaning(StoreWord(hypen)),i,i); // post form -colored
			char word[MAX_WORD_SIZE];
			strcpy(word,wordStarts[i]);
			word[hypen+1-wordStarts[i]] = 0;
			MarkFacts(MakeMeaning(StoreWord(word)),i,i); // pre form  light-
		}
		
		D = (CL) ? CL : CU; //   best recognition
		char* last;
		if (D && D->properties & NOUN && !(D->internalBits & UPPERCASE_HASH) && (last = strrchr(D->word,'_')) && finalPosValues[i] & NOUN) StdMark(MakeMeaning(FindWord(last+1,0)), i, i); //   composite noun, store last word as referenced also

        if (trace & TRACE_PREPARE || prepareMode == PREPARE_MODE) Log(STDUSERLOG,"\r\n");
    }
 
	//   check for repeat input by user - but only if more than 2 words or are unknown (we dont mind yes, ok, etc repeated)
	//   track how many repeats, for escalating response
	unsigned int sentenceLength = endSentence - startSentence + 1;
	bool brief = (sentenceLength > 2);
	if (sentenceLength == 1 && !FindWord(wordStarts[startSentence])) brief = true;
    unsigned int counter = 0;
    if (brief && humanSaidIndex) for (int j = 0; j < (int)(humanSaidIndex-1); ++j)
    {
        if (strlen(humanSaid[j]) > 5 && !stricmp(humanSaid[humanSaidIndex-1],humanSaid[j])) //   he repeats himself
        {
            ++counter;
            char buf[100];
			strcpy(buf,"~repeatinput");
			buf[12] = (char)('0' + counter);
			buf[13] = 0;
 			MarkFacts(MakeMeaning(FindWord(buf,0,PRIMARY_CASE_ALLOWED)),1,1); //   you can see how many times
        }
    }

	//   now see if he is repeating stuff I said
	counter = 0;
    if (sentenceLength > 2) for (int j = 0; j < (int)chatbotSaidIndex; ++j)
    {
        if (humanSaidIndex && strlen(chatbotSaid[j]) > 5 && !stricmp(humanSaid[humanSaidIndex-1],chatbotSaid[j])) //   he repeats me
        {
			if (counter < sentenceLength) ++counter;
			MarkFacts(MakeMeaning(FindWord("~repeatme",0,PRIMARY_CASE_ALLOWED)),counter,counter); //   you can see how many times
        }
    }

    //   handle phrases now
	markLength = 0;
    SetSequenceStamp(); //   sequences of words

	ChangeDepth(-1,"MarkAllImpliedWords");
}
