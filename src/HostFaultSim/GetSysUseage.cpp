#include "GetSysUseage.h"
#ifdef _WINDOWS
#pragma  comment(lib,"pdh") 
#include <malloc.h> 
#include <locale.h> 
#include <pdh.h> 
#include <pdhmsg.h> 
#include <tchar.h> 
#endif
CGetSysUseage::CGetSysUseage(void)
{
	m_bStop = false;
	activate();
}
CGetSysUseage::~CGetSysUseage(void)
{
}

void CGetSysUseage::GetSysUseage(double &CPUUsage,double &MemUsage,unsigned long &IOKByte)
{
	CPUUsage = m_CPUUsage;
	MemUsage = m_MemUsage;
	IOKByte = m_IOKByte;
}
int CGetSysUseage::svc()
{
#ifdef _WINDOWS
	//系统内存总数     
	MEMORYSTATUSEX statex;       
	statex.dwLength = sizeof(statex);      
	GlobalMemoryStatusEx(&statex);       
	//m_lSysTotalMem = statex.ullTotalPhys / 1024; 
	HQUERY  hQuery;       
	HCOUNTER hcSysAvailableMem, hcSysCpu,hcSysIO,hcWSysIO,hcRSysIO;             
	PDH_STATUS lStatus = PdhOpenQuery(NULL, NULL, &hQuery);       
	if (lStatus != ERROR_SUCCESS)     
	{        
		return -1;     
	}       
	PdhAddCounter(hQuery, _T("\\Memory\\Available Bytes"), NULL, &hcSysAvailableMem);//可用内存       
	PdhAddCounter(hQuery, _T("\\Processor(_Total)\\% Processor Time"), NULL, &hcSysCpu);       
	//PdhAddCounter(hQuery, _T("\\Process(_Total)\\IO Data Bytes/sec"), NULL, &hcSysIOByte); 
	//PdhAddCounter(hQuery, _T("\\PhysicalDisk(_Total)\\% Disk Time"), NULL, &hcSysIO); 
	//PdhAddCounter(hQuery, _T("\\PhysicalDisk(_Total)\\Avg. Disk Bytes/Transfer"), NULL, &hcSysIO); 
	//PdhAddCounter(hQuery, _T("\\PhysicalDisk(_Total)\\Disk Bytes/sec"), NULL, &hcSysIO);
	PdhAddCounter(hQuery, _T("\\PhysicalDisk(_Total)\\Avg. Disk Bytes/Write"), NULL, &hcWSysIO);
	PdhAddCounter(hQuery, _T("\\PhysicalDisk(_Total)\\Avg. Disk Bytes/Read"), NULL, &hcRSysIO);
	PDH_FMT_COUNTERVALUE cv;  
	while (1)
	{ 
		DWORD dTime1 = GetTickCount();
		memset(&cv,0,sizeof(PDH_FMT_COUNTERVALUE));       
		lStatus = PdhCollectQueryData(hQuery);      
		if (lStatus != ERROR_SUCCESS)     
		{        
			return -1;     
		}     
		// CPU时间，必须等待一下     
		Sleep(100);      
		lStatus = PdhCollectQueryData(hQuery);       
		if (lStatus != ERROR_SUCCESS)     
		{         
			return -1;     
		}       

		//可用内存
		lStatus = PdhGetFormattedCounterValue(hcSysAvailableMem, PDH_FMT_LONG, NULL, &cv);       
		if (lStatus == ERROR_SUCCESS)     
		{         
			m_MemUsage = ((double)cv.longValue/(double)statex.ullTotalPhys) * 100;    
		} 
		//CPU
		lStatus = PdhGetFormattedCounterValue(hcSysCpu, PDH_FMT_LONG, NULL, &cv);       
		if (lStatus == ERROR_SUCCESS)     
		{         
			m_CPUUsage = cv.longValue;    
		}
		//IO
// 		lStatus = PdhGetFormattedCounterValue(hcSysIO, PDH_FMT_LONG, NULL, &cv);       
// 		if (lStatus == ERROR_SUCCESS)     
// 		{         
// 			m_IOKByte = cv.longValue;    
// 		}
		lStatus = PdhGetFormattedCounterValue(hcWSysIO, PDH_FMT_LONG, NULL, &cv);       
		if (lStatus == ERROR_SUCCESS)     
		{         
			m_IOKByte = cv.longValue;    
		}
		lStatus = PdhGetFormattedCounterValue(hcRSysIO, PDH_FMT_LONG, NULL, &cv);       
		if (lStatus == ERROR_SUCCESS)     
		{         
			m_IOKByte += cv.longValue/1024;    
		}


		if (m_bStop)   //线程终止
		{
			m_bStop = false;
			break;
		}
		DWORD dTime2 = GetTickCount();
		if (dTime2 - dTime1 < 2000)   //2s周期
		{
			Sleep(2000 - (dTime2 - dTime1));
		}
	}
	PdhRemoveCounter(hcSysAvailableMem);      
	PdhRemoveCounter(hcSysCpu);      
	PdhRemoveCounter(hcWSysIO);  
	PdhRemoveCounter(hcRSysIO);     
	PdhCloseQuery(hQuery);
#else

#endif
	return 0;
}

void CGetSysUseage::StopThread()
{
	m_bStop = true;
}