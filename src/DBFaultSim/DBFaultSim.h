
#ifndef _DBFAULTSIM_H_
#define _DBFAULTSIM_H_

#include "ace/Task.h"
#include "ace/Thread_Mutex.h"
#include "DBFaultSimPublic.h"
#include <string>

class CDBAccessBase;
class DBFAULTSIM_API CDBFaultSim
{
private:
	CDBFaultSim(void);

public:
	static CDBFaultSim* GetInstance();
	virtual ~CDBFaultSim(void);

	int SetDBType(const char* pchDBType );

	int StartListener( const char* pchListener = NULL );
	int StopListener( const char* pchListener = NULL  );

	int StartService( const char* pchServiceName = NULL );
	int StopService( const char* pchServiceName = NULL  );

	int Connect( const char* pchConnString, int iConnCnt = 1 );
	int DisConnect();

	int ChangePWD( const char* pConnString, const char* pchUserName, const char* pchOldPWD, const char* pchNewPWD);
	int RecoverPWD( const char* pchUserName , const char* pchOldPWD );

	int FillTableSpace(const char* pchTabSpaceName);
	int FreeTableSpace(const char* pchTabSpaceName);

private:

	static CDBFaultSim*		sm_pSingleton;
	static ACE_Thread_Mutex	sm_Mutex ;

	std::string m_strDBType;

	CDBAccessBase* m_pDBAccess;

};


class DBFAULTSIM_API CTestTask : public ACE_Task_Base
{
public:
	/*!
	*	\brief	缺省构造函数
	*/
	CTestTask(void);

	/*!
	*	\brief	缺省析构函数
	*/
	virtual ~CTestTask(void);

	/*!
	*	\brief	获取收集任务是否停止运行
	*
	*	\retval bool	停止运行标志,true:停止, false 运行
	*/
	bool done (void)
	{
		return m_bShutDown;
	}

	/*!
	*	\brief	线程退出
	*/
	void shutdown()
	{
		m_bShutDown = true;
		//!等待线程退出
// 		while(this->thr_count() >0 )
// 			ACE_OS::sleep(ACE_Time_Value(0,100*1000));
	}

	/*!
	*	\brief	根据本机身份，判断是否进行收集
	*/
	//bool run(void);

	virtual int svc (void)
	{
		return 0;
	}



	//停止运行标志,true:停止, false 运行
	bool m_bShutDown;

private:
	
	
	
};

#endif //!_DBFAULTSIM_H_
