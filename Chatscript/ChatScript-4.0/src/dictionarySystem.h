#ifndef _DICTIONARYSYSTEMH_
#define _DICTIONARYSYSTEMH_

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


typedef unsigned int DICTINDEX;	//   indexed ref to a dictionary entry

//   This file has formatting restrictions because it is machine read by AcquireDefines. All macro and function calls
//   must keep their ( attached to the name and all number defines must not.
//   All tokens of a define must be separated by spaces ( TEST|TEST1  is not legal).

//   These are the properties of a dictionary entry.
//   IF YOU CHANGE THE VALUES or FIELDS of a dictionary node,
//   You need to rework ReadDictionary/WriteDictionary/ReadBinaryEntry/WriteBinaryEntry

// D->properties bits

//   BASIC_POS POS bits like these must be on the bottom of any 64-bit word, so they match MEANING values (at top of 32 bit) as well.
#define NOUN					0x0000000080000000ULL	// bit header PENNBANK: NN NNS NNP NNPS
#define VERB					0x0000000040000000ULL	// bit header
#define ADJECTIVE				0x0000000020000000ULL	// bit header  PENNBANK: JJ JJR JJS
#define ADVERB					0x0000000010000000ULL   // bit header  PENNBANK: RB RBR RBS
#define BASIC_POS				( NOUN | VERB | ADJECTIVE | ADVERB )
//   the above four should be first, reflecting the Wordnet files (leaving room for wordnet offset)

#define PREPOSITION				0x0000000008000000ULL	// Pennbank: IN (see also CONJUNCTION_SUBORDINATE)
#define ESSENTIAL_FLAGS			( BASIC_POS | PREPOSITION ) //  these are type restrictions on MEANINGs and are shared onto systemflags for most likely pos-tag candidate type

// kinds of conjunctions
#define CONJUNCTION_COORDINATE  0x0000000004000000ULL	// Pennbank: CC
#define CONJUNCTION_SUBORDINATE 0x0000000002000000ULL	// Pennbank: IN (see also PREPOSIITON)
#define CONJUNCTION		( CONJUNCTION_COORDINATE | CONJUNCTION_SUBORDINATE ) // direct enumeration as header

// kinds of determiners 
#define PREDETERMINER 			0x0000000001000000ULL  // Pennbank: PDT
#define DETERMINER  			0x0000000000800000ULL   // Pennbank: DT
#define POSSESSIVE				0x0000000000400000ULL	// is a possessive like 's or Taiwanese (but not possessive pronoun)
#define PRONOUN_POSSESSIVE		0x0000000000200000ULL	// my your his her its our their				
#define POSSESSIVE_BITS			( PRONOUN_POSSESSIVE | POSSESSIVE )
#define DETERMINER_BITS		   ( DETERMINER | PREDETERMINER | POSSESSIVE_BITS ) // come before adjectives/nouns/ adverbs leading to those

// adverbs
#define ADVERB_NORMAL 			0x0000000000100000ULL 	
#define ADVERB_BITS				( ADVERB_NORMAL )

// kinds of adjectives
#define ADJECTIVE_NOUN			0x0000000000080000ULL	//  noun used as an adjective in front of another ("bank clerk") "attributive noun"
#define ADJECTIVE_NORMAL		0X0000000000040000ULL	// "friendly" // "friendlier" // "friendliest"
#define ADJECTIVE_PARTICIPLE	0x0000000000020000ULL	// the "walking" dead  or  the "walked" dog
#define ADJECTIVE_NUMBER		0x0000000000010000ULL	// the five dogs, the 5 dogs, and the fifth dog
#define ADJECTIVE_BITS ( ADJECTIVE_NOUN | ADJECTIVE_NORMAL | ADJECTIVE_PARTICIPLE | ADJECTIVE_NUMBER )

// punctuation
#define COMMA 					0x0000000000008000ULL	
#define PAREN					0x0000000000004000ULL	
#define PUNCTUATION				0x0000000000002000ULL	// this covers --  and  -  and [  ] etc BUT not COMMA or PAREN or QUOTE
#define QUOTE 					0x0000000000001000ULL	// double quoted string	
#define CURRENCY				0x0000000000000800ULL	// currency marker as stand alone

// unusual words
#define INTERJECTION			0x0000000000000400ULL	 
#define THERE_EXISTENTIAL		0x0000000000000200ULL	// "There" is no future in it. There is actually a unique kind of pronoun.

