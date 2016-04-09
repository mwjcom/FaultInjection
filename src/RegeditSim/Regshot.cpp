////Download by http://www.NewXing.com
///*************************************************
// * Chinese notes about the source code release   *
// * [Sorry for the non-Chinese reader,I must use  *
// * my mother language to explain something ]     *
// * ***********************************************
// 
// 
// * TiANWEi at http://www.digitalnuke.com/
// 
// * Thanks to all of you!
// 
///*************************************************
// *  RegShot Win9x,NT Registry compare tool       *
// *  http://regshot.yeah.net  spring_w@163.com    *
// *   latest version: 1.61 2002/03/30             *
// *************************************************
// Regshot is a free small utility that can compare
// registry changes.The tech of RegShot is based on 
// Windows Registry APIs:
// RegOpenKeyEx()
// RegEnumValue()
// RegEnumKeyEx()
// RegQureyInfoKey() ...
//
// The most important routines in Regshot are :
// CompareFirstSubKey()
// GetRegistrySnap() ...
//
// For the multilanguage to work prop,the definations
// IN resource.h must be in their order
// 
//
// ==============
// Tab size=4
// Indent size=4
// Keep tabs
// ==============
// 
// The shortcomings are listed at the end of this DOC.
//*/
//
#include "RegShot.h"
#include <string>
//#include <QtGui/QApplication>

#ifdef WIN32




MSG				msg;							//Windows MSG struct
HWND			hWnd;							//The handle of REGSHOT
HMENU			hMenu,hMenuClear;				//The handles of shortcut menus
HANDLE			hFile,hFileWholeReg;			//Handle of file regshot use
HCURSOR			hHourGlass;						//Handle of cursor
HCURSOR			hSaveCursor;					//Handle of cursor
BOOL			is1;							//Flag to determine is the 1st shot
BOOL			is1LoadFromHive,is2LoadFromHive;//Flag to determine are shots load from hive files
RECT			rect;							//Window RECT
FILETIME		ftLastWrite;					//Filetime struct
BROWSEINFO		BrowseInfo1;					//BrowseINFO struct
OPENFILENAME	opfn;							//Openfilename struct

//Pointers to compare result [see above.]
LPCOMRESULT	lpKEYADD;							
LPCOMRESULT	lpKEYDEL;
LPCOMRESULT	lpVALADD;
LPCOMRESULT	lpVALDEL;
LPCOMRESULT	lpVALMODI;
LPCOMRESULT	lpFILEADD;
LPCOMRESULT	lpFILEDEL;
LPCOMRESULT	lpFILEMODI;
LPCOMRESULT	lpDIRADD;
LPCOMRESULT	lpDIRDEL;
LPCOMRESULT	lpDIRMODI;


LPCOMRESULT	lpKEYADDHEAD;
LPCOMRESULT	lpKEYDELHEAD;
LPCOMRESULT	lpVALADDHEAD;
LPCOMRESULT	lpVALDELHEAD;
LPCOMRESULT	lpVALMODIHEAD;
LPCOMRESULT	lpFILEADDHEAD;
LPCOMRESULT	lpFILEDELHEAD;
LPCOMRESULT	lpFILEMODIHEAD;
LPCOMRESULT	lpDIRADDHEAD;
LPCOMRESULT	lpDIRDELHEAD;
LPCOMRESULT	lpDIRMODIHEAD;

//Number of Modification detected
DWORD	nKEYADD;
DWORD	nKEYDEL;
DWORD	nVALADD;
DWORD	nVALDEL;
DWORD	nVALMODI;
DWORD	nFILEADD;
DWORD	nFILEDEL;
DWORD	nFILEMODI;
DWORD	nDIRADD;
DWORD	nDIRDEL;
DWORD	nDIRMODI;


//Some DWORD used to show the progress bar and etc
DWORD	nGettingValue;
DWORD	nGettingKey,nComparing,nComparingStep,nSavingKey,nSavingStep;
DWORD	nGettingTime,nBASETIME,nBASETIME1;
DWORD	nGettingFile,nGettingDir;
DWORD	nMask=0xf7fd; //not used now ,but should be added
DWORD	nRegMessageCount=0;
DWORD	NumberOfBytesWritten;

//Pointers to Registry Key
LPKEYCONTENT	lpHeadLocalMachine1;		//Pointer to HKEY_LOCAL_MACHINE	1
LPKEYCONTENT	lpHeadLocalMachine2;		//Pointer to HKEY_LOCAL_MACHINE 2
LPKEYCONTENT	lpHeadUsers1;				//Pointer to HKEY_USERS 1
LPKEYCONTENT	lpHeadUsers2;				//Pointer to HKEY_USERS 1
LPHEADFILE		lpHeadFile;					//Pointer to Headfile
LPSTR			lpTempHive1,lpTempHive2;	//Pointer for load hive files
LPSTR			lpComputerName1,lpComputerName2,lpUserName1,lpUserName2;
SYSTEMTIME FAR * lpSystemtime1,* lpSystemtime2;

//Some pointers need to allocate enought space to working
LPSTR	lpKeyName,lpMESSAGE,lpExtDir,lpOutputpath,lpLastSaveDir,lpLastOpenDir,lpCurrentLanguage;
LPSTR	lpWindowsDirName,lpTempPath,lpStartDir,lpIni,lpFreeStrings,lpCurrentTranslator;

LPSTR	lpProgramDir; //tfx 定义
#define SIZEOF_REGSHOT	65535
#define MAXREGSHOT		100
LPDWORD lpSnapRegs, lpSnapFiles;
LPSTR	lpRegshotIni;
LPSTR   lpSnapRegsStr,lpSnapFilesStr,lpSnapKey,lpSnapReturn;
LPSTR	REGSHOTINI			="regshot.ini";
LPSTR	INI_SETUP			="Setup";
LPSTR	INI_FLAG			="Flag";
LPSTR	INI_EXTDIR			="ExtDir";
LPSTR	INI_OUTDIR			="OutDir";
LPSTR	INI_SKIPREGKEY		="SkipRegKey";
LPSTR	INI_SKIPDIR			="SkipDir";

LPSTR	REGSHOTDATFILE		="rgst152.dat";
LPSTR	REGSHOTLANGUAGEFILE	="language.ini";
LPSTR	lpSectionCurrent	="CURRENT";
LPSTR	lpItemTranslator	="Translator";
LPSTR	lpMyName			="[Original]";
LPSTR	lpDefaultLanguage	="English";
//LPSTR	USERSSTRING			="HKEY_USERS";
//LPSTR	LOCALMACHINESTRING	="HKEY_LOCAL_MACHINE";
LPSTR	USERSSTRING			="HKU";
LPSTR	LOCALMACHINESTRING	="HKLM";
LPSTR	lpRegSaveFileHeader	="REGSHOTHIVE";
LPDWORD	ldwTempStrings;

