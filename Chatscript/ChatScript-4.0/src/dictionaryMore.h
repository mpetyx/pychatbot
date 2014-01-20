
#define MAX_SYNLOOP	60

#define MAX_DICTIONARY	 0x000fffff  //   1M word vocabulary limit (doubling this FAILS on amazon server)

#define MAX_HASH_BUCKETS 50000 
#ifdef WIN32
#define MAX_ENTRIES      0x000fffff 
#else
#define MAX_ENTRIES      (0x000fffff/2)
#endif

#define NODEBITS 0x00ffffff
#define GETNEXTNODE(D) (D->nextNode & NODEBITS)		// top byte is the length of joined phrases of which this is header
#define MULTIHEADERBITS 0xFF000000
#define GETMULTIWORDHEADER(D)  (D->nextNode >> 24)
#define SETMULTIWORDHEADER(D,n) (  D->nextNode &= NODEBITS, D->nextNode |= n << 24 )

#define IS_NEW_WORD(x) (!x || !(x->internalBits & BASE_DEFINED)) // created by user volley

#define ALL_OBJECTS ( MAINOBJECT | MAININDIRECTOBJECT | OBJECT2 | INDIRECTOBJECT2 )

// system internal bits on dictionary entries internalBits

// these values of word->internalBits are NOT stored in the dictionary text files

// various livedata substitions allowed
#define ESSENTIALS_FILE			0x00000001 
#define SUBSTITUTIONS_FILE		0x00000002
#define CONTRACTIONS_FILE		0x00000004
#define INTERJECTIONS_FILE		0x00000008
#define BRITISH_FILE			0x00000010 
#define SPELLING_FILE			0x00000020 
#define TEXTING_FILE			0x00000040 
#define PRIVATE_FILE			0x00000080
//	0x00000100 
// 200
// 400
// 800

#define UTF8					0x00001000		// word has utf8 char in it
#define UPPERCASE_HASH			0x00002000		// word has upper case English character in it
#define VAR_CHANGED				0x00004000		// $variable has changed value this volley
#define WORDNET_ID				0x00008000		// a wordnet synset header node (MASTER w gloss )
#define INTERNAL_MARK			0x00010000		// transient marker for Intersect coding and Country testing in :trim 
#define BEEN_HERE				0x00020000		// used in internal word searches that might recurse
#define BASE_DEFINED			0x00040000		// word had part of speech bits when dictionary was locked (enables you to tell new properties filled in on old dict entries
#define DELETED_MARK			0x00080000		// transient marker for  deleted words in dictionary build - they dont get written out - includes script table macros that are transient
#define BUILD0					0x00100000		// comes from build0 data (marker on functions, concepts, topics)
#define BUILD1					0x00200000		// comes from build1 data
#define HAS_EXCLUDE				0x00400000		// concept/topic has keywords to exclude
#define TRACE_MACRO				0x00800000		// turn on tracing for this function
#define QUERY_KIND				0x01000000		// is a query item (from LIVEDATA)
#define LABEL					QUERY_KIND		// transient scriptcompiler use
#define HAS_GLOSS				0x02000000		// has gloss ptr	
#define HAS_SUBSTITUTE			0x04000000		//   word has substitute attached 
#define DEFINES					0x08000000		// word is a define

///   DEFINITION OF A MEANING 
#define TYPE_RESTRICTION	BASIC_POS  //  0xf0000000
#define SYNSET_MARKER		0x08000000  // this meaning is a synset head - on keyword import, its quote flag for binary read

#define INDEX_BITS          0x03F00000  //   7 bits of ontology meaning indexing ability  63 possible meanings allowed
#define INDEX_OFFSET        20          //   shift for ontoindex  (rang 0..63)  
#define MAX_MEANING			63			// limit
#define INDEX_MINUS			0x00100000  // what to decrement to decrement the meaning index

#define MEANING_BASE		0x000fffff	//   the index of the dictionary item

#define STDMEANING ( INDEX_BITS | MEANING_BASE | TYPE_RESTRICTION ) // no synset marker
#define SIMPLEMEANING ( INDEX_BITS | MEANING_BASE ) // simple meaning, no type

//   codes for BurstWord argument
#define SIMPLE 0
#define STDBURST 0		// normal burst behavior
#define POSSESSIVES 1
#define CONTRACTIONS 2
#define HYPHENS 4
#define COMPILEDBURST 8  // prepare argument as though it were output script		
#define NOBURST 16		// dont burst (like for a quoted text string)

#define FUNCTIONSTRING '^'