// kinds of pronouns
#define PRONOUN_SUBJECT  		0x0000000000000100ULL	// I you he she it we  they
#define PRONOUN_OBJECT			0x0000000000000080ULL	// me, you, him, her, etc
#define PRONOUN_BITS	( PRONOUN_OBJECT | PRONOUN_SUBJECT ) // there is no pronoun flag, just the bits

// kinds of nouns
#define NOUN_SINGULAR			0x0000000000000040ULL	// Pennbank: NN
#define NOUN_PLURAL				0x0000000000000020ULL	// Pennbank: NNS
#define NOUN_PROPER_SINGULAR	0x0000000000000010ULL	//   A proper noun that is NOT a noun is a TITLE like Mr.
#define NOUN_PROPER_PLURAL		0x0000000000000008ULL	// ( or )
#define NOUN_GERUND				0x0000000000000004ULL	// "Walking" is fun
#define NOUN_NUMBER				0x0000000000000002ULL	// I followed the "20".
#define NOUN_INFINITIVE 		0x0000000000000001ULL	
#define TO_INFINITIVE	 		0x0000800000000000ULL	// attaches to NOUN_INFINITIVE
#define NOUN_PROPER	 ( NOUN_PROPER_SINGULAR | NOUN_PROPER_PLURAL  )
#define NORMAL_NOUN_BITS ( NOUN_SINGULAR | NOUN_PLURAL | NOUN_PROPER | NOUN_NUMBER )
#define NOUN_BITS ( NORMAL_NOUN_BITS | NOUN_GERUND | NOUN_INFINITIVE )
#define STARTTAGS				0x0000800000000000ULL	// the top bit of the 48 bits visible to tagger

// kinds of verbs (tenses)

#define VERB_PRESENT			0x0000400000000000ULL	// present plural (usually infinitive)
#define VERB_PRESENT_3PS		0x0000200000000000ULL	// 3rd person singular singular
#define VERB_PRESENT_PARTICIPLE 0x0000100000000000ULL	// GERUND,  Pennbank: VBG
#define VERB_PAST				0x0000080000000000ULL	// Pennbank: VBD
#define VERB_PAST_PARTICIPLE    0x0000040000000000ULL	// Pennbank VBN
#define VERB_INFINITIVE			0x0000020000000000ULL	//   all tense forms are linked into a circular ring
#define PARTICLE				0x0000010000000000ULL	//   multiword separable verb (the preposition component) (full verb marked w systemflag SEPARABLE_PHRASAL_VERB or such) 
#define VERB_TENSES (  VERB_INFINITIVE | VERB_PRESENT | VERB_PRESENT_3PS | VERB_PAST | VERB_PAST_PARTICIPLE | VERB_PRESENT_PARTICIPLE  )

#define  FOREIGN_WORD			0x0000008000000000ULL	

// aux verbs
#define AUX_VERB				0x0000004000000000ULL	// bit header for all
#define	AUX_DO 					0x0000002000000000ULL	
#define AUX_HAVE				0x0000001000000000ULL 
#define	AUX_BE					0x0000000800000000ULL	
#define AUX_VERB_PRESENT		0x0000000400000000ULL
#define AUX_VERB_FUTURE			0x0000000200000000ULL
#define AUX_VERB_PAST			0x0000000100000000ULL
#define AUX_VERB_TENSES ( AUX_VERB_PRESENT | AUX_VERB_FUTURE | AUX_VERB_PAST ) // modal verbs
#define AUX_VERB_BITS ( AUX_VERB_TENSES | AUX_BE | AUX_HAVE | AUX_DO )


//////////////////////////////16 bits below here can not be used in posValues[] of tagger  
#define AS_IS					0x8000000000000000ULL   //  TRANSIENT INTERNAL dont try to reformat the word (transient flag passed to StoreWord)

#define NOUN_HUMAN				0x4000000000000000ULL  //   person or group of people that uses WHO, he, she, anyone
#define NOUN_FIRSTNAME			0x2000000000000000ULL  //   a known first name -- wiULL also be a sexed name probably
#define NOUN_SHE				0x1000000000000000ULL	//   female sexed word (used in sexed person title detection for example)
#define NOUN_HE					0x0800000000000000ULL	//   male sexed word (used in sexed person title detection for example)
#define NOUN_THEY				0x0400000000000000ULL   
#define NOUN_TITLE_OF_ADDRESS	0x0200000000000000LL	//   eg. mr, miss
#define NOUN_TITLE_OF_WORK		0x0100000000000000ULL
#define LOWERCASE_TITLE			0X0080000000000000ULL	//   lower case word may be in a title (but not a noun)
#define NOUN_ABSTRACT			0x0040000000000000ULL	// can be an abstract noun (maybe also concrete)
#define NOUN_MASS				0x0020000000000000ULL  
#define NOUN_NODETERMINER		0x0010000000000000ULL	// nouns of location that require no determiner (like "Home is where the heart is") 