//This is the dimension for MultiLanguage Default Strings[English]
unsigned char str_default[][22]=
{
	"Keys:",
	"Values:",
	"Dirs:",
	"Files:",
	"Time:",
	"Keys added:",
	"Keys deleted:",
	"Values added:",
	"Values deleted:",
	"Values modified:",
	"Files added:",
	"Files deleted:",
	"Files[attr]modified:",
	"Folders added:",
	"Folders deleted:",
	"Folders[attr]changed:",
	"Total changes:",
	"Comments:",
	"Datetime:",
	"Computer:",
	"Username:",
	"About",
	"Error",
	"Error call ex-viewer",
	"Error create file",
	"Error open file",
	"Error move fp",
	"&1st shot",
	"&2nd shot",
	"c&Ompare",
	"&Clear",
	"&Quit",
	"&About",
	"&Monitor",
	"Compare logs save as:",
	"Output path:",
	"Add comment into log:",
	"Plain &TXT",
	"&HTML document",
	"&Scan dir1[;dir2;...]",
	"&Shot",
	"Shot and Sa&ve...",
	"Loa&d...",
	"&Clear All",
	"Clear &1st shot",
	"Clear &2nd shot"
};

/////This is the Pointer to Language Strings
LPSTR	str_key;
LPSTR	str_value;
LPSTR	str_dir;
LPSTR	str_file;
LPSTR	str_time;
LPSTR	str_keyadd;
LPSTR	str_keydel;
LPSTR	str_valadd;
LPSTR	str_valdel;
LPSTR   str_valmodi;
LPSTR	str_fileadd;
LPSTR	str_filedel;
LPSTR	str_filemodi;
LPSTR	str_diradd;
LPSTR	str_dirdel;
LPSTR	str_dirmodi;
LPSTR	str_total;
LPSTR	str_comments;
LPSTR	str_datetime;
LPSTR	str_computer;
LPSTR	str_username;
LPSTR	str_about;
LPSTR	str_error;
LPSTR   str_errorexecviewer;
LPSTR	str_errorcreatefile;
LPSTR	str_erroropenfile;
LPSTR	str_errormovefp;
LPSTR	str_menushot;
LPSTR	str_menushotsave;
LPSTR	str_menuload;
LPSTR	str_menuclearallshots;
LPSTR	str_menuclearshot1;
LPSTR	str_menuclearshot2;



/////Some strings use to write to HTML or TEXT file
LPSTR	str_line	="\r\n----------------------------------\r\n";
LPSTR	lpBR		="<BR>";
LPSTR	lpHTMLbegin	="<HTML>";
LPSTR	lpHTMLover	="</HTML>";
LPSTR	lpHEADbegin	="<HEAD>";
LPSTR	lpHEADover	="</HEAD>";
LPSTR	lpTd1Begin	="<TR><TD BGCOLOR=669999 ALIGN=LEFT><FONT COLOR=WHITE><B>";
LPSTR	lpTd2Begin	="<TR><TD NOWRAP><FONT COLOR=BLACK>";
LPSTR	lpTd1Over	="</B></FONT></TD></TR>";
LPSTR	lpTd2Over	="</FONT></TD></TR>";
LPSTR	lpstyle		="<STYLE TYPE=\"text/css\">td{font-family:\"Tahoma\";font-size:9pt}tr{font-size:9pt}body{font-size:9pt}</STYLE>";
LPSTR	lpBodyBegin	="<BODY BGCOLOR=FFFFFF TEXT=000000 LINK=C8C8C8>";
LPSTR	lpBodyOver	="</BODY>";
LPSTR	lpTableBegin="<TABLE BORDER=0 WIDTH=480>";
LPSTR	lpTableOver	="</TABLE>";
LPSTR	str_website	="<FONT COLOR=C8C8C8>Bug reports to:<A HREF=\"http://regshot.yeah.net/\">regshot.yeah.net</FONT></A>";
LPSTR	str_aboutme	="Regshot 1.7\n(c) Copyright 2000-2002\tTiANWEi\n(c) Copyright 2004-2004\ttulipfan\n\nhttp://regshot.yeah.net/\nhttp://regshot.ist.md/ [Thanks Alexander Romanenko!]\n\n";
LPSTR	str_lognote	="Regshot 1.7\r\n";
LPSTR	str_prgname ="Regshot 1.7"; //tfx 程序标题

unsigned char lpCR[]={0x0d,0x0a,0x00};
unsigned char strDefResPre[]="~res";
unsigned char str_filter[]={"Regshot hive files [*.hiv]\0*.hiv\0All files\0*.*\0\0"};

//Former definations used at Dynamic Monitor Engine.Not Used NOW
//#define	DIOCPARAMSSIZE	20	//4+4+4+8 bytes DIOCParams size!
//#define	MAXLISTBOXLEN	1024
//#define	RING3TDLEN		8	//ring3 td name length
//LPSTR	str_errorini="Error create Dialog!";
//INT		tabarray[]={40,106,426,466};		// the tabstops is the len addup!
//BOOL	bWinNTDetected;
//UINT			WM_REGSHOT=0;

#ifdef	DEBUGLOG
LPSTR	lstrdb1;
#endif

////-------------------------------------------------------------
////Regshot error message routine[simple]
////-------------------------------------------------------------
//void	MyErrorMessage(HANDLE hDialog,LPSTR note)
//{
//	MessageBox(hDialog,note,str_error,MB_ICONHAND);
//}
//
////-------------------------------------------------------------
////Roution to debug,NOT used!
////-------------------------------------------------------------
//#ifdef  DEBUGLOG
//void	DebugLog(LPSTR filename,LPSTR lpstr,HWND hDlg,BOOL bisCR)
//{
//	DWORD	length;
//	DWORD	nPos;
//	
//	hFile = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
//	if( hFile == INVALID_HANDLE_VALUE) 
//		MyErrorMessage(hDlg,str_errorcreatefile);
//	else
//	{
//		nPos=SetFilePointer(hFile,0,NULL,FILE_END);
//		if(nPos==0xFFFFFFFF)
//			MyErrorMessage(hDlg,str_errormovefp);
//		else
//		{
//		
//			length=lstrlen(lpstr);
//			WriteFile(hFile,lpstr,length,&NumberOfBytesWritten,NULL);
//			if(NumberOfBytesWritten!=length)
//			{
//				//MyErrorMessage(hDlg,str_errorwritefile);
//				
//			}
//			if(bisCR==TRUE)
//			WriteFile(hFile,lpCR,2,&NumberOfBytesWritten,NULL);
//		}
//	}
//	CloseHandle(hFile);
//}
//#endif
//
////-------------------------------------------------------------
////Routine wasted
////-------------------------------------------------------------
///*LPVOID	MyHeapAlloc(DWORD type,DWORD size)
//{
//	if((bTurboMode==FALSE)&&((lpMyHeap+size)<(lpMyHeap+MYHEAPSIZE)))
//	{
//		lpMyHeap=lpMyHeap+size;
//		if(type==LPTR)
//			ZeroMemory(lpMyHeap,size);
//	}
//	else
//	{
//		lpMyHeap=GlobalAlloc(type,size);
//	}
//	return lpMyHeap;
//}
// */


