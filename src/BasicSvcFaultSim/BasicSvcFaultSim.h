
#ifndef _BASICSVCFAULTSIM_H_
#define _BASICSVCFAULTSIM_H_

#include "ace/Thread_Mutex.h"
#include "BasicSvcFaultSimPublic.h"
#include <string>
#include <vector>

struct ServiceInfo
{
	std::string strSvcName;
	int iSvcStatus;
};

struct  ProcessInfo
{
	std::string strProcName;
	unsigned long PID;
};

class BASICSVCFAULTSIM_API CBasicSvcFaultSim
{
private:
	CBasicSvcFaultSim(void);

public:
	static CBasicSvcFaultSim* GetInstance();
	virtual ~CBasicSvcFaultSim(void);
	void GetServiceInfos(std::vector<ServiceInfo>& vSvcInfos);	
	void GetProcessInfos(std::vector<ProcessInfo>& vProInfos);
	int TerminateProcess( int iPID );
	int GetServiceStatus(const char * chServiceName);

private:

	int ExecCmd(const char *chCmd, std::vector<std::string> & vResult); 

	static CBasicSvcFaultSim*		sm_pSingleton;
	static ACE_Thread_Mutex	sm_Mutex ;
	
};

#endif //!_DBFAULTSIM_H_