#define QWORD 	 				0x0008000000000000ULL 	// who what where why when how whose -- things that can start a question

#define DETERMINER_SINGULAR		0x0004000000000000ULL		
#define DETERMINER_PLURAL 		0x0002000000000000ULL	

#define IDIOM 					0x0001000000000000ULL	// multi word expression of which end is at TAIL (like multiword prep)

//////////////////////////////bits above here are used in posValues[] of tagger  

#define NOUN_TITLE		 (  NOUN | NOUN_TITLE_OF_WORK | NOUN_PROPER_SINGULAR )
#define NOUN_HUMANGROUP		( NOUN | NOUN_HUMAN | NOUN_TITLE_OF_WORK | NOUN_THEY )
#define NOUN_MALEHUMAN     ( NOUN | NOUN_HUMAN | NOUN_HE | NOUN_PROPER_SINGULAR )
#define NOUN_FEMALEHUMAN    ( NOUN |  NOUN_HUMAN | NOUN_SHE | NOUN_PROPER_SINGULAR )
#define NOUN_HUMANNAME ( NOUN_HUMAN | NOUN | NOUN_PROPER_SINGULAR )

#define TAG_TEST ( INTERJECTION | IDIOM | PUNCTUATION | QUOTE | COMMA | CURRENCY | PAREN | PARTICLE | VERB_TENSES | NOUN_BITS | FOREIGN_WORD | NOUN_INFINITIVE | PREDETERMINER | DETERMINER | ADJECTIVE_BITS | AUX_VERB_BITS | ADVERB_BITS  | PRONOUN_BITS | CONJUNCTION | POSSESSIVE_BITS | THERE_EXISTENTIAL | PREPOSITION | TO_INFINITIVE )

#define NOUN_DESCRIPTION_BITS ( ADJECTIVE_BITS | DETERMINER_BITS  | ADVERB_BITS  | NOUN_BITS )

// system flags -- THIS IS A REQUIRED MARKER FOR BELOW ZONE

// english word attribues 
#define ANIMATE_BEING 				0x0000000000000001ULL 
#define	HOWWORD 				    0x0000000000000002ULL
#define TIMEWORD 					0x0000000000000004ULL
#define LOCATIONWORD				0x0000000000000008ULL	// prepositions will be ~placepreposition, not this - concepts have to mark this for place named nouns

// verb conjucations
#define VERB_CONJUGATE3				0x0000000000000010ULL	//	3rd word of composite verb extensions	
#define VERB_CONJUGATE2				0x0000000000000020ULL	//   2nd word of composite verb extensions  (e.g., re-energize)
#define VERB_CONJUGATE1				0x0000000000000040ULL	//   1st word of composite verb extensions  (e.g. peter_out)


#define PRESENTATION_VERB			0x0000000000000080ULL 	// will occur after THERE_EXISTENTIAL
#define COMMON_PARTICIPLE_VERB		0x0000000000000100ULL	// will be adjective after "be or seem" rather than treated as a verb participle

// these apply to adjectives AND adverbs
#define MORE_FORM					0x0000000000000200ULL  
#define MOST_FORM					0x0000000000000400ULL  


// phrasal verb controls
#define INSEPARABLE_PHRASAL_VERB		0x0000000000000800ULL	//  cannot be split apart ever
#define MUST_BE_SEPARATE_PHRASAL_VERB	0x0000000000001000ULL 	// phrasal MUST separate - "take my mother into" but not "take into my mother" 	
#define SEPARABLE_PHRASAL_VERB			0x0000000000002000ULL  // can be separated
#define PHRASAL_VERB 					0x0000000000004000ULL  // accepts particles - when lacking INSEPARABLE and MUST_SEPARABLE, can do either 

