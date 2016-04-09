
#include "DBAccessBase.h"


CDBAccessBase::CDBAccessBase(void)
	:m_pConnectionPool( NULL )
	,m_iConnCnt( 0 )
	,m_strConnString("")
{
}


CDBAccessBase::~CDBAccessBase(void)
{
}

int CDBAccessBase::StartListener( const char* pchListener )
{
	int iRet = 0;
	return iRet;
}
int CDBAccessBase::StopListener( const char* pchListener )
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::StartService( const char* pchServiceName )
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::StopService( const char* pchServiceName )
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::Connect( const char* pchConnString, int iConnCnt )
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::DisConnect()
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::ChangePWD( const char* pConnString, const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD)
{
	int iRet = -1;
	iRet = Connect(pConnString);
	if (0 == iRet)
	{
		iRet = ChangePWD(pchUserName,pchOldPWD,pchNewPWD);
	}
	return iRet;	
}

int CDBAccessBase::ChangePWD(const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD)
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::RecoverPWD( const char* pchUserName , const char* pchOldPWD )
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::FillTableSpace(const char* pchTabSpaceName)
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::FreeTableSpace(const char* pchTabSpaceName)
{
	int iRet = 0;
	return iRet;
}

int CDBAccessBase::BreakFillTableSpace()
{
	int iRet = 0;
	return iRet;
}