// pos tagger roles and states on roles[] && needRoles[] (32 bit limit)
// needRoles values can be composed of multiple choices. roles are exactly one choice
#define MAINSUBJECT				0x00000001			// noun roles like Mainsubject etc are ordered lowest first for pronoun IT priority
#define MAINVERB				0x00000002
#define MAININDIRECTOBJECT		0x00000004 
#define MAINOBJECT				0x00000008

#define SUBJECT2				0x00000010
#define VERB2					0x00000020
#define INDIRECTOBJECT2			0x00000040 
#define OBJECT2					0x00000080

#define CLAUSE 					0x00000100 	
#define VERBAL 					0x00000200	 
#define PHRASE					0x00000400	// true postnominal adjective or verbal that acts as postnominal adjective  (noun role is named also, so anonymous means adverb role)
		 	
#define SUBJECT_COMPLEMENT		0x00000800  // main object after a linking verb is an adjective "he seems *strong" (aka subject complement) -- a prep phrase instead is also considered a subject complement
#define OBJECT_COMPLEMENT		0x00001000	// can be noun or adjective or to/infinitive after direct object..."that should keep them *happy" "I knight you *Sir *Peter" (verb expects it)
#define TO_INFINITIVE_OBJECT 	0x00002000	// expecting either indirectobject then to infinitve or just to infinitive
#define VERB_INFINITIVE_OBJECT	0x00004000  // expecting either indirectobject then infinitive or just infinitive
// 0x00008000

// end of needRoles, the rest are roles


// what does coordinate conjunction conjoin (7 choices = 3 bits) -- NOT available on needRoles  // 		there can be no conjoined particles
#define CONJUNCT_CLAUSE 		0x00010000 	
#define CONJUNCT_SENTENCE		0x00020000	
#define CONJUNCT_NOUN			0x00030000	
#define CONJUNCT_VERB 			0x00040000	
#define CONJUNCT_ADJECTIVE		0x00050000	
#define CONJUNCT_ADVERB			0x00060000
#define CONJUNCT_PHRASE			0x00070000
#define CONJUNCT_KINDS			( CONJUNCT_PHRASE | CONJUNCT_CLAUSE | CONJUNCT_SENTENCE | CONJUNCT_NOUN | CONJUNCT_VERB | CONJUNCT_ADJECTIVE | CONJUNCT_ADVERB )

#define SENTENCE_END            0x00080000
#define WHENUNIT				0x00100000
#define WHEREUNIT				0x00200000
#define WHYUNIT					0x00300000
#define HOWUNIT					0x00400000
#define ADVERBIALTYPE ( WHENUNIT | HOWUNIT | WHEREUNIT | WHYUNIT )
#define REFLEXIVE		 		0x00800000   	
#define COMMA_PHRASE 			0x01000000  // describes noun before it
// 0x02000000
// 0x04000000
#define POSTNOMINAL_ADJECTIVE	 0x08000000  			// true postnominal adjective or verbal that acts as postnominal adjective  (noun role is named also, so anonymous means adverb role)

#define APPOSITIVE				0x10000000		//	2nd noun restates first
#define ADJECTIVE_COMPLEMENT	0x20000000		//2ndary modifier of an adjective "he was ready *to *go"	    noun clause or a prepositional phrase 
#define ABSOLUTE_PHRASE			0x40000000	  // "wings flapping", the man runs
// 	0x80000000  // describe verb somewhere before it

// above are 32 bit available to needRoles and roles. Below only suitable for 64bit roles
#define NOUN_OMITTED_ADJECTIVE	0x0000000100000000ULL // implied people as noun: "the *beautiful marry well"
#define OMITTED_TIME_PREP	0x0000000200000000ULL 
#define ADVERBIAL	0x0000000400000000ULL 
#define ADJECTIVAL	0x0000000800000000ULL 
#define ADDRESS 	0x0000001000000000ULL  // for factitive verbs
#define TAGQUESTION	0x0000002000000000ULL

#define MACRO_ARGUMENT_COUNT(D) ((unsigned char)(*D->w.fndefinition - 'A')) // for user macros not plans

#define KINDS_OF_PHRASES ( CLAUSE | PHRASE | VERBAL | OMITTED_TIME_PREP )

