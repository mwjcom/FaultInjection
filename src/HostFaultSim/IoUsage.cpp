#include "IoUsage.h"
#ifdef _WINDOWS
#pragma  comment(lib,"pdh") 
#include <malloc.h> 
#include <locale.h> 
#include <pdh.h> 
#include <pdhmsg.h> 
#include <tchar.h> 
#else
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <vector>
using namespace std;
CIoUsage::CIoUsage(void)
{
	m_IoUsage = 0;
	m_IoWCount = 0;
	m_IoRCount = 0;
	m_bStop = false;
}


CIoUsage::~CIoUsage(void)
{
}


int CIoUsage::SetIoUsage1(bool bIsSet,unsigned long Usage)
{
	int iRet = 0;
	//m_QM.lock();
	m_IoUsage = Usage;

	if (bIsSet == true )
	{
		//start();
		activate();
	}
	else
	{
		m_bStop = true;
	}
	//m_QM.unlock();
	return iRet;
}
unsigned long CIoUsage::GetIoCount1()
{
	unsigned long IOKByte = 0;
#ifdef _WINDOWS
	//系统内存总数     
	MEMORYSTATUSEX statex;       
	statex.dwLength = sizeof(statex);      
	GlobalMemoryStatusEx(&statex);       
	//m_lSysTotalMem = statex.ullTotalPhys / 1024; 
	HQUERY  hQuery;       
	HCOUNTER hcWSysIO,hcRSysIO;             
	PDH_STATUS lStatus = PdhOpenQuery(NULL, NULL, &hQuery);       
	if (lStatus != ERROR_SUCCESS)     
	{      
		//TODO 报告错误日志
		return IOKByte;     
	}       
	PdhAddCounter(hQuery, _T("\\PhysicalDisk(_Total)\\Avg. Disk Bytes/Write"), NULL, &hcWSysIO);
	PdhAddCounter(hQuery, _T("\\PhysicalDisk(_Total)\\Avg. Disk Bytes/Read"), NULL, &hcRSysIO);
	PDH_FMT_COUNTERVALUE cv;  
	DWORD dTime1 = GetTickCount();
	memset(&cv,0,sizeof(PDH_FMT_COUNTERVALUE));       
	lStatus = PdhCollectQueryData(hQuery);         
	if (lStatus != ERROR_SUCCESS)     
	{   
		//报告错误日志
// 		char pLog[MAX_LOG_LEN];
// 		LOG(pLog, "GetIoCount PdhCollectQueryData ret=%l", lStatus);
		return IOKByte;     
	}       
	lStatus = PdhGetFormattedCounterValue(hcWSysIO, PDH_FMT_LONG, NULL, &cv);       
	if (lStatus == ERROR_SUCCESS)     
	{         
		IOKByte = cv.longValue;    
	}
	lStatus = PdhGetFormattedCounterValue(hcRSysIO, PDH_FMT_LONG, NULL, &cv);       
	if (lStatus == ERROR_SUCCESS)     
	{         
		IOKByte += cv.longValue/1024;    
	}
	PdhRemoveCounter(hcWSysIO);  
	PdhRemoveCounter(hcRSysIO);     
	PdhCloseQuery(hQuery);
#else
	std::vector<string> resvec;
	resvec.clear();
	char chCmd[] = "iostat -d";
	FILE *pp = popen(chCmd, "r"); //建立管道
	if (!pp) 
	{
		//报告错误日志
// 		char pLog[MAX_LOG_LEN];
// 		LOG(pLog, "GetIoCount popen iostat -d error");
		return 0;
	}
	char tmp[1024]; //设置一个合适的长度，以存储每一行输出
	while (fgets(tmp, sizeof(tmp), pp) != NULL) 
	{
		if (tmp[strlen(tmp) - 1] == '\n') 
		{
			tmp[strlen(tmp) - 1] = '\0'; //去除换行符
		}
		resvec.push_back(tmp);
	}
	if (resvec.size() >= 3)
	{
		char name[50];
		float ftps;
                float IORByte = 0;
                float IOWByte = 0;
		unsigned long TIORByte = 0;
		unsigned long TIOWByte = 0;
                string sIOInfo = resvec[3];
                sscanf (sIOInfo.c_str(), "%s%f%f%f%u%u", name, &ftps,&IORByte, &IOWByte,&TIORByte, &TIOWByte);
                IOKByte = (unsigned long)(IORByte + IOWByte);
	}
        resvec.clear();
	pclose(pp); //关闭管道

#endif
	return IOKByte;
}

