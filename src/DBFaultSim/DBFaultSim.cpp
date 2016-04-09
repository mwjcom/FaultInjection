
#include "ace/Guard_T.h"
#include "DBFaultSim.h"
#include "OracleAccess.h"
#include "MySQLAccess.h"

CDBFaultSim* CDBFaultSim::sm_pSingleton = NULL;
ACE_Thread_Mutex CDBFaultSim::sm_Mutex ;

CDBFaultSim::CDBFaultSim(void)
	:m_strDBType("")
	,m_pDBAccess(NULL)
{
}


CDBFaultSim::~CDBFaultSim(void)
{
}

CDBFaultSim* CDBFaultSim::GetInstance()
{
	if ( sm_pSingleton == NULL )
	{
		ACE_Guard<ACE_Thread_Mutex> guard(sm_Mutex);
		if ( sm_pSingleton == NULL )
		{
			sm_pSingleton = new CDBFaultSim();
		}
	}
	return sm_pSingleton;
}

int CDBFaultSim::SetDBType(const char* pchDBType )
{
	int iRet = 0;
	if ( NULL == pchDBType )
	{
		iRet = -1;
	}
	else
	{
		m_strDBType = pchDBType;
		printf("SetDBType[%s] .\n", m_strDBType.c_str() );
		if (m_strDBType == "Oracle")
		{
			m_pDBAccess = new COracleAccess();
		}
		else if ( m_strDBType == "MySQL")
		{
			m_pDBAccess = new CMySQLAccess();
		}
		else if ( m_strDBType == "SQLServer" )
		{
			iRet = -2;
		} 
		else
		{
			iRet = -3;
		}		
	}
	return iRet;
}

int CDBFaultSim::StartListener( const char* pchListener )
{
	int iRet = 0;
	if ( NULL == pchListener )
	{
		printf("Start Listener .\n");
		iRet = -1;
	}
	else
	{
		if ( NULL != m_pDBAccess )
		{
			iRet = m_pDBAccess->StartListener( pchListener );
		}
		else
		{
			iRet = -1;
		}
		printf("Start Listener[%s] Ret[%d] .\n", pchListener,iRet );
	}
	return iRet;
}

int CDBFaultSim::StopListener( const char* pchListener )
{
	int iRet = 0;
	if ( NULL == pchListener )
	{
		printf("Stop Listener.\n");
		iRet = -1;
	}
	else
	{
		if ( NULL != m_pDBAccess )
		{
			iRet = m_pDBAccess->StopListener( pchListener );
		}
		else
		{
			iRet = -1;
		}
		printf("Stop Listener[%s] Ret[%d] .\n", pchListener,iRet );
	}
	return iRet;
}

int CDBFaultSim::StartService( const char* pchServiceName )
{
	int iRet = 0;
	if ( NULL == pchServiceName )
	{
		printf("Start Service.\n");
		iRet = -1;
	}
	else
	{
		if ( NULL != m_pDBAccess )
		{
			iRet = m_pDBAccess->StartService( pchServiceName );
		}
		else
		{
			iRet = -1;
		}
		printf("Start Service[%s] Ret[%d] .\n", pchServiceName,iRet );
	}
	return iRet;
}

int CDBFaultSim::StopService( const char* pchServiceName )
{
	int iRet = 0;
	if ( NULL == pchServiceName )
	{
		printf("Stop Service.\n");
		iRet = -1;
	}
	else
	{
		if ( NULL != m_pDBAccess )
		{
			iRet = m_pDBAccess->StopService( pchServiceName );
		}
		else
		{
			iRet = -1;
		}
		printf("Stop Service[%s] Ret[%d] .\n", pchServiceName,iRet );
	}
	return iRet;
}

int CDBFaultSim::Connect(const char* pchConnString, int iConnCnt)
{
	int iRet = 0;
	if ( NULL == pchConnString )
	{
		printf("Connect.\n");
		iRet = -1;
	}
	else
	{
		if ( NULL != m_pDBAccess )
		{
			iRet = m_pDBAccess->Connect( pchConnString );
		}
		else
		{
			iRet = -1;
		}
		printf("Connect[%s ,%d ] Ret[%d] .\n", pchConnString,iConnCnt,iRet );
	}
	return iRet;
}

int CDBFaultSim::DisConnect()
{
	int iRet = 0;
	if ( NULL != m_pDBAccess )
	{
		iRet = m_pDBAccess->DisConnect();
	}
	else
	{
		iRet = -1;
	}
	printf("DisConnect Ret[%d].\n",iRet);
	return iRet;
}

int CDBFaultSim::ChangePWD( const char* pConnString, const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD)
{
	int iRet = 0;
	if ( NULL == pConnString || NULL == pchUserName || NULL == pchOldPWD || pchNewPWD == NULL )
	{
		printf("ChangePWD.\n");
		iRet = -1;
	}
	else
	{
		if ( NULL != m_pDBAccess )
		{
			iRet = m_pDBAccess->ChangePWD(pConnString,pchUserName,pchOldPWD,pchNewPWD);
		}
		else
		{
			iRet = -1;
		}
		printf("ChangePWD[%s ,%s ,%s ,%s ] Ret[%d] .\n", pConnString,pchUserName,pchOldPWD,pchNewPWD,iRet );
	}
	return iRet;
}

int CDBFaultSim::RecoverPWD( const char* pchUserName , const char* pchOldPWD)
{
	int iRet = 0;	
	if ( NULL != m_pDBAccess )
	{
		iRet = m_pDBAccess->RecoverPWD(pchUserName, pchOldPWD );
	}
	else
	{
		iRet = -1;
	}
	printf("RecoverPWD Ret[%d] .\n", iRet );
	return iRet;
}

int CDBFaultSim::FillTableSpace(const char* pchTabSpaceName)
{
	int iRet = 0;
	if ( NULL == pchTabSpaceName )
	{
		printf("FillTableSpace.\n");
		iRet = -1;
	}
	else
	{
		if ( NULL != m_pDBAccess )
		{
			iRet = m_pDBAccess->FillTableSpace( pchTabSpaceName );
		}
		else
		{
			iRet = -1;
		}
		printf("FillTableSpace[%s] Ret[%d] .\n", pchTabSpaceName,iRet );
	}
	return iRet;
}

int CDBFaultSim::FreeTableSpace(const char* pchTabSpaceName)
{ 
	int iRet = 0;
	if ( NULL == pchTabSpaceName )
	{
		printf("FreeTableSpace.\n");
		iRet = -1;
	}
	else
	{
		if ( NULL != m_pDBAccess )
		{
			iRet = m_pDBAccess->FreeTableSpace( pchTabSpaceName );
		}
		printf("FreeTableSpace[%s] Ret[%d].\n", pchTabSpaceName,iRet );
	}
	return iRet;
}

CTestTask::CTestTask(void)
	:m_bShutDown(false)
{
}

CTestTask::~CTestTask(void)
{
	
}

// bool CTestTask::run()
// {
// 	// 根据主从判断
// 	return true;
// }






