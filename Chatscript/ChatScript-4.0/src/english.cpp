
#include "common.h"

unsigned int posTiming;

typedef struct EndingInfo 
{
	char* word;	
	uint64 properties;
	uint64 flags;
} EndingInfo;
	
EndingInfo noun2[] = 
{
	{"th",NOUN|NOUN_SINGULAR}, 
	{"ar",NOUN|NOUN_SINGULAR}, 
	{"ty",NOUN|NOUN_SINGULAR}, 
	{"et",NOUN|NOUN_SINGULAR}, 
	{"or",NOUN|NOUN_SINGULAR}, 
	{"al",NOUN|NOUN_SINGULAR}, 
	{"er",NOUN|NOUN_SINGULAR}, 
	{"ee",NOUN|NOUN_SINGULAR}, 
	{"id",NOUN|NOUN_SINGULAR}, 
	{"cy",NOUN|NOUN_SINGULAR}, 
	{0},
};

EndingInfo noun3[] = 
{
	{"ory",NOUN|NOUN_SINGULAR}, 
	{"ant",NOUN|NOUN_SINGULAR}, 
	{"eer",NOUN|NOUN_SINGULAR}, 
	{"log",NOUN|NOUN_SINGULAR}, 
	{"oma",NOUN|NOUN_SINGULAR}, 
	{"dom",NOUN|NOUN_SINGULAR}, 
	{"ard",NOUN|NOUN_SINGULAR}, 
	{"ide",NOUN|NOUN_SINGULAR}, 
	{"oma",NOUN|NOUN_SINGULAR}, 
	{"ity",NOUN|NOUN_SINGULAR}, 
	{"ist",NOUN|NOUN_SINGULAR}, 
	{"ism",NOUN|NOUN_SINGULAR}, 
	{"ing",NOUN|NOUN_SINGULAR}, 
	{"gon",NOUN|NOUN_SINGULAR}, 
	{"gam",NOUN|NOUN_SINGULAR}, 
	{"ese",NOUN|NOUN_SINGULAR}, 
	{"ure",NOUN|NOUN_SINGULAR}, 
	{"acy",NOUN|NOUN_SINGULAR}, 
	{"age",NOUN|NOUN_SINGULAR}, 
	{"ade",NOUN|NOUN_SINGULAR}, 
	{"ery",NOUN|NOUN_SINGULAR}, 
	{"ary",NOUN|NOUN_SINGULAR}, 
	{"let",NOUN|NOUN_SINGULAR}, 
	{"ess",NOUN|NOUN_SINGULAR}, 
	{"ice",NOUN|NOUN_SINGULAR}, 
	{"ice",NOUN|NOUN_SINGULAR}, 
	{"ine",NOUN|NOUN_SINGULAR}, 
	{"ent",NOUN|NOUN_SINGULAR}, 
	{"ion",NOUN|NOUN_SINGULAR}, 
	{"oid",NOUN|NOUN_SINGULAR}, 
	{"ite",NOUN|NOUN_SINGULAR}, 
	{0},
};
EndingInfo noun4[] = 
{
	{"tion",NOUN|NOUN_SINGULAR}, 
	{"ment",NOUN|NOUN_SINGULAR}, 
	{"emia",NOUN|NOUN_SINGULAR}, 
	{"opsy",NOUN|NOUN_SINGULAR}, 
	{"itis",NOUN|NOUN_SINGULAR}, 
	{"opia",NOUN|NOUN_SINGULAR}, 
	{"hood",NOUN|NOUN_SINGULAR}, 
	{"ness",NOUN|NOUN_SINGULAR}, 
	{"logy",NOUN|NOUN_SINGULAR}, 
	{"ette",NOUN|NOUN_SINGULAR}, 
	{"cide",NOUN|NOUN_SINGULAR}, 
	{"sion",NOUN|NOUN_SINGULAR}, 
	{"ling",NOUN|NOUN_SINGULAR}, 
	{"cule",NOUN|NOUN_SINGULAR}, 
	{"osis",NOUN|NOUN_SINGULAR}, 
	{"esis",NOUN|NOUN_SINGULAR}, 
	{"ware",NOUN|NOUN_SINGULAR}, 
	{"tude",NOUN|NOUN_SINGULAR}, 
	{"cian",NOUN|NOUN_SINGULAR}, 
	{"ency",NOUN|NOUN_SINGULAR}, 
	{"ence",NOUN|NOUN_SINGULAR}, 
	{"ancy",NOUN|NOUN_SINGULAR}, 
	{"ance",NOUN|NOUN_SINGULAR}, 
	{"tome",NOUN|NOUN_SINGULAR}, 
	{"tomy",NOUN|NOUN_SINGULAR}, 
	{"crat",NOUN|NOUN_SINGULAR}, 
	{"ship",NOUN|NOUN_SINGULAR}, 
	{"pnea",NOUN|NOUN_SINGULAR}, 
	{"path",NOUN|NOUN_SINGULAR}, 
	{"gamy",NOUN|NOUN_SINGULAR}, 
	{"onym",NOUN|NOUN_SINGULAR}, 
	{"icle",NOUN|NOUN_SINGULAR}, 
	{"wise",NOUN|NOUN_SINGULAR}, 
	{0},
};
EndingInfo noun5[] = 
{
	{"cracy",NOUN|NOUN_SINGULAR}, 
	{"scope",NOUN|NOUN_SINGULAR}, 
	{"scopy",NOUN|NOUN_SINGULAR}, 
	{"ocity",NOUN|NOUN_SINGULAR}, 
	{"acity",NOUN|NOUN_SINGULAR}, 
	{"loger",NOUN|NOUN_SINGULAR}, 
	{"ation",NOUN|NOUN_SINGULAR}, 
	{"arian",NOUN|NOUN_SINGULAR}, 
	{"ology",NOUN|NOUN_SINGULAR}, 
	{"algia",NOUN|NOUN_SINGULAR}, 
	{"sophy",NOUN|NOUN_SINGULAR}, 
	{"cycle",NOUN|NOUN_SINGULAR}, 
	{"orium",NOUN|NOUN_SINGULAR}, 
	{"arium",NOUN|NOUN_SINGULAR}, 
	{"phone",NOUN|NOUN_SINGULAR}, 
	{"iasis",NOUN|NOUN_SINGULAR}, 
	{"pathy",NOUN|NOUN_SINGULAR}, 
	{"phile",NOUN|NOUN_SINGULAR}, 
	{"phyte",NOUN|NOUN_SINGULAR}, 
	{"otomy",NOUN|NOUN_SINGULAR}, 
	{0},
};
EndingInfo noun6[] = 
{
	{"bility",NOUN|NOUN_SINGULAR}, 
	{"script",NOUN|NOUN_SINGULAR}, 
	{"phobia",NOUN|NOUN_SINGULAR}, 
	{"iatric",NOUN|NOUN_SINGULAR}, 
	{"logist",NOUN|NOUN_SINGULAR}, 
	{"oholic",NOUN|NOUN_SINGULAR}, 
	{"aholic",NOUN|NOUN_SINGULAR}, 
	{"plegia",NOUN|NOUN_SINGULAR}, 
	{"plegic",NOUN|NOUN_SINGULAR}, 
	{"ostomy",NOUN|NOUN_SINGULAR}, 
	{"ectomy",NOUN|NOUN_SINGULAR}, 
	{"trophy",NOUN|NOUN_SINGULAR}, 
	{"escent",NOUN|NOUN_SINGULAR}, 
	{0},
};
EndingInfo noun7[] = 
{
	{"escence",NOUN|NOUN_SINGULAR}, 
	{"ization",NOUN|NOUN_SINGULAR}, 
	{0},
};

	
EndingInfo verb5[] = 
{
	{"scribe",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{0},
};
EndingInfo verb4[] = 
{
	{"sect",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{0},
};
EndingInfo verb3[] = 
{
	{"ise",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{"ize",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{"ify",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{"ate",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{0},
};
EndingInfo verb2[] = 
{
	{"en",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{"er",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{"fy",VERB|VERB_INFINITIVE|VERB_PRESENT}, 
	{0},
};

EndingInfo adverb5[] = 
{
	{"wards",ADVERB|ADVERB_NORMAL,0}, 
	{0},
};

EndingInfo adverb4[] = 
{
	{"wise",ADVERB|ADVERB_NORMAL,0}, 
	{"ward",ADVERB|ADVERB_NORMAL,0}, 
	{0},
};
EndingInfo adverb3[] = 
{
	{"ily",ADVERB|ADVERB_NORMAL,0}, 
	{"bly",ADVERB|ADVERB_NORMAL,0}, 
	{0},
};
EndingInfo adverb2[] = 
{
	{"ly",ADVERB|ADVERB_NORMAL,0}, 
	{0},
};


EndingInfo adjective7[] = 
{
	{"iferous",ADJECTIVE|ADJECTIVE_NORMAL,0}, // comprised of
	{0},
};
EndingInfo adjective6[] = 
{
	{"escent",ADJECTIVE|ADJECTIVE_NORMAL,0},
	{0},
};
EndingInfo adjective5[] = 
{
	{"ative",ADJECTIVE|ADJECTIVE_NORMAL,0}, // tending toward
	{"esque",ADJECTIVE|ADJECTIVE_NORMAL,0},
	{"-free",ADJECTIVE|ADJECTIVE_NORMAL,0}, 
	{"gonal",ADJECTIVE|ADJECTIVE_NORMAL,0}, 
	{"gonic",ADJECTIVE|ADJECTIVE_NORMAL,0}, // angle
	{"proof",ADJECTIVE|ADJECTIVE_NORMAL,0}, 
	{"sophic",ADJECTIVE|ADJECTIVE_NORMAL,0}, // knowledge
	{"esque",ADJECTIVE|ADJECTIVE_NORMAL,0}, // in the style of
	{0},
};
EndingInfo adjective4[] = 
{
		{"less",ADJECTIVE|ADJECTIVE_NORMAL,0}, // without
		{"etic",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to
		{"_out",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to
		{"ular",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to
		{"uous",ADJECTIVE|ADJECTIVE_NORMAL,0}, // characterized by
		{"ical",ADJECTIVE|ADJECTIVE_NORMAL,0}, // pertaining to
		{"-off",ADJECTIVE|ADJECTIVE_NORMAL,0}, // capable of being
		{"ious",ADJECTIVE|ADJECTIVE_NORMAL,0}, // characterized by
		{"able",ADJECTIVE|ADJECTIVE_NORMAL,0}, // capable of being
		{"ible",ADJECTIVE|ADJECTIVE_NORMAL,0}, // capable of being
		{"like",ADJECTIVE|ADJECTIVE_NORMAL,0}, // resembling
		{"some",ADJECTIVE|ADJECTIVE_NORMAL,0}, // characterized by
		{"ward",ADJECTIVE|ADJECTIVE_NORMAL,0}, // direction of
		{"wise",ADJECTIVE|ADJECTIVE_NORMAL,0}, // direction of
	{0},
};
EndingInfo adjective3[] = 
{
		{"ial",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to
		{"oid",ADJECTIVE|ADJECTIVE_NORMAL,0}, // shape of
		{"ble",ADJECTIVE|ADJECTIVE_NORMAL,0}, // able to
		{"ous",ADJECTIVE|ADJECTIVE_NORMAL,0}, // characterized by
		{"ive",ADJECTIVE|ADJECTIVE_NORMAL,0}, // having the nature of
		{"ate",ADJECTIVE|ADJECTIVE_NORMAL,0}, // quality of
		{"ful",ADJECTIVE|ADJECTIVE_NORMAL,0}, // quality of
		{"ful",ADJECTIVE|ADJECTIVE_NORMAL,0}, // notable for
		{"ese",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to a place
		{"fic",ADJECTIVE|ADJECTIVE_NORMAL,0}, 
		{"ant",ADJECTIVE|ADJECTIVE_NORMAL,0}, // inclined to
		{"ent",ADJECTIVE|ADJECTIVE_NORMAL,0}, // one who causes
		{"ern",ADJECTIVE|ADJECTIVE_NORMAL,0}, // quality of
		{"ian",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to
		{"ile",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to
		{"_to",ADJECTIVE|ADJECTIVE_PARTICIPLE,0}, // to
		{"_of",ADJECTIVE|ADJECTIVE_PARTICIPLE,0}, // of
		{"ing",ADJECTIVE|ADJECTIVE_PARTICIPLE,0},  // verb present participle as adjective  // BUG adjectiveFormat = ADJECTIVE_PARTICIPLE;
		{"ied",ADJECTIVE|ADJECTIVE_PARTICIPLE,0}, // verb past participle as adjective
		{"ine",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to
		{"ual",ADJECTIVE|ADJECTIVE_NORMAL,0}, // gradual
	{0},
};
EndingInfo adjective2[] = 
{
		{"ic",ADJECTIVE|ADJECTIVE_NORMAL,0},  // pertaining to
		{"ar",ADJECTIVE|ADJECTIVE_NORMAL,0},  // relating to
		{"ac",ADJECTIVE|ADJECTIVE_NORMAL,0},  // pertaining to
		{"al",ADJECTIVE|ADJECTIVE_NORMAL,0},  // pertaining to
		{"en",ADJECTIVE|ADJECTIVE_NORMAL,0}, 
		{"an",ADJECTIVE|ADJECTIVE_NORMAL,0}, // relating to
	{0},
};
EndingInfo adjective1[] = 
{
		{"y",ADJECTIVE|ADJECTIVE_NORMAL,0}, 
	{0},
};

uint64 GetPosData(unsigned int at, char* original,WORDP &entry,WORDP &canonical,uint64& sysflags,uint64 &cansysflags,bool firstTry,bool nogenerate,unsigned int start) // case sensitive, may add word to dictionary, will not augment flags of existing words
{ // this is not allowed to write properties/systemflags/internalbits if the word is preexisting
	if (start == 0) start = 1;
	uint64 properties = 0;
	sysflags = cansysflags = 0;
	canonical = 0;
	entry = FindWord(original,0,PRIMARY_CASE_ALLOWED);

	///////////// PUNCTUATION

	if (*original == '-' && original[start] == '-' && !original[start+1])  // -- mdash equivalent WSJ
	{
		entry = canonical = StoreWord(original,PUNCTUATION);
		return PUNCTUATION;
	}
	if (*original == '.')
	{
		if (!original[start] || !strcmp(original,"..."))  // periods we NORMALLY kill off  .   and ...
		{
			entry = canonical = StoreWord(original,PUNCTUATION);
			return PUNCTUATION;
		}
	}

	///////////// URL WEBLINKS
	if (IsUrl(original,0)) 
	{
		properties = NOUN|NOUN_SINGULAR;
		entry = canonical = StoreWord(original,properties,WEB_URL); 
		cansysflags = sysflags = WEB_URL;
		return properties;
	}


	///////// WORD REWRITES particularly from pennbank tokenization
	
	if (IsUpperCase(*original) && firstTry) // someone capitalized things we think of as ordinary.
	{
		WORDP check = FindWord(original,0,LOWERCASE_LOOKUP);
		if (check && check->properties & (PREPOSITION|DETERMINER_BITS|CONJUNCTION|PRONOUN_BITS|POSSESSIVE_BITS)) 
		{
			entry =  canonical = FindWord(original,0,LOWERCASE_LOOKUP); //force lower case pronoun, dont want "His" as plural of HI nor thi's
			wordStarts[at] = entry->word;
			original = entry->word;
		}
	}

	if (*original == '@' && !original[1])
	{
		strcpy(original,"at");
		entry = canonical = FindWord(original,0,PRIMARY_CASE_ALLOWED);
		original = entry->word; 
	}

	if (!stricmp(original,"His") || !stricmp(original,"This")  || !stricmp(original,"Then"))
	{
		entry =  canonical = FindWord(original,0,LOWERCASE_LOOKUP); //force lower case pronoun, dont want "His" as plural of HI nor thi's
		wordStarts[at] = entry->word;
		original = entry->word;
	}
	if (!stricmp(original,"yes") )
	{
		entry =  canonical = FindWord(original,0,LOWERCASE_LOOKUP); //force lower case pronoun, dont want "yes" to be Y's
		wordStarts[at] = entry->word;
		original = entry->word;
	}

	if (!stricmp(original,"ca") &&  !stricmp(wordStarts[at+1],"not"))
	{
		entry = canonical = FindWord("can",0,LOWERCASE_LOOKUP); // casing irrelevant with not after it was "can't" split by pennbank to ca n't
		wordStarts[at] = entry->word;
		original = entry->word;
	}

	if (!stricmp(original,"wo") &&  !stricmp(wordStarts[at+1],"not"))
	{
		entry = canonical = FindWord("will",0,LOWERCASE_LOOKUP); // casing irrelevant with not after it was "can't" split by pennbank to ca n't
		cansysflags = sysflags = entry->systemFlags; // probably nothing here
		wordStarts[at] = entry->word;
		original = entry->word;
	}

	if (!stricmp(original,"n'") )
	{
		entry = canonical = FindWord("and",0,LOWERCASE_LOOKUP);
		wordStarts[at] = entry->word;
		original = entry->word;
	}

	// compute length NOW after possible changes to original
	size_t len = strlen(original);

	// ILLEGAL STUFF that our tokenization wouldn't provide
	if (len > 2 && original[len-2] == '\'')  // "it's  and other illegal words"
	{
		canonical = entry = StoreWord(original,0);
		cansysflags = sysflags = entry->systemFlags; // probably nothing here
		return 0;
	}
	
	// hyphenated word which is number on one side:   Cray-3  3-second
	char* hyphen = strchr(original,'-');

	// numeric words
	if (IsDigit(*original) || IsDigit(original[1]))
	{
		// DATE IN 2 DIGIT OR 4 DIGIT NOTATION
		char word[MAX_WORD_SIZE];
		*word = 0;
		// 4digit year 1990 and year range 1950s and 1950's
		if (IsDigit(*original)  && IsDigit(original[2]) && IsDigit(original[3])  &&
			(!original[4] || (original[4] == 's' && !original[5]) || (original[4] == '\'' && original[5] == 's' && !original[6] ))) sprintf(word,"%d",atoi(original));
		//  2digit year and range '40 and '40s
		if (*original == '\'' && IsDigit(original[2]) &&
			(!original[3] || (original[3] == 's' && !original[4]) || (original[3] == '\'' && original[4] == 's' && !original[5] ))) sprintf(word,"%d",atoi(original+1));
		if (*word)
		{
			properties = NOUN|NOUN_NUMBER|ADJECTIVE|ADJECTIVE_NUMBER;
			entry = StoreWord(original,properties,TIMEWORD);
			canonical = StoreWord(word,properties,TIMEWORD);
			sysflags = entry->systemFlags | TIMEWORD;
			cansysflags = canonical->systemFlags | TIMEWORD;
			return properties;
		}
	
		// handle time like 4:30
		if (len < 6 && IsDigit(original[len-1]) && (original[1] == ':' || original[2] == ':')) // 18:32
		{
			properties = NOUN|NOUN_NUMBER|ADJECTIVE|ADJECTIVE_NUMBER;
			entry = canonical = StoreWord(original,properties); // 18:32
			return properties;
		}

		// handle number range data like 120:129 
		char* at = original;
		int colon = 0;
		while (*++at && (IsDigit(*at)|| *at == ':')) 
		{
			if (*at == ':') ++colon;
		}
		if (!*at && colon == 1) // was completely digits and a single colon
		{
			properties = NOUN|NOUN_NUMBER|ADJECTIVE|ADJECTIVE_NUMBER;
			entry = canonical = StoreWord(original,properties); 
			return properties;
		}

		// mark numeric fractions
		char* fraction = strchr(original,'/');
		if (fraction)
		{
			char number[MAX_WORD_SIZE];
			strcpy(number,original);
			number[fraction-original] = 0;
			if (IsNumber(number) && IsNumber(fraction+1))
			{
				int x = atoi(number);
				int y = atoi(fraction+1);
				float val = (float)((float)x / (float)y);
				sprintf(number,"%1.2f",val);
				properties = ADJECTIVE|NOUN|ADJECTIVE_NUMBER|NOUN_NUMBER;
				if (!entry) entry = StoreWord(original,properties);
				canonical = FindWord(number,0,PRIMARY_CASE_ALLOWED);
				if (canonical) properties |= canonical->properties;
				else canonical = StoreWord(number,properties);
				sysflags = entry->systemFlags;
				cansysflags = entry->systemFlags;
				return properties;
			}
		}
	}
	unsigned int kind = IsNumber(original);
	if (kind) // penn numbers as words do not go to change their entry value
	{
		if (kind != ROMAN_NUMBER) MakeLowerCase(original);
		entry = StoreWord(original);
		char number[MAX_WORD_SIZE];
		char* value;
		uint64 baseflags = (entry) ? entry->properties : 0;
		if (kind == PLACE_NUMBER)
		{
			sprintf(number,"%d",(int)Convert2Integer(original));
			sysflags |= ORDINAL;
			properties = ADVERB|ADVERB_NORMAL|ADJECTIVE|ADJECTIVE_NUMBER|NOUN|NOUN_NUMBER| (baseflags & TAG_TEST); // place numbers all all potential adverbs:  "*first, he wept"  but not in front of an adjective or noun, only as verb effect
		}
		else if (kind == FRACTION_NUMBER) // word fraction
		{
			char* br = hyphen;
			if (!br) br = strchr(original,'_');
			char c = *br;
			*br = 0;
			int64 val1 = Convert2Integer(original);
			int64 val2 = Convert2Integer(br+1);
			float val = (float)((float)val1 / (float)val2);
			sprintf(number,"%1.2f",val );
			properties = ADJECTIVE|NOUN|ADJECTIVE_NUMBER|NOUN_NUMBER;
			*br = c;
			entry = StoreWord(original,properties);
			canonical = StoreWord(number,properties);
			properties |= canonical->properties;
			sysflags = entry->systemFlags;
			cansysflags = canonical->systemFlags;
			return properties;
		}
		else if (kind == CURRENCY_NUMBER) // money
		{
			GetCurrency(original,value);
			int64 n = Convert2Integer(value);
			float fn = (float)atof(value);
			if ((float)n == fn) 
			{
#ifdef WIN32
				sprintf(number,"%I64d",n); 
#else
				sprintf(number,"%lld",n); 
#endif
			}
			else if (strchr(value,'.')) sprintf(number,"%1.2f",fn);
			else 
			{
#ifdef WIN32
				sprintf(number,"%I64d",n); 
#else
				sprintf(number,"%lld",n); 
#endif
			}
			properties = NOUN|NOUN_NUMBER;
		}
		else
		{
			if (strchr(original,'.')) sprintf(number,"%1.2f",atof(original));
			else 
			{
				int64 val = Convert2Integer(original);
				if (val < 1000000000 && val >  -1000000000)
				{
					int smallval = (int) val;
					sprintf(number,"%d",smallval);
				}
				else
				{
#ifdef WIN32
					sprintf(number,"%I64d",val);	
#else
					sprintf(number,"%lld",val);	
#endif
				}
			
			}
			properties = ADJECTIVE|NOUN|ADJECTIVE_NUMBER|NOUN_NUMBER;
		}
		canonical = StoreWord(number,properties,sysflags);
		cansysflags |= sysflags;

		// other data already existing on the number

		if (entry->properties & PART_OF_SPEECH) 
		{
			uint64 val = entry->properties; // numbers we "know" in some form should be as we know them. like "once" is adverb and adjective, not cardinal noun
			if (entry->properties & NOUN && !(entry->properties & NOUN_BITS)) // we dont know its typing other than as noun... figure it out
			{
				if (IsUpperCase(*entry->word)) val |= NOUN_PROPER_SINGULAR;
				else val |= NOUN_SINGULAR;
			}
			if (val & ADJECTIVE_NORMAL) // change over to known number
			{
				properties ^= ADJECTIVE_NORMAL;
				properties |= ADJECTIVE_NUMBER|ADJECTIVE;
			}
			if (val & NOUN_SINGULAR) // change over to known number
			{
				properties ^= NOUN_SINGULAR;
				properties |= NOUN_NUMBER|NOUN;
				if (tokenControl & TOKEN_AS_IS) canonical = entry;
			}
			if (val & ADVERB_BITS) 
			{
				properties |= entry->properties & (ADVERB_NORMAL|ADVERB);
				if (tokenControl & TOKEN_AS_IS) canonical = entry;
			}
			if (val & PREPOSITION) 
			{
				properties |= PREPOSITION; // like "once"
				if (tokenControl & TOKEN_AS_IS) canonical = entry;
			}
			if (val & PRONOUN_BITS)  // in Penntags this is CD always but "no one is" is NN or PRP
			{
				properties |= entry->properties & PRONOUN_BITS;
				//if (at > 1 && !stricmp(wordStarts[at-1],"no"))
				//{
					//properties |= val & PRONOUN_BITS; // like "one"
					if (tokenControl & TOKEN_AS_IS) canonical = entry;
				//}
			}
			if (val & VERB) 
			{
				properties |= entry->properties & ( VERB_TENSES | VERB); // like "once"
				if (tokenControl & TOKEN_AS_IS) canonical = FindWord(GetInfinitive(original,false),0,LOWERCASE_LOOKUP);
			}
			if (val & POSSESSIVE && tokenControl & TOKEN_AS_IS && !stricmp(original,"'s") && at > start) // internal expand of "it 's" and "What 's" and capitalization failures that contractions.txt wouldn't have handled 
			{
				properties |= AUX_BE | POSSESSIVE | VERB_PRESENT_3PS | VERB;
				entry = FindWord("'s",0,PRIMARY_CASE_ALLOWED);
			}
		}
		entry = StoreWord(original,properties);
		sysflags |= entry->systemFlags;
		cansysflags |= canonical->systemFlags;
		return properties;
	}
		
	// check for ANY digit in the word when not a hypenated word like 100-year
	char* b = original;
	if (!hyphen) while (*b)
	{
		if (IsDigit(*b++)) // Sept.30 
		{
			properties = NOUN|NOUN_NUMBER|ADJECTIVE|ADJECTIVE_NUMBER;
			canonical = entry = StoreWord(original,properties,MODEL_NUMBER|TIMEWORD);
			sysflags = cansysflags = entry->systemFlags;
			sysflags |= MODEL_NUMBER|TIMEWORD;
			cansysflags |= MODEL_NUMBER|TIMEWORD;
			return properties;
		}
	}
	
	// use forced canonical?
	if (!canonical && entry)
	{
		WORDP E = entry;
		char* canon = GetCanonical(E );
		if (canon) canonical = StoreWord(canon);
	}
	
	if (entry && entry->properties & (PART_OF_SPEECH|TAG_TEST|PUNCTUATION)) // we know this usefully already
	{
		properties |= entry->properties;
		sysflags |= entry->systemFlags;
		if (properties & VERB_PAST)
		{
			char* participle = GetPastParticiple(GetInfinitive(original,true));
			if (participle && !strcmp(participle,original)) properties |= VERB_PAST_PARTICIPLE; // wordnet exceptions doesnt bother to list both
		}
		char* canon = GetCanonical(entry);
		canonical = (canon) ? FindWord(canon,0,PRIMARY_CASE_ALLOWED) : NULL;
		if (canonical) cansysflags = canonical->systemFlags;

		// possessive pronoun-determiner like my is always a determiner, not a pronoun. 
		if (entry->properties & (COMMA | PUNCTUATION | PAREN | QUOTE | POSSESSIVE | PUNCTUATION)) return properties;
	}
	bool known = (entry) ? ((entry->properties & PART_OF_SPEECH)  != 0) : false;
	bool preknown = known;

	/////// WHETHER OR NOT WE KNOW THE WORD, IT MIGHT BE ALSO SOME OTHER WORD IN ALTERED FORM (like plural noun or comparative adjective)

	if (!(properties & VERB_TENSES)) // could it be a verb we dont know directly (even if we know the word)
	{
		char* verb =  GetInfinitive(original,true); 
		if (verb)  // inifinitive will be different from original or we would already have found the word
		{
			known = true;
			properties |= VERB | verbFormat;
			if (verbFormat & VERB_PAST) // possible shared form with participle
			{
				char* pastparticiple = GetPastParticiple(verb);
				if (pastparticiple && !strcmp(pastparticiple,original)) properties |= VERB_PAST_PARTICIPLE;
			}
			entry = StoreWord(original,properties);
			canonical =  FindWord(verb,0,PRIMARY_CASE_ALLOWED); // we prefer verb as canonical form
		}
	}
	
	if (!(properties & (NOUN_BITS|PRONOUN_BITS))) // could it be plural noun we dont know directly -- eg dogs or the plural of a singular we know differently-- "arms is both singular and plural" - avoid pronouns like "his" or "hers"
	{
		if (original[len-1] == 's')
		{
			char* noun = GetSingularNoun(original,true,true);
			if (noun && strcmp(noun,original)) 
			{
				entry = StoreWord(original,NOUN);
				uint64 which = (entry->internalBits & UPPERCASE_HASH) ? NOUN_PROPER_PLURAL : NOUN_PLURAL;
				AddProperty(entry,which);
				properties |= NOUN|which;
				if (!canonical) canonical = FindWord(noun,0,PRIMARY_CASE_ALLOWED); // 2ndary preference for canonical is noun
			}
		}
	}

	if (!(properties & ADJECTIVE_BITS)) // could it be comparative adjective we werent recognizing even if we know the word
	{
		if (original[len-1] == 'r' && original[len-2] == 'e')
		{
			char* adjective = GetAdjectiveBase(original,true);
			if (adjective && strcmp(adjective,original)) 
			{
				WORDP D = StoreWord(original,ADJECTIVE|ADJECTIVE_NORMAL);
				if (!entry) entry = D;
				properties |= ADJECTIVE|ADJECTIVE_NORMAL;
				if (!canonical) canonical = FindWord(adjective,0,PRIMARY_CASE_ALLOWED); 
				sysflags |= adjectiveFormat;
			}
		}
		else if (original[len-1] == 't' && original[len-2] == 's'  && original[len-3] == 'e')
		{
			char* adjective = GetAdjectiveBase(original,true);
			if (adjective && strcmp(adjective,original)) 
			{
				WORDP D = StoreWord(original,ADJECTIVE|ADJECTIVE_NORMAL);
				if (!entry) entry = D;
				properties |= ADJECTIVE|ADJECTIVE_NORMAL;
				if (!canonical) canonical = FindWord(adjective,0,PRIMARY_CASE_ALLOWED); 
				sysflags |= adjectiveFormat;
			}
		}
	}

	if (!(properties & ADVERB_BITS)) // could it be comparative adverb even if we know the word
	{
		if (original[len-1] == 'r' && original[len-2] == 'e')
		{
			char* adverb = GetAdverbBase(original,true);
			if (adverb && strcmp(adverb,original)) 
			{
				WORDP D = StoreWord(original,ADVERB|ADVERB_NORMAL);
				if (!entry) entry = D;
				properties |= ADVERB|ADVERB_NORMAL;
				if (!canonical) canonical = FindWord(adverb,0,PRIMARY_CASE_ALLOWED); 
				sysflags |= adverbFormat;
			}
		}
		else if (original[len-1] == 't' && original[len-2] == 's'  && original[len-3] == 'e')
		{
			char* adverb = GetAdverbBase(original,true);
			if (adverb && strcmp(adverb,original)) 
			{
				WORDP D = StoreWord(original,ADVERB|ADVERB_NORMAL);
				if (!entry) entry = D;
				properties |= ADVERB|ADVERB_NORMAL;
				if (!canonical) canonical = FindWord(adverb,0,PRIMARY_CASE_ALLOWED); 
				sysflags |= adverbFormat;
			}
		}
	}

	// DETERMINE CANONICAL OF A KNOWN WORD
	if (!canonical && !IS_NEW_WORD(entry)) // we dont know the word and didn't interpolate it from noun or verb advanced forms (cannot get canonical of word created this volley)
	{
		if (properties & (VERB|NOUN_GERUND)) canonical = FindWord(GetInfinitive(original,true),0,PRIMARY_CASE_ALLOWED); // verb or known gerund (ing) or noun plural (s) which might be a verb instead
		
		if (properties & NOUN) // EVEN if we do know it... flies is a singular and needs canonical for fly BUG
		{
			char* singular = GetSingularNoun(original,true,true);
			// even if it is a noun, if it ends in s and the root is also a noun, make it plural as well (e.g., rooms)
			if (original[len-1] == 's' && singular && stricmp(singular,original)) 
			{
				known = true;
				properties |= NOUN_PLURAL;
			}
			if (!canonical) canonical = FindWord(singular,0,PRIMARY_CASE_ALLOWED);
		}

		if (!canonical) canonical = FindWord(GetAdjectiveBase(original,true),0,PRIMARY_CASE_ALLOWED);

		if (properties & (ADJECTIVE_NORMAL|ADVERB_NORMAL)) // some kind of known adjective or adverb
		{
			char* adjective;
			char* adverb;
			if (hyphen) // could prefix or postfix be comparative adjective or adverb so entire word inherits that?
			{
				*hyphen = 0;
				char word[MAX_WORD_SIZE];
				WORDP X = FindWord(original,0,LOWERCASE_LOOKUP);
				if (X && X->properties & ADJECTIVE_NORMAL) // front part is known adjective
				{
					adjective = GetAdjectiveBase(original,true); 
					if (adjective && strcmp(adjective,original)) // base is not the same
					{
						if (!canonical) 
						{
							sprintf(word,"%s-%s",adjective,hyphen+1);
							canonical = StoreWord(word,ADJECTIVE_NORMAL|ADJECTIVE);
						}
						sysflags |= adjectiveFormat;
					}
				}
				else
				{
					WORDP Y = FindWord(hyphen+1,0,LOWERCASE_LOOKUP);
					if (Y  && Y ->properties & ADJECTIVE_NORMAL) // back part is known adjective
					{
						adjective = GetAdjectiveBase(hyphen+1,true); 
						if (adjective && strcmp(adjective,hyphen+1)) // base is not the same
						{
							if (!canonical) 
							{
								sprintf(word,"%s-%s",original,adjective);
								canonical = StoreWord(word,ADJECTIVE_NORMAL|ADJECTIVE);
							}
							sysflags |= adjectiveFormat;
						}
					}
				}
				if (X && X->properties & ADVERB_NORMAL) // front part is known adverb
				{
					adverb = GetAdverbBase(original,true); 
					if (adverb && strcmp(adverb,original)) // base is not the same
					{
						if (!canonical)
						{
							sprintf(word,"%s-%s",adverb,hyphen+1);
							canonical = StoreWord(word,ADVERB_NORMAL|ADVERB);
						}
						sysflags |= adverbFormat;
					}
				}
				else
				{
					WORDP Y = FindWord(hyphen+1,0,LOWERCASE_LOOKUP);
					if (Y && Y->properties & ADVERB_NORMAL) // back part is known adjective
					{
						adverb = GetAdverbBase(hyphen+1,true); 
						if (adverb && strcmp(adverb,hyphen+1)) // base is not the same
						{
							if (!canonical) 
							{
								sprintf(word,"%s-%s",original,adverb);
								canonical = StoreWord(word,ADVERB_NORMAL|ADVERB);
							}
							sysflags |= adverbFormat;
						}
					}
				}
				*hyphen = '-';
			}
		}
		else if (!canonical && properties & (NOUN_SINGULAR | NOUN_PROPER_SINGULAR)) canonical = entry;
	}
	if (preknown && !canonical) canonical = entry; //  for all others

	// A WORD WE NEVER KNEW - figure it out
	if (!preknown) // if we didnt know the original word, then even if we've found noun/verb forms of it, we need to test other options
	{
		// process by know parts of speech or potential parts of speech
		if (!(properties & NOUN)) // could it be a noun but not already known as a known (eg noun_gerund from verb)
		{
			char* noun = GetSingularNoun(original,true,true); 
			if (noun) 
			{
				known = true;
				properties |= NOUN | nounFormat;
				entry = StoreWord(original,0);
				if (!canonical || !stricmp(canonical->word,original)) canonical = FindWord(noun,0,PRIMARY_CASE_ALLOWED);
			}
		}
		if (!(properties & ADJECTIVE)) 
		{
			char* adjective = GetAdjectiveBase(original,true); 
			if (!adjective && hyphen && !strcmp(original+len-3,"ing")) // some kind of verb ing formation which can be adjective particple
			{
				*hyphen = 0;
				char word[MAX_WORD_SIZE];
				WORDP X = FindWord(original,0,LOWERCASE_LOOKUP);
				if (X && X->properties & ADJECTIVE_NORMAL)
				{
					adjective = GetAdjectiveBase(original,true); 
					if (adjective && strcmp(adjective,original)) // base is not the same
					{
						sprintf(word,"%s-%s",adjective,hyphen+1);
						canonical = StoreWord(word,ADJECTIVE_NORMAL|ADJECTIVE);
					}
				}
				*hyphen = '-';
			}
			if (!adjective && hyphen) // third-highest
			{
				adjective = GetAdjectiveBase(hyphen+1,true);
				if (adjective && strcmp(hyphen+1,adjective)) // base is not the same
				{
					char word[MAX_WORD_SIZE];
					sprintf(word,"%s-%s",original,adjective);
					canonical = StoreWord(word,ADJECTIVE_NORMAL|ADJECTIVE);
					sysflags |= adjectiveFormat;
				}
			}
			if (adjective && hyphen) 
			{
				known = true;
				properties |= ADJECTIVE|ADJECTIVE_NORMAL;
				entry = StoreWord(original,0,adjectiveFormat);
				sysflags |= adjectiveFormat;
				if (!canonical) canonical = entry;
			}
		}
		if (!(properties & ADVERB)) 
		{
			char* adverb = GetAdverbBase(original,true); 
			if (!canonical && adverb) canonical = FindWord(adverb,0,PRIMARY_CASE_ALLOWED);
			if (!adverb && hyphen) 
			{
				adverb = GetAdverbBase(hyphen+1,true);
				if (adverb && strcmp(hyphen+1,adverb)) // base is not the same
				{
					char word[MAX_WORD_SIZE];
					sprintf(word,"%s-%s",original,adverb);
					canonical = StoreWord(word,ADJECTIVE_NORMAL|ADJECTIVE);
					sysflags |= adverbFormat;
				}
			}
			if (adverb && hyphen)  
			{
				known = true;
				properties |= ADVERB|ADVERB_NORMAL;
				entry = StoreWord(original,0,adverbFormat);
				sysflags |= adverbFormat;
				if (!canonical) canonical = FindWord(adverb,0,PRIMARY_CASE_ALLOWED);
			}
		}
		
		if (hyphen) // unknown hypenated words even when going to base
		{
			WORDP X;
			// co-author   could be noun with front stuff in addition
			char* noun = GetSingularNoun(original,true,true);
			if (!noun) // since we actually know the base, we are not trying to interpolate
			{
				noun = GetSingularNoun(hyphen+1,true,true);
				if (noun)
				{
					properties |= NOUN | nounFormat;
					X = FindWord(noun,0,PRIMARY_CASE_ALLOWED);
					if (X) properties |= X->properties & (NOUN|NOUN_PROPERTIES);

					entry = StoreWord(original,properties);
					char word[MAX_WORD_SIZE];
					strcpy(word,original);
					strcpy(word+(hyphen+1-original),noun);
					if (!canonical || !stricmp(canonical->word,original)) canonical = StoreWord(word,NOUN|NOUN_SINGULAR);
				}
			}
			char* verb = GetInfinitive(hyphen+1,true);
			if (verb)
			{
				X = FindWord(verb,0,PRIMARY_CASE_ALLOWED);
				if (X)  properties |= X->properties & (VERB|VERB_TENSES);
			}

			if (!properties) // since we recognize no component of the hypen, try as number stuff
			{
				*hyphen = 0;
				if (IsDigit(*original) || IsDigit(hyphen[1]) ||  IsNumber(original) || IsNumber(hyphen+1))
				{
					char word[MAX_WORD_SIZE];
					int64 n;
					n = Convert2Integer((IsNumber(original) || IsDigit(*original)) ? original : (hyphen+1));
					#ifdef WIN32
					sprintf(word,"%I64d",n); 
#else
					sprintf(word,"%lld",n); 
#endif
					*hyphen = '-';
					properties = NOUN|NOUN_NUMBER|ADJECTIVE|ADJECTIVE_NUMBER;
					entry = StoreWord(original,properties,TIMEWORD|MODEL_NUMBER);
					canonical = StoreWord(word,properties,TIMEWORD|MODEL_NUMBER);
					sysflags |= MODEL_NUMBER | TIMEWORD;
					cansysflags |= MODEL_NUMBER|TIMEWORD;
					return properties;
				}
				*hyphen = '-';
			}
		}
	}

	// fill in supplemental flags
	if (properties & NOUN && !(properties & NOUN_BITS))
	{
		if (entry->internalBits & UPPERCASE_HASH) properties |= NOUN_PROPER_SINGULAR;
		else properties |= NOUN_SINGULAR;
	}
	if (canonical && entry) entry->systemFlags |= canonical->systemFlags & AGE_LEARNED; // copy age data across
	else if (IS_NEW_WORD(entry)) canonical = DunknownWord;

	if (properties){;}
	else if (tokenControl & ONLY_LOWERCASE) {;}
	else if (tokenControl & STRICT_CASING && at != start && *wordStarts[at-1] != ':'){;} // can start a sentence after a colon (like newspaper headings
	else 
	{
#ifndef NOPOSPARSER
		// we will NOT allow capitalization shift on 1st word if it appears to be part of proper noun, unless its a simple finite word potentially
		WORDP X = (at == start) ? FindWord(wordStarts[at],0,LOWERCASE_LOOKUP) : NULL;
		if (X)
		{
			if (!IsUpperCase(*wordStarts[at+1]) || !wordStarts[at+1][1] || !wordStarts[at+1][2]) X = NULL; // next word is not big enough upper case
			else
			{
				WORDP Y = FindWord(wordStarts[at+1],0,UPPERCASE_LOOKUP);
				if (Y && Y->properties & (NOUN_TITLE_OF_ADDRESS|NOUN_FIRSTNAME)) X = NULL;	// first name wont have stuff before it
				else if (X->properties & ADVERB_BITS && !(X->properties & ADJECTIVE_BITS)) X = NULL; // adverb wont likely head a name
				else if (X->properties & (DETERMINER_BITS|VERB_INFINITIVE|PRONOUN_BITS|CONJUNCTION)) X = NULL; // nor will these
			}
		}

		if (X) {;}	// probable proper name started
		else if (firstTry ) // auto try for opposite case if we dont recognize the word
		{
			char alternate[MAX_WORD_SIZE];
			if (IsUpperCase(*original)) MakeLowerCopy(alternate,original);
			else MakeUpperCopy(alternate,original);
			WORDP D1,D2;
			uint64 flags1 = GetPosData(at,alternate,D1,D2,sysflags,cansysflags,false,nogenerate,start);
			if (flags1) 
			{
				wordStarts[at] = D1->word;
				entry = D1;
				canonical = D2;
				return flags1;
			}
		}
#else
		if ( IsUpperCase(*original)) // dont recognize this, see if we know  lower case if this was upper case
		{
			WORDP D = FindWord(original,0,LOWERCASE_LOOKUP);
			if (D) return GetPosData(at,D->word,entry,canonical,sysflags,cansysflags,false,nogenerate);
		}
#endif
	}

	// we still dont know this word, go generic if its lower and upper not known
	char word[MAX_WORD_SIZE];
	strcpy(word,original);
	word[len-1] = 0; // word w/o trailing s if any
	if (nogenerate){;}
	else if (!firstTry) {;} // this is second attempt
	else if (!properties && firstTry) 
	{
		size_t len = strlen(original);
		if (IsUpperCase(*original) || IsUpperCase(original[1]) || IsUpperCase(original[2]) ) // default it noun upper case
		{
			properties |= (original[len-1] == 's') ? NOUN_PROPER_PLURAL : NOUN_PROPER_SINGULAR;
			entry = StoreWord(original,0);
		}
		else // was lower case
		{
			properties |= (original[len-1] == 's') ? (NOUN_PLURAL|NOUN_SINGULAR) : NOUN_SINGULAR;
			entry = StoreWord(original,0);
		}
		properties |= NOUN;

		// default everything else
		bool mixed = (entry->internalBits & UPPERCASE_HASH) ? true : false;
		for (unsigned int i = 0; i < len; ++i) 
		{
			if (!IsAlpha(original[i]) && original[i] != '-' && original[i] != '_') 
			{
				mixed = true; // has non alpha in it
				break;
			}
		}
		if (!mixed) // could be ANYTHING
		{
			properties |= VERB | VERB_PRESENT | VERB_INFINITIVE |  ADJECTIVE | ADJECTIVE_NORMAL | ADVERB | ADVERB_NORMAL;
			if (properties & VERB)
			{
				if (!strcmp(original+len-2,"ed")) properties |= VERB_PAST | VERB_PAST_PARTICIPLE;
				if (original[len-1] == 's') properties |= VERB_PRESENT_3PS;
			}
			if (ProbableAdjective(original,len)) 
			{
				AddSystemFlag(entry,ADJECTIVE); // probable decode
				sysflags |= ADJECTIVE;
			}
			if (ProbableAdverb(original,len)) 
			{
				AddSystemFlag(entry,ADVERB);		 // probable decode
				sysflags |= ADVERB;
			}
			if (ProbableNoun(original,len)) 
			{
				AddSystemFlag(entry,NOUN);			// probable decode
				sysflags |= NOUN;
			}
			uint64 vflags = ProbableVerb(original,len);
			if (vflags ) 
			{
				AddSystemFlag(entry,VERB);			// probable decode
				sysflags |= VERB;
				properties &= -1 ^ VERB_TENSES;			// remove default tenses
				properties |= vflags;					// go with projected tenses
			}
		}

		// treat all hypenated words as adjectives "bone-headed"
		canonical = DunknownWord;
	}
	if (!entry) 
	{
		entry = StoreWord(original,properties);	 // nothing found (not allowed alternative)
		canonical = DunknownWord;
	}
	if (!canonical) canonical = entry;
	AddProperty(entry,properties);
	// interpolate singular normal nouns to adjective_noun EXCEPT qword nouns like "why"
	//if (properties & (NOUN_SINGULAR|NOUN_PLURAL) && !(entry->properties & QWORD) && !strchr(entry->word,'_')) flags |= ADJECTIVE_NOUN; // proper names also when followed by ' and 's  merge to be adjective nouns  
	//if (properties & (NOUN_PROPER_SINGULAR|NOUN_PROPER_PLURAL) && *wordStarts[at+1] == '\'' && (wordStarts[at+1][1] == 0 || wordStarts[at+1][1] == 's')) flags |= ADJECTIVE_NOUN; 
	//if (properties & NOUN_PROPER_SINGULAR && !(tokenControl & TOKEN_AS_IS)) flags |= ADJECTIVE_NOUN; // could be before "*US president"

	if (properties & VERB_INFINITIVE) properties |= NOUN_INFINITIVE;
	if (properties & (VERB_PRESENT_PARTICIPLE|VERB_PAST_PARTICIPLE)) properties |= ADJECTIVE_PARTICIPLE|ADJECTIVE;
	if (properties & VERB_PRESENT_PARTICIPLE) properties |= NOUN_GERUND|NOUN;
	if (*entry->word == '~' || *entry->word == '^' || *entry->word == '$') canonical = entry;	// not unknown, is self
	cansysflags |= canonical->systemFlags;
	sysflags |= entry->systemFlags;
	return properties;
}

void English_SetSentenceTense(unsigned int start, unsigned int end)
{
	uint64 aux[25];
	unsigned int auxIndex = 0;
	uint64 mainverbTense = 0;
	unsigned int mainVerb = 0;
	unsigned int mainSubject = 0;
	memset(aux,0,25 * sizeof(uint64));
	unsigned int defaultTense = 0;

	// command?
	unsigned int i;
	for (i = start; i <= end; ++i)
    {
		if (ignoreWord[i]) continue;
		if (roles[i] & (MAINOBJECT|MAINSUBJECT|MAINVERB)) break;
	}
	if (!stricmp(wordStarts[start],"why"))
	{
		tokenFlags |= QUESTIONMARK; 
		if (posValues[start+1] & VERB_INFINITIVE || posValues[start+2] & VERB_INFINITIVE) tokenFlags |= IMPLIED_YOU;
	}
	else if (roles[i] & MAINVERB && posValues[i] & VERB_INFINITIVE) tokenFlags |= COMMANDMARK|IMPLIED_YOU;

	// determine sentence tense when not past from verb using aux (may pick wrong aux)
	for (unsigned int i = start; i <= end; ++i)
    {
		if (ignoreWord[i]) continue;
		if (roles[i] & MAINSUBJECT) mainSubject = i;

		bool notclauseverbal = true;
#ifndef DISCARDPARSER
		if (roles[i] & MAINVERB) 
		{
			mainverbTense = posValues[i] & VERB_TENSES;
			mainVerb = i;
			if (!(tokenFlags & FUTURE) && posValues[i] & (VERB_PAST_PARTICIPLE|VERB_PAST) && !(tokenFlags & PRESENT)) tokenFlags |= PAST; // "what are you interested in"
			else if (!(tokenFlags & FUTURE) && posValues[i] & (VERB_PRESENT|VERB_PRESENT_3PS)) defaultTense = PRESENT;
		}
		// if not parsing but are postagging
		if (!mainverbTense && !roles[i] && posValues[i] & VERB_TENSES) 
		{
			mainverbTense = posValues[i] & VERB_TENSES;
			mainVerb = i;
			if (!(tokenFlags & FUTURE) && posValues[i] & (VERB_PAST_PARTICIPLE|VERB_PAST)) tokenFlags |= PAST;
			else if (!(tokenFlags & FUTURE) && posValues[i] & (VERB_PRESENT|VERB_PRESENT_3PS)) defaultTense = PRESENT;
		}
		if (clauses[i] || verbals[i]) notclauseverbal = false;

#endif

		if (posValues[i] & AUX_VERB_BITS && notclauseverbal)
		{
			aux[auxIndex] = originalLower[i] ? (originalLower[i]->properties & (AUX_VERB_BITS | VERB_TENSES)) : 0;	// pattern of aux

			// question? 
			if (i == start && !(tokenControl & NO_INFER_QUESTION) ) tokenFlags |= QUESTIONMARK;
			if (defaultTense){;}
			else if (aux[auxIndex] & AUX_BE)
			{
				if (aux[auxIndex]  & (VERB_PRESENT|VERB_PRESENT_3PS)) tokenFlags |= PRESENT;
				else if (aux[auxIndex]  & (VERB_PAST | VERB_PAST_PARTICIPLE)) tokenFlags |= PAST;
			}
			else if (aux[auxIndex]  & AUX_VERB_FUTURE ) tokenFlags |= FUTURE;
			else if (aux[auxIndex]  & AUX_VERB_PAST) tokenFlags |= PAST;
			auxIndex++;
			if (auxIndex > 20) break;
		}
	}
	if (!auxIndex && canonicalLower[start] && !stricmp(canonicalLower[start]->word,"be") && !(tokenControl & NO_INFER_QUESTION)) tokenFlags |= QUESTIONMARK;  // are you a bank teller
	if ( canonicalLower[start] && canonicalLower[start]->properties & QWORD && canonicalLower[start+1] && canonicalLower[start+1]->properties & AUX_VERB_BITS  && !(tokenControl & NO_INFER_QUESTION))   tokenFlags |= QUESTIONMARK;  // what are you doing?  what will you do?
	else if ( posValues[start] & PREPOSITION && canonicalLower[start+1] && canonicalLower[start+1]->properties & QWORD && canonicalLower[start+2] && canonicalLower[start+2]->properties & AUX_VERB_BITS  && !(tokenControl & NO_INFER_QUESTION))   tokenFlags |= QUESTIONMARK;  // what are you doing?  what will you do?

#ifdef INFORMATION
	Active tenses:  have + past participle makes things perfect  --		 be + present particicple makes things continuous
		1. present
			a. simple
			b. perfect - I have/has + past participle
			c. continuous - I am + present participle
			d. perfect continuous - I have been + present participle
		2.  past
			a. simple
			b. perfect - I had + past participle
			c. continuous - I was + present participle
			d. perfect continuous - I had been + present participle
		3. future
			a: simple - I will + infinitive or  I am going_to + infinitive
			b. perfect - I will have + past participle 
			c. continuous - I will be + present participle
			d. perfect continuous - I will have been + present participle
	Passive tenses:  be + past participle makes things passive
		1. present
			a. simple - I am  + past participle
			b. perfect - I have/has + been + past participle
			c. continuous - I am + being + past participle
			d. perfect continuous - I have been + being + past participle
		2.  past
			a. simple - I was + past participle
			b. perfect - I had + been + past participle
			c. continuous - I was +  being + past participle
			d. perfect continuous - I had been + being + past participle
		3. future
			a: simple - I will + be + past participle  or  I am going_to + be + past participle
			b. perfect - I will have + been + past participle 
			c. continuous - I will be + past participle
			d. perfect continuous - I will have been + being + past participle
#endif

	if (auxIndex)
	{
		// special future "I am going to swim"
		if (*aux & (VERB_PRESENT|VERB_PRESENT_3PS) && *aux & AUX_BE && auxIndex == 1 && mainverbTense == VERB_PRESENT_PARTICIPLE && !stricmp(wordStarts[mainVerb],"going") && posValues[mainVerb+1] == TO_INFINITIVE)  tokenFlags |= FUTURE;
		else if (aux[auxIndex-1] & AUX_HAVE && mainverbTense & VERB_PAST_PARTICIPLE) 
		{
			if (*aux & (VERB_PRESENT|VERB_PRESENT_3PS)) 
			{
				tokenFlags |= PRESENT_PERFECT|PAST; 
				if (tokenFlags & PRESENT)  tokenFlags ^= (PRESENT|PERFECT);
			}
			else tokenFlags |= PERFECT; 
		}
		else if (aux[auxIndex-1] & AUX_BE && mainverbTense & VERB_PRESENT_PARTICIPLE) tokenFlags |= CONTINUOUS; 

		// compute passive
		if ((aux[auxIndex-1] & AUX_BE || (canonicalLower[auxIndex-1] && !stricmp(canonicalLower[auxIndex-1]->word,"get"))) && mainverbTense & VERB_PAST_PARTICIPLE) // "he is lost" "he got lost"
		{
			tokenFlags |= PASSIVE;
			if (aux[auxIndex-1] & VERB_PRESENT_PARTICIPLE)  tokenFlags |= CONTINUOUS;	// being xxx
		}
		
		if (*aux & AUX_HAVE && aux[auxIndex-1] & AUX_BE && mainverbTense & VERB_PRESENT_PARTICIPLE) 
		{
				tokenFlags |= PERFECT;	// I have/had been xxx
		}
		if (aux[1] & AUX_HAVE && aux[auxIndex-1] & AUX_BE && mainverbTense & VERB_PRESENT_PARTICIPLE) 
		{
				tokenFlags |= PERFECT;	// I will have/had been xxx
		}
		if (*aux & AUX_VERB_FUTURE)  
		{
			tokenFlags |= FUTURE;
			if (mainverbTense & VERB_PRESENT_PARTICIPLE && aux[auxIndex-1] & AUX_BE && aux[auxIndex-1] & VERB_INFINITIVE) tokenFlags |= CONTINUOUS;	// be painting
		}
		else if (*aux & (AUX_VERB_PAST | VERB_PAST))  tokenFlags |= PAST; 
		if (tokenFlags & PERFECT && !(tokenFlags & FUTURE)) 
		{
				tokenFlags |= PAST; // WE CAN NOT LET PRESENT PERFECT STAND, we need it to be in the past! "we have had sex"
		}
		else if (!(tokenFlags & (PAST|FUTURE))) tokenFlags |= PRESENT; 
	}
	else if (defaultTense) tokenFlags |=  defaultTense;
	else if (!(tokenFlags & (PRESENT|PAST|FUTURE))) tokenFlags |= PRESENT; 

	if (!(tokenFlags & IMPLIED_YOU) && (!mainSubject | !mainVerb) ) tokenFlags  |= NOT_SENTENCE;
}

static char* MakePastTense(char* original,WORDP D,bool participle)
{
	static char buffer[MAX_WORD_SIZE];
	char word[MAX_WORD_SIZE];
	strcpy(word,original);
	if (*original == '-' || *original == '_') return NULL;	// protection

    //   check for multiword behavoir. Always change the 1st word only
    char* at =  strchr(word,'_'); 
	if (!at) at = strchr(word,'-');
    if (at && at[1])
    {
		int cnt = BurstWord(word,HYPHENS);
		char trial[MAX_WORD_SIZE];
        char words[10][MAX_WORD_SIZE];
		unsigned int lens[10];
		char separators[10];
		if (cnt > 9) return NULL;
		int len = 0;
		for (int i = 0; i < cnt; ++i) //   get its components
		{
			strcpy(words[i],GetBurstWord(i));
			lens[i] = strlen(words[i]);
			len += lens[i];
			separators[i] = word[len++];
		}
		for (int i = 0; i < cnt; ++i)
		{
			if (D && (D->systemFlags & (VERB_CONJUGATE1|VERB_CONJUGATE2|VERB_CONJUGATE3)))
			{
				if (D->systemFlags & VERB_CONJUGATE1 && i != 0) continue;
				if (D->systemFlags & VERB_CONJUGATE2 && i != 1) continue;
				if (D->systemFlags & VERB_CONJUGATE3 && i != 2) continue;
			}
			// even if not verb, we will interpolate: "slob_around" doesnt have slob as a verb - faff_about doesnt even have "faff" as a word
			char* inf;
			inf = (participle) ? (char*) GetPastParticiple(words[i]) : (char*) GetPastTense(words[i]);
			if (!inf) continue;
			*trial = 0;
			char* at1 = trial;
			for (int j = 0; j < cnt; ++j) //   rebuild word
			{
				if (j == i)
				{
					strcpy(at1,inf);
					at1 += strlen(inf);
					*at1++ = separators[j];
				}
				else
				{
					strcpy(at1,words[j]);
					at1 += lens[j];
					*at1++ = separators[j];
				}
			}
			strcpy(buffer,trial);
			return buffer;
		}
	}

    strcpy(buffer,word);
    size_t len = strlen(buffer);
    if (buffer[len-1] == 'e') strcat(buffer,"d");
    else if (!(IsVowel(buffer[len-1]) || buffer[len-1] == 'y' ) && IsVowel(buffer[len-2]) && len > 2 && !IsVowel(buffer[len-3])) 
    {
        char lets[2];
        lets[0] = buffer[len-1];
        lets[1] = 0;
        strcat(buffer,lets); 
        strcat(buffer,"ed");
    }
    else if (buffer[len-1] == 'y' && !IsVowel(buffer[len-2])) 
    {
        buffer[len-1] = 'i';
        strcat(buffer,"ed");
    }   
    else strcat(buffer,"ed");
    return buffer; 
}

char* English_GetPastTense(char* original)
{
    WORDP D = FindWord(original,0,LOWERCASE_LOOKUP);
    if (D && D->properties & VERB && GetTense(D)) //   die is both regular and irregular?
    {
        int n = 9;
        while (--n && D)
        {
            if (D->properties & VERB_PAST) return D->word; //   might be an alternate present tense moved to main
            D = GetTense(D); //   known irregular
        }
    }
	return MakePastTense(original,D,false);
}

char* English_GetPastParticiple(char* word)
{
	if (!word) return NULL;
    WORDP D = FindWord(word,0,LOWERCASE_LOOKUP);
    if (D && D->properties & VERB && GetTense(D) ) 
    {
        int n = 9;
        while (--n && D)
        {
            if (D->properties & VERB_PAST_PARTICIPLE) return D->word; //   might be an alternate present tense moved to main
            D = GetTense(D); //   known irregular
        }
   }
   return MakePastTense(word,D,true);
}

char* English_GetPresentParticiple(char* word)
{
    static char buffer[MAX_WORD_SIZE];
    WORDP D = FindWord(word,0,LOWERCASE_LOOKUP);
    if (D && D->properties & VERB && GetTense(D) ) 
    {//   born (past) -> bore (a past and a present) -> bear 
        int n = 9;
        while (--n && D) //   we have to cycle all the conjugations past and present of to be
        {
            if (D->properties &  VERB_PRESENT_PARTICIPLE) return D->word; 
            D = GetTense(D); //   known irregular
        }
    }
    
    strcpy(buffer,word);
    size_t len = strlen(buffer);
    char* inf = GetInfinitive(word,false);
    if (!inf) return 0;

    if (buffer[len-1] == 'g' && buffer[len-2] == 'n' && buffer[len-3] == 'i' && (!inf || stricmp(inf,word))) return word;   //   ISNT participle though it has ing ending (unless its base is "ing", like "swing"

    //   check for multiword behavoir. Always change the 1st word only
    char* at =  strchr(word,'_'); 
	if (!at) at = strchr(word,'-');
    if (at)
    {
		int cnt = BurstWord(word,HYPHENS);
		char trial[MAX_WORD_SIZE];
        char words[10][MAX_WORD_SIZE];
		int lens[10];
		char separators[10];
		if (cnt > 9) return NULL;
		int len = 0;
		for (int i = 0; i < cnt; ++i) //   get its components
		{
			strcpy(words[i],GetBurstWord(i));
			lens[i] = strlen(words[i]);
			len += lens[i];
			separators[i] = word[len++];
		}
		for (int i = 0; i < cnt; ++i)
		{
			if (D && (D->systemFlags & (VERB_CONJUGATE1|VERB_CONJUGATE2|VERB_CONJUGATE3)))
			{
				if (D->systemFlags & VERB_CONJUGATE1 && i != 0) continue;
				if (D->systemFlags & VERB_CONJUGATE2 && i != 1) continue;
				if (D->systemFlags & VERB_CONJUGATE3 && i != 2) continue;
			}
			WORDP E = FindWord(words[i],0,LOWERCASE_LOOKUP);
			if (!E || !(E->properties & VERB)) continue;
			char* inf = GetPresentParticiple(words[i]); //   is this word an infinitive?
			if (!inf) continue;
			*trial = 0;
			char* at = trial;
			for (int j = 0; j < cnt; ++j) //   rebuild word
			{
				if (j == i)
				{
					strcpy(at,inf);
					at += strlen(inf);
					*at++ = separators[j];
				}
				else
				{
					strcpy(at,words[j]);
					at += lens[j];
					*at++ = separators[j];
				}
			}
			strcpy(buffer,trial);
			return buffer;
		}
	}


    strcpy(buffer,inf); //   get the real infinitive

    if (!stricmp(buffer,"be"));
    else if (buffer[len-1] == 'h' || buffer[len-1] == 'w' ||  buffer[len-1] == 'x' ||  buffer[len-1] == 'y'); //   no doubling w,x,y h, 
    else if (buffer[len-2] == 'i' && buffer[len-1] == 'e') //   ie goes to ying
    {
        buffer[len-2] = 'y';
        buffer[len-1] = 0;
    }
    else if (buffer[len-1] == 'e' && !IsVowel(buffer[len-2]) ) //   drop ending Ce  unless -nge (to keep the j sound) 
    {
        if (buffer[len-2] == 'g' && buffer[len-3] == 'n');
        else buffer[len-1] = 0; 
    }
    else if (buffer[len-1] == 'c' ) //   add k after final c
    {
        buffer[len-1] = 'k'; 
        buffer[len] = 0;
    }
   //   double consonant 
    else if (!IsVowel(buffer[len-1]) && IsVowel(buffer[len-2]) && (!IsVowel(buffer[len-3]) || (buffer[len-3] == 'u' && buffer[len-4] == 'q'))) //   qu sounds like consonant w
    {
        char* base = GetInfinitive(word,false);
        WORDP D = FindWord(base,0,LOWERCASE_LOOKUP);
        if (D && D->properties & VERB && GetTense(D) ) 
        {
            int n = 9;
            while (--n && D)
            {
                if (D->properties & VERB_PAST) 
                {
                    unsigned int len = D->length;
                    if (D->word[len-1] != 'd' || D->word[len-2] != 'e' || len < 5) break; 
                    if (IsVowel(D->word[len-3])) break; //   we ONLY want to see if a final consonant is doubled. other things would just confuse us
                    strcpy(buffer,D->word);
                    buffer[len-2] = 0;      //   drop ed
                    strcat(buffer,"ing");   //   add ing
                    return buffer; 
                }
                D = GetTense(D); //   known irregular
            }
            if (!n) ReportBug("verb loop") //   complain ONLY if we didnt find a past tense
        }

        char lets[2];
        lets[0] = buffer[len-1];
        lets[1] = 0;
        strcat(buffer,lets);    //   double consonant
    }
    strcat(buffer,"ing");
    return buffer; 
}

uint64 ProbableVerb(char* original, unsigned int len)
{
	char word[MAX_WORD_SIZE];
	if (len == 0) len = strlen(original);
	strncpy(word,original,len);
	word[len] = 0;
	
	char* hyphen = strchr(original,'-');
	if (hyphen)
	{
		WORDP X = FindWord(hyphen+1,0,PRIMARY_CASE_ALLOWED);
		if (X && X->properties & VERB) return X->properties & (VERB_TENSES|VERB);
		char* verb = GetInfinitive(hyphen+1,false);
		if (verb) return VERB | verbFormat;
		uint64 flags = ProbableVerb(hyphen+1,len - (hyphen-original+1));
		if (flags) return flags;
	}

	char* item;
	char* test;
	int i;
	if (len >= 8) // word of 3 + suffix of 5
	{
		test = word+len-5;
		i = -1;
		while ((item = verb5[++i].word)) if (!stricmp(test,item)) return verb5[i].properties;
	}	
	if (len >= 7) // word of 3 + suffix of 4
	{
		test = word+len-4;
		i = -1;
		while ((item = verb4[++i].word)) if (!stricmp(test,item)) return verb4[i].properties;
	}
	if (len >= 6) // word of 3 + suffix of 3
	{
		test = word+len-3;
		i = -1;
		while ((item = verb3[++i].word)) if (!stricmp(test,item)) return verb3[i].properties;
	}
	if (len >= 5) // word of 3 + suffix of 2
	{
		test = word+len-2;
		i = -1;
		while ((item = verb2[++i].word)) if (!stricmp(test,item)) return verb2[i].properties;
	}
	return 0;
}

static char* InferVerb(char* original, unsigned int len)
{
	char word[MAX_WORD_SIZE];
	if (len == 0) len = strlen(original);
	strncpy(word,original,len);
	word[len] = 0;
	uint64 flags = ProbableVerb(original,len);
	if (flags) return StoreWord(word,flags)->word;
	return NULL;
}

char* English_GetThirdPerson(char* word)
{
    if (!word || !*word) return NULL;
    size_t len = strlen(word);
    if (len == 0) return NULL;
    WORDP D = FindWord(word,len,LOWERCASE_LOOKUP);
	if (!D || (D->properties & VERB) == 0) return NULL; 

    if (D && D->properties & VERB_PRESENT_3PS)  return D->word; 

    if (D && D->properties & VERB && GetTense(D) ) 
    {//   born (past) -> bore (a past and a present) -> bear 
		unsigned int n = 10;
        while (--n && D) //   scan all the conjugations 
        {
            if (D->properties & VERB_PRESENT_3PS) return D->word; 
            D = GetTense(D); //   known irregular
        }
    }

	static char result[MAX_WORD_SIZE];
	strcpy(result,word);
	strcat(result,"s");
	return result;
}

char* English_GetInfinitive(char* word, bool nonew)
{
	uint64 controls = tokenControl & STRICT_CASING ? PRIMARY_CASE_ALLOWED : LOWERCASE_LOOKUP;
	verbFormat = 0;	//   secondary answer- std infinitive or unknown
    if (!word || !*word) return NULL;
    size_t len = strlen(word);
    if (len == 0) return NULL;
    WORDP D = FindWord(word,len,controls);
    if (D && D->properties & VERB_INFINITIVE) 
	{
		verbFormat = VERB_INFINITIVE;  // fall  (fell) conflict
		return D->word; //    infinitive value
	}

    if (D && D->properties & VERB && GetTense(D) ) 
    {//   born (past) -> bore (a past and a present) -> bear 
		if (D->properties & VERB_PRESENT_PARTICIPLE) verbFormat = VERB_PRESENT_PARTICIPLE;
		else if (D->properties & (VERB_PAST|VERB_PAST_PARTICIPLE)) 
		{
			verbFormat = 0;
			if (D->properties & VERB_PAST) verbFormat |= VERB_PAST;
			if (D->properties & VERB_PAST_PARTICIPLE) verbFormat |= VERB_PAST_PARTICIPLE|ADJECTIVE_PARTICIPLE;
		}
		else if (D->properties & (VERB_PRESENT|VERB_PRESENT_3PS)) verbFormat = VERB_PRESENT;
		unsigned int n = 10;
        while (--n && D) //   scan all the conjugations 
        {
            if (D->properties & VERB_INFINITIVE) return D->word; 
            D = GetTense(D); //   known irregular
        }
    }

	char last = word[len-1];  
    char prior = (len > 2) ? word[len-2] : 0;  //   Xs
    char prior1 = (len > 3) ? word[len-3] : 0; //   Xes
    char prior2 = (len > 4) ? word[len-4] : 0; //   Xhes
	char prior3 = (len > 5) ? word[len-5] : 0; //   Xhes

    //   check for multiword behavior. 
	int cnt = BurstWord(word,HYPHENS);
    if (cnt > 1)
    {
		char trial[MAX_WORD_SIZE];
        char words[10][MAX_WORD_SIZE];
		unsigned int lens[10];
		char separators[10];
		if (cnt > 9) return NULL;
		unsigned int len = 0;
		for (int i = 0; i < cnt; ++i) //   get its components
		{
			strcpy(words[i],GetBurstWord(i));
			lens[i] = strlen(words[i]);
			len += lens[i];
			separators[i] = word[len++];
		}
		for (int i = 0; i < cnt; ++i)
		{
			char* inf = GetInfinitive(words[i],false); //   is this word an infinitive?
			if (!inf) continue;
			*trial = 0;
			char* at = trial;
			for (int j = 0; j < cnt; ++j) //   rebuild word
			{
				if (j == i)
				{
					strcpy(at,inf);
					at += strlen(inf);
					*at++ = separators[j];
				}
				else
				{
					strcpy(at,words[j]);
					at += lens[j];
					*at++ = separators[j];
				}
			}
			*at = 0;
			WORDP E = FindWord(trial,0,controls);
			if (E && E->properties & VERB_INFINITIVE) return E->word;
		}

       return NULL;  //   not a verb
    }

    //   not known verb, try to get present tense from it
    if (last == 'd' && prior == 'e' && len > 3)   //   ed ending?
    {
		verbFormat = VERB_PAST|VERB_PAST_PARTICIPLE|ADJECTIVE_PARTICIPLE;

		// if vowel-vowel-consonant e d, prefer that
		if (len > 4 && !IsVowel(prior1) && IsVowel(prior2) && IsVowel(prior3))
		{
			D = FindWord(word,len-2,controls);    //   drop ed
 			if (D && D->properties & VERB) return D->word;
		}

		D = FindWord(word,len-1,controls);	//   drop d, on scare
		if (D && D->properties & VERB) return D->word;
        D = FindWord(word,len-2,controls);    //   drop ed
        if (D && D->properties & VERB) return D->word; //   found it
        D = FindWord(word,len-1,controls);    //   drop d
        if (D && D->properties & VERB) return D->word; //   found it
        if (prior1 == prior2  )   //   repeated consonant at end
        {
            D = FindWord(word,len-3,controls);    //   drop Xed
            if (D && D->properties & VERB) return D->word; //   found it
        }
        if (prior1 == 'i') //   ied came from y
        {
            word[len-3] = 'y'; //   change i to y
            D = FindWord(word,len-2,controls);    //   y, drop ed
            word[len-3] = 'i';
            if (D && D->properties & VERB) return D->word; //   found it
        }

		if (!buildDictionary && !nonew && !fullDictionary)
		{
			char wd[MAX_WORD_SIZE];
			strcpy(wd,word);
			if (len > 4 && !IsVowel(prior1) && IsVowel(prior2) && IsVowel(prior3))
			{
				wd[len-2] = 0;
			}
			else if (prior1 == prior2) // double last and add ed
			{
				wd[len-3] = 0;
			}
			else if (!IsVowel(prior2) && prior1 == 'i') // ied => y copied->copy
			{
				strcpy(wd+len-3,"y");
			}
			else if (!IsVowel(prior1) && IsVowel(prior2)) // Noted -> note 
			{
				wd[len-1] = 0;	// just chop off the s, leaving the e
			}
			else wd[len-2] = 0; // chop ed off
			return StoreWord(wd,VERB|VERB_PAST|VERB_PAST_PARTICIPLE|ADJECTIVE_PARTICIPLE|ADJECTIVE)->word;
		}
     }
   
    //   could this be a participle verb we dont know about?
    if (prior1 == 'i' && prior == 'n' && last == 'g' && len > 4)//   maybe verb participle
    {
        char word1[MAX_WORD_SIZE];
		verbFormat = VERB_PRESENT_PARTICIPLE;
 
        //   try removing doubled consonant
        if (len > 4 &&  word[len-4] == word[len-5])
        {
            D = FindWord(word,len-4,controls);    //   drop Xing spot consonant repeated
            if (D && D->properties & VERB) return D->word; //   found it
        }

        //   y at end, maybe came from ie
        if (word[len-4] == 'y')
        {
            strcpy(word1,word);
            word1[len-4] = 'i';
            word1[len-3] = 'e';
            word1[len-2] = 0;
            D = FindWord(word1,len-2,controls);    //   drop ing but add ie
            if (D && D->properties & VERB) return D->word; //   found it
        }

        //   two consonants at end, see if raw word is good priority over e added form
        if (len > 4 && !IsVowel(word[len-4]) && !IsVowel(word[len-5])) 
        {
            D = FindWord(word,len-3,controls);    //   drop ing
            if (D && D->properties & VERB) return D->word; //   found it
        }

        //   otherwise try stem with e after it
        strcpy(word1,word);
        word1[len-3] = 'e';
        word1[len-2] = 0;
        D = FindWord(word1,len-2,controls);    //   drop ing and put back 'e'
        if (D && D->properties & VERB) return D->word; //   found it

        //   simple ing added to word
        D = FindWord(word,len-3,controls);    //   drop ing
        if (D && D->properties & VERB) return D->word; //   found it

		if (!buildDictionary && !nonew)
		{
			char wd[MAX_WORD_SIZE];
			strcpy(wd,word);
			if (prior3 == prior2) // double last and add ing like swimming => swim
			{
				wd[len-4] = 0;
			}
			else wd[len-3] = 0; // chop ing off
			return StoreWord(wd,VERB|VERB_PRESENT_PARTICIPLE|NOUN_GERUND|NOUN)->word;
		}
	}
    //   ies from y
    if (prior1 == 'i' && prior == 'e' && last == 's' && len > 4)//   maybe verb participle
    {
 		verbFormat = VERB_PRESENT_3PS;
        char word1[MAX_WORD_SIZE];
        strcpy(word1,word);
        word1[len-3] = 'y';
        word1[len-2] = 0;
        D = FindWord(word1,len-2,controls);    //   drop ing, add e
        if (D && D->properties & VERB) return D->word; //   found it
	}

     //   unknown singular verb
    if (last == 's' && len > 3 && prior != 'u' && prior != '\'') // but should not be "us" ending (adjectives)
    {
 		verbFormat = VERB_PRESENT_3PS;
        D = FindWord(word,len-1,controls);    //   drop s
        if (D && D->properties & VERB && D->properties & VERB_INFINITIVE) return D->word; //   found it
		if (D && D->properties & NOUN) return NULL; //   dont move bees to be
        else if (prior == 'e')
        {
            D = FindWord(word,len-2,controls);    //   drop es
            if (D && D->properties & VERB) return D->word; //   found it
        }

		if (!buildDictionary && !nonew)
		{
			char wd[MAX_WORD_SIZE];
			strcpy(wd,word);
			if ( prior == 'e' && prior1 == 'i') // was toadies  from toady
			{
				strcpy(wd+len-3,"y");
			}
			else wd[len-1] = 0; // chop off tail s
			return StoreWord(wd,VERB_PRESENT_3PS|VERB|NOUN|NOUN_SINGULAR)->word;
		}
   }

    if (IsHelper(word)) 
	{
		verbFormat = 0;
		return word;
	}
	if ( nonew || buildDictionary ) return NULL;
	verbFormat = VERB_INFINITIVE;
	return InferVerb(word,len);
}

char* English_GetPluralNoun(WORDP noun)
{
	if (!noun) return NULL;
    if (noun->properties & NOUN_PLURAL) return noun->word; 
    WORDP plural = GetPlural(noun);
	if (noun->properties & (NOUN_SINGULAR|NOUN_PROPER_SINGULAR)) 
    {
        if (plural) return plural->word;
        static char word[MAX_WORD_SIZE];
		unsigned int len = noun->length;
		char end = noun->word[len-1];
		char before = (len > 1) ? (noun->word[len-2]) : 0;
		if (end == 's') sprintf(word,"%ses",noun->word); // glass -> glasses
		else if (end == 'h' && (before == 'c' || before == 's')) sprintf(word,"%ses",noun->word); // witch -> witches
		else if ( end == 'o' && !IsVowel(before)) sprintf(word,"%ses",noun->word); // hero -> heroes>
		else if ( end == 'y' && !IsVowel(before)) // cherry -> cherries
		{
			if (IsUpperCase(*noun->word)) sprintf(word,"%ss",noun->word); // Germany->Germanys
			else
			{
				strncpy(word,noun->word,len-1);
				strcpy(word+len-1,"ies"); 
			}
		}
		else sprintf(word,"%ss",noun->word);
        return word;
    }
    return noun->word;
}

static char* InferNoun(char* original,unsigned int len) // from suffix might it be singular noun? If so, enter into dictionary
{
	if (len == 0) len = strlen(original);
	char word[MAX_WORD_SIZE];
	strncpy(word,original,len);
	word[len] = 0;
	uint64 flags = ProbableNoun(original,len);

	// ings (plural of a gerund like paintings)
	if (len > 6 && !stricmp(word+len-4,"ings"))
	{
		StoreWord(word,NOUN|NOUN_PLURAL);
		word[len-1] = 0;
		return StoreWord(word,NOUN|NOUN_SINGULAR)->word; // return the singular form
	}
		
	// ves (plural form)
	if (len > 4 && !strcmp(word+len-3,"ves") && IsVowel(word[len-4])) // knife
	{
		//Plurals of words that end in -f or -fe usually change the f sound to a v sound and add s or -es.
		word[len-3] = 'f';
		char* singular = GetSingularNoun(word,false,false);
		if (singular && !stricmp(singular,word)) 
		{
			nounFormat = NOUN_PLURAL;
			return StoreWord(singular,NOUN|NOUN_SINGULAR)->word;
		}
		word[len-2] = 'e';
		singular = GetSingularNoun(word,false,true);
		if (singular && !stricmp(singular,word)) 
		{
			nounFormat = NOUN_PLURAL;
			return StoreWord(singular,NOUN|NOUN_SINGULAR)->word;
		}
	}

	if (flags) return StoreWord(word,flags)->word;

	if (strchr(word,'_')) return NULL;		// dont apply suffix to multiple word stuff

	if (IsUpperCase(*word)) return StoreWord(word,NOUN|NOUN_PROPER_SINGULAR)->word;

	// if word is an abbreviation it is a noun (proper if uppcase)
	if (strchr(word,'.')) return StoreWord(word,NOUN|NOUN_SINGULAR)->word;

	// hypenated word check word at end
	char* hypen = strchr(word+1,'-');
	if ( hypen && len > 2)
	{
		char* stem = GetSingularNoun(word+1,true,false);
		if (stem && !stricmp(stem,word+1)) return StoreWord(word,NOUN|NOUN_SINGULAR)->word;
		if (stem)
		{
			strcpy(word+1,stem);
			return StoreWord(word,NOUN|NOUN_SINGULAR)->word;
		}
	}
	return NULL;
}

uint64 ProbableNoun(char* original,unsigned int len) // from suffix might it be singular noun? 
{
	if (len == 0) len = strlen(original);
	char word[MAX_WORD_SIZE];
	strncpy(word,original,len);
	word[len] = 0;

	char* hyphen = strchr(original,'-');
	if (hyphen)
	{
		WORDP X = FindWord(hyphen+1,0,PRIMARY_CASE_ALLOWED);
		if (X && X->properties & NOUN) return X->properties & (NOUN_BITS|NOUN);
		uint64 flags = ProbableNoun(hyphen+1,len - (hyphen-original+1));
		if (flags) return flags;
	}
	
	char* item;
	char* test;
	int i;
	if (len >= 10) // word of 3 + suffix of 7
	{
		test = word+len-7;
		i = -1;
		while ((item = noun7[++i].word)) if (!stricmp(test,item)) return noun7[i].properties;
	}	
	if (len >= 9) // word of 3 + suffix of 6
	{
		test = word+len-6;
		i = -1;
		while ((item = noun6[++i].word)) if (!stricmp(test,item)) return noun6[i].properties;
	}	
	if (len >= 8) // word of 3 + suffix of 5
	{
		test = word+len-5;
		i = -1;
		while ((item = noun5[++i].word)) if (!stricmp(test,item)) return noun5[i].properties;
	}	
	if (len >= 7) // word of 3 + suffix of 4
	{
		test = word+len-4;
		i = -1;
		while ((item = noun4[++i].word)) if (!stricmp(test,item)) return noun4[i].properties;
	}
	if (len >= 6) // word of 3 + suffix of 3
	{
		test = word+len-3;
		i = -1;
		while ((item = noun3[++i].word)) if (!stricmp(test,item)) return noun3[i].properties;
	}
	if (len >= 5) // word of 3 + suffix of 2
	{
		test = word+len-2;
		i = -1;
		while ((item = noun2[++i].word)) if (!stricmp(test,item)) return noun2[i].properties;
	}

	// ings (plural of a gerund like paintings)
	if (len > 6 && !stricmp(word+len-4,"ings")) return NOUN|NOUN_SINGULAR;
		
	// ves (plural form)
	if (len > 4 && !strcmp(word+len-3,"ves") && IsVowel(word[len-4])) return NOUN|NOUN_PLURAL; // knife

	return 0;
}

char* English_GetSingularNoun(char* word, bool initial, bool nonew)
{ 
	uint64 controls = PRIMARY_CASE_ALLOWED;
	nounFormat = 0;
    if (!word) return NULL;
    size_t len = strlen(word);
    if (len == 0) return NULL;
    WORDP D = FindWord(word,0,controls);
	nounFormat = NOUN_SINGULAR;
	if (D && D->properties & NOUN_PROPER_SINGULAR) //   is already singular
	{
		nounFormat = NOUN_PROPER_SINGULAR;
		return D->word;
	}

    //   we know the noun and its plural, use singular
    if (D && D->properties & (NOUN_SINGULAR|NOUN_PROPER_SINGULAR|NOUN_NUMBER)) //   is already singular
    {
		nounFormat = D->properties & (NOUN_SINGULAR|NOUN_PROPER_SINGULAR|NOUN_NUMBER);
		if (word[len-1] == 's') // even if singular, if simpler exists, use that. Eg.,  military "arms"  vs "arm" the part
		{
			if (nonew)
			{
				WORDP E = FindWord(word,len-1,controls);
				if  (E && E->properties & NOUN) return E->word;
			}
			char* sing = InferNoun(word,len);
			if (sing) return sing;
		}

        //   HOWEVER, some singulars are plurals--- words has its own meaning as well
        unsigned int len = D->length;
        if (len > 4 && D->word[len-1] == 's')
        {
            WORDP F = FindWord(D->word,len-1,controls);
            if (F && F->properties & NOUN)  return F->word;  
        }
        return D->word; 
    }
	WORDP plural = (D) ? GetPlural(D) : NULL;
    if (D  && D->properties & NOUN_PLURAL && plural) 
	{
		nounFormat = NOUN_PLURAL;
		return plural->word; //   get singular form from plural noun
	}

	if (D && D->properties & NOUN && !(D->properties & NOUN_PLURAL) && !(D->properties & (NOUN_PROPER_SINGULAR|NOUN_PROPER_PLURAL))) return D->word; //   unmarked as plural, it must be singular unless its a name
    if (!D && IsNumber(word))  return word;
	if (D && D->properties & AUX_VERB) return NULL; // avoid "is" or "was" as plural noun

	// check known from plural s or es
	if (len > 2 && word[len-1] == 's')
	{
		char mod[MAX_WORD_SIZE];
		strcpy(mod,word);
		mod[len-1] = 0;
		char* singular = GetSingularNoun(mod,false,true);
		nounFormat = (IsUpperCase(*word)) ? NOUN_PROPER_PLURAL : NOUN_PLURAL; // would fail on iPhones BUG
		uint64 format = nounFormat; // would fail on iPhones BUG
		if (singular) return singular; // take off s is correct
		if (len > 3 && word[len-2] == 'e' && (word[len-3] == 'x' || word[len-3] == 's' || word[len-3] == 'h' || IsVowel(word[len-3]) )) // es is rarely addable (not from james) ,  tornado  and fish minus fox  allow it
		{
			mod[len-2] = 0;
			singular = GetSingularNoun(mod,false,true);
			nounFormat = format;
			if (singular) return singular; // take off es is correct
			//With words that end in a consonant and a y, you'll need to change the y to an i and add es
			if (len > 4 && mod[len-3] == 'i' && !IsVowel(mod[len-4]))
			{
				mod[len-3] = 'y';
				singular = GetSingularNoun(mod,false,true);
				nounFormat = format;
				if (singular) return singular; // take off ies is correct, change to y
			}

			//Plurals of words that end in -f or -fe usually change the f sound to a v sound and add s or -es.
			if (len > 4 && mod[len-3] == 'v')
			{
				mod[len-3] = 'f';
				singular = GetSingularNoun(mod,false,true);
				nounFormat = format;
				if (singular) return singular; // take off ves is correct, change to f
				mod[len-2] = 'e';
				singular = GetSingularNoun(mod,false,true);
				nounFormat = format;
				if (singular) return singular; // take off ves is correct, change to fe
			}
		}
	}
	if ( nonew || buildDictionary ) return NULL;

	nounFormat = (IsUpperCase(*word)) ? NOUN_PROPER_SINGULAR : NOUN_SINGULAR;
	return InferNoun(word,len);
}


static char* InferAdverb(char* original, unsigned int len) // might it be adverb based on suffix? if so, enter into dictionary
{
	char word[MAX_WORD_SIZE];
	if (len == 0) len = strlen(original);
	strncpy(word,original,len);
	word[len] = 0;
	uint64 flags = ProbableAdverb(original,len);
	
	//est
	if (len > 4 && !strcmp(word+len-3,"est"))
	{
		adjectiveFormat = MOST_FORM;
		WORDP E = StoreWord(word,ADVERB|ADVERB_NORMAL,MOST_FORM);
		WORDP base = E;
		if (word[len-3] == word[len-4])  // doubled consonant
		{
			char word1[MAX_WORD_SIZE];
			strcpy(word1,word);
			word1[len-2] = 0;
			base =  StoreWord(word,ADVERB|ADVERB_NORMAL,0);
			SetComparison(E,MakeMeaning(base));
			SetComparison(base,MakeMeaning(E));
		}
		return base->word;
	}
	//er
	if (len > 4 && !strcmp(word+len-2,"er")  )
	{
		adjectiveFormat = MORE_FORM;
		WORDP E = StoreWord(word,ADVERB|ADVERB_NORMAL,MORE_FORM);
		WORDP base = E;
		if (word[len-3] == word[len-4]) 
		{
			char word1[MAX_WORD_SIZE];
			strcpy(word1,word);
			word1[len-2] = 0;
			base =  StoreWord(word,ADVERB,0);
			SetComparison(E,MakeMeaning(base));
			SetComparison(base,MakeMeaning(E));
		}
		return base->word;
	}
	if (flags) return StoreWord(word,flags)->word;

	return NULL;
}

uint64 ProbableAdverb(char* original, unsigned int len) // might it be adverb based on suffix? 
{
	char word[MAX_WORD_SIZE];
	if (len == 0) len = strlen(original);
	strncpy(word,original,len);
	word[len] = 0;
	
	char* hyphen = strchr(original,'-');
	if (hyphen)
	{
		WORDP X = FindWord(hyphen+1,0,PRIMARY_CASE_ALLOWED);
		if (X && X->properties & ADVERB) return X->properties & (ADVERB_BITS|ADVERB);
		uint64 flags = ProbableVerb(hyphen+1,len - (hyphen-original+1));
		if (flags) return flags;
	}


	char* item;
	char* test;
	int i;
	if (len >= 8) // word of 3 + suffix of 5
	{
		test = word+len-5;
		i = -1;
		while ((item = adverb5[++i].word)) if (!stricmp(test,item)) return adverb5[i].properties;
	}
	if (len >= 7) // word of 3 + suffix of 4
	{
		test = word+len-4;
		i = -1;
		while ((item = adverb4[++i].word)) if (!stricmp(test,item)) return adverb4[i].properties;
	}
	if (len >= 6) // word of 3 + suffix of 3
	{
		test = word+len-3;
		i = -1;
		while ((item = adverb3[++i].word)) if (!stricmp(test,item)) return adverb3[i].properties;
	}
	if (len >= 5) // word of 3 + suffix of 2
	{
		test = word+len-2;
		i = -1;
		while ((item = adverb2[++i].word)) if (!stricmp(test,item)) return adverb2[i].properties;
	}

	//est
	if (len > 4 && !strcmp(word+len-3,"est")) return ADVERB|ADVERB_NORMAL;
	//er
	if (len > 4 && !strcmp(word+len-2,"er")  ) return ADVERB|ADVERB_NORMAL;
	return 0;
}

char* English_GetAdverbBase(char* word, bool nonew)
{
	uint64 controls = tokenControl & STRICT_CASING ? PRIMARY_CASE_ALLOWED : LOWERCASE_LOOKUP;
 	adverbFormat = 0;
    if (!word) return NULL;
	if (IsUpperCase(*word)) return NULL; // not as proper
    size_t len = strlen(word);
    if (len == 0) return NULL;
    char lastc = word[len-1];  
    char priorc = (len > 2) ? word[len-2] : 0; 
    char prior2c = (len > 3) ? word[len-3] : 0; 
    WORDP D = FindWord(word,0,controls);
	adverbFormat = 0;
    if (D && D->properties &  QWORD) return D->word; //   we know it as is
	if (D && D->properties & ADVERB_NORMAL && !(D->systemFlags & (MORE_FORM|MOST_FORM) )) return D->word; //   we know it as is

	if (D && D->properties & ADVERB)
    {
        int n = 5;
		WORDP original = D;
        while (--n  && D)
        {
            D = GetComparison(D);
            if (D && !(D->systemFlags & (MORE_FORM|MOST_FORM))) 
			{
				if (original->systemFlags & MORE_FORM) adjectiveFormat = MOST_FORM;
				else if (original->systemFlags & MOST_FORM) adjectiveFormat = MOST_FORM;
				return D->word;
			}
        }
    }
 
    if (len > 4 && priorc == 'l' && lastc == 'y')
    {
		char form[MAX_WORD_SIZE];
        D = FindWord(word,len-2,controls); // rapidly
        if (D && D->properties & (VERB|ADJECTIVE)) return D->word;
		if (prior2c == 'i')
		{
			D = FindWord(word,len-3,controls); // lustily
			if (D && D->properties & (VERB|ADJECTIVE)) return D->word;
			// if y changed to i, change it back
			strcpy(form,word);
			form[len-3] = 'y';
			form[len-2] = 0;
			D = FindWord(word,len-2,controls); // happily  from happy
			if (D && D->properties & (VERB|ADJECTIVE)) return D->word;
		}
		// try terrible -> terribly
		strcpy(form,word);
		form[len-1] = 'e';
 		D = FindWord(word,len-2,controls); // happily  from happy
		if (D && D->properties & (VERB|ADJECTIVE)) return D->word;
    }
	if (len >= 5 && priorc == 'e' && lastc == 'r')
    {
        D = FindWord(word,len-2,controls);
        if (D && D->properties & ADVERB) 
		{
			adverbFormat = MORE_FORM;
			return D->word;
		}
    }
	if (len > 5 && prior2c == 'e' && priorc == 's' && lastc == 't')
    {
        D = FindWord(word,len-3,controls);
        if (D && D->properties & ADVERB) 
		{
			adverbFormat = MOST_FORM;
			return D->word;
		}
    }
	if ( nonew || buildDictionary) return NULL;
	
	return InferAdverb(word,len);
}


static char* InferAdjective(char* original, unsigned int len) // might it be adjective based on suffix? If so, enter into dictionary
{
	char word[MAX_WORD_SIZE];
	if (len == 0) len = strlen(original);
	strncpy(word,original,len);
	word[len] = 0;

	// est -  comparative
	if (len >= 4 &&  !strcmp(word+len-3,"est")  )
	{
		adjectiveFormat = MOST_FORM;
		WORDP E = StoreWord(word,ADJECTIVE|ADJECTIVE_NORMAL,MOST_FORM);
		WORDP base = E;
		if (word[len-4] == word[len-5]) 
		{
			char word1[MAX_WORD_SIZE];
			strcpy(word1,word);
			word1[len-3] = 0;
			base =  StoreWord(word,ADJECTIVE|ADJECTIVE_NORMAL,0);
			SetComparison(E,MakeMeaning(base));
			SetComparison(base,MakeMeaning(E));
		}
		
		return base->word;
	}

	// er -  comparative
	if (len >= 4 &&  !strcmp(word+len-2,"er")  )
	{
		adjectiveFormat = MORE_FORM;
		WORDP E = StoreWord(word,ADJECTIVE|ADJECTIVE_NORMAL,MORE_FORM);
		WORDP base = E;
		if (word[len-3] == word[len-4]) 
		{
			char word1[MAX_WORD_SIZE];
			strcpy(word1,word);
			word1[len-2] = 0;
			base =  StoreWord(word,ADJECTIVE|ADJECTIVE_NORMAL,0);
			SetComparison(E,MakeMeaning(base));
			SetComparison(base,MakeMeaning(E));
		}
		return base->word;
	}
	
	uint64 flags = ProbableAdjective(original,len);
	if (flags) return StoreWord(word,flags)->word;

	return 0;
}

uint64 ProbableAdjective(char* original, unsigned int len) // probable adjective based on suffix?
{
	char word[MAX_WORD_SIZE];
	if (len == 0) len = strlen(original);
	strncpy(word,original,len);
	word[len] = 0;
		
	char* hyphen = strchr(original,'-');
	if (hyphen)
	{
		WORDP X = FindWord(hyphen+1,0,PRIMARY_CASE_ALLOWED);
		if (X && X->properties & ADJECTIVE) return X->properties & (ADJECTIVE_BITS|ADJECTIVE);
		uint64 flags = ProbableVerb(hyphen+1,len - (hyphen-original+1));
		if (flags) return flags;

		if (!strcmp(hyphen,"-looking" )) return ADJECTIVE|ADJECTIVE_NORMAL; // good-looking gross-looking
		if (!strcmp(hyphen,"-old" )) return ADJECTIVE|ADJECTIVE_NORMAL; // centuries-old
	}

	int i;
	char* test;
	char* item;
	if (len >= 10) // word of 3 + suffix of 7
	{
		test = word+len-7;
		i = -1;
		while ((item = adjective7[++i].word)) if (!stricmp(test,item)) return adjective7[i].properties;
	}
	if (len >= 9) // word of 3 + suffix of 6
	{
		test = word+len-6;
		i = -1;
		while ((item = adjective6[++i].word)) if (!stricmp(test,item)) return adjective6[i].properties;
	}
	if (len >= 8) // word of 3 + suffix of 5
	{
		test = word+len-5;
		i = -1;
		while ((item = adjective5[++i].word)) if (!stricmp(test,item)) return adjective5[i].properties;
	}
	if (len >= 7) // word of 3 + suffix of 4
	{
		test = word+len-4;
		i = -1;
		while ((item = adjective4[++i].word)) if (!stricmp(test,item)) return adjective4[i].properties;
	}
	if (len >= 6) // word of 3 + suffix of 3
	{
		test = word+len-3;
		i = -1;
		while ((item = adjective3[++i].word)) if (!stricmp(test,item)) return adjective3[i].properties;
	}
	if (len >= 5) // word of 3 + suffix of 2
	{
		test = word+len-2;
		i = -1;
		while ((item = adjective2[++i].word)) if (!stricmp(test,item)) return adjective2[i].properties;
	}
	if (len >= 4) // word of 3 + suffix of 1
	{
		test = word+len-1;
		i = -1;
		while ((item = adjective1[++i].word)) if (!stricmp(test,item)) return adjective1[i].properties;
	}

	// est -  comparative
	if (len >= 4 &&  !strcmp(word+len-3,"est")  ) return ADJECTIVE|ADJECTIVE_NORMAL;

	// er -  comparative
	if (len >= 4 &&  !strcmp(word+len-2,"er")  ) return ADJECTIVE|ADJECTIVE_NORMAL;

	return 0;
}


char* English_GetAdjectiveBase(char* word, bool nonew)
{
	uint64 controls = tokenControl & STRICT_CASING ? PRIMARY_CASE_ALLOWED : LOWERCASE_LOOKUP;
	adjectiveFormat = 0;
    size_t len = strlen(word);
    if (len == 0) return NULL;
    WORDP D = FindWord(word,0,controls);
	char lastc = word[len-1];  
    char priorc = (len > 2) ? word[len-2] : 0;  //   Xs
    char priorc1 = (len > 3) ? word[len-3] : 0; //   Xes
    char priorc2 = (len > 4) ? word[len-4] : 0; //   Xhes
    char priorc3 = (len > 5) ? word[len-5] : 0; //   Xgest
 
    if (D && D->properties & ADJECTIVE && !(D->systemFlags & (MORE_FORM|MOST_FORM)))
	{
		adjectiveFormat = 0;
		return D->word; //   already base
	}
    if (D && D->properties & ADJECTIVE)
    {
        int n = 5;
		WORDP original = D;
        while (--n  && D)
        {
            D = GetComparison(D);
            if (D && !(D->systemFlags & (MORE_FORM|MOST_FORM))) // this is the base
			{
				if (original->systemFlags & MORE_FORM) adjectiveFormat = MORE_FORM;
				else if (original->systemFlags & MOST_FORM) adjectiveFormat = MOST_FORM;
				return D->word;
			}
        }
    }
 
    //   see if composite
    char composite[MAX_WORD_SIZE];
    strcpy(composite,word);
    char* hyphen = strchr(composite+1,'-');
    if (hyphen)
    {
        hyphen -= 4;
		char* althyphen = (hyphen - composite) + word;
        if (hyphen[2] == 'e' && hyphen[3] == 'r') //   lower-density?
        {
            strcpy(hyphen+2,althyphen+4); //   remove er
            char* answer = GetAdjectiveBase(composite,false);
            if (answer) return answer;
        }
        if (hyphen[1] == 'e' && hyphen[2] == 's' && hyphen[2] == 't' ) //   lowest-density?
        {
            strcpy(hyphen+1,althyphen+4); //   remove est
            char* answer = GetAdjectiveBase(composite,false);
            if (answer) return answer;
        }
    }

    if (len > 4 && priorc == 'e' && lastc == 'r') //   more
    {
		 adjectiveFormat = MORE_FORM;
         D = FindWord(word,len-2,controls);
         if (D && D->properties & ADJECTIVE) return D->word; //   drop er
         D = FindWord(word,len-1,controls);
         if (D && D->properties & ADJECTIVE) return D->word; //   drop e (already ended in e)

         if (priorc1 == priorc2  )  
         {
            D = FindWord(word,len-3,controls);
            if (D && D->properties & ADJECTIVE) return D->word; //   drop Xer
         }
         if (priorc1 == 'i') //   changed y to ier?
         {
            word[len-3] = 'y';
            D = FindWord(word,len-2,controls);
            word[len-3] = 'i';
            if (D && D->properties & ADJECTIVE) return D->word; //   drop Xer
          }
	}  
	else if (len > 5 && priorc1 == 'e' &&  priorc == 's' && lastc == 't') //   most
    {
		adjectiveFormat = MOST_FORM;
        D = FindWord(word,len-3,controls);//   drop est
        if (D && D->properties & ADJECTIVE) return D->word; 
        D = FindWord(word,len-2,controls);//   drop st (already ended in e)
        if (D && D->properties & ADJECTIVE) return D->word; 
        if (priorc2 == priorc3  )   
        {
             D = FindWord(word,len-4,controls);//   drop Xest
             if (D && D->properties & ADJECTIVE) return D->word; 
        }
        if (priorc2 == 'i') //   changed y to iest
        {
             word[len-4] = 'y';
             D = FindWord(word,len-3,controls); //   drop est
             word[len-4] = 'i';
             if (D && D->properties & ADJECTIVE) return D->word; 
        }   
    }
	if ( nonew || buildDictionary) return NULL;
	
	return InferAdjective(word,len);
}
