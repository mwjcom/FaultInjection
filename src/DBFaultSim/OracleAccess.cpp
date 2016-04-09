
#include "ace/OS.h"
#include "OracleAccess.h"
#include "soci.h"
#include "../backends/oracle/soci-oracle.h"

COracleAccess::COracleAccess(void)
	:m_bBreakFill(false)
{
}


COracleAccess::~COracleAccess(void)
{
}

int COracleAccess::StartListener( const char* pchListener )
{
	int iRet = 0;
	std::string strCmd = "lsnrctl start";
	iRet = ACE_OS::system( strCmd.c_str() );		
	if ( 0 == iRet)
	{
		printf("COracleAccess::StartListener(%s): success.\n",strCmd.c_str() );
	}
	else
	{
		printf("COracleAccess::StartListener(%s): failed (%d).\n", strCmd.c_str(),iRet );
	}
	return iRet;
}
int COracleAccess::StopListener( const char* pchListener )
{
	int iRet = 0;
	std::string strCmd = "lsnrctl stop";
	iRet = ACE_OS::system( strCmd.c_str() );		
	if ( 0 == iRet)
	{
		printf("COracleAccess::StopListener(%s): success.\n",strCmd.c_str() );
	}
	else
	{
		printf("COracleAccess::StopListener(%s): failed (%d).\n", strCmd.c_str(),iRet );
	}
	return iRet;
}

int COracleAccess::StartService( const char* pchServiceName )
{
	int iRet = 0;
	if ( NULL != pchServiceName )
	{
		std::string strCmd = "";
#ifdef _WINDOWS
		strCmd = "net start ";
		strCmd += pchServiceName;
#else
		strCmd = "service ";
		strCmd += pchServiceName;
		strCmd += " start";
#endif //!_WINDOWS
		iRet = ACE_OS::system( strCmd.c_str() );		
		if ( 0 == iRet)
		{
			printf("COracleAccess::StartService(%s): success.\n", strCmd.c_str());
		}
		else
		{
			printf("COracleAccess::StartService(%s): failed (%d).\n", strCmd.c_str(),iRet );
		}
	}
	return iRet;
}

int COracleAccess::StopService( const char* pchServiceName )
{
	int iRet = 0;
	if ( NULL != pchServiceName )
	{
		std::string strCmd = "";
#ifdef _WINDOWS
		strCmd = "net stop ";
		strCmd += pchServiceName;
#else
		strCmd = "service ";
		strCmd += pchServiceName;
		strCmd += " stop";
#endif //!_WINDOWS
		iRet = ACE_OS::system( strCmd.c_str() );		
		if ( 0 == iRet)
		{
			printf("COracleAccess::StopService(%s): success.\n", strCmd.c_str() );
		}
		else
		{
			printf("COracleAccess::StopService(%s): failed (%d).\n", strCmd.c_str(),iRet );
		}
	}
	return iRet;
}

int COracleAccess::Connect( const char* pchConnString, int iConnCnt )
{
	int iRet = 0;
	DisConnect();
	m_iConnCnt = iConnCnt;
	m_strConnString = pchConnString;
	if ( NULL == m_pConnectionPool)
	{
		m_pConnectionPool = new soci::connection_pool(iConnCnt);
		if( NULL != m_pConnectionPool )
		{
			try
			{
				for (size_t i = 0; i != m_iConnCnt; i++)
				{
					soci::session& sql = m_pConnectionPool->at(i);
					sql.open(pchConnString);
				}
			}
			catch(soci::oracle_soci_error &e)
			{
				iRet = e.err_num_;
				printf("COracleAccess::Connect[ConnString=%s] oracle_soci_error, errno=%d [errmsg=%s].\n ",
					m_strConnString.c_str() ,e.err_num_, e.what() );
				
			}
			catch (soci::soci_error const &e)
			{
				iRet = -1;
				printf("COracleAccess::Connect[ConnString=%s] soci_error, [errmsg=%s].\n",
					m_strConnString.c_str(),e.what() );
			}
			catch(...)
			{
				iRet = -1;
			}
		}
		else
		{
			iRet = -1;
		}			
	}
	else
	{
		iRet = -1;
	}

	if ( iRet < 0 && NULL != m_pConnectionPool)
	{
		for (size_t i = 0; i != m_iConnCnt; i++)
		{
			soci::session& sql = m_pConnectionPool->at(i);
			sql.close();
		}
		delete m_pConnectionPool;
		m_pConnectionPool = NULL;			
	}
	return iRet;
}

int COracleAccess::DisConnect()
{
	int iRet = 0;
	if( NULL != m_pConnectionPool )
	{
		for (size_t i = 0; i != m_iConnCnt; i++)
		{
			soci::session& sql = m_pConnectionPool->at(i);
			sql.close();
		}
		delete m_pConnectionPool;
		m_pConnectionPool = NULL;
	}
	m_iConnCnt = 0;
	return iRet;
}

