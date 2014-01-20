#include "common.h"

#define DEFAULT_USER_CACHE 50000

#define NO_CACHEID -1

static unsigned int cacheHead = 0;
static unsigned int* cacheIndex = 0;
char* cacheBase = 0;

int currentCache = NO_CACHEID;
unsigned int userCacheCount = 1; // holds 1 users
unsigned int userCacheSize = DEFAULT_USER_CACHE;
int volleyLimit =  -1; // default save user records to file every n volley

void InitCache(unsigned int dictStringSize)
{
	cacheBase = (char*) malloc(dictStringSize + userTopicStoreSize + userTableSize );
	if (!cacheBase)
	{
		printf("Out of  memory space for dictionary w user cache %d %d %d %d\r\n",dictStringSize,userTopicStoreSize,userTableSize,MAX_ENTRIES);
		ReportBug("Cannot allocate memory space for dictionary %ld\r\n",(long int)(dictStringSize + userTopicStoreSize))
		myexit("out of memory space for dictionary to allocate");
	}
	cacheIndex = (unsigned int*) (cacheBase + userTopicStoreSize);
	char* ptr = cacheBase;
	for (unsigned int i = 0; i < userCacheCount; ++i) 
	{
		*ptr = 0; // item is empty
		ptr += userCacheSize;
		cacheIndex[PRIOR(i)] = i - 1; // before ptr
		cacheIndex[NEXT(i)] = i + 1; // after ptr
	}
	cacheIndex[PRIOR(0)] = userCacheCount-1; // last one as prior
	cacheIndex[NEXT(userCacheCount-1)] = 0;	// start as next one (circular list)
}

void CloseCache()
{
	free(cacheBase);
	cacheBase = NULL;
}

bool WriteCache(unsigned int which,size_t size)
{
	char* ptr = GetCacheBuffer(which);
	if (!*ptr) return false;	// nothing to write out
	if (size == 0) // request to compute size
	{
		size = strlen(ptr) + 1; // login string
		size += strlen(ptr+size);
	}
	FILE* out = FopenWrite(ptr,"wb");
	if (!out) // see if we can create the directory (assuming its missing)
	{
#ifdef WIN32
		system("mkdir USERS");
		out = FopenUTF8Write(ptr);
#endif
		if (!out) 
		{
			ReportBug("cannot open user state file %s to write\r\n",ptr)
			return false;
		}
	}

#ifdef LOCKUSERFILE
#ifdef LINUX
	if (server)
	{
        int fd = fileno(out);
        if (fd < 0) 
		{
			fclose(out);
			return false;
        }

        struct flock fl;
        fl.l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK	*/
        fl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
        fl.l_start  = 0;	/* Offset from l_whence         */
        fl.l_len    = 0;	/* length, 0 = to EOF           */
        fl.l_pid    = getpid(); /* our PID                      */
        if (fcntl(fd, F_SETLKW, &fl) < 0) 
		{
             fclose(out);
             return false;
        }
	}
#endif
#endif

	fwrite(ptr,1,size,out);
	fclose(out);
	cacheIndex[TIMESTAMP(which)] &= 0x00ffffff;	// clear volley count since last written
	return true;
}

void FlushCache()
{
	unsigned int start = cacheHead;
	while (WriteCache(start,0))
	{
		if (cacheIndex[NEXT(start)] == cacheHead) break;	// end of loop
	}
}

static char* GetFreeCache()
{
	if (!userCacheCount) return NULL;
	unsigned int duration = (clock() / 	CLOCKS_PER_SEC) - startSystem; // how many seconds since start of program launch
	// need to find a cache that's free else flush oldest one
	unsigned int last = cacheIndex[PRIOR(cacheHead)];	// PRIOR ptr of list start
	char* ptr = GetCacheBuffer(last);
	if (*ptr) 
	{
		WriteCache(last,0);  // there are none free, we have to use the last (not allowed to fail to get a buffer)
		*ptr = 0; // clear cache so it can be reused
	}
	cacheIndex[TIMESTAMP(last)] = duration; // includes 0 volley count and when this was allocated
	currentCache = cacheHead = last; // just rotate the ring  has more than one block
	return ptr;
}

void FreeUserCache()
{
	if (currentCache != NO_CACHEID )
	{
		char* ptr = GetCacheBuffer(currentCache);
		*ptr = 0;
		cacheHead = cacheIndex[NEXT(currentCache)];
		currentCache = NO_CACHEID;
	}
}

