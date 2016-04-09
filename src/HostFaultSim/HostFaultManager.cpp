#include "HostFaultManager.h"

USAGEMANAGER_API CHostFaultManager* g_pHostFaultManager = NULL;
CHostFaultManager::CHostFaultManager(void)
{
	m_pUsageManager = new CUsageManager;
}


CHostFaultManager::~CHostFaultManager(void)
{
	if (m_pUsageManager)
	{
		delete m_pUsageManager;
		m_pUsageManager = NULL;
	}
}
int CHostFaultManager::SetHostCpuSimParm(HostCpuSimPar & hostCpuSimPar)
{
	int iRet = 0;
	if (m_pUsageManager)
	{
		iRet = m_pUsageManager->SetHostCpuSimParm(hostCpuSimPar);
	}
	return iRet;
}
int CHostFaultManager::SetHostMemSimParm(HostMemSimPar & hostMemSimPar)
{
	int iRet = 0;
	if (m_pUsageManager)
	{
		iRet = m_pUsageManager->SetHostMemSimParm(hostMemSimPar);
	}
	return iRet;
}
int CHostFaultManager::SetHostIOSimParm(HostIOSimPar & hostIOSimPar)
{
	int iRet = 0;
	if (m_pUsageManager)
	{
		iRet = m_pUsageManager->SetHostIOSimParm(hostIOSimPar);
	}
	return iRet;
}