// pos tagger ZONE roles for a comma zone
#define ZONE_SUBJECT			0x000001	// noun before any verb
#define ZONE_VERB				0x000002
#define ZONE_OBJECT				0x000004	// noun AFTER a verb
#define ZONE_CONJUNCT			0x000008	// coord or subord conjunction
#define ZONE_FULLVERB			0x000010	// has normal verb tense or has aux
#define ZONE_AUX				0x000020	// there is aux in the zone
#define ZONE_PCV				0x000040	// zone is entirely phrases, clauses, and verbals
#define ZONE_ADDRESS			0x000080	// zone is an addressing name start. "Bob, you are amazing."
#define ZONE_ABSOLUTE			0x000100	// absolute zone has subject and partial participle verb, used to describe noun in another zone
#define ZONE_AMBIGUOUS			0x000200	// type of zone is not yet known

//   values for FindWord lookup
#define PRIMARY_CASE_ALLOWED 1
#define SECONDARY_CASE_ALLOWED 2
#define STANDARD_LOOKUP (PRIMARY_CASE_ALLOWED |  SECONDARY_CASE_ALLOWED )
#define LOWERCASE_LOOKUP 4
#define UPPERCASE_LOOKUP 8

#define NO_EXTENDED_WRITE_FLAGS ( SUBSTITUTE_RECIPIENT | PATTERN_WORD | FUNCTION_NAME | FUNCTION_BITS  | TOPIC )
#define MARK_FLAGS ( KINDERGARTEN | GRADE1_2 | GRADE3_4 | GRADE5_6  | MORE_FORM |  MOST_FORM | TIMEWORD | ACTUAL_TIME | WEB_URL | LOCATIONWORD )

// postag composites 
#define FINITE_SET_WORDS ( PREPOSITION | DETERMINER_BITS | CONJUNCTION | AUX_VERB | PRONOUN_BITS )
#define PART_OF_SPEECH		( FINITE_SET_WORDS  | BASIC_POS   ) 
#define NORMAL_WORD			( PART_OF_SPEECH | FOREIGN_WORD | INTERJECTION | THERE_EXISTENTIAL )
#define PUNCTUATION_BITS	( COMMA | PAREN | PUNCTUATION | QUOTE | CURRENCY )

#define ADVERB_SYSTEM_PROPERTIES ( MORE_FORM | MOST_FORM  ) 
#define ADJECTIVE_SYSTEM_PROPERTIES ( ADJECTIVE_POSTPOSITIVE | MORE_FORM | MOST_FORM ) 

#define VERB_CONJUGATION_PROPERTIES ( VERB_CONJUGATE1 | VERB_CONJUGATE2 | VERB_CONJUGATE3 ) 
#define VERB_PHRASAL_PROPERTIES ( INSEPARABLE_PHRASAL_VERB | MUST_BE_SEPARATE_PHRASAL_VERB | SEPARABLE_PHRASAL_VERB | PHRASAL_VERB )
#define VERB_OBJECTS ( VERB_NOOBJECT | VERB_INDIRECTOBJECT | VERB_DIRECTOBJECT | VERB_TAKES_GERUND | VERB_TAKES_ADJECTIVE | VERB_TAKES_INDIRECT_THEN_TOINFINITIVE | VERB_TAKES_INDIRECT_THEN_VERBINFINITIVE | VERB_TAKES_TOINFINITIVE | VERB_TAKES_VERBINFINITIVE )
#define VERB_SYSTEM_PROPERTIES ( PRESENTATION_VERB | COMMON_PARTICIPLE_VERB | VERB_CONJUGATION_PROPERTIES | VERB_PHRASAL_PROPERTIES | VERB_OBJECTS ) 

#define NOUN_PROPERTIES ( NOUN_MASS | NOUN_HE | NOUN_THEY | NOUN_SINGULAR | NOUN_PLURAL | NOUN_PROPER_SINGULAR | NOUN_PROPER_PLURAL | NOUN_ABSTRACT | NOUN_HUMAN | NOUN_FIRSTNAME | NOUN_SHE | NOUN_TITLE_OF_ADDRESS | NOUN_TITLE_OF_WORK  )
#define SIGNS_OF_NOUN_BITS ( NOUN_DESCRIPTION_BITS | PRONOUN_SUBJECT | PRONOUN_OBJECT )

#define NUMBER_BITS ( NOUN_NUMBER | ADJECTIVE_NUMBER )

#define VERB_PROPERTIES (  VERB_TENSES )

#define Index2Word(n) (dictionaryBase+n)
#define Word2Index(D) ((unsigned int) (D-dictionaryBase))
#define GetMeanings(D) ((MEANING*) Index2String(D->meanings))
#define GetMeaning(D,k) GetMeanings(D)[k]
#define GetMeaningsFromMeaning(T) (GetMeanings(Meaning2Word(T)))
#define Meaning2Index(x) ((unsigned int)((x & INDEX_BITS) >> INDEX_OFFSET)) //   which dict entry meaning