void FreeAllUserCaches()
{
	FlushCache();
	unsigned int start = cacheHead;
	while (userCacheCount)
	{
		start = cacheIndex[NEXT(start)];
		char* ptr = GetCacheBuffer(start);
		*ptr = 0;
		if (start == cacheHead) break;	// end of loop
	}
	currentCache = NO_CACHEID;
}

char* FindUserCache(char* word)
{
	// already in cache?
	unsigned int start = cacheHead;
	while (userCacheCount)
	{
		char* ptr = GetCacheBuffer(start);
		if (!stricmp(ptr,word)) // this is the user
		{
			if (start != cacheHead) // make him FIRST on the list
			{
				unsigned int prior = cacheIndex[PRIOR(start)];
				unsigned int next = cacheIndex[NEXT(start)];
				// decouple from where it is
				cacheIndex[NEXT(prior)] = next;
				cacheIndex[PRIOR(next)] = prior;

				// now insert in front
				prior = cacheIndex[PRIOR(cacheHead)];
				cacheIndex[PRIOR(cacheHead)] = start;

				cacheIndex[NEXT(prior)] = start;
				cacheIndex[PRIOR(start)] = prior;
				cacheIndex[NEXT(start)] = cacheHead;
			}
			currentCache = cacheHead = start;
			return ptr;
		}
		start = cacheIndex[NEXT(start)];
		if (start == cacheHead) break;	// end of loop
	}
	return NULL;
}

char* GetFileRead(char* user,char* computer)
{
	char word[MAX_WORD_SIZE];
	sprintf(word,"USERS/%stopic_%s_%s.txt",GetUserPath(loginID),user,computer);
	char* buffer = FindUserCache(word); // sets currentCache and makes it first if non-zero return
	if (buffer) return buffer;

	// have to go read it
    FILE* in = FopenReadWritten(word); // user topic file
	buffer = GetFreeCache(); // get cache buffer 

#ifdef LOCKUSERFILE
#ifdef LINUX
	if (server && in)
	{
		int fd = fileno(in);
		if (fd < 0) 
		{
		    fclose(in);
			in = 0;
		}
		else
		{
			struct flock fl;
			fl.l_type   = F_RDLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK	*/
			fl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
			fl.l_start  = 0;	/* Offset from l_whence		*/
			fl.l_len    = 0;	/* length, 0 = to EOF		*/
			fl.l_pid    = getpid(); /* our PID			*/
			if (fcntl(fd, F_SETLKW, &fl) < 0) 
			{
				fclose(in);
				in = 0;
			}
		}
	}
#endif
#endif

	if (in) // read in data if file exists
	{
		fseek(in, 0, SEEK_END);
		unsigned int actualSize = (unsigned int) ftell(in);
		if ((int)actualSize != -1) 
		{
			fseek(in, 0, SEEK_SET);
			size_t readit = fread(buffer,1,actualSize,in);	// read it all in
			buffer[readit] = 0;
			if (readit != actualSize) *buffer = 0; // read failure
		}
		fclose(in);
	}
	return buffer;
}

char* GetCacheBuffer(int which)
{
	return (which < 0) ? GetFreeCache() : (cacheBase+(which * userCacheSize)); // NOT from cache system, get a cache buffer
}

void Cache(char* buffer, size_t size) // save into cache
{
	unsigned int duration = (clock() / 	CLOCKS_PER_SEC) - startSystem; // how many seconds since start of program launch

	// dont want to overflow 24bit second store... so reset system start if needed
	if ( duration > 0x000fffff) 
	{
		startSystem = clock() / CLOCKS_PER_SEC; 
		duration = 0; // how many seconds since start of program launch
		// allow all in cache to stay longer
		for (unsigned int i = 0; i < userCacheCount; ++i) cacheIndex[TIMESTAMP(i)] = duration; 
	}

	// write out users that haven't been saved recently
	unsigned int volleys = ( cacheIndex[TIMESTAMP(currentCache)] >> 24) + 1; 
	cacheIndex[TIMESTAMP(currentCache)] = duration | (volleys << 24); 
	if ( (int)volleys >= volleyLimit ) WriteCache(currentCache,size); // writecache clears the volley count
	if (!volleyLimit) FreeUserCache(); // force reload each time

	currentCache = NO_CACHEID;
}
