
#ifndef _ORACLEACCESS_H_
#define _ORACLEACCESS_H_

#include "DBAccessBase.h"

namespace soci
{
	class session;
}

const int ORA_SUCCESS = 0;
const int ORA_ERR_OBJEXIST = 955;
const int ORA_ERR_INSUFFICIENTTABSPACE = 1653;
const int ORA_ERR_INSUFFICIENTTABSPACE1 = 1658;


class COracleAccess :
	public CDBAccessBase
{
public:
	COracleAccess(void);
	virtual ~COracleAccess(void);

	virtual int StartListener( const char* pchListener );
	virtual int StopListener( const char* pchListener );

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
// 	 std::string m_strUserName;
// 	 std::string m_strOldPWD;
	 
};

#endif //! _ORACLEACCESS_H_