#define FACTBACK 0
#define GetFactBack(D) ((D->temps) ? GetTemps(D)[FACTBACK] : 0) // transient per search
#define WHEREINSENTENCE 1
unsigned char* GetWhereInSentence(WORDP D); // always skips the linking field at front
#define USEDTEMPSLIST 2
#define TRIEDBITS 3
#define WORDVALUE 4

#define PrepareConjugates(D) if (!D->extensions) D->extensions = String2Index(AllocateString(NULL,4 * sizeof(MEANING),false,true));
#define PLURALFIELD 0
#define AccessPlural(D) ((MEANING*)Index2String(D->extensions))[PLURALFIELD]
#define GetPlural(D) ((D->extensions) ? Meaning2Word(AccessPlural(D)) : 0)
#define SetPlural(D,M) {PrepareConjugates(D) AccessPlural(D) = M; }
#define COMPARISONFIELD 1
#define AccessComparison(D) ((MEANING*)Index2String(D->extensions))[COMPARISONFIELD]
#define GetComparison(D) ((D->extensions) ? Meaning2Word(AccessComparison(D)) : 0)
#define SetComparison(D,M) { PrepareConjugates(D) AccessComparison(D) = M; }
#define TENSEFIELD 2
#define AccessTense(D) ((MEANING*)Index2String(D->extensions))[TENSEFIELD]
#define GetTense(D) ((D->extensions) ? Meaning2Word(AccessTense(D)) : 0)
#define SetTense(D,M) {PrepareConjugates(D) AccessTense(D) = M; }
#define CANONICALFIELD 3
#define AccessCanonical(D) ((MEANING*)Index2String(D->extensions))[CANONICALFIELD]
#define SetCanonical(D,M) {PrepareConjugates(D) AccessCanonical(D) = M; }
char* GetCanonical(WORDP D);
void ReadSubstitutes(char* name,unsigned int fileFlag);

// memory data
extern WORDP dictionaryBase;
extern char* stringBase;
extern char* stringFree;
extern unsigned long maxDictEntries;
extern unsigned long maxStringBytes;
extern unsigned int userTopicStoreSize;
extern unsigned int userTableSize;
extern unsigned long maxHashBuckets;
extern bool setMaxHashBuckets;
extern WORDP dictionaryLocked;

extern bool fullDictionary;

extern uint64 verbFormat;
extern uint64 nounFormat;
extern uint64 adjectiveFormat;
extern uint64 adverbFormat;
extern MEANING posMeanings[64];
extern MEANING sysMeanings[64];
extern bool buildDictionary;
extern char language[40];
extern FACT* factLocked;
extern char* stringLocked;

extern WORDP dictionaryPreBuild0;
extern WORDP dictionaryPreBuild1;
extern WORDP dictionaryFree;
extern char dictionaryTimeStamp[20];

// internal references to defined words
extern WORDP Dplacenumber;
extern WORDP Dpropername;
extern MEANING Mphrase;
extern MEANING MabsolutePhrase;
extern MEANING MtimePhrase;
extern WORDP Dclause;
extern WORDP Dverbal;
extern WORDP Dmalename,Dfemalename,Dhumanname;
extern WORDP Dtime;
extern WORDP Dunknown;
extern WORDP DunknownWord;
extern WORDP Dpronoun;
extern WORDP Dchild,Dadult;
extern MEANING Mmoney;
extern MEANING Mchatoutput;
extern MEANING Mburst;
extern MEANING Mpending;
extern MEANING Mkeywordtopics;
extern MEANING Mintersect;
extern MEANING MconceptComment;
extern MEANING MgambitTopics;
extern MEANING MadjectiveNoun;
extern MEANING Mnumber;
extern char livedata[500];

// language dependent routines
extern char* (*P_GetAdjectiveBase)(char*, bool);   
extern char* (*P_GetAdverbBase)(char*,bool);
extern void (*P_SetSentenceTense)(unsigned int start, unsigned int end);
extern char* (*P_GetPastTense)(char*);
extern char* (*P_GetPastParticiple)(char*);
extern char* (*P_GetPresentParticiple)(char*);
extern char* (*P_GetThirdPerson)(char*);
extern char* (*P_GetInfinitive)(char*,bool);
extern char* (*P_GetSingularNoun)(char*,bool,bool);
extern char* (*P_GetPluralNoun)(WORDP);
extern void (*P_TagIt)();

