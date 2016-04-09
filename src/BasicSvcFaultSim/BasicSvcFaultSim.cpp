
#include "ace/Guard_T.h"
#include "BasicSvcFaultSim.h"
#include "ace/ACE.h"

#ifdef _WINDOWS
#include <Tlhelp32.h>
#endif


CBasicSvcFaultSim* CBasicSvcFaultSim::sm_pSingleton = NULL;
ACE_Thread_Mutex CBasicSvcFaultSim::sm_Mutex ;

CBasicSvcFaultSim::CBasicSvcFaultSim(void)
{
}


CBasicSvcFaultSim::~CBasicSvcFaultSim(void)
{
}

CBasicSvcFaultSim* CBasicSvcFaultSim::GetInstance()
{
	if ( sm_pSingleton == NULL )
	{
		ACE_Guard<ACE_Thread_Mutex> guard(sm_Mutex);
		if ( sm_pSingleton == NULL )
		{
			sm_pSingleton = new CBasicSvcFaultSim();
		}
	}
	return sm_pSingleton;
}

void CBasicSvcFaultSim::GetServiceInfos( std::vector<ServiceInfo>& vSvcInfos )
{
	vSvcInfos.clear();
#ifdef _WINDOWS
	SC_HANDLE SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); //打开系统服务控制器
	if(SCMan == NULL)
	{
		printf("OpenSCManager Eorror.\n");
		return;
	}
	LPENUM_SERVICE_STATUS service_status; //保存系统服务的结构
	DWORD cbBytesNeeded = NULL; 
	DWORD ServicesReturned = NULL;
	DWORD ResumeHandle = NULL;
			
	service_status = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, 1024 * 64);//分配内存，注意大小
			
			
	//获取系统服务的简单信息
	bool ESS = EnumServicesStatus(SCMan, //系统服务句柄
		SERVICE_WIN32, //服务的类型
		SERVICE_STATE_ALL,  //服务的状态
		(LPENUM_SERVICE_STATUS)service_status,  //输出参数，系统服务的结构
		1024 * 64,  // 结构的大小
		&cbBytesNeeded, //输出参数，接收返回所需的服务
		&ServicesReturned, //输出参数，接收返回服务的数量
		&ResumeHandle); //输入输出参数，第一次调用必须为0，返回为0代表成功
	if(ESS == NULL) 
	{
		printf("EnumServicesStatus Eorror.\n");
		return;
	}
	ServiceInfo svcInfo;
	for(int i = 0; i < ServicesReturned; i++)
	{		
		//printf("服务名: %s ", service_status[i].lpDisplayName);
		svcInfo.strSvcName = service_status[i].lpServiceName;
		switch(service_status[i].ServiceStatus.dwCurrentState)// 服务状态
		{
		case SERVICE_CONTINUE_PENDING:
			//printf("状态: 挂起");
			svcInfo.iSvcStatus = SERVICE_CONTINUE_PENDING;
			break;
		case SERVICE_PAUSE_PENDING:
			//printf("状态: 正在暂停 ");
			svcInfo.iSvcStatus = SERVICE_PAUSE_PENDING;
			break;
		case SERVICE_PAUSED:
			//printf("状态: 已暂停 ");
			svcInfo.iSvcStatus = SERVICE_PAUSED;
			break;
		case SERVICE_RUNNING:
			//printf("状态: 运行 ");
			svcInfo.iSvcStatus = SERVICE_RUNNING;
			break;
		case SERVICE_START_PENDING:
			//printf("状态: 启动 ");
			svcInfo.iSvcStatus = SERVICE_START_PENDING;
			break;
		case SERVICE_STOPPED:
			//printf("状态: 停止 ");
			svcInfo.iSvcStatus = SERVICE_STOPPED;
			break;
		default:
			//printf("状态: 未知 ");
			svcInfo.iSvcStatus = 8;
			break;
		}
		vSvcInfos.push_back(svcInfo);
		/*
		LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL; //服务详细信息结构
		SC_HANDLE service_curren = NULL; //当前的服务句柄
		service_curren = OpenService(SCMan, service_status[i].lpServiceName, SERVICE_QUERY_CONFIG); //打开当前服务
		lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 8 * 1024);      //分配内存， 最大为8kb 
				
		if(NULL == QueryServiceConfig(service_curren, lpServiceConfig, 8 * 1024, &ResumeHandle))
		{
			printf("QueryServiceConfig Error/n");
			return;
		}
		printf("路径: %s ", lpServiceConfig->lpBinaryPathName);//服务的路径
		printf("\n");
		CloseServiceHandle(service_curren);//关闭当前服务的句柄
		*/
	}
	CloseServiceHandle(SCMan);//关闭服务管理器的句柄