//-------------------------------------------------------------
//Routine to get Whole File Name[root dir] from a FILECONTENT
//-------------------------------------------------------------
LPSTR	GetWholeFileName(LPFILECONTENT lpFileContent)
{
	LPFILECONTENT lpf;
	DWORD	nWholeLen=0,nLen,i;
	LPSTR	lpWholeName=NULL,lptail;
	for(lpf=lpFileContent;lpf!=NULL;lpf=lpf->lpfatherfile)
	{
		nWholeLen=nWholeLen+lstrlen(lpf->lpfilename)+1;
	}
	lpWholeName=(LPSTR)GlobalAlloc(LMEM_FIXED,nWholeLen);

	lptail=lpWholeName+nWholeLen-1;
	*lptail=0x00;
	
	for(lpf=lpFileContent;lpf!=NULL;lpf=lpf->lpfatherfile)
	{
		nLen=lstrlen(lpf->lpfilename);
		for(lptail--,i=1;i<=nLen;i++,lptail--)
		{
			*lptail=*((lpf->lpfilename)+nLen-i);
			
		}
		if (lptail>lpWholeName)
			*lptail=0x5c; //"\\"

	}
	return lpWholeName;
}

//-------------------------------------------------------------
//Routine to Get Whole Key Name from KEYCONTENT
//-------------------------------------------------------------
LPSTR	GetWholeKeyName(LPKEYCONTENT lpKeyContent)
{
	LPKEYCONTENT lpf;
	DWORD	nWholeLen=0,nLen,i;
	LPSTR	lpWholeName=NULL,lptail;
	for(lpf=lpKeyContent;lpf!=NULL;lpf=lpf->lpfatherkey)
	{
		nWholeLen=nWholeLen+lstrlen(lpf->lpkeyname)+1;
	}
	lpWholeName=(LPSTR)GlobalAlloc(LMEM_FIXED,nWholeLen);

	lptail=lpWholeName+nWholeLen-1;
	*lptail=0x00;
	for(lpf=lpKeyContent;lpf!=NULL;lpf=lpf->lpfatherkey)
	{
		nLen=lstrlen(lpf->lpkeyname);
		for(lptail--,i=1;i<=nLen;i++,lptail--)
		{
			*lptail=*((lpf->lpkeyname)+nLen-i);
			
		}
		if (lptail>lpWholeName)
			*lptail=0x5c; //"\\"

	}
	return lpWholeName;
}


//-------------------------------------------------------------
//Routine to Get Whole VALUE Name from VALUECONTENT
//-------------------------------------------------------------
LPSTR	GetWholeValueName(LPVALUECONTENT lpValueContent)
{
	LPKEYCONTENT lpf;
	DWORD	nWholeLen=0,nLen,i;
	LPSTR	lpWholeName=NULL,lptail;
	nLen=lstrlen(lpValueContent->lpvaluename);
	nWholeLen=nLen+1;
	for(lpf=lpValueContent->lpfatherkey;lpf!=NULL;lpf=lpf->lpfatherkey)
	{
		nWholeLen=nWholeLen+lstrlen(lpf->lpkeyname)+1;
	}
	lpWholeName=(LPSTR)GlobalAlloc(LMEM_FIXED,nWholeLen);
	lptail=lpWholeName+nWholeLen-1;
	*lptail=0x00;

	for(lptail--,i=1;i<=nLen;i++,lptail--)
	{
		*lptail=*((lpValueContent->lpvaluename)+nLen-i);
	}
	*lptail=0x5c; //"\\"
	for(lpf=lpValueContent->lpfatherkey;lpf!=NULL;lpf=lpf->lpfatherkey)
	{
		nLen=lstrlen(lpf->lpkeyname);
		for(lptail--,i=1;i<=nLen;i++,lptail--)
		{
			*lptail=*((lpf->lpkeyname)+nLen-i);
		}
		if (lptail>lpWholeName)
			*lptail=0x5c; //"\\"

	}
	return lpWholeName;
}

//-------------------------------------------------------------
//Routine Trans VALUECONTENT.data[which in binary] into strings
//-------------------------------------------------------------
LPSTR	TransData(LPVALUECONTENT lpValueContent,DWORD type)
{
	LPSTR	lpvaluedata=NULL;
	DWORD	c,size=lpValueContent->datasize;
	switch(type)
	{

		case	REG_SZ:
		//case	REG_EXPAND_SZ: Not used any more,they all included in [default]
				lpvaluedata=(LPSTR)GlobalAlloc(LPTR,size+5); //5 is enough
				lstrcpy(lpvaluedata,": \"");
				if(lpValueContent->lpvaluedata!=NULL) //added in 1.62? not compiled
					lstrcat(lpvaluedata,(LPCSTR)lpValueContent->lpvaluedata);
				lstrcat(lpvaluedata,"\"");
				//wsprintf has a bug that can not print string too long one time!);
				//wsprintf(lpvaluedata,"%s%s%s",": \"",lpValueContent->lpvaluedata,"\"");
				break;
		case	REG_MULTI_SZ:
				//Be sure to add below line outside of following "if",
				//for that GlobalFree(lp) must had lp already located!
				lpvaluedata=(LPSTR)GlobalAlloc(LPTR,size+5);//5 is enough
				for(c=0;c<size;c++)
				{
					if (*((LPBYTE)(lpValueContent->lpvaluedata+c))==0)
					{
						if (*((LPBYTE)(lpValueContent->lpvaluedata+c+1))!=0)
							*((LPBYTE)(lpValueContent->lpvaluedata+c))=0x20; ////???????
						else
						   break;
					}
				}
				//*((LPBYTE)(lpValueContent->lpvaluedata+size))=0x00; // for some illegal multisz
				lstrcpy(lpvaluedata,": '");
				lstrcat(lpvaluedata,(LPCSTR)lpValueContent->lpvaluedata);
				lstrcat(lpvaluedata,"'");
				//wsprintf(lpvaluedata,"%s%s%s",": \"",lpValueContent->lpvaluedata,"\"");
				break;
		case	REG_DWORD:
		//case	REG_DWORD_BIG_ENDIAN: Not used any more,they all included in [default]
				lpvaluedata=(LPSTR)GlobalAlloc(LPTR,13); //13 is enough
				wsprintf(lpvaluedata,"%s%08X",": 0x",*(LPDWORD)(lpValueContent->lpvaluedata));
				break;
		default	:
				lpvaluedata=(LPSTR)GlobalAlloc(LPTR,3*(size+1)); //3*(size+1) is enough
				*lpvaluedata=0x3a;
				//for the resttype lengthofvaluedata doesn't contains the 0!
				for	(c=0;c<size;c++)
					wsprintf(lpvaluedata+3*c+1," %02X",*(lpValueContent->lpvaluedata+c));
	}
	return lpvaluedata;
}