char* AllocateString(char* word,size_t len = 0,bool align64=false,bool clear = false);
WORDP StoreWord(int);
WORDP StoreWord(char* word, uint64 properties = 0);
WORDP StoreWord(char* word, uint64 properties, uint64 flags);
WORDP FindWord(const char* word, int len = 0,uint64 caseAllowed = STANDARD_LOOKUP);
WORDP FullStore(char* word, uint64 properties, uint64 flags);
unsigned char BitCount(uint64 n);
void ReadQueryLabels(char* file);

char* UseDictionaryFile(char* name);
inline char* Index2String(unsigned int offset) { return stringBase - offset;}
inline unsigned int String2Index(char* str) {return stringBase- str;}
inline unsigned int GlossIndex(MEANING M) { return M >> 24;}
void ReadAbbreviations(char* file);
void Write32(unsigned int val, FILE* out);
unsigned int Read32(FILE* in);
void ReadLiveData();
void ReadLivePosData();
WORDP GetSubstitute(WORDP D);
void ShowStats(bool reset);

// adjust data on a dictionary entry
void AddProperty(WORDP D, uint64 flag);
void RemoveProperty(WORDP D, uint64 flag);
void RemoveSystemFlag(WORDP D, uint64 flag);
void AddSystemFlag(WORDP D, uint64 flag);
void AddInternalFlag(WORDP DP, uint64 flag);
void RemoveInternalFlag(WORDP D,unsigned int flag);
void WriteDWord(WORDP ptr, FILE* out);
WORDP ReadDWord(FILE* in);
void AddCircularEntry(WORDP base, unsigned int field,WORDP entry);
void SetWordValue(WORDP D, int x);
int GetWordValue(WORDP D);

inline unsigned int GetMeaningCount(WORDP D) { return (D->meanings) ? GetMeaning(D,0) : 0;}
inline unsigned int GetGlossCount(WORDP D) 
{
	if (D->internalBits & HAS_GLOSS)  return D->w.glosses[0];
	return 0;
}
char* GetGloss(WORDP D, unsigned int index);
unsigned int GetGlossIndex(WORDP D,unsigned int index);

// startup and shutdown routines
void InitDictionary();
void CloseDictionary();
void LoadDictionary();
void ExtendDictionary();
void WordnetLockDictionary();
void ReturnDictionaryToWordNet();
void Build0LockDictionary();
void ReturnDictionaryToBuild0();
void FreezeBasicData();
void ReturnToFreeze();
void DeleteDictionaryEntry(WORDP D);
void BuildDictionary(char* junk);

// read and write dictionary or its entries
void WriteDictionary(WORDP D, uint64 data);
void DumpDictionaryEntry(char* word,unsigned int limit);
bool ReadDictionary(char* file);
char* ReadDictionaryFlags(WORDP D, char* ptr,unsigned int *meaningcount = NULL, unsigned int *glosscount = NULL);
void WriteDictionaryFlags(WORDP D, FILE* out);
void WriteBinaryDictionary();
bool ReadBinaryDictionary();
void Write64(uint64 val, FILE* out);
uint64 Read64(FILE* in);
void Write24(unsigned int val, FILE* out);

// utilities
void ReadWordsOf(char* file,uint64 mark);
void WalkDictionary(DICTIONARY_FUNCTION func,uint64 data = 0);
char* FindCanonical(char* word, unsigned int i, bool nonew = false);
void VerifyEntries(WORDP D,uint64 junk);
void NoteLanguage();


bool IsHelper(char* word);
bool IsFutureHelper(char* word);
bool IsPresentHelper(char* word);
bool IsPastHelper(char* word);


///   code to manipulate MEANINGs
MEANING MakeTypedMeaning(WORDP x, unsigned int y, unsigned int flags);
MEANING MakeMeaning(WORDP x, unsigned int y = 0);
WORDP Meaning2Word(MEANING x);
MEANING AddMeaning(WORDP D,MEANING M);
MEANING AddTypedMeaning(WORDP D,unsigned int type);
MEANING AddGloss(WORDP D,char* gloss,unsigned int index);
void RemoveMeaning(MEANING M, MEANING M1);
MEANING ReadMeaning(char* word,bool create=true,bool precreated = false);
char* WriteMeaning(MEANING T,bool withPOS = false);
MEANING GetMaster(MEANING T);
unsigned int GetMeaningType(MEANING T);
MEANING FindSynsetParent(MEANING T,unsigned int which = 0);
MEANING FindSetParent(MEANING T,int n);

