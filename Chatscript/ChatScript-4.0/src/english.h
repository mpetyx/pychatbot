#ifndef _ENGLISHH_
#define _ENGLISHH_
#ifdef INFORMATION
Copyright (C) 2012 by Bruce Wilcox

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#endif

#define GetAdjectiveBase(word,nonew) (*P_GetAdjectiveBase)(word,nonew)
#define GetAdverbBase(word,nonew) (*P_GetAdverbBase)(word,nonew)
#define GetPastTense(word) (*P_GetPastTense)(word)
#define GetPastParticiple(word) (*P_GetPastParticiple)(word)
#define GetPresentParticiple(word) (*P_GetPresentParticiple)(word)
#define GetThirdPerson(word) (*P_GetThirdPerson)(word)
#define GetInfinitive(word,nonew) (*P_GetInfinitive)(word,nonew)
#define GetSingularNoun(word,initial,nonew) (*P_GetSingularNoun)(word,initial,nonew)
#define GetPluralNoun(word) (*P_GetPluralNoun)(word)
#define SetSentenceTense(start,end) (*P_SetSentenceTense)(start,end)

#define MAX_CLAUSES 25

extern unsigned int posTiming;
extern unsigned char quotationInProgress;
extern unsigned int roleIndex;
extern unsigned int needRoles[MAX_CLAUSES]; 
extern unsigned char verbStack[MAX_CLAUSES];
void SetRole(unsigned int i, uint64 role, bool revise = false, unsigned int currentVerb = verbStack[roleIndex]);

uint64 GetPosData(unsigned int at, char* original,WORDP &entry,WORDP &canonical,uint64 &sysflags,uint64 &cansysflags, bool firstTry = true,bool nogenerate = false,unsigned int start = 0);
char* English_GetAdjectiveBase(char* word,bool nonew);
char* English_GetAdverbBase(char* word,bool nonew);
char* English_GetPastTense(char* word);
char* English_GetPastParticiple(char* word);
char* English_GetPresentParticiple(char* word);
char* English_GetThirdPerson(char* word);
char* English_GetInfinitive(char* word,bool nonew);
char* English_GetSingularNoun(char* word,bool initial,bool nonew);
char* English_GetPluralNoun(WORDP noun);
void English_SetSentenceTense(unsigned int start, unsigned int end);
uint64 ProbableAdjective(char* original, unsigned int len);
uint64 ProbableAdverb(char* original, unsigned int len);
uint64 ProbableNoun(char* original,unsigned int len);
uint64 ProbableVerb(char* original,unsigned int len);
bool IsDeterminedNoun(unsigned int i,unsigned int& det);
#endif