//-------------------------------------------------------------
//Routine to Get Whole Value Data from VALUECONTENT
//-------------------------------------------------------------
LPSTR	GetWholeValueData(LPVALUECONTENT lpValueContent)
{
	LPSTR	lpvaluedata=NULL;
	DWORD	c,size=lpValueContent->datasize;
	switch(lpValueContent->typecode)
	{

		case	REG_SZ:
		case	REG_EXPAND_SZ:
				if(lpValueContent->lpvaluedata!=NULL) //added in 1.62 not compiled
				{
				if(size==(DWORD)lstrlen((LPCSTR)lpValueContent->lpvaluedata)+1)
					lpvaluedata=TransData(lpValueContent,REG_SZ);
				else
					lpvaluedata=TransData(lpValueContent,REG_BINARY);
				}
				else
					lpvaluedata=TransData(lpValueContent,REG_SZ);
				break;
		case	REG_MULTI_SZ:
				if(*((LPBYTE)(lpValueContent->lpvaluedata))!=0x00)
				{
					for(c=0;;c++)
					{
						if (*((LPWORD)(lpValueContent->lpvaluedata+c))==0)
						   break;
					}
					if(size==c+2)
						lpvaluedata=TransData(lpValueContent,REG_MULTI_SZ);
					else
						lpvaluedata=TransData(lpValueContent,REG_BINARY);
				}
				else
					lpvaluedata=TransData(lpValueContent,REG_BINARY);
				break;
		case	REG_DWORD:
		case	REG_DWORD_BIG_ENDIAN:
				if(size==SIZEOFREG_DWORD)
					lpvaluedata=TransData(lpValueContent,REG_DWORD);
				else
					lpvaluedata=TransData(lpValueContent,REG_BINARY);
				break;
		default	:
				lpvaluedata=TransData(lpValueContent,REG_BINARY);	
	}
	return lpvaluedata;
}


//-------------------------------------------------------------
//Routine to create new compare result,distribute to different lp???MODI
//-------------------------------------------------------------

void	CreateNewResult(DWORD actiontype,LPDWORD lpcount,LPSTR lpresult)
{
	LPCOMRESULT	lpnew;
	lpnew=(LPCOMRESULT)GlobalAlloc(LPTR,sizeof(COMRESULT));
	lpnew->lpresult=lpresult;
	
	switch(actiontype)
	{
		case KEYADD:
				*lpcount==0 ? (lpKEYADDHEAD=lpnew):(lpKEYADD->lpnextresult=lpnew);
				lpKEYADD=lpnew;
				break;
		case KEYDEL:
				*lpcount==0 ? (lpKEYDELHEAD=lpnew):(lpKEYDEL->lpnextresult=lpnew);
				lpKEYDEL=lpnew;
				break;
		case VALADD:
				*lpcount==0 ? (lpVALADDHEAD=lpnew):(lpVALADD->lpnextresult=lpnew);
				lpVALADD=lpnew;
				break;
		case VALDEL:
				*lpcount==0 ? (lpVALDELHEAD=lpnew):(lpVALDEL->lpnextresult=lpnew);
				lpVALDEL=lpnew;
				break;
		case VALMODI:
				*lpcount==0 ? (lpVALMODIHEAD=lpnew):(lpVALMODI->lpnextresult=lpnew);
				lpVALMODI=lpnew;
				break;
		case FILEADD:
				*lpcount==0 ? (lpFILEADDHEAD=lpnew):(lpFILEADD->lpnextresult=lpnew);
				lpFILEADD=lpnew;
				break;
		case FILEDEL:
				*lpcount==0 ? (lpFILEDELHEAD=lpnew):(lpFILEDEL->lpnextresult=lpnew);
				lpFILEDEL=lpnew;
				break;
		case FILEMODI:
				*lpcount==0 ? (lpFILEMODIHEAD=lpnew):(lpFILEMODI->lpnextresult=lpnew);
				lpFILEMODI=lpnew;
				break;
		case DIRADD:
				*lpcount==0 ? (lpDIRADDHEAD=lpnew):(lpDIRADD->lpnextresult=lpnew);
				lpDIRADD=lpnew;
				break;
		case DIRDEL:
				*lpcount==0 ? (lpDIRDELHEAD=lpnew):(lpDIRDEL->lpnextresult=lpnew);
				lpDIRDEL=lpnew;
				break;
		case DIRMODI:
				*lpcount==0 ? (lpDIRMODIHEAD=lpnew):(lpDIRMODI->lpnextresult=lpnew);
				lpDIRMODI=lpnew;
				break;

	}
	(*lpcount)++;
}

std::string GetType(DWORD dwType)
{
	std::string qsType("");
	switch(dwType)
	{
	case REG_SZ:
		qsType = "REG_SZ";
		break;
	case REG_BINARY:
		qsType = "REG_BINARY";
		break;
	case REG_DWORD:
		qsType = "REG_DWORD";
		break;
	case REG_QWORD:
		qsType = "REG_QWORD";
		break;
	case REG_MULTI_SZ:
		qsType = "REG_MULTI_SZ";
		break;
	case REG_EXPAND_SZ:
		qsType = "REG_EXPAND_SZ";
		break;
	}

	return qsType;
}

////！添加比较记录
//void AddCompareResult(QTableWidget* pTable, int nType, 
//	QString qsPath, QString qsKey = "", DWORD dwType = 0,
//	QString qsVal1 = "", QString qsVal2 = "")
//{
//	//! display
//	int iRow = pTable->rowCount();
//	pTable->insertRow(iRow);
//	QString strType;
//	switch(nType)
//	{
//	case KEYADD:
//		strType = "KEYADD";
//		break;
//	case KEYDEL:
//		strType = "KEYDEL";
//		break;
//	case VALADD:
//		strType = "VALADD";
//		break;
//	case VALDEL:
//		strType = "VALDEL";
//		break;
//	case VALMODI:
//		strType = "VALMODI";
//		break;
//	}
//	pTable->setItem(iRow, 0 , new QTableWidgetItem(strType));
//	pTable->setItem(iRow, 1 , new QTableWidgetItem(qsPath));
//	if ( qsKey != "")
//	{
//		pTable->setItem(iRow, 2, new QTableWidgetItem(qsKey));
//		pTable->setItem(iRow, 3, new QTableWidgetItem(GetType(dwType)));
//		pTable->setItem(iRow, 4, new QTableWidgetItem(qsVal1));
//		pTable->setItem(iRow, 5, new QTableWidgetItem(qsVal2));
//		pTable->selectRow(iRow);
//	}
//
//}