int CIoUsage::SetIoCount1(bool bIsSet,unsigned long ulIOCount,unsigned long ulRIOCount)
{
	int iRet = 0;
	//m_QM.lock();
	m_IoWCount = ulIOCount;
	m_IoRCount = ulRIOCount;
	if (bIsSet == true )
	{
        m_bStop = false;
		activate();

	}
	else
	{
		m_bStop = true;
	}
	//m_QM.unlock();
	unsigned long unIOCount = GetIoCount1();
	return iRet;
}
int  CIoUsage::svc()
{
	int iRet = 0;
	//m_QM.lock();
	if (m_IoUsage != 0)   // 设置IO使用率
	{
		unsigned long ulSize = 1024*1024;
		int x = 10;
        char *Buffer = GetRandomBuffer(ulSize*x);
	#ifdef _WINDOWS
		CHAR* pFileName = "D:\\IO.dat";
		HANDLE FileHandle = CreateFile(pFileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING | FILE_ATTRIBUTE_NORMAL, NULL);
	
		while (1)
		{
			DWORD Writed;
			unsigned ta = GetTickCount();   
			while(GetTickCount() - ta < m_IoUsage*10) 
			{
				WriteFile(FileHandle, Buffer, ulSize*x, &Writed, NULL);
			}  
			SetFilePointer(FileHandle, 0,0, FILE_BEGIN);
			Sleep((100 - m_IoUsage) * 10);  
			if (m_bStop)
			{
				m_bStop = false;
				break;
			}
		}
		CloseHandle(FileHandle);
		DeleteFile(pFileName);
	#else
		FILE *fp;
		if((fp = fopen("IO.dat","r+")) == NULL)
		{
// 			char pLog[MAX_LOG_LEN];
// 			LOG(pLog, " fopen  IO.dat error");
			//printf("open file failer!\n");
			return;
		}
		fseek(fp,0,SEEK_END);

		int busyTime = 10;   //忙的时间
		int idleTime = 0;    //空闲时间
		long timeuse = 0;    //实际循环用的时间
		struct timeval tpstart,tpend; 
		gettimeofday(&tpstart,NULL); //得到当前的系统时间
		while(1)  
		{  
			gettimeofday(&tpstart,NULL); //得到当前的系统时间
			while (timeuse <= busyTime)
			{
				gettimeofday(&tpend,NULL);
				timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + (tpend.tv_usec - tpstart.tv_usec);  
				timeuse /= 1000;               //转换成ms   
				fwrite(Buffer,1,strlen(Buffer),fp);
			}
			rewind(fp);//使指针重新返回到文件的开头
            //idleTime = ((100 * busyTime) / m_dRatio) - busyTime;

			sleep(idleTime / 1000);    //转化成ms
            if (true == m_bStop)
			{
				break;
			}
		}
		fclose(fp);
	#endif
		free(Buffer);
	}
	else if (m_IoWCount != 0 || m_IoRCount != 0)
	{
		unsigned long ulWSize = m_IoWCount*1024;
		unsigned long ulRSize = m_IoRCount*1024;
        char * WBuffer = GetRandomBuffer(ulWSize);
        char * RBuffer = GetRandomBuffer(ulRSize+1024);
	#ifdef _WINDOWS
		//创建写文件
		CHAR* pWFileName = "D:\\IOW.dat";
		HANDLE FileHandleW = CreateFile(pWFileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING | FILE_ATTRIBUTE_NORMAL, NULL);
		//创建读文件
		CHAR* pRFileName = "D:\\IOR.dat";
		HANDLE FileHandleR = CreateFile(pRFileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING | FILE_ATTRIBUTE_NORMAL, NULL);
		SetFilePointer(FileHandleR, 0,0, FILE_BEGIN);
		DWORD Writed;
		WriteFile(FileHandleR, RBuffer, ulRSize+1024, &Writed, NULL);
		FlushFileBuffers(FileHandleR);

		while (1)
		{	
			DWORD Readed;
			DWORD dTime1 = GetTickCount();   
			SetFilePointer(FileHandleW, 0,0, FILE_BEGIN);
			WriteFile(FileHandleW, WBuffer, ulWSize, &Writed, NULL);
			FlushFileBuffers(FileHandleW);
			SetFilePointer(FileHandleR, 0,0, FILE_BEGIN);
			ReadFile(FileHandleR,RBuffer,ulRSize,&Readed,NULL);
			DWORD dTime2 = GetTickCount();
			if ((dTime2 - dTime1) < 1000)
			{
				Sleep(1000- (dTime2 - dTime1));
			}
			  
			if (m_bStop)
			{
				m_bStop = false;
				break;

			}
		}
		CloseHandle(FileHandleW);
		DeleteFile(pWFileName);
		CloseHandle(FileHandleR);
		DeleteFile(pRFileName);
	#else
		//TODO linux
		while(1)  
		{
			long timeuse = 0;    //实际循环用的时间
			struct timeval tpstart,tpend;
			timeuse = 0;
			gettimeofday(&tpstart,NULL); //得到当前的系统时间
			rewind(fpW);//使指针回到文件的开头
			int fd = open ("IOW.dat", O_WRONLY | O_CREAT | O_APPEND, 0660);
			int iWsize = strlen(WBuffer);
			write (fd, WBuffer, strlen (WBuffer));
			write (fd, "\n", 1);
			fsync (fd);
			close (fd);

			gettimeofday(&tpend,NULL);
			timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + (tpend.tv_usec - tpstart.tv_usec);  
			//timeuse /= 1000;               //转换成ms
			if(timeuse < 1000000)
			{
				usleep(1000000 - timeuse);
			}
			if(remove("IOW.dat"))
			{
// 				char pLog[MAX_LOG_LEN];
// 				LOG(pLog, " remove  IOW.dat error");
				//上报日志
			}
			if (true == m_bStop)
			{
				break;
			}

		}

#endif
	free(WBuffer);
	free(RBuffer);
	}
	return iRet;
	//m_QM.unlock();
}
void CIoUsage::StopThread()
{
	m_bStop = true;
}
char * CIoUsage::GetRandomBuffer(unsigned long Len)
{
    char *Buffer = (char *)malloc(Len);
	if (Buffer==0)
	{
		return 0;
	}

	for (unsigned long i = 0; i < Len; i++)
	{
        Buffer[i] = '1';//rand()%256;
	}

	return Buffer;
}