#else
    std::string strCmd = "service --status-all 2>&1";
	std::vector<std::string> vCmdResult;
	std::string strCmdResult;
	//!  " [ + ]  acpid"
	//!  " [ - ]  anacron"
	//!  " [ ? ]  apport"
    int iRet = ExecCmd(strCmd.c_str(),vCmdResult);
	if ( iRet < 0 )
	{
		printf("GetServiceInfos: ExecCmd failed.(%d)\n", iRet);
	}
	else
	{
		printf("GetServiceInfos: ExecCmd success.(%d)\n", iRet);
		ServiceInfo svcInfo;
		for ( int iCur = 0; iCur < iRet ;iCur++ )
		{
			strCmdResult = vCmdResult[iCur];
            svcInfo.strSvcName = strCmdResult.substr(8);
            char chStatus = strCmdResult.at(3);
            if (chStatus == '+')
			{
				svcInfo.iSvcStatus = 4;
			}
            else if (chStatus == '-')
			{
				svcInfo.iSvcStatus = 1;
			}
            else if (chStatus == '?')
			{
				svcInfo.iSvcStatus = 8;
			}
			else
			{
				svcInfo.iSvcStatus = 8;
			}
			vSvcInfos.push_back(svcInfo);
		}
	}
#endif
}

void CBasicSvcFaultSim::GetProcessInfos(std::vector<ProcessInfo>& vProInfos)
{
	vProInfos.clear();
#ifdef _WINDOWS

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);
	bRet = Process32First(hProcessSnap,&pe32);
	int iCur = 0;
	ProcessInfo procInfo;
	while (bRet)
	{
		bRet = Process32Next(hProcessSnap,&pe32);
		if (  TRUE == bRet  )
		{
			//printf("%d,进程号：%d,进程名：%s\n",iCur, pe32.th32ProcessID,&pe32.szExeFile);
			procInfo.strProcName = pe32.szExeFile;
			procInfo.PID = pe32.th32ProcessID;
			vProInfos.push_back(procInfo);
		}		
		iCur++;
	}

#else
	//! 3217,su
	//! 3218,bash
	//! 3790,ps
	//! 3791,awk
    std::string strCmd = "ps -A|awk 'NR>1{print $1\",\"$4}'";
	std::vector<std::string> vCmdResult;
	std::string strCmdResult;
	std::string strPID;
	int iRet = ExecCmd(strCmd.c_str(),vCmdResult);
	if ( iRet < 0 )
	{
		printf("GetProcessInfos: ExecCmd failed.(%d)\n", iRet);
	}
	else
	{
		printf("GetProcessInfos: ExecCmd success.(%d)\n", iRet);
		ProcessInfo procInfo;
		for ( int iCur = 0; iCur < iRet ;iCur++ )
		{
			strCmdResult = vCmdResult[iCur];
			size_t pos = strCmdResult.find(',');
			if (std::string::npos != pos )
			{
				strPID = strCmdResult.substr(0, pos);
				procInfo.PID = atoi( strPID.c_str() );
				procInfo.strProcName = strCmdResult.substr(pos + 1);
			}
			vProInfos.push_back(procInfo);
		}
	}
#endif
}