//-------------------------------------------------------------
//Write compare results into memory and call CreateNewResult()
//-------------------------------------------------------------
void	LogToMem(QTableWidget* pTable,
	DWORD actiontype,LPDWORD lpcount,LPVOID lp, LPVOID lpOld = NULL)
{
	LPSTR	lpname,lpdata,lpall;
	if(actiontype==KEYADD||actiontype==KEYDEL)
	{
		lpname=GetWholeKeyName((LPKEYCONTENT)lp);
		CreateNewResult(actiontype,lpcount,lpname);
		AddCompareResult(pTable, actiontype, lpname);
	}

	else
	{
		if(actiontype==VALADD||actiontype==VALDEL||actiontype==VALMODI)
		{		
			lpname=GetWholeValueName((LPVALUECONTENT)lp);
			lpdata=GetWholeValueData((LPVALUECONTENT)lp);
			lpall=(LPSTR)GlobalAlloc(LMEM_FIXED,lstrlen(lpname)+lstrlen(lpdata)+2);
			//do not use:wsprintf(lpall,"%s%s",lpname,lpdata); !!! strlen limit!
			lstrcpy(lpall,lpname);
			lstrcat(lpall,lpdata);			
			CreateNewResult(actiontype,lpcount,lpall);
			if ( actiontype==VALADD||actiontype==VALDEL )
			{				
				AddCompareResult(pTable, actiontype, lpname, 
					((LPVALUECONTENT)lp)->lpvaluename, ((LPVALUECONTENT)lp)->typecode,lpdata);				
			}
			else
			{
				LPSTR lpdataOld = GetWholeValueData((LPVALUECONTENT)lpOld);
				AddCompareResult(pTable, actiontype, lpname, 
					((LPVALUECONTENT)lp)->lpvaluename, ((LPVALUECONTENT)lp)->typecode, lpdata, lpdataOld);
				GlobalFree(lpdataOld);
			}
			GlobalFree(lpname);
			GlobalFree(lpdata);
		}
		//else
		//{
		//	lpname=GetWholeFileName((LPFILECONTENT)lp);
		//	CreateNewResult(actiontype,lpcount,lpname);
		//}

	}
}

//-------------------------------------------------------------
//Routine to walk through all values of current key
//-------------------------------------------------------------
void	GetAllValue(QTableWidget* pTable,DWORD typevalue,LPDWORD lpcountvalue,LPKEYCONTENT lpKeyContent)
{
	LPVALUECONTENT lpv;
	for(lpv=lpKeyContent->lpfirstvalue;lpv!=NULL;lpv=lpv->lpnextvalue)
	{
		LogToMem(pTable, typevalue,lpcountvalue,lpv);
	}
}


//-------------------------------------------------------------
//Routine to walk through sub keytree of current Key
//-------------------------------------------------------------
void	GetAllSubName(
	QTableWidget* pTable,
	BOOL needbrother,
	DWORD typekey,DWORD typevalue,
	LPDWORD lpcountkey,LPDWORD lpcountvalue,
	LPKEYCONTENT lpKeyContent
	)
{

	LPVALUECONTENT lpv;
	LogToMem(pTable, typekey,lpcountkey,lpKeyContent);

	if(lpKeyContent->lpfirstsubkey!=NULL)
	{
		GetAllSubName(pTable, TRUE,typekey,typevalue,lpcountkey,lpcountvalue,lpKeyContent->lpfirstsubkey);
	}

	if(needbrother==TRUE)
		if(lpKeyContent->lpbrotherkey!=NULL)
		{
			GetAllSubName(pTable, TRUE,typekey,typevalue,lpcountkey,lpcountvalue,lpKeyContent->lpbrotherkey);
		}

		for(lpv=lpKeyContent->lpfirstvalue;lpv!=NULL;lpv=lpv->lpnextvalue)
		{
			LogToMem(pTable,typevalue,lpcountvalue,lpv);
		}
}

VOID * CompareFirstSubKey(LPKEYCONTENT lpHead1,LPKEYCONTENT lpHead2, QPushButton* pBtn,QTableWidget* pTable)
{
	LPKEYCONTENT	lp1,lp2;
	LPVALUECONTENT	lpvalue1,lpvalue2;
	DWORD	i;

	for(lp1=lpHead1; lp1!=NULL; lp1=lp1->lpbrotherkey)
	{
		for(lp2=lpHead2; lp2!=NULL; lp2=lp2->lpbrotherkey)
		{
			if((lp2->bkeymatch==NOTMATCH)&&lstrcmp(lp1->lpkeyname,lp2->lpkeyname)==0)
			{	//Same key found! we compare their values and their subkeys!

				lp2->bkeymatch=ISMATCH;
				if (lp1->lpfirstvalue==NULL&&lp2->lpfirstvalue!=NULL)
				{	//Key1 has no values,so lpvalue2 is added! we find all values belongs to lp2!
					GetAllValue(pTable,VALADD,&nVALADD,lp2);
				}
				else
				{
					if (lp1->lpfirstvalue!=NULL&&lp2->lpfirstvalue==NULL)
					{	//Key2 has no values,so lpvalue1 is delted! we find all values belongs to lp1!
						GetAllValue(pTable,VALDEL,&nVALDEL,lp1);
					}
					else
					{	//Two keys all has values,so we loop them

						for(lpvalue1=lp1->lpfirstvalue;lpvalue1!=NULL;lpvalue1=lpvalue1->lpnextvalue)
						{
							for(lpvalue2=lp2->lpfirstvalue;lpvalue2!=NULL;lpvalue2=lpvalue2->lpnextvalue)
							{	//Loop lp2 to find a value matchs lp1's
								if((lpvalue2->bvaluematch==NOTMATCH)&&(lpvalue1->typecode==lpvalue2->typecode))
								{	//Same valuedata type
									if(lstrcmp(lpvalue1->lpvaluename,lpvalue2->lpvaluename)==0)
									{	//Same valuename
										if(lpvalue1->datasize==lpvalue2->datasize)
										{	//Same size of valuedata
											for(i=0;i<lpvalue1->datasize;i++)
											{
												if (*((lpvalue1->lpvaluedata)+i)!=*((lpvalue2->lpvaluedata)+i))
													break;
											}
											if (i==lpvalue1->datasize)
											{	//Same valuedata,keys are the same!

												lpvalue2->bvaluematch=ISMATCH;
												break;//Be sure not to do lp2==NULL
											}
											else
											{	//Valuedata not match due to data mismatch!,we found a modified valuedata!*****
												lpvalue2->bvaluematch=ISMODI;
												LogToMem(pTable, VALMODI,&nVALMODI,lpvalue2,lpvalue1);
												//LogToMem(pTable, VALMODI,&nVALMODI,lpvalue2);
												//nVALMODI--;
												break;
											}
										}
										else
										{	//Waluedata does not match due to size,we found a modified valuedata!******
											lpvalue2->bvaluematch=ISMODI;
											//LogToMem(pTable, VALMODI,&nVALMODI,lpvalue1);
											LogToMem(pTable, VALMODI,&nVALMODI,lpvalue2,lpvalue1);
											//nVALMODI--;
											break;
										}
									}
								}
							}
							if( lpvalue2 == NULL )
							{	//We found a value in lp1 but not in lp2,we found a deleted value*****
								LogToMem(pTable,VALDEL,&nVALDEL,lpvalue1);
							}
						}
						//After we loop to end,we do extra loop use flag we previouse made to get added values
						for(lpvalue2=lp2->lpfirstvalue;lpvalue2!=NULL;lpvalue2=lpvalue2->lpnextvalue)
						{
							if(lpvalue2->bvaluematch!=ISMATCH&&lpvalue2->bvaluematch!=ISMODI)
							{	//We found a value in lp2's but not in lp1's,we found a added value****
								LogToMem(pTable,VALADD,&nVALADD,lpvalue2);

							}
						}
					}
				}

				//////////////////////////////////////////////////////////////
				//After we walk through the values above,we now try to loop the sub keys of current key
				if(lp1->lpfirstsubkey==NULL&&lp2->lpfirstsubkey!=NULL)
				{	//lp2's firstsubkey added!
					GetAllSubName(pTable,TRUE,KEYADD,VALADD,&nKEYADD,&nVALADD,lp2->lpfirstsubkey);
				}
				if(lp1->lpfirstsubkey!=NULL&&lp2->lpfirstsubkey==NULL)
				{	//lp1's firstsubkey deleted!
					GetAllSubName(pTable,TRUE,KEYDEL,VALDEL,&nKEYDEL,&nVALDEL,lp1->lpfirstsubkey);
				}
				if(lp1->lpfirstsubkey!=NULL&&lp2->lpfirstsubkey!=NULL)
					CompareFirstSubKey(lp1->lpfirstsubkey,lp2->lpfirstsubkey,pBtn,pTable);
				break;
			}
		}
		if(lp2==NULL)
		{	//We did not find a lp2 matches a lp1,so lp1 is deleted!
			GetAllSubName(pTable,FALSE,KEYDEL,VALDEL,&nKEYDEL,&nVALDEL,lp1);
		}

	}

	//After we loop to end,we do extra loop use flag we previouse made to get added keys
	for(lp2=lpHead2;lp2!=NULL;lp2=lp2->lpbrotherkey) //->lpbrotherkey
	{
		nComparing++;
		if(lp2->bkeymatch==NOTMATCH)
		{   //We did not find a lp1 matches a lp2,so lp2 is added!
			GetAllSubName(pTable,FALSE,KEYADD,VALADD,&nKEYADD,&nVALADD,lp2);
		}
	}

	// Progress bar update
	if (nGettingKey!=0)
	{
		if (nComparing%nGettingKey>nComparingStep)
		{
			nComparing=0;
			//SendDlgItemMessage(hWnd,IDC_PBCOMPARE,PBM_STEPIT,(WPARAM)0,(LPARAM)0);
		}
	}

	nGettingTime = GetTickCount();
	if ( (nGettingTime - nBASETIME1) > REFRESHINTERVAL )
	{
		nBASETIME1 = nGettingTime;
		char pch[6];
		sprintf(pch, "%ds", (nGettingTime-nBASETIME)/1000);
		pBtn->setText(pch);
		QCoreApplication::processEvents();
		Sleep(10);
	}
	return NULL;
}


