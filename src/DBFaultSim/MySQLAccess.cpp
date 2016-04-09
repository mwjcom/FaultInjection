
#include "ace/OS.h"
#include "MySQLAccess.h"
#include "soci.h"
#include "../backends/mysql/soci-mysql.h"

CMySQLAccess::CMySQLAccess(void)
{
}


CMySQLAccess::~CMySQLAccess(void)
{
}

int CMySQLAccess::StartService( const char* pchServiceName )
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
			printf("CMySQLAccess::StartService(%s): failed (%d).", strCmd.c_str(),iRet );
		}
	}

	return iRet;
}

int CMySQLAccess::StopService( const char* pchServiceName )
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
			printf("CMySQLAccess::StopService(%s): failed (%d).", strCmd.c_str(),iRet );
		}
	}
	return iRet;
}

int CMySQLAccess::Connect( const char* pchConnString, int iConnCnt )
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
			catch(soci::mysql_soci_error &e)
			{
				iRet = e.err_num_;
				printf("CMySQLAccess::Connect[ConnString=%s] mysql_soci_error, errno=%d [errmsg=%s] ",
					m_strConnString.c_str() ,e.err_num_, e.what() );

			}
			catch (soci::soci_error const &e)
			{
				iRet = -1;
				printf("CMySQLAccess::Connect[ConnString=%s] soci_error, [errmsg=%s]",
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

int CMySQLAccess::DisConnect()
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

int CMySQLAccess::ChangePWD(const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD)
{
	int iRet = -1;
	m_strUserName = "";
	m_strOldPWD = "";
	if ( NULL != pchUserName && NULL != pchOldPWD && NULL != pchNewPWD )
	{
		//! use mysql 
		//! update user set password=password('mysql2') where user='mysql';
		//! flush privileges;		
		if ( NULL != m_pConnectionPool )
		{
			soci::session * pSession = NULL;
			pSession = new soci::session(*m_pConnectionPool);
			if ( NULL != pSession )
			{
				std::string strSQL = "update mysql.user set password=password('";
				strSQL += pchNewPWD;
				strSQL += "') where user='";
				strSQL += pchUserName;
				iRet = Exec( pSession, strSQL);
				if ( MYSQL_SUCCESS == iRet )
				{
					strSQL = "flush privileges;"
;					iRet = Exec( pSession, strSQL);
					if ( MYSQL_SUCCESS == iRet )
					{
						m_strUserName = pchUserName;
						m_strOldPWD = pchOldPWD;
					}					
				}
				delete pSession;
				pSession = NULL;
			}
		}
		else
		{
			printf("CMySQLAccess::ChangePWD ConnectionPool is NULL \n");
		}

	}
	return iRet;
}

int CMySQLAccess::RecoverPWD( const char* pchUserName , const char* pchOldPWD )
{
	int iRet = -1;;
	if ( false == m_strOldPWD.empty() )
	{
		if ( NULL != m_pConnectionPool )
		{
			soci::session * pSession = NULL;
			if ( NULL != pSession )
			{
				std::string strSQL = "update mysql.user set password=password('";
				strSQL += m_strOldPWD;
				strSQL += "') where user='";
				strSQL += m_strUserName;
				iRet = Exec( pSession, strSQL);
				if ( MYSQL_SUCCESS == iRet )
				{
					strSQL = "flush privileges;";
					iRet = Exec( pSession, strSQL);
					if ( MYSQL_SUCCESS == iRet )
					{
						m_strUserName = "";
						m_strOldPWD = "";
					}					
				}
				delete pSession;
				pSession = NULL;
			}			
		}
		else
		{
			printf("COracleAccess::RecoverPWD ConnectionPool is NULL \n");
		}

	}
	return iRet;
}

int CMySQLAccess::FillTableSpace(const char* pchTabSpaceName)
{
	int iRet = -1;
// 	m_bBreakFill = false;
// 	std::string strSQL = "create table tab_invader(recordID int, recordName char(256)) tablespace ";
// 	strSQL += pchTabSpaceName;
// 	if ( NULL != m_pConnectionPool )
// 	{
// 		soci::session * pSession = NULL;
// 		pSession = new soci::session(*m_pConnectionPool);
// 		if ( NULL != pSession )
// 		{
// 			iRet = Exec( pSession, strSQL);
// 			if (MYSQL_SUCCESS == iRet || MYSQL_SUCCESS == iRet)
// 			{
// 				strSQL = "insert into tab_invader values(1,'abcd')";
// 				iRet = Exec( pSession, strSQL);
// 				strSQL = "insert into tab_invader select * from tab_invader";
// 				while ( false == m_bBreakFill && MYSQL_SUCCESS == iRet )
// 				{
// 					iRet = Exec( pSession, strSQL);
// 				}
// 
// 				if ( MYSQL_ERR_INSUFFICIENTTABSPACE == iRet)
// 				{
// 					iRet = 0;
// 				}
// 			}
// 			delete pSession;
// 			pSession = NULL;
// 		}
// 
// 	}
// 	else
// 	{
// 		printf("CMySQLAccess::FillTableSpace ConnectionPool is NULL \n");
// 	}

	return iRet;
}

int CMySQLAccess::FreeTableSpace(const char* pchTabSpaceName)
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
		printf("CMySQLAccess::FreeTableSpace ConnectionPool is NULL \n");
	}
	return iRet;
}

int CMySQLAccess::Exec( soci::session * pSession, std::string const & strSQL )
{
	int iRet = 0;
	if ( NULL != pSession && !strSQL.empty())
	{
		try
		{
			*pSession << strSQL ;
		}
		catch (soci::mysql_soci_error const & e)
		{
			iRet = e.err_num_;
			printf("CMySQLAccess::Exec, oracle_soci_error errno=%d [errmsg=%s] [ sql=%s]",
				e.err_num_, e.what(),strSQL.c_str() );

		}
		catch (soci::soci_error const & e)
		{
			iRet = -1;
			printf("CMySQLAccess::Exec, soci_error [errmsg=%s][ sql=%s]",
				e.what(),strSQL.c_str() );

		}
		catch(...)
		{
			iRet = -2;
			printf( "CMySQLAccess::Exec, error[ sql=%s] .",strSQL.c_str() );
		}
	}
	return iRet;
}

int CMySQLAccess::BreakFillTableSpace()
{
	int iRet = 0;
	m_bBreakFill = true;
	return iRet;
}