int CBasicSvcFaultSim::TerminateProcess( int iPID )
{
	int iRet = ACE::terminate_process( iPID );
	return iRet;
}

int CBasicSvcFaultSim::GetServiceStatus(const char * chServiceName)
{
	int iRet = -1;
#ifdef _WINDOWS

	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwBytesNeeded;


	//! Get a handle to the SCM database
	SC_HANDLE schSCManager = OpenSCManager( 
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SERVICE_QUERY_STATUS);  // query status rights 

	if (NULL == schSCManager) 
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
	}
	else
	{
		SC_HANDLE schService = OpenService( 
			schSCManager,         // SCM database 
			chServiceName,        // name of service 
			SERVICE_QUERY_STATUS );  

		if (schService == NULL)
		{ 
			printf("OpenService failed (%d)\n", GetLastError()); 
			CloseServiceHandle(schSCManager);				
		}
		else
		{
			if (!QueryServiceStatusEx( 
				schService,                     // handle to service 
				SC_STATUS_PROCESS_INFO,         // information level
				(LPBYTE) &ssStatus,             // address of structure
				sizeof(SERVICE_STATUS_PROCESS), // size of structure
				&dwBytesNeeded ) )              // size needed if buffer is too small
			{
				printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager);

			}
			else
			{
				switch(ssStatus.dwCurrentState)
				{
				case SERVICE_CONTINUE_PENDING:
					//printf("状态: 挂起");
					iRet = SERVICE_CONTINUE_PENDING;
					break;
				case SERVICE_PAUSE_PENDING:
					//printf("状态: 正在暂停 ");
					iRet = SERVICE_PAUSE_PENDING;
					break;
				case SERVICE_PAUSED:
					//printf("状态: 已暂停 ");
					iRet = SERVICE_PAUSED;
					break;
				case SERVICE_RUNNING:
					//printf("状态: 运行 ");
					iRet = SERVICE_RUNNING;
					break;
				case SERVICE_START_PENDING:
					//printf("状态: 启动 ");
					iRet = SERVICE_START_PENDING;
					break;
				case SERVICE_STOPPED:
					//printf("状态: 停止 ");
					iRet = SERVICE_STOPPED;
					break;
				default:
					//printf("状态: 未知 ");
					iRet = 8;
					break;
				}
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager);
			}
		}
	}
#else
	FILE *fstream = NULL;
	char buff[512];
	memset(buff,0,sizeof(buff));
    std::string cmd = "service --status-all 2>&1|grep -w ";
	cmd += chServiceName;

	fstream = popen(cmd.c_str(), "r");

	if(fstream != NULL)
	{
		fgets(buff, 512, fstream);
		if ( NULL != buff[0] )
		{
			char chStatus = buff[3];
			if (chStatus == '+')
			{
				iRet = 4;
			}
			else if (chStatus == '-')
			{
				iRet = 1;
			}
			else if (chStatus == '?')
			{
				iRet = 8;
			}
			else
			{
				iRet = 8;
			}
		}
		else
		{
			iRet = 8; // 未知
		}
		pclose(fstream);
	}

#endif //! _WINDOWS
	return iRet;
}


int CBasicSvcFaultSim::ExecCmd(const char *chCmd, std::vector<std::string> & vResult)
{
	int iRet = 0;
	vResult.clear();

#ifdef _WINDOWS

#else
	//!建立管道
    FILE *pp = popen(chCmd, "r");
	if (NULL != pp) 
	{
		char tmp[4096]; //设置一个合适的长度，以存储每一行输出
		while (fgets(tmp, sizeof(tmp), pp) != NULL)
		{
			if (tmp[strlen(tmp) - 1] == '\n')
			{
				tmp[strlen(tmp) - 1] = '\0'; //去除换行符
			}
			vResult.push_back(tmp);
		}
		iRet = vResult.size();
	} 
	else
	{
		iRet = -1;
	}	
	pclose(pp); //关闭管道
#endif	
	return iRet;
}