int COracleAccess::ChangePWD(const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD)
{
	int iRet = -1;
// 	m_strUserName = "";
// 	m_strOldPWD = "";
	if ( NULL != pchUserName && NULL != pchOldPWD && NULL != pchNewPWD )
	{
		std::string strSQL = "alter user ";
		strSQL += pchUserName;
		strSQL += " identified by ";
		strSQL += pchNewPWD;
		if ( NULL != m_pConnectionPool )
		{
			soci::session * pSession = NULL;
			pSession = new soci::session(*m_pConnectionPool);
			if ( NULL != pSession )
			{
				iRet = Exec( pSession, strSQL);
				if ( ORA_SUCCESS == iRet )
				{
					//! 执行成功
				}
				delete pSession;
				pSession = NULL;
			}
		}
		else
		{
			printf("COracleAccess::ChangePWD ConnectionPool is NULL.\n");
		}

	}
	return iRet;
}

int COracleAccess::RecoverPWD( const char* pchUserName , const char* pchOldPWD )
{
	int iRet = -1;;
	if ( NULL != pchUserName && NULL != pchOldPWD )
	{
		std::string strSQL = "alter user ";
		strSQL += pchUserName;
		strSQL += " identified by ";
		strSQL += pchOldPWD;
		if ( NULL != m_pConnectionPool )
		{
			soci::session * pSession = NULL;
			pSession = new soci::session(*m_pConnectionPool);
			if ( NULL != pSession )
			{				
				iRet = Exec( pSession, strSQL);
				if ( ORA_SUCCESS == iRet )
				{
					//! 执行成功
				}
				delete pSession;
				pSession = NULL;
			}			
		}
		else
		{
			printf("COracleAccess::RecoverPWD ConnectionPool is NULL.\n");
		}

	}
	return iRet;
}

int COracleAccess::FillTableSpace(const char* pchTabSpaceName)
{
	int iRet = -1;
	m_bBreakFill = false;
	std::string strSQL = "create table tab_invader(recordID number(10), recordName char(256)) tablespace ";
	strSQL += pchTabSpaceName;
	if ( NULL != m_pConnectionPool )
	{
		soci::session * pSession = NULL;
		pSession = new soci::session(*m_pConnectionPool);
		if ( NULL != pSession )
		{
			iRet = Exec( pSession, strSQL);
			if (ORA_ERR_OBJEXIST == iRet || ORA_SUCCESS == iRet)
			{
				strSQL = "insert into tab_invader values(1,'abcd')";
				iRet = Exec( pSession, strSQL);
				strSQL = "insert into tab_invader select * from tab_invader";
				while ( false == m_bBreakFill && ORA_SUCCESS == iRet )
				{
					iRet = Exec( pSession, strSQL);
				}

				if ( ORA_ERR_INSUFFICIENTTABSPACE == iRet )
				{
					iRet = 0;
				}
			}
			else if ( ORA_ERR_INSUFFICIENTTABSPACE1 == iRet )
			{
				iRet = 0;
			}
			delete pSession;
			pSession = NULL;
		}
		
	}
	else
	{
		printf("COracleAccess::FillTableSpace ConnectionPool is NULL.\n");
	}

	return iRet;
}

int COracleAccess::FreeTableSpace(const char* pchTabSpaceName)
{
	int iRet = -1;
	std::string strSQL = "drop table tab_invader";
	if ( NULL != m_pConnectionPool )
	{
		soci::session * pSession = NULL;
		pSession = new soci::session(*m_pConnectionPool);
		if ( NULL != pSession )
		{
			iRet = Exec( pSession, strSQL);
			delete pSession;
			pSession = NULL;
		}
	}
	else
	{
		printf("COracleAccess::FreeTableSpace ConnectionPool is NULL.\n");
	}
	return iRet;
}

int COracleAccess::Exec( soci::session * pSession, std::string const & strSQL )
{
	int iRet = 0;
	if ( NULL != pSession && !strSQL.empty())
	{
		try
		{
			*pSession << strSQL ;
			pSession->commit();
		}
		catch (soci::oracle_soci_error const & e)
		{
			iRet = e.err_num_;
			printf("COracleAccess::Exec, oracle_soci_error errno=%d [errmsg=%s] [ sql=%s].\n",
				e.err_num_, e.what(),strSQL.c_str() );

		}
		catch (soci::soci_error const & e)
		{
			iRet = -1;
			printf("COracleAccess::Exec, soci_error [errmsg=%s][ sql=%s].\n",
				e.what(),strSQL.c_str() );

		}
		catch(...)
		{
			iRet = -2;
			printf( "COracleAccess::Exec, error[ sql=%s].\n",strSQL.c_str() );
		}
	}
	return iRet;
}

int COracleAccess::BreakFillTableSpace()
{
	int iRet = 0;
	m_bBreakFill = true;
	return iRet;
}
