#ifndef HOSTFAULTMANAGER_H_
#define HOSTFAULTMANAGER_H_

#include "UsageManager.h"
 #ifdef USAGEMANAGER_EXPORTS
 #define USAGEMANAGER_API  USAGEDLL_EXPORT
 #else
 #define USAGEMANAGER_API USAGEDLL_IMPORT
 #endif
 class USAGEMANAGER_API CHostFaultManager
 {
 public:
 	CHostFaultManager(void);
 	~CHostFaultManager(void);
 
 	int SetHostCpuSimParm(HostCpuSimPar & hostCpuSimPar);
    int SetHostMemSimParm(HostMemSimPar & hostMemSimPar);
    int SetHostIOSimParm(HostIOSimPar & hostIOSimPar);
 
 private:
 	CUsageManager * m_pUsageManager;
 };
 extern USAGEMANAGER_API CHostFaultManager* g_pHostFaultManager; //系统资源故障注入管理全局对象
#endif