// verb objects
#define VERB_NOOBJECT  				0x0000000000008000ULL 	
#define VERB_INDIRECTOBJECT 		0x0000000000010000ULL	
#define VERB_DIRECTOBJECT 			0x0000000000020000ULL	 
#define VERB_TAKES_GERUND			0x0000000000040000ULL	// "keep on" singing
#define VERB_TAKES_ADJECTIVE		0x0000000000080000ULL	//  be seem etc (copular/linking verb) links adjectives and adjective participles to subjects
#define VERB_TAKES_INDIRECT_THEN_TOINFINITIVE	0x0000000000100000ULL    // proto 24  --  verbs taking to infinitive after object: "Somebody ----s somebody to INFINITIVE"  "I advise you *to go" + ~TO_INFINITIVE_OBJECT_verbs
#define VERB_TAKES_INDIRECT_THEN_VERBINFINITIVE	0x0000000000200000ULL 	// proto 25  -  verbs that take direct infinitive after object  "Somebody ----s somebody INFINITIVE"  "i heard her sing"  + ~causal_directobject_infinitive_verbs
#define VERB_TAKES_TOINFINITIVE					0x0000000000400000ULL    // proto 28 "Somebody ----s to INFINITIVE"   "we agreed to plan" -- when seen no indirect object
#define VERB_TAKES_VERBINFINITIVE				0x0000000000800000ULL 	// proto 32, 35 "Somebody ----s INFINITIVE"   "Something ----s INFINITIVE"  -- when seen no indirect object
//  ~factitive_adjective_Verbs take object complement adjective after direct object noun
// ~factitive_noun_verbs take object complement noun after direct object noun
// ~adjectivecomplement_taking_noun_infinitive adjectives can take a noun to infinitive after them as adjective "I was able to go"

#define OMITTABLE_TIME_PREPOSITION	0x0000000001000000ULL // can be used w/o preposition
#define ALWAYS_PROPER_NAME_MERGE	0x0000000002000000ULL // if see in any form in input, do proper name merge upon it
#define CONJUNCT_SUBORD_NOUN 		0x0000000004000000ULL	  

// these match the values in properties above
// PREPOSITION TIEBREAK				0x0000000008000000ULL
// ADVERB TIEBREAK					0x0000000010000000ULL
// ADJECTIVE TIEBREAK				0x0000000020000000ULL
// VERB TIEBREAK					0x0000000040000000ULL
// NOUN TIEBREAK					0x0000000080000000ULL

#define PRONOUN_PLURAL				0x0000000100000000ULL	
#define PRONOUN_SINGULAR			0x0000000200000000ULL	
#define ACTUAL_TIME  				0x0000000400000000ULL // a time word like 14:00
#define PREDETERMINER_TARGET		0x0000000800000000ULL	// predeterminer can come before these (a an the)

#define ADJECTIVE_POSTPOSITIVE		0x0000001000000000ULL	// adjective can occur AFTER the noun (marked on adjective)
// #define 				0x0000002000000000ULL	
//				0x0000004000000000ULL	
#define TAKES_POSTPOSITIVE_ADJECTIVE 	0x0000008000000000ULL	// word takes adjectives AFTER like something, nothing, etc (marked on word, not on adjective) 


#define GRADE5_6					0x0000010000000000ULL
#define GRADE3_4					0x0000020000000000ULL
#define GRADE1_2  					0x0000040000000000ULL
#define KINDERGARTEN				0x0000080000000000ULL
#define AGE_LEARNED ( KINDERGARTEN | GRADE1_2 | GRADE3_4 | GRADE5_6 )  // adult is all the rest  (3 bits)

// 	0x0000100000000000ULL // of can follow this verb
#define	WEB_URL						0x0000200000000000ULL	
#define ORDINAL						0x0000400000000000ULL  // for adjectives and nouns labeled ADJECTIVE_NUMBER or NOUN_NUMBER, it is cardinal if not on and ordinal if it is	

#define PRONOUN_REFLEXIVE				0x0000800000000000ULL 

// 16 bits CANNOT REFERENCE BELOW IN POS RULES - chatscript internal markers
#define IS_PATTERN_MACRO			0x0001000000000000ULL 
#define EXTENT_ADVERB				0x0002000000000000ULL	
#define MODEL_NUMBER				0x0004000000000000ULL  // Noun_number can be a normal number word like "one" or a model number like "Cray-3"

// chatscript roles of dictionary entry
//					0x0008000000000000ULL  
#define SUBSTITUTE_RECIPIENT		0x0010000000000000ULL	
#define FUNCTION_NAME				0x0020000000000000ULL 	//   name of a ^function  (has non-zero ->x.codeIndex if system, else is user but can be patternmacro,outputmacro, or plan) only applicable to ^ words
#define CONCEPT						0x0040000000000000ULL	// topic or concept has been read via a definition
#define TOPIC						0x0080000000000000ULL	//  this is a ~xxxx topic name in the system - only applicable to ~ words

#define IS_OUTPUT_MACRO				0x0100000000000000ULL	// function is an output macro
#define IS_TABLE_MACRO				0x0200000000000000ULL	// function is a table macro - transient executable output function
#define IS_PLAN_MACRO				0x0300000000000000ULL	// function is a plan macro (specialized form of IS_OUTPUT_MACRO has a codeindex which is the topicindex)
#define FUNCTION_BITS ( IS_PATTERN_MACRO | IS_OUTPUT_MACRO | IS_TABLE_MACRO | IS_PLAN_MACRO )

