#include "NetManager.h"
#include "common.h"
#include "NetSimulates.h"
#include "NetSimMgr.h"

#ifdef WIN32
	#include <Windows.h>
	#include "Psapi.h"  
	#pragma comment (lib,"Psapi.lib")    
	#include <Iprtrmib.h>  
	typedef struct  
	{  
		DWORD dwState;      //连接状态  
		DWORD dwLocalAddr;  //本地地址  
		DWORD dwLocalPort;  //本地端口  
		DWORD dwRemoteAddr; //远程地址  
		DWORD dwRemotePort; //远程端口  
		DWORD dwProcessId;  //进程标识  

	}MIB_TCPEXROW,*PMIB_TCPEXROW;  


	typedef struct  
	{  
		DWORD dwLocalAddr;  //本地地址  
		DWORD dwLocalPort;  //本地端口  
		DWORD dwProcessId;  //进程标识  

	}MIB_UDPEXROW,*PMIB_UDPEXROW;  


	typedef struct  
	{  
		DWORD dwState;      //连接状态  
		DWORD dwLocalAddr;  //本地地址  
		DWORD dwLocalPort;  //本地端口  
		DWORD dwRemoteAddr; //远程地址  
		DWORD dwRemotePort; //远程端口  
		DWORD dwProcessId;  //进程标识  
		DWORD Unknown;      //待定标识  

	}MIB_TCPEXROW_VISTA,*PMIB_TCPEXROW_VISTA;  


	typedef struct  
	{  
		DWORD dwNumEntries;  
		MIB_TCPEXROW table[ANY_SIZE];  

	}MIB_TCPEXTABLE,*PMIB_TCPEXTABLE;  


	typedef struct  
	{  
		DWORD dwNumEntries;  
		MIB_TCPEXROW_VISTA table[ANY_SIZE];  

	}MIB_TCPEXTABLE_VISTA,*PMIB_TCPEXTABLE_VISTA;  


	typedef struct  
	{  
		DWORD dwNumEntries;  
		MIB_UDPEXROW table[ANY_SIZE];  

	}MIB_UDPEXTABLE,*PMIB_UDPEXTABLE;  


	//=====================================================================================//  
	//Name: DWORD AllocateAndGetTcpExTableFromStack()                                      //  
	//                                                                                     //  
	//Descripion: 该函数仅仅只在 Windows XP，Windows Server 2003 下有效                           //  
	//                                                                                     //  
	//=====================================================================================//  
	typedef DWORD (WINAPI *PFNAllocateAndGetTcpExTableFromStack)(  
		PMIB_TCPEXTABLE *pTcpTabel,  
		bool bOrder,  
		HANDLE heap,  
		DWORD zero,  
		DWORD flags  
		);  

	//=====================================================================================//  
	//Name: DWORD AllocateAndGetUdpExTableFromStack()                                      //  
	//                                                                                     //  
	//Descripion: 该函数仅仅只在 XP，Windows Server 2003 下有效                                   //  
	//                                                                                     //  
	//=====================================================================================//  
	typedef DWORD (WINAPI *PFNAllocateAndGetUdpExTableFromStack)(  
		PMIB_UDPEXTABLE *pUdpTable,  
		bool bOrder,  
		HANDLE heap,  
		DWORD zero,  
		DWORD flags  
		);  

	//=====================================================================================//  
	//Name: DWORD InternalGetTcpTable2()                                                   //  
	//                                                                                     //  
	//Descripion: 该函数在 Windows Vista 以及 Windows 7 下面效                                  //  
	//                                                                                     //  
	//=====================================================================================//  
	typedef DWORD (WINAPI *PFNInternalGetTcpTable2)(  
		PMIB_TCPEXTABLE_VISTA *pTcpTable_Vista,  
		HANDLE heap,  
		DWORD flags  
		);  

	//=====================================================================================//  
	//Name: DWORD InternalGetUdpTableWithOwnerPid()                                        //  
	//                                                                                     //  
	//Descripion: 该函数在 Windows Vista 以及 Windows 7 下面效                                  //  
	//                                                                                     //  
	//=====================================================================================//  
	typedef DWORD (WINAPI *PFNInternalGetUdpTableWithOwnerPid)(  
		PMIB_UDPEXTABLE *pUdpTable,  
		HANDLE heap,  
		DWORD flags  
		); 

	typedef DWORD (WINAPI *PFNGetModuleFileNameEx)(
		_In_     HANDLE  hProcess,
		_In_opt_ HMODULE hModule,
		_Out_    LPTSTR  lpFilename,
		_In_     DWORD   nSize
		);
