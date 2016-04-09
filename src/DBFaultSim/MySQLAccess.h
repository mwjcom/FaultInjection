#ifndef _MySQLACCESS_H_
#define _MySQLACCESS_H_

#include "DBAccessBase.h"

namespace soci
{
	class session;
}

const int MYSQL_SUCCESS = 0;
const int MYSQL_ERR_OBJEXIST = 1064;
//const int MYSQL_ERR_INSUFFICIENTTABSPACE = 1653; //! MySQL ´ý¶¨

class CMySQLAccess :
	public CDBAccessBase
{
public:
	CMySQLAccess(void);
	virtual ~CMySQLAccess(void);

	virtual int StartService( const char* pchServiceName );
	virtual int StopService( const char* pchServiceName );

	virtual int Connect( const char* pchConnString, int iConnCnt = 1 );
	virtual int DisConnect();
	virtual int ChangePWD( const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD);
	virtual int RecoverPWD( const char* pchUserName , const char* pchOldPWD );

	virtual int FillTableSpace(const char* pchTabSpaceName);
	virtual int FreeTableSpace(const char* pchTabSpaceName);
	virtual int BreakFillTableSpace();

private:
	int Exec( soci::session * pSession, std::string const & strSQL );
	bool m_bBreakFill;
	std::string m_strUserName;
	std::string m_strOldPWD;
};

#endif //! _MySQLACCESS_H_