#define CONDITIONAL_ADVERB_IDIOM 	0x0400000000000000ULL	// "a little" may or may not merge to a single adverb depending of values of SYSTEMFLAG tiebreakers and next word

// chatscript properties

#define NO_PROPER_MERGE				0x0800000000000000ULL	// do not merge this word into any proper name
#define MARKED_WORD					0x1000000000000000ULL	// transient word marker that USER can store on word and find facts that connect to it && building dictionary uses it to mean save this word regardless

#define PATTERN_WORD 				0x2000000000000000ULL

#define DELAYED_RECURSIVE_DIRECT_MEMBER	0x4000000000000000ULL  // concept will be built with all indirect members made direct
#define PRONOUN_INDIRECTOBJECT		0x8000000000000000ULL	// can be an indirect object

// CANNOT REFERNCE ABOVE IN POS TAGGING RULES

// end system flags -- THIS IS A REQUIRED MARKER FOR ABOVE ZONE

// these values of word->internalBits are NOT stored in the dictionary text files
//#define UTF8						0x00001000		// word has utf8 char in it
//#define UPPERCASE_HASH			0x00002000		// word has upper case English character in it
//#define VAR_CHANGED				0x00004000		// $variable has changed value this volley
//#define WORDNET_ID				0x00008000		// a wordnet synset header node (MASTER w gloss )
//#define INTERNAL_MARK				0x00010000		// transient marker for Intersect coding and Country testing in :trim 
//				0x00020000		
//#define BASE_DEFINED				0x00040000		// word had part of speech bits when dictionary was locked (enables you to tell new properties filled in on old dict entries
//#define DELETED_MARK				0x00080000		// transient marker for  deleted words in dictionary build - they dont get written out - includes script table macros that are transient
//#define BUILD0					0x00100000		// comes from build0 data (marker on functions, concepts, topics)
//#define BUILD1					0x00200000		// comes from build1 data
//#define HAS_EXCLUDE				0x00400000		// concept/topic has keywords to exclude
//#define TRACE_MACRO				0x00800000		// turn on tracing for this function
//#define QUERY_KIND				0x01000000		// is a query item (from LIVEDATA)
//#define HAS_GLOSS					0x02000000		// has a glosses ptr

// flags on facts  FACT FLAGS


// USER FLAGS: 0xffff0000ULL

#define USER_FLAGS			0xFFFFF000
#define SYSTEM_FLAGS		0x00000FFF

// transient flags
#define MARKED_FACT         0x00000800  //   TRANSIENT : used during inferencing sometimes to see if fact is marked
#define ITERATOR_FACT		MARKED_FACT	// used by iterator
#define MARKED_FACT2        0x00000400  //   TRANSIENT: used during inferencing sometimes to see if 2ndary fact is marked
#define FACTDEAD			0x00000200  //   has been killed off
#define FACTTRANSIENT       0x00000100  //   save only with a set, not with a user or base system

// permanent flags
#define FACTSUBJECT         0x00000080  //   index is - relative number to fact 
#define FACTVERB			0x00000040	//   is 1st in its bucket (transient flag for read/WriteBinary) which MIRRORS DICT BUCKETHEADER flag: 
#define FACTOBJECT		    0x00000020  //   does not apply to canonical forms of words, only the original form - for classes and sets, means dont chase the set
#define FACTDUPLICATE		0x00000010	//   allow repeats of this face

#define FACTATTRIBUTE	    0x00000008  // fact is an attribute fact, object can vary while subject/verb should be fixed 
//#define UNUSEDF2	        0x00000004  
//#define UNUSEDF1	        0x00000002 
#define ORIGINAL_ONLY       0x00000001  //  dont match on canonicals


// topic control flags

// permanent flags 
#define TOPIC_KEEP 1		// don't erase rules
#define TOPIC_REPEAT 2      // allow repeated output
#define TOPIC_RANDOM 4      // random access responders (not gambits)
#define TOPIC_SYSTEM 8		// combines NOSTAY, KEEP, and special status on not accessing its gambits  
#define TOPIC_NOSTAY 16		// do not stay in this topic
#define TOPIC_PRIORITY 32	// prefer this more than normal
#define TOPIC_LOWPRIORITY 64 // prefer this less than normal
#define TOPIC_NOBLOCKING 128 // :verify should not complain about blocking
#define TOPIC_NOKEYS 256	// :verify should not complain about keywords missing
#define TOPIC_NOPATTERNS 512	// :verify should not complain about patterns that fail
#define TOPIC_NOSAMPLES 1024
#define TOPIC_NOGAMBITS 2048
#define TOPIC_SAFE	4096		// update safe