#else
	#include <iostream>
	#include <fstream>
	#include <errno.h>
#endif

extern NETSIM_API CNetManager* g_pNetManager = NULL;

CNetManager::CNetManager(void)
{
	m_bIsSimulating = false;
}


CNetManager::~CNetManager(void)
{
}

#ifdef WIN32
//! 获取系统当前被占用的端口信息
int CNetManager::GetPortInfo(std::map<unsigned int, portInfo*>& mapTcpPorts,
	std::map<unsigned int, portInfo*>& mapUdpPorts)
{
	HMODULE hModule = LoadLibraryW(L"iphlpapi.dll");
	if (hModule == NULL)
	{
		return -1;
	}

	// 表明查询的是 TCP 信息
	PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;
	pAllocateAndGetTcpExTableFromStack = 
		(PFNAllocateAndGetTcpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetTcpExTableFromStack");
	if (pAllocateAndGetTcpExTableFromStack != NULL)
	{
		// 表明为 XP 或者 Server 2003 操作系统
		PMIB_TCPEXTABLE pTcpExTable = NULL;
		if (pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)
		{
			if (pTcpExTable)
			{
				HeapFree(GetProcessHeap(), 0, pTcpExTable);
			}

			FreeLibrary(hModule);
			hModule = NULL;

			return -1;
		}

		for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)
		{
			portInfo* info = new portInfo;
			info->uiPid = pTcpExTable->table[i].dwProcessId;
			info->iLocalPort = pTcpExTable->table[i].dwLocalPort;
			info->nType = TcpType;
			if ( info->uiPid == 4 )
			{
				info->strPname = "System";
			}
			else
			{
				//!获取进程名
				HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info->uiPid);
				if ( hProcess == 0 )
				{
					delete info;
					break;
				}
				char* procName = new char[MAX_PATH];
				GetModuleFileNameEx(hProcess, NULL, (LPWSTR)procName, MAX_PATH);
				info->strPname = procName;
			}

			mapTcpPorts.insert(std::make_pair(info->iLocalPort, info));
		}

		if (pTcpExTable)
		{
			HeapFree(GetProcessHeap(), 0, pTcpExTable);
		}

		FreeLibrary(hModule);
		hModule = NULL;

		return 0;
	}
	else
	{
		// 表明为 Vista 或者 7 操作系统
		PMIB_TCPEXTABLE_VISTA pTcpExTable = NULL;
		PFNInternalGetTcpTable2 pInternalGetTcpTable2 = 
			(PFNInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");
		if (pInternalGetTcpTable2 == NULL)
		{
			if (pTcpExTable)
			{
				HeapFree(GetProcessHeap(), 0, pTcpExTable);
			}

			FreeLibrary(hModule);
			hModule = NULL;

			return 0;
		}

		if (pInternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1))
		{
			if (pTcpExTable)
			{
				HeapFree(GetProcessHeap(), 0, pTcpExTable);
			}
			FreeLibrary(hModule);
			hModule = NULL;
			return 0;
		}

		for (UINT i = 0;i < pTcpExTable->dwNumEntries; i++)
		{
			portInfo* info = new portInfo;
			info->uiPid = pTcpExTable->table[i].dwProcessId;
			info->iLocalPort = pTcpExTable->table[i].dwLocalPort;
			info->nType = TcpType;
			if ( info->uiPid == 4 )
			{
				info->strPname = "System";
			}
			else
			{
				//!获取进程名
				HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info->uiPid);
				if ( hProcess == 0 )
				{
					delete info;
					break;
				}
				char* procName = new char[MAX_PATH];
				GetModuleFileNameEx(hProcess, NULL, (LPWSTR)procName, MAX_PATH);
				//GetModuleBaseName(hProcess, NULL, procName, MAX_PATH);
				info->strPname = procName;
			}

			mapTcpPorts.insert(std::make_pair(info->iLocalPort, info));
		}

		if (pTcpExTable)
		{
			HeapFree(GetProcessHeap(), 0, pTcpExTable);
		}

		FreeLibrary(hModule);
		hModule = NULL;
		return 0;
	}

	// 表明查询的是 UDP 信息
	PMIB_UDPEXTABLE pUdpExTable = NULL;
	PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;
	pAllocateAndGetUdpExTableFromStack = 
		(PFNAllocateAndGetUdpExTableFromStack)GetProcAddress(hModule,"AllocateAndGetUdpExTableFromStack");
	if (pAllocateAndGetUdpExTableFromStack != NULL)
	{
		// 表明为 XP 或者 Server 2003 操作系统
		if (pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)
		{
			if (pUdpExTable)
			{
				HeapFree(GetProcessHeap(), 0, pUdpExTable);
			}

			FreeLibrary(hModule);
			hModule = NULL;

			return -1;
		}

		for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)
		{
			portInfo* info = new portInfo;
			info->uiPid = pUdpExTable->table[i].dwProcessId;
			info->iLocalPort = pUdpExTable->table[i].dwLocalPort;
			info->nType = UdpType;
			if ( info->uiPid == 4 )
			{
				info->strPname = "System";
			}
			else
			{
				//!获取进程名
				HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info->uiPid);
				if ( hProcess == 0 )
				{
					delete info;
					break;
				}
				char* procName = new char[MAX_PATH];
				GetModuleFileNameEx(hProcess, NULL, (LPWSTR)procName, MAX_PATH);
				//GetModuleBaseName(hProcess, NULL, procName, MAX_PATH);
				info->strPname = procName;
			}

			mapUdpPorts.insert(std::make_pair(info->iLocalPort, info));
		}

		if (pUdpExTable)
		{
			HeapFree(GetProcessHeap(), 0, pUdpExTable);
		}

		FreeLibrary(hModule);
		hModule = NULL;

		return 0;
	}
	else
	{
		// 表明为 Vista 或者 7 操作系统
		PFNInternalGetUdpTableWithOwnerPid pInternalGetUdpTableWithOwnerPid;
		pInternalGetUdpTableWithOwnerPid = 
			(PFNInternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");
		if (pInternalGetUdpTableWithOwnerPid != NULL)
		{
			if (pInternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1))
			{
				if (pUdpExTable)
				{
					HeapFree(GetProcessHeap(), 0, pUdpExTable);
				}

				FreeLibrary(hModule);
				hModule = NULL;

				return -1;
			}

			for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)
			{
				portInfo* info = new portInfo;
				info->uiPid = pUdpExTable->table[i].dwProcessId;
				info->iLocalPort = pUdpExTable->table[i].dwLocalPort;
				info->nType = UdpType;
				if ( info->uiPid == 4 )
				{
					info->strPname = "System";
				}
				else
				{
					//!获取进程名
					HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, info->uiPid);
					if ( hProcess == 0 )
					{
						delete info;
						break;
					}
					char* procName = new char[MAX_PATH];
					GetModuleFileNameEx(hProcess, NULL, (LPWSTR)procName, MAX_PATH);
					//GetModuleBaseName(hProcess, NULL, procName, MAX_PATH);
					info->strPname = procName;
				}

				mapUdpPorts.insert(std::make_pair(info->iLocalPort, info));
			}
		}

		if (pUdpExTable)
		{
			HeapFree(GetProcessHeap(), 0, pUdpExTable);
		}

		FreeLibrary(hModule);
		hModule = NULL;

		return 0;
	}

	return 0;
}