//-------------------------------------------------------------
//Routine to Free All Compare Results [Release Memory]
//-------------------------------------------------------------
void	FreeAllCom(LPCOMRESULT lpComResult)
{
	LPCOMRESULT lp,lpold;
	for(lp=lpComResult;lp!=NULL;)
	{
		if(lp->lpresult!=NULL)
			GlobalFree(lp->lpresult);
		lpold=lp;
		lp=lp->lpnextresult;
		GlobalFree(lpold);
	}	
}

//-------------------------------------------------------------
//Routine to Free All Keys and Values
//-------------------------------------------------------------
void FreeAllKey(LPKEYCONTENT lpKey, QPushButton* pBtn)
{
	if(lpKey!=NULL)
	{
		FreeAllKey(lpKey->lpfirstsubkey,pBtn);
		FreeAllKey(lpKey->lpbrotherkey, pBtn);

		LPVALUECONTENT lpv,lpvold;
		for(lpv=lpKey->lpfirstvalue;lpv!=NULL;)
		{
			GlobalFree(lpv->lpvaluename);
			if(lpv->lpvaluedata!=NULL)
				GlobalFree(lpv->lpvaluedata);
			lpvold=lpv;
			lpv=lpv->lpnextvalue;
			GlobalFree(lpvold);
		}
		GlobalFree(lpKey->lpkeyname);
		GlobalFree(lpKey);
	}

	nGettingTime = GetTickCount();
	if ( (nGettingTime - nBASETIME1) > REFRESHINTERVAL )
	{
		nBASETIME1 = nGettingTime;
		char pch[6];
		sprintf(pch, "%ds", (nGettingTime-nBASETIME)/1000);
		pBtn->setText(pch);
		QCoreApplication::processEvents();
		Sleep(10);
	}
}

void CancelSnap(LPKEYCONTENT lpKey1, LPKEYCONTENT lpKey2, QPushButton* pBtn)
{
	nBASETIME = GetTickCount();
	nBASETIME1 = nBASETIME;
	FreeAllKey(lpKey1, pBtn);
	FreeAllKey(lpKey2, pBtn);
}
//-------------------------------------------------------------
//Routines to Free All File tree
//-------------------------------------------------------------
void FreeAllFileContent(LPFILECONTENT lpFile)
{
	if(lpFile!=NULL)
	{
		FreeAllFileContent(lpFile->lpfirstsubfile);
		FreeAllFileContent(lpFile->lpbrotherfile);
		GlobalFree(lpFile->lpfilename);
		GlobalFree(lpFile);
	}
}

//-------------------------------------------------------------
//Routines to Free Head Files
//-------------------------------------------------------------
void FreeAllFileHead(LPHEADFILE lp)
{
	if(lp!=NULL)
	{
		FreeAllFileHead(lp->lpnextheadfile);
		FreeAllFileContent(lp->lpHeadFile1);
		FreeAllFileContent(lp->lpHeadFile2);
		GlobalFree(lp);
	}

}


//-------------------------------------------------------------
//Clear Flag previous made by Compare Routine for the next compare
//-------------------------------------------------------------
VOID ClearKeyMatchTag(LPKEYCONTENT lpKey)
{
	LPVALUECONTENT lpv;
	if(lpKey!=NULL)
	{
		lpKey->bkeymatch=0;
		for(lpv=lpKey->lpfirstvalue;lpv!=NULL;lpv=lpv->lpnextvalue)
		{
			lpv->bvaluematch=0;
		}
		
		if(lpKey->lpfirstsubkey!=NULL)
		{
			ClearKeyMatchTag(lpKey->lpfirstsubkey);
		}
		
		if(lpKey->lpbrotherkey!=NULL)
		{
			ClearKeyMatchTag(lpKey->lpbrotherkey);
		}
	}
}