//   TRANSIENT FLAGS
#define TOPIC_GAMBITTED 8192	//   (transient per user) gambit issued from this topic
#define TOPIC_RESPONDED 16384	//   (transient per user) responder issued from this topic
#define TOPIC_REJOINDERED 32768	//   (transient per user) rejoinder issued from this topic
#define ACCESS_FLAGS ( TOPIC_GAMBITTED | TOPIC_RESPONDED | TOPIC_REJOINDERED ) 
#define TOPIC_BLOCKED 65536		//   (transient per user) disabled by some users 
#define TOPIC_USED 131072		//   (transient per user) indicates need to write out the topic
#define TRANSIENT_FLAGS ( TOPIC_BLOCKED | TOPIC_USED | ACCESS_FLAGS ) 

// pos tagger result operators
#define DISCARD 1
#define KEEP 2
#define TRACE 8

// pos tagger pattern values   5 bits (0-31) + 3 flags
#define HAS 1 // any bit matching 
#define IS 2	// is exactly this
#define INCLUDE 3 // has bit AND has other bits
#define ISORIGINAL 4 // is this word
#define POSITION 5 // sentence boundary
#define START POSITION
#define END POSITION
#define ISCANONICAL 6 // canonical word is this
#define ISMEMBER 7	// is member of this set
#define RESETLOCATION 8	
#define HAS2VERBS 9	
#define CANONLYBE  10	// has all these bits by category 
#define HASPROPERTY 11
#define HASALLPROPERTIES 12	
#define ENDSWITH 13			// suffix it ends in
#define ORIGINALVALUE 14		
#define HASCANONICALPROPERTY 15
#define ISQWORD 16
#define ISQUESTION 17
#define ISABSTRACT 18
#define NONEAFTER 19
#define NONEBEFORE 20
#define POSSIBLEUNDETERMINEDNOUN 21
#define PARSEMARK 22
#define PRIORCANONICAL 23
#define PROBABLEPARTICLE 24
#define POSSIBLETOLESSVERB 25
#define PRIORCOMMA  26
#define HOWSTART 27
#define LASTCONTROL HOWSTART  // add new ops to optable as well
	
#define SKIP 1 // if it matches, move ptr along, if it doesnt DONT
#define STAY 2
#define NOTCONTROL 4 


// values of parseFlags (mapping of ~special_english_attributes)
  

#define FACTITIVE_ADJECTIVE_VERB	0x00000001		// ~factitive_adjective_Verbs
#define FACTITIVE_NOUN_VERB			0x00000002	// ~factitive_noun_Verbs
#define CAUSAL_TOINFINITIVE_VERB	0x00000004  //  "I asked him *to run"  see sysflag VERB_TAKES_INDIRECT_THEN_TOINFINITIVE ~causal_to_infinitive_verbs 
#define QUOTEABLE_VERB				0x00000008 // he said "I love you" ~quotable_verbs 
#define ADJECTIVE_TAKING_NOUN_INFINITIVE 0x00000010 // "he is *determined to go home" -- complement ~adjectivecomplement_taking_noun_infinitive 
#define OMITTABLE_THAT_VERB			0x00000020 // that in object clause might be omitted ~omittable_that_verbs 
#define NEGATIVE_ADVERB_STARTER		0x00000040 // "never can I see him" - before aux is NOT a question ~negative_adverb_starter
#define NON_COMPLEMENT_ADJECTIVE	 0x00000080	// cannot be used as adjective complement in "I am xxx" ~non_complement_adjectives 
#define CAUSAL_DIRECTINFINITIVE_VERB 0x00000100	 //   "I made john *run"  now under systemflag VERB_TAKES_INDIRECT_THEN_VERBINFINITIVE (object_complement) ~causal_direct_infinitive_verbs
#define CONJUNCTIONS_OF_TIME		0x00000200	// ~conjunctions_of_time
#define CONJUNCTIONS_OF_SPACE		0x00000400	 // ~conjunctions_of_space
#define CONJUNCTIONS_OF_ADVERB		0x00000800 // ~conjunctions_of_adverb  
#define NONDESCRIPTIVE_ADJECTIVE	0x00001000  // not usable as adjective complement  ~nondescriptiveadjective 
#define NEGATIVE_SV_INVERTER		0x00002000 // no never not at start of sentence can flip sv order ~negativeinverters
#define LOCATIONAL_INVERTER			0x00004000 // here there nowhere at start of sentence can flip sv order  ~locationalinverters
#define ADJECTIVE_NOT_SUBJECT_COMPLEMENT 0x00008000 // will be adverbs ~adjective_not_subject_complement 
#define ADVERB_POSTADJECTIVE		0x00010000  // modified a subject complement adjective postpositively  ~postadjective_adverb
#define QUANTITY_NOUN				0x00020000 // can have adverb modifying it like "over half" ~counting_nouns
#define CONJUNCTIVE_ADVERB			0x00040000  // join two main sentences with ; xxx ,   ~conjunctive_adverb
#define CORRELATIVE_ADVERB			0x00080000  // I like either boys or girls   ~correlative_adverb
#define POTENTIAL_CLAUSE_STARTER	0x00100000 // who whomever, etc might start a clause ~potential_clause_starter 
#define VERB_ALLOWS_OF_AFTER		0x00200000 // most cant-- used by LIVEDATA rules ~verb_with_of_after
#define ADJECTIVE_GOOD_SUBJECT_COMPLEMENT 0x00400000 // adjectives in conflict with adverbs which WILL be subject complements  ~adjective_good_subject_complement
#define PREP_ALLOWS_UNDETERMINED_NOUN 0x00800000 //  men *of faith  two *per box