#else

//! 获取系统当前被占用的端口信息
int CNetManager::GetPortInfo(std::map<unsigned int, portInfo*>& mapTcpPorts,
                             std::map<unsigned int, portInfo*>& mapUdpPorts)
{
    char pLog[MAX_LOG_LEN];
    char buffer[512];
    //! TCP
    int iRet = system("netstat -anpt4 >/tmp/tcpnet.tmp");
    LOG(pLog, "%s", "netstat -anpt4 >/tmp/tcpnet.tmp");
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return iRet;
    }
    else {
        LOG(pLog, "system Successed!");
    }
    std::ifstream ifTcp("/tmp/tcpnet.tmp");
    if(!ifTcp){
        LOG(pLog, "file /tmp/tcpnet.tmp not found");
        ifTcp.close();
    }
    else{
        //! step over the first two lines
        while (! ifTcp.eof() )
        {            
            ifTcp.getline(buffer, 512);
            QString strBuf(buffer);
            int iIndex = strBuf.indexOf(':');
            if( iIndex != -1 )
            {
                portInfo* infoTcp = new portInfo;
                QString strRight = strBuf.right(strBuf.length()-iIndex-1);
                iIndex = strRight.indexOf(' ');
                if ( iIndex != -1 )
                {
                    infoTcp->iLocalPort = strRight.left(iIndex).toUInt();
                }
                iIndex = strRight.lastIndexOf('/');
                if ( iIndex != -1 )
                {
                    infoTcp->strPname = strRight.right(strRight.length()-iIndex-1).trimmed().toStdString();
                    strBuf = strRight.left(iIndex);
                    iIndex = strBuf.lastIndexOf(' ');
                    if( iIndex != -1 )
                    {
                        infoTcp->uiPid = strBuf.right(strBuf.length()-iIndex-1).toUInt();
                    }
                }
                infoTcp->nType = TcpType;
                mapTcpPorts.insert(std::make_pair(infoTcp->iLocalPort, infoTcp));
            }

        }
        ifTcp.close();
    }

    //! UDP
    iRet = system("netstat -anpu4 >/tmp/udpnet.tmp");
    LOG(pLog, "%s", "netstat -anpu4 >/tmp/udpnet.tmp");
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return iRet;
    }
    else {
        LOG(pLog, "system Successed!");
    }
    std::ifstream ifUdp("/tmp/udpnet.tmp");
    if(!ifUdp){
        LOG(pLog, "file /tmp/udpnet.tmp not found");
        ifUdp.close();
    }
    else{
        //! step over the first two lines
        while (! ifUdp.eof() )
        {
            ifUdp.getline(buffer,512);
            QString strBuf(buffer);
            int iIndex = strBuf.indexOf(':');
            if( iIndex != -1 )
            {
                portInfo* infoUdp = new portInfo;
                QString strRight = strBuf.right(strBuf.length()-iIndex-1);
                iIndex = strRight.indexOf(' ');
                if ( iIndex != -1 )
                {
                    infoUdp->iLocalPort = strRight.left(iIndex).toUInt();
                }
                iIndex = strRight.lastIndexOf('/');
                if ( iIndex != -1 )
                {
                    infoUdp->strPname = strRight.right(strRight.length()-iIndex-1).trimmed().toStdString();
                    strBuf = strRight.left(iIndex);
                    iIndex = strBuf.lastIndexOf(' ');
                    if( iIndex != -1 )
                    {
                        infoUdp->uiPid = strBuf.right(strBuf.length()-iIndex-1).toUInt();
                    }
                }
                infoUdp->nType = UdpType;
                mapUdpPorts.insert(std::make_pair(infoUdp->iLocalPort, infoUdp));
            }
        }
        ifUdp.close();
    }
	return 0;
}