void FreeAllCompareResults(void)
{	
	FreeAllCom(lpKEYADDHEAD);
	FreeAllCom(lpKEYDELHEAD);
	FreeAllCom(lpVALADDHEAD);
	FreeAllCom(lpVALDELHEAD);
	FreeAllCom(lpVALMODIHEAD);
	//FreeAllCom(lpFILEADDHEAD);
	//FreeAllCom(lpFILEDELHEAD);
	//FreeAllCom(lpFILEMODIHEAD);
	//FreeAllCom(lpDIRADDHEAD);
	//FreeAllCom(lpDIRDELHEAD);
	//FreeAllCom(lpDIRMODIHEAD);


	nKEYADD=0;nKEYDEL=0;nVALADD=0;nVALDEL=0;nVALMODI=0;nFILEADD=0;nFILEDEL=0;nFILEMODI=0;nDIRADD=0;nDIRDEL=0;nDIRMODI=0;
	lpKEYADDHEAD=NULL;lpKEYDELHEAD=NULL;lpVALADDHEAD=NULL;lpVALDELHEAD=NULL;lpVALMODIHEAD=NULL;
	lpFILEADDHEAD=NULL;lpFILEDELHEAD=NULL;lpFILEMODIHEAD=NULL;lpDIRADDHEAD=NULL;lpDIRDELHEAD=NULL;lpDIRMODIHEAD=NULL;
}

//**/
//------------------------------------------------------------
//Routine to SCAN THE REGISTRY!!
//------------------------------------------------------------
VOID	*	GetRegistrySnap(
HKEY	hkey,
LPKEYCONTENT	lpFatherKeyContent,
QPushButton*		pBtn)
{
    
	HKEY  Subhkey;
	DWORD i,NTr;
	DWORD TypeCode;
	DWORD LengthOfKeyName;
	DWORD LengthOfValueName;
	DWORD LengthOfValueData;
	DWORD LengthOfLongestValueName;
	DWORD LengthOfLongestValueData;
	DWORD LengthOfLongestSubkeyName;
	LPSTR lpValueName;
	LPSTR lpValueData;
	LPKEYCONTENT	lpKeyContent,lpKeyContentLast;
	LPVALUECONTENT	lpValueContent,lpValueContentLast;

	//To detemine MAX length
	if(RegQueryInfoKey(
		hkey,
		NULL,						//lpClassName_nouse,
		NULL,						//&nClassName_nouse_length,
		NULL,
		NULL,						//&NumberOfSubkeys,
		&LengthOfLongestSubkeyName, //chars
		NULL,						//&nClassName_nouse_longestlength,
		NULL,						//&NumberOfValue,
		&LengthOfLongestValueName,	//chars
		&LengthOfLongestValueData,	//bytes
		NULL,						//&nSecurity_length_nouse,
		NULL						//&ftLastWrite
		)!=ERROR_SUCCESS)
		return NULL;
	LengthOfLongestSubkeyName =LengthOfLongestSubkeyName*2+3; //yeah,may be x+1 is enought! x=chars
	LengthOfLongestValueName  =LengthOfLongestValueName*2+3; //yeah,may be x+1 is enought! x=chars
	LengthOfLongestValueData  =LengthOfLongestValueData+1;
	lpValueName=(LPSTR)GlobalAlloc(LMEM_FIXED,LengthOfLongestValueName);
	lpValueData=(LPSTR)GlobalAlloc(LMEM_FIXED,LengthOfLongestValueData);

	//Get Values
	for(i=0;;i++)
	{
		
		*(LPBYTE)lpValueName=(BYTE)0x00;//That's the bug in 2000! thanks zhangl@digiark.com!
		*(LPBYTE)lpValueData=(BYTE)0x00;
		//DebugBreak();
		LengthOfValueName=LengthOfLongestValueName;
		LengthOfValueData=LengthOfLongestValueData;
		NTr=RegEnumValue(hkey,i,lpValueName,&LengthOfValueName,
			NULL,&TypeCode,(LPBYTE)lpValueData,&LengthOfValueData);
		if	(NTr==ERROR_NO_MORE_ITEMS)
			break;
		else 
		{
			if	(NTr!=ERROR_SUCCESS)
			{
				continue;
			}
		}

#ifdef DEBUGLOG		
		DebugLog("debug_trytogetvalue.log","trying:",hDlg,FALSE);
		DebugLog("debug_trytogetvalue.log",lpValueName,hDlg,TRUE);
#endif

		lpValueContent=(LPVALUECONTENT)GlobalAlloc(LPTR,sizeof(VALUECONTENT));
		//I had done if(i==0) in 1.50b- ! thanks fisttk@21cn.com and non-standard 
		if(lpFatherKeyContent->lpfirstvalue==NULL)
			lpFatherKeyContent->lpfirstvalue=lpValueContent;
		else
			lpValueContentLast->lpnextvalue=lpValueContent;
		lpValueContentLast=lpValueContent;
		lpValueContent->typecode=TypeCode;
		lpValueContent->datasize=LengthOfValueData;
		lpValueContent->lpfatherkey=lpFatherKeyContent;
		lpValueContent->lpvaluename=(LPSTR)GlobalAlloc(LMEM_FIXED,lstrlen(lpValueName)+1);
		lstrcpy(lpValueContent->lpvaluename,lpValueName);

		if(LengthOfValueData!=0)
		{
			lpValueContent->lpvaluedata=(LPBYTE)GlobalAlloc(LMEM_FIXED,LengthOfValueData);
			CopyMemory(lpValueContent->lpvaluedata,lpValueData,LengthOfValueData);
			//	*(lpValueContent->lpvaluedata+LengthOfValueData)=0x00;
		}
		nGettingValue++;

#ifdef DEBUGLOG		
		lstrdb1=GlobalAlloc(LPTR,100);
		wsprintf(lstrdb1,"LGVN:%08d LGVD:%08d VN:%08d VD:%08d",LengthOfLongestValueName,LengthOfLongestValueData,LengthOfValueName,LengthOfValueData);
		DebugLog("debug_valuenamedata.log",lstrdb1,hDlg,TRUE);
		DebugLog("debug_valuenamedata.log",GetWholeValueName(lpValueContent),hDlg,FALSE);
		DebugLog("debug_valuenamedata.log",GetWholeValueData(lpValueContent),hDlg,TRUE);
		//DebugLog("debug_valuenamedata.log",":",hDlg,FALSE);
		//DebugLog("debug_valuenamedata.log",lpValueData,hDlg,TRUE);
		GlobalFree(lstrdb1);

#endif
	}

	GlobalFree(lpValueName);
	GlobalFree(lpValueData);
	
	for(i=0;;i++)
	{
		LengthOfKeyName=LengthOfLongestSubkeyName;
		*(LPBYTE)lpKeyName=(BYTE)0x00;
		NTr=RegEnumKeyEx(hkey,i,lpKeyName,&LengthOfKeyName,NULL,NULL,NULL,&ftLastWrite);
		if	(NTr==ERROR_NO_MORE_ITEMS)
			break;
		else 
		{
			if	(NTr!=ERROR_SUCCESS)
			{
				continue;
			}
		}
		lpKeyContent=(LPKEYCONTENT)GlobalAlloc(LPTR,sizeof(KEYCONTENT));
		if	(lpFatherKeyContent->lpfirstsubkey==NULL)
			lpFatherKeyContent->lpfirstsubkey=lpKeyContent;
		else
			lpKeyContentLast->lpbrotherkey=lpKeyContent;
		lpKeyContentLast=lpKeyContent;
		lpKeyContent->lpkeyname=(LPSTR)GlobalAlloc(LMEM_FIXED,lstrlen(lpKeyName)+1);
		lstrcpy(lpKeyContent->lpkeyname,lpKeyName);
		lpKeyContent->lpfatherkey=lpFatherKeyContent;
		//DebugLog("debug_getkey.log",lpKeyName,hDlg,TRUE);

#ifdef DEBUGLOG		
		lstrdb1=GlobalAlloc(LPTR,100);
		wsprintf(lstrdb1,"LGKN:%08d KN:%08d",LengthOfLongestSubkeyName,LengthOfKeyName);
		DebugLog("debug_key.log",lstrdb1,hDlg,TRUE);
		DebugLog("debug_key.log",GetWholeKeyName(lpKeyContent),hDlg,TRUE);
		GlobalFree(lstrdb1);

#endif
		
		nGettingKey++;

		if(/*IsInSkipList(lpKeyName,lpSnapRegs)||*/
			RegOpenKeyEx(hkey,lpKeyName,0,KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS,&Subhkey)!=ERROR_SUCCESS)
		{ //tfx
			continue;
		}
		GetRegistrySnap(Subhkey,lpKeyContent,pBtn);
		RegCloseKey(Subhkey);
	}
	
	nGettingTime = GetTickCount();
	if ( (nGettingTime - nBASETIME1) > REFRESHINTERVAL )
	{
		nBASETIME1 = nGettingTime;
		char pch[6];
		sprintf(pch, "%ds", (nGettingTime-nBASETIME)/1000);
		pBtn->setText(pch);
		QCoreApplication::processEvents();
		Sleep(10);
	}

	return NULL;
}