// control over tokenization (tokenControl set from user $token variable)
// these values MIRRORED as resulting used values in %tokenflags (tokenFlags)
#define DO_ESSENTIALS			0x00000001 
#define DO_SUBSTITUTES			0x00000002
#define DO_CONTRACTIONS			0x00000004
#define DO_INTERJECTIONS		0x00000008
#define DO_BRITISH				0x00000010 
#define DO_SPELLING				0x00000020 
#define DO_TEXTING				0x00000040 
#define DO_SUBSTITUTE_SYSTEM	0x0000007f	// DOES NOT INCLUDE PRIVATE
#define DO_PRIVATE				0x00000080	// file specific to scripter, not in common release

#define DO_NUMBER_MERGE			0x00000100
#define DO_PROPERNAME_MERGE		0x00000200
#define DO_SPELLCHECK			0x00000400
#define DO_INTERJECTION_SPLITTING 0x00000800

// this do not echo into tokenFlags
#define DO_POSTAG				0x00001000  
#define DO_PARSE				0x00003000  // controls pos tagging and parsing both
#define NO_IMPERATIVE			0x00004000
#define NO_VERB					0x00008000
#define NO_WITHIN				0x00010000 // dont look inside composite words
#define NO_SENTENCE_END			0x00020000
//	0x00040000  /// UNUSED
#define NO_INFER_QUESTION		0x00080000  // require ? in input

// tenses do not echo into tokenControl
#define PRESENT					0x00001000    // basic tense
#define PAST					0x00002000	  // basic tense- both present perfect and past perfect map to it
#define FUTURE					0x00004000    // basic tense
#define PRESENT_PERFECT			0x00008000    // distinguish PAST PERFECT from PAST PRESENT_PERFECT
#define CONTINUOUS				0x00010000 
#define PERFECT					0x00020000    
#define PASSIVE					0x00040000    

// tokencontrol parallel values  echoed to tokenflags
#define NO_HYPHEN_END			0x00100000 // dont end sentences using hypens  - shares PRESENT BIT
#define NO_COLON_END			0x00200000 // dont end sentences using colons  - shares PAST BIT
#define NO_SEMICOLON_END		0x00400000 // dont end sentences using semicolons  - shares FUTURE BIT
#define STRICT_CASING			0x00800000 // trust that user means casing (on non-start words)
#define ONLY_LOWERCASE			0x01000000 // never recognize uppercase words, treat them all as lowercase
#define TOKEN_AS_IS				0x02000000 // let pennbank tokens be untouched

//   values of tokenFlags (seen processing input) 
#define QUESTIONMARK			0x10000000    
#define EXCLAMATIONMARK			0x20000000   
#define PERIODMARK				0x40000000   

#define USERINPUT				0x0000000080000000ULL  
#define COMMANDMARK 			0x0000000100000000ULL
#define IMPLIED_YOU 			0x0000000200000000ULL // commands and some why questions where you is implied

#define FOREIGN_TOKENS			0x0000000400000000ULL
#define FAULTY_PARSE			0x0000000800000000ULL   
#define QUOTATION				0x0000001000000000ULL

#define NO_MARK					0x0000002000000000ULL // dont mark stuff (used by pennmatch)
#define NOT_SENTENCE			0x0000004000000000ULL   

