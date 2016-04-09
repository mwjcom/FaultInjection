#ifdef WIN32
#ifndef REG_SHOT_H
#define REG_SHOT_H

#include <windows.h>
#include <shlobj.h>

//
#define WIN32_LEAN_AND_MEAN 
//#pragma comment(linker,"/FILEALIGN:0x200 /MERGE:.data=.text /MERGE:.rdata=.text /SECTION:.text,EWR /IGNORE:4078")

//#define DEBUGLOG
#define SIZEOFREG_DWORD	4		//In current windows ,reg_dword's size =4 
#define NOTMATCH		0		//Define modification type in compare results
#define ISMATCH			1
#define ISDEL			2
#define ISADD			3
#define ISMODI			4

#define KEYADD			1
#define KEYDEL			2
#define VALADD			3
#define VALDEL			4
#define VALMODI			5
#define FILEADD			6
#define FILEDEL			7
#define FILEMODI		8
#define DIRADD			9
#define DIRDEL			10
#define DIRMODI			11

#define REFRESHINTERVAL	110	//Define progress refresh rate
#define MAXPBPOSITION	100		//Define progress bar length
#define COMMENTLENGTH	50		//Define commentfield length on the MainForm
#define HTMLWRAPLENGTH	1000	//Define html out put wrap length
#define MAXAMOUNTOFFILE	10000	//Define out put file counts
#define	EXTDIRLEN	MAX_PATH*3	//Define Searching Directory field length
#define COMPUTERNAMELEN	64		//Define COMPUTER name length
#define HIVEBEGINOFFSET	512		//Hive file out put header computerlen*2+sizeof(systemtime)+32 must <hivebeginoffset!!!!!!!!!!!!!!


////Some definations of MutiLanguage strings [Free space length]
//#define SIZEOF_LANGUAGE_SECTIONNAMES_BUFFER 2048
//#define SIZEOF_SINGLE_LANGUAGENAME	64
//#define SIZEOF_FREESTRINGS 16384
//#define SIZEOF_ABOUTBOX 2048
//
//
//This is main struct I used for Windows Registry Key 
struct	_KEYCONTENT
{					
	LPSTR	lpkeyname;							//Pointer to Key Name
	struct	_VALUECONTENT FAR * lpfirstvalue;	//Pointer to Key's first Value
	struct	_KEYCONTENT	FAR * lpfirstsubkey;	//Pointer to Key's first Sub Key
	struct	_KEYCONTENT	FAR * lpbrotherkey;		//Pointer to Key's brother
	struct	_KEYCONTENT	FAR * lpfatherkey;		//Pointer to Key's father
	BYTE	bkeymatch;							//Flag used at comparing

};

//This is main struct I used for Windows Registry Value
struct	_VALUECONTENT
{
	DWORD	typecode;							//Type of Value [DWORD,STRING...]
	DWORD	datasize;							//Value Data size in bytes
	LPSTR	lpvaluename;						//Pointer to Value Name
	LPBYTE	lpvaluedata;						//Pointer to Value Data
	struct	_VALUECONTENT FAR * lpnextvalue;	//Pointer to Value's brother
	struct	_KEYCONTENT	FAR * lpfatherkey;		//Pointer to Value's father[Key]
	BYTE	bvaluematch;						//Flag used at comparing
};

//This is main struct I used for Windows File System
struct	_FILECONTENT
{
	LPSTR	lpfilename;							//Pointer to File Name
	DWORD	writetimelow;						//File write time [LOW  DWORD]
	DWORD	writetimehigh;						//File write time [HIGH DWORD]
	DWORD	filesizelow;						//File size	 [LOW  DWORD]
	DWORD	filesizehigh;						//File size	 [HIGH DWORD]
	DWORD	fileattr;							//File Attributes
	struct	_FILECONTENT FAR * lpfirstsubfile;	//Pointer to Files[DIRS] first sub file
	struct	_FILECONTENT FAR * lpbrotherfile;	//Pointer to Files[DIRS] brother
	struct	_FILECONTENT FAR * lpfatherfile;	//Pointer to Files father
	BYTE	bfilematch;							//Flag used at comparing
};

// This is the struct I use for compare result  Output 
struct  _COMRESULT
{
	LPSTR	lpresult;							//Pointer to result string
	struct	_COMRESULT FAR * lpnextresult;		//Pointer to next _COMRESULT
};

typedef struct _KEYCONTENT KEYCONTENT,FAR * LPKEYCONTENT;
typedef struct _VALUECONTENT VALUECONTENT,FAR * LPVALUECONTENT;
typedef struct _COMRESULT COMRESULT,FAR * LPCOMRESULT;
typedef struct _FILECONTENT FILECONTENT,FAR * LPFILECONTENT;

//That is the struct I use for file tree compare
struct	_HEADFILE
{
	LPFILECONTENT	lpHeadFile1;				//Pointer to header file at 1st shot
	LPFILECONTENT	lpHeadFile2;				//Pointer to header file at 2nd shot
	struct _HEADFILE	FAR *	lpnextheadfile;	//Pointer to next headfile struc
};
typedef	struct	_HEADFILE	HEADFILE,FAR * LPHEADFILE;

void init_shot();

VOID *RegistrySnap(HKEY	hkey,LPKEYCONTENT	lpFatherKeyContent);
void FreeAllKey(LPKEYCONTENT lpKey);
void FreeAllCompareResults(void);
void CancelSnap(LPKEYCONTENT lpKey1, LPKEYCONTENT lpKey2);

BOOL RegCompare(LPKEYCONTENT lpKey1, LPKEYCONTENT lpKey2);

#endif
#endif