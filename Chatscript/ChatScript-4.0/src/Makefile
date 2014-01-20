UNAME := $(shell uname)
DEBUG=-g

CFLAGS=-c -Wall -O2 -Wno-write-strings -Wno-char-subscripts -Wno-strict-aliasing
CC=g++

ifeq ($(UNAME), Linux)
LDFLAGS=-pthread -lrt
endif

ifeq ($(UNAME), Darwin)
LDFLAGS=-mmacosx-version-min=10.7 -pthread
CFLAGS=-mmacosx-version-min=10.7 -c -Wall -O2 -Wno-write-strings -Wno-char-subscripts -funsigned-char
endif

INCLUDEDIRS=
SOURCES=constructCode.cpp evserver.cpp csocket.cpp cs_ev.c debugger.cpp dictionarySystem.cpp englishTagger.cpp factSystem.cpp  functionExecute.cpp english.cpp infer.cpp mainSystem.cpp markSystem.cpp os.cpp outputSystem.cpp patternSystem.cpp readrawdata.cpp scriptCompile.cpp spellcheck.cpp systemVariables.cpp tagger.cpp testing.cpp textUtilities.cpp tokenSystem.cpp topicSystem.cpp userCache.cpp userSystem.cpp variableSystem.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ChatScript
EXECUTABLE_CLIENT=ChatScript-client

all: $(SOURCES) binary

client: EXECUTABLE=$(EXECUTABLE_CLIENT)
client: all

server: DEFINES+= -DUSERPATHPREFIX=1 -DLOCKUSERFILE=1 -DEVSERVER=1 -DEVSERVER_FORK=1
server: INCLUDEDIRS=-I../lib/libev
server: all

clean:
	-rm -f $(EXECUTABLE)
	-rm -f ../$(EXECUTABLE)
	-rm -f $(EXECUTABLE_CLIENT)
	-rm -f ../$(EXECUTABLE_CLIENT)
	-rm -f *.o

binary: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(DEFINES) $(INCLUDEDIRS) -o $(EXECUTABLE)
	cp $(EXECUTABLE) ..

.cpp.o:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDEDIRS) $< -o $@