VOID	*	RegistrySnap(
	HKEY	hkey,
	LPKEYCONTENT	lpFatherKeyContent,
	QPushButton* pBtn)
{
	//! 记录开始时间
	nBASETIME = GetTickCount();
	nBASETIME1 = nBASETIME;

	return GetRegistrySnap(hkey, lpFatherKeyContent, pBtn);
}

//------------------------------------------------------------
//routine to call Compare Engine
//------------------------------------------------------------
BOOL RegCompare(LPKEYCONTENT lpKey1, LPKEYCONTENT lpKey2, QPushButton* pBtn, QTableWidget* pTable)
{
	BOOL	isHTML;//,bSaveWithCommentName;
	LPSTR	lpstrcomp,lpExt,lpDestFileName;
	DWORD	buffersize=2048,nTotal,filetail=0,nLengthofStr;
	LPHEADFILE	lphf;
	FILETIME ftime1,ftime2;
	
	CompareFirstSubKey(lpKey1, lpKey2, pBtn,pTable);	

	return TRUE;
}

void init_shot()
{
	lpExtDir=(LPSTR)GlobalAlloc(LPTR,(EXTDIRLEN+2));
	lpOutputpath=(LPSTR)GlobalAlloc(LPTR,MAX_PATH+5);
	lpCurrentLanguage=(LPSTR)GlobalAlloc(LPTR,128);
	lpKeyName=(LPSTR)GlobalAlloc(LPTR,MAX_PATH+1);
	lpMESSAGE=(LPSTR)GlobalAlloc(LPTR,100);
	lpStartDir=(LPSTR)GlobalAlloc(LPTR,MAX_PATH+1);
	lpWindowsDirName=(LPSTR)GlobalAlloc(LPTR,MAX_PATH+5);
	lpTempPath=(LPSTR)GlobalAlloc(LPTR,MAX_PATH+2);
	lpComputerName1=(LPSTR)GlobalAlloc(LPTR,COMPUTERNAMELEN);
	lpComputerName2=(LPSTR)GlobalAlloc(LPTR,COMPUTERNAMELEN);
	lpUserName1=(LPSTR)GlobalAlloc(LPTR,COMPUTERNAMELEN);
	lpUserName2=(LPSTR)GlobalAlloc(LPTR,COMPUTERNAMELEN);
	lpSystemtime1=(SYSTEMTIME*)GlobalAlloc(LPTR,sizeof(SYSTEMTIME));
	lpSystemtime2=(SYSTEMTIME*)GlobalAlloc(LPTR,sizeof(SYSTEMTIME));
	lpCurrentTranslator=lpMyName;

	GetWindowsDirectory(lpWindowsDirName,MAX_PATH);
	int nLengthofStr=lstrlen(lpWindowsDirName);
	if (nLengthofStr>0&&*(lpWindowsDirName+nLengthofStr-1)=='\\')
		*(lpWindowsDirName+nLengthofStr-1)=0x00;
	GetTempPath(MAX_PATH,lpTempPath);

	lpStartDir=GetCommandLine();
	lpIni=strrchr(lpStartDir,'\\');lpStartDir++;*++lpIni=0x0;

	lpIni=(LPSTR)GlobalAlloc(LPTR,MAX_PATH*2);
	lstrcpy(lpIni,lpStartDir);
	lstrcat(lpIni,REGSHOTLANGUAGEFILE);

	lpFreeStrings=(LPSTR)GlobalAlloc(LMEM_FIXED,256);
	ldwTempStrings=(LPDWORD)GlobalAlloc(LPTR,4*60); //max is 60 strings
			
	lpLastSaveDir=lpOutputpath;
	lpLastOpenDir=lpOutputpath;
			
	lpRegshotIni=(LPSTR)GlobalAlloc(LPTR,MAX_PATH);
	lstrcpy(lpRegshotIni,lpStartDir);
	if (*(lpRegshotIni+lstrlen(lpRegshotIni)-1)!='\\')
		lstrcat(lpRegshotIni,"\\");
	lstrcat(lpRegshotIni,REGSHOTINI);
}

///* Shortcomings of Regshot
//
//1) It was written in C,not in OOL as MFC or something,So it
//   is rather difficult to read and understand;and it is not easy
//   to Export.[But it is so small,30K~50K,the early versions have
//   some ASM codes,only 20K]
//
//2) The memory allocate API[GlobalAlloc()] I used was so slow under Windows NT4,
//   I should use VisulAlloc() or something that would allocate a big block of memory 
//   rather than thousands of small pieces of memory.And if I do that,the ReAlign()
//   would be useless,and the whole program will be super fast!
//
//3) Currently,files are not save in hive file.
//
// *********************************************************
// *      Code by  TiANWEi[Huainan,Anhui,China]            *
// *********************************************************/

#endif