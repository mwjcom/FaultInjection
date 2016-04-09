#ifndef _DBACCESSBASE_H_
#define _DBACCESSBASE_H_

#include <string>

namespace soci
{
	class connection_pool;
}

class CDBAccessBase
{
public:
	CDBAccessBase(void);
	virtual ~CDBAccessBase(void);

	virtual int StartListener( const char* pchListener );
	virtual int StopListener( const char* pchListener );

	virtual int StartService( const char* pchServiceName );
	virtual int StopService( const char* pchServiceName );

	virtual int Connect( const char* pchConnString, int iConnCnt = 1 );
	virtual int DisConnect();

	int ChangePWD( const char* pConnString, const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD);
	virtual int ChangePWD( const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD);
	virtual int RecoverPWD( const char* pchUserName , const char* pchOldPWD );

	virtual int FillTableSpace(const char* pchTabSpaceName);
	virtual int FreeTableSpace(const char* pchTabSpaceName);
	virtual int BreakFillTableSpace();

protected:
	soci::connection_pool* m_pConnectionPool;
	int m_iConnCnt;
	std::string m_strConnString;
};

#endif //!_DBACCESSBASE_H_