#endif

int CNetManager::PortsOccupy( std::vector<int> vtPorts )
{
	for (int iRow = 0; iRow < vtPorts.size(); iRow++)
	{
		//! 判断端口号是否合法
		if ( vtPorts[iRow] < 1 || vtPorts[iRow] > 65534 )
		{
//			LOG(pLog, "端口号 %d 不合法，不能被占用！", listPort[iRow]);
			break;
		}
#ifdef WIN32
		//! ///创建套接字
		SOCKET Listen_Sock = socket(AF_INET, SOCK_STREAM, 0);
		if (Listen_Sock == INVALID_SOCKET)
		{
//			LOG(pLog, "创建套接字失败！");
			break;
		}
#else
		int Listen_Sock = socket(AF_INET, SOCK_STREAM, 0);
		if (Listen_Sock == -1)
		{
			LOG(pLog, "创建套接字失败！");
			break;
		}
#endif

#ifdef WIN32
		SOCKADDR_IN serverAddr;
		ZeroMemory((char *)&serverAddr,sizeof(serverAddr));
#else
		struct sockaddr_in serverAddr;
#endif
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(vtPorts[iRow]);        /*本地监听端口:1234*/
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);		/*有IP*/
#ifdef WIN32
		//绑定
		if(bind(Listen_Sock, (SOCKADDR *)&serverAddr, sizeof(SOCKADDR)) != 0)
		{
			//LOG(pLog, "绑定端口 %d 失败！", vtPorts[iRow]);
			break;
		}

		if ( SOCKET_ERROR == listen(Listen_Sock, 1) )
		{
			//LOG(pLog, "监听端口 %d 失败，错误码：%d！", vtPorts[iRow], WSAGetLastError());
			break;
		}
		else
		{
			//LOG(pLog, "占用网络端口 %d 成功！", vtPorts[iRow]);
		}

		socketList.insert(std::map<int,SOCKET>::value_type(vtPorts[iRow],Listen_Sock));
#else
		//绑定
		if(bind(Listen_Sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		{
			//LOG(pLog, "绑定端口 %d 失败！", vtPorts[iRow]);
			break;
		}

		if ( -1 == listen(Listen_Sock, 1) )
		{
			//LOG(pLog, "监听端口 %d 失败，错误：%s！(errno:%d)", listPort[iRow], strerror(errno), errno);
			break;
		}
		else
		{
			//LOG(pLog, "占用网络端口 %d 成功！", vtPorts[iRow]);
		}

		socketList.insert(std::map<int,int>::value_type(vtPorts[iRow],Listen_Sock));
#endif
	}
	return 0;
}

int CNetManager::PortsRelease( std::vector<int> vtPorts )
{
	//char pLog[MAX_LOG_LEN];
	for (int iRow = 0; iRow < vtPorts.size(); iRow++)
	{
#ifdef WIN32
		std::map<int, SOCKET>::iterator it = socketList.find(vtPorts[iRow]);
		if ( it != socketList.end() )
		{
			closesocket(it->second);
			//LOG(pLog, "释放网络端口 %d 成功！", listPort[iRow]);
		}
#else
		std::map<int, int>::iterator it = socketList.find(vtPorts[iRow]);
		if ( it != socketList.end() )
		{
			::close(it->second);
			//LOG(pLog, "释放网络端口 %d 成功！", listPort[iRow]);
		}
#endif
	}
	return 0;
}

int CNetManager::StartNetSim(NetSimParam& simParam)
{
	//g_simParam = simParam;
	if ( m_bIsSimulating )
	{
		return -99;
	}

	char buf[MSG_BUFSIZE];
	std::string pch = simParam.strFilter;
#ifndef WIN32
	char pchTemp[MSG_BUFSIZE];
	//!
	std::string pOutOpr = "tc qdisc add dev eth0 parent 1:4 handle 40: netem";
	std::string pInOpr = "tc qdisc add dev ifb0 parent 1:4 handle 40: netem";
	if(simParam.ood.bEnable && simParam.lag.bEnable)
	{
		//! in
		if(ui.oodin->checkState() == Qt::Checked && ui.lagin->checkState() == Qt::Checked) {
			sprintf(pchTemp, " delay %sms reorder %s%% 50%%",
				ui.lagtime->text().toStdString().c_str(),
				ui.oodchance->text().toStdString().c_str());
		}
		else if(ui.oodin->checkState() == Qt::Checked) {
			sprintf(pchTemp, " delay 10ms reorder %s%% 50%%",
				ui.oodchance->text().toStdString().c_str());
		}
		else if(ui.lagin->checkState() == Qt::Checked) {
			sprintf(pchTemp, " delay %sms", ui.lagtime->text().toStdString().c_str());
		}
		pInOpr += std::string(pchTemp);

		//! out
		if(ui.oodout->checkState() == Qt::Checked && ui.lagout->checkState() == Qt::Checked) {
			sprintf(pchTemp, " delay %sms reorder %s%% 50%%",
				ui.lagtime->text().toStdString().c_str(),
				ui.oodchance->text().toStdString().c_str());
		}
		else if(ui.oodout->checkState() == Qt::Checked) {
			sprintf(pchTemp, " delay 10ms reorder %s%% 50%%",
				ui.oodchance->text().toStdString().c_str());
		}
		else if(ui.lagout->checkState() == Qt::Checked) {
			sprintf(pchTemp, " delay %sms", ui.lagtime->text().toStdString().c_str());
		}
		pOutOpr += std::string(pchTemp);
	}
	else if(LagEnable())
	{
		sprintf(pchTemp, " delay %sms", ui.lagtime->text().toStdString().c_str());
		if(ui.lagin->checkState() == Qt::Checked)
			pInOpr += std::string(pchTemp);
		if(ui.lagout->checkState() == Qt::Checked)
			pOutOpr += std::string(pchTemp);
	}
	else if(OODEnable())
	{
		sprintf(pchTemp, " delay 10ms reorder %s%% 50%%",
			ui.oodchance->text().toStdString().c_str());
		if(ui.lagin->checkState() == Qt::Checked)
			pInOpr += std::string(pchTemp);
		if(ui.lagout->checkState() == Qt::Checked)
			pOutOpr += std::string(pchTemp);
	}

	if(DropEnable())
	{
		sprintf(pchTemp, " loss %s%%", ui.dropchance->text().toStdString().c_str());
		if(ui.lagin->checkState() == Qt::Checked)
			pInOpr += std::string(pchTemp);
		if(ui.lagout->checkState() == Qt::Checked)
			pOutOpr += std::string(pchTemp);
	}
	if(DuplicateEnable())
	{
		sprintf(pchTemp, " duplicate %s %s%%",
			ui.dupcount->text().toStdString().c_str(),
			ui.dupchance->text().toStdString().c_str());
		if(ui.lagin->checkState() == Qt::Checked)
			pInOpr += std::string(pchTemp);
		if(ui.lagout->checkState() == Qt::Checked)
			pOutOpr += std::string(pchTemp);
	}
	if(TamperEnable())
	{
		sprintf(pchTemp, " corrupt %s%%",
			ui.tamperchance->text().toStdString().c_str());
		if(ui.lagin->checkState() == Qt::Checked)
			pInOpr += std::string(pchTemp);
		if(ui.lagout->checkState() == Qt::Checked)
			pOutOpr += std::string(pchTemp);
	}
	if(ThrottleEnable())
	{
	}
	
	sprintf(buf, "%s", pOutOpr.c_str());
	//LOG(pLog, "%s", pOpr.c_str());
	int iRet = g_pNetSimMgr->start(pch.c_str(), buf, pInOpr.c_str());
#else
	int iRet = g_pNetSimMgr->start(pch.c_str(), buf);
#endif


	if ( iRet != 0 )		//! 启动失败
	{
		return -1;
	}

	m_bIsSimulating = true;
	return 0;
}
int CNetManager::StopNetSim(NetSimParam& simParam)
{
	g_pNetSimMgr->stop();
	m_bIsSimulating = false;
	return 0;
}