// these change from parsing
#define SENTENCE_TOKENFLAGS  ( QUOTATION | COMMANDMARK | IMPLIED_YOU | FOREIGN_TOKENS | FAULTY_PARSE  | NOT_SENTENCE | PRESENT | PAST | FUTURE | PRESENT_PERFECT | CONTINUOUS | PERFECT | PASSIVE )


// flags to control output processing
#define    OUTPUT_ONCE 1 
#define    OUTPUT_KEEPSET  2			// don't expand class or set
#define    OUTPUT_KEEPVAR  4			// don't expand a var past its first level
#define    OUTPUT_KEEPQUERYSET 8		// don't expand a fact var like @1object
#define    OUTPUT_SILENT 16				// don't show stuff if trace is on
#define    OUTPUT_NOCOMMANUMBER 32		// don't add to numbers
#define    OUTPUT_NOTREALBUFFER  64		// don't back up past start at all
#define	   OUTPUT_ISOLATED_PERIOD 128	// don't join periods onto numbers
#define    OUTPUT_NOQUOTES  256			// strip quotes off strings
#define	   OUTPUT_LOOP 512				// coming from a loop, fail does not cancel output
#define	   OUTPUT_UNTOUCHEDSTRING 1024	// leave string alone
#define	   OUTPUT_FACTREAD 2048			// reading in fact field
#define    OUTPUT_EVALCODE 4096			
#define	   OUTPUT_DQUOTE_FLIP 8192
#define	   OUTPUT_ECHO 16384
#define	   OUTPUT_STRING_EVALED 32768	// format string should be treated like an output call

struct WORDENTRY;
typedef WORDENTRY* WORDP;

typedef void (*DICTIONARY_FUNCTION)(WORDP D, uint64 data);

struct FACT;
typedef unsigned int MEANING; //   a flagged indexed dict ptr

typedef unsigned int FACTOID; //   a fact index

typedef struct WORDENTRY //   a dictionary entry  - starred items are written to the dictionary
{
	uint64  properties;				//   main language description of this node 
	uint64	hash;					//   we presume 2 hashs never collide, so we dont check the string for matching
	uint64  systemFlags;			//   additional dictionary and non-dictionary properties

	//   if you edit this, you may need to change ReadBinaryEntry and WriteBinaryEntry
	char*     word;					//   entry name
	union {
		char* botNames;				//   for topic name (start with ~) or planname (start with ^) - bot topic applies to  - only used by script compiler
		unsigned int planArgCount;	// number of arguments in a plan
	    unsigned char* fndefinition; //   for nonplan macro name (start with ^) - if FUNCTION_NAME is on and not system function, is user script - 1st byte is argument count
	    char* userValue;			//   if a $uservar (start with $) OR if a search label uservar 
		WORDP substitutes;			//   words (with internalBits HAS_SUBSTITUTE) that should be adjusted to during tokenization
		MEANING*  glosses;			//   for ordinary words: list of glosses for synset head meanings - is offset to allocstring and id index of meaning involved.
	}w;
		
	FACTOID subjectHead;		//  start threads for facts run thru here 
	FACTOID verbHead;			//  start threads for facts run thru here 
	FACTOID objectHead;			//  start threads for facts run thru here 
	
	unsigned int internalBits;
  	MEANING  meanings;			//  list of meanings (synsets) of this word - Will be wordnet synset id OR self ptr -- 1-based since 0th meaning means all meanings
	MEANING extensions;			//  assigned from permanent string space for 4 things: irregular nouns, irregular verbs,  irregular adjective/adverbs and canonicals  (8K of 200K have this filled in)
	MEANING	temps;				//	assigned from transient string space - 3 things: backtrace thread on search, where xref markings, and tried bits (all transient per sentence, cleared via ClearWhereInSentence)

    union {
          unsigned short topicIndex;	//   for a ~topic or %systemVariable or plan, this is its id
		  unsigned short codeIndex;		//   for a system function, its the table index for it
		  unsigned short debugIndex;	//   for a :test function, its the table index for it
		  unsigned short macroFlags;	//	 for a table, these bits signify special handling of its arguments (1 bit per argument for 30 argument limit)
    }x;
    unsigned short length;		//  length of the word
	
	unsigned int inferMark;		// no need to erase been here marker during marking facts, inferencing (for propogation) and during scriptcompile 

    MEANING spellNode;			// next word of same length as this
  	unsigned int nextNode;		// bucket-link for dictionary hash + top bye GETMULTIWORDHEADER // can this word lead a phrase to be joined - can vary based on :build state -- really only needs 4 bits
} WORDENTRY;



#include "dictionaryMore.h"

#endif
