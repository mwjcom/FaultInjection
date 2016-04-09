#include "UsageManager.h"
#include "ace/OS.h"
#include "CpuUsage.h"
#include "MemUsage.h"
#include "IoUsage.h"
//USAGEMANAGER_API CUsageManager* g_pUsageManager = NULL;  //初始化全局变量
CUsageManager::CUsageManager(void)
{
 	m_pCpuUsage = NULL;
 	m_pCpuUsage = new CCpuUsage;
	m_pMemUsage = NULL;
	m_pMemUsage = new CMemUsage;
	m_IoUsage = NULL;
	m_IoUsage = new CIoUsage;
	m_vecHostCpuSimPar.clear();
	m_vecHostMemSimPar.clear();
	m_vecHostIOSimPar.clear();
	m_bCpuExeJob = false;
	m_bMemExeJob = false;
	m_bIOExeJob = false;
	m_bIsStop = false;
	activate();
}


CUsageManager::~CUsageManager(void)
{
	if (m_pCpuUsage != NULL)
	{
		delete m_pCpuUsage;
		m_pCpuUsage = NULL;
	}

	if (m_pMemUsage != NULL)
	{
		delete m_pMemUsage;
		m_pMemUsage = NULL;
	}

	if (m_IoUsage != NULL)
	{
		delete m_IoUsage;
		m_IoUsage = NULL;
	}
	m_vecHostCpuSimPar.clear();
	m_vecHostMemSimPar.clear();
	m_vecHostIOSimPar.clear();
}

int CUsageManager::SetHostCpuSimParm(HostCpuSimPar & newSimPar)
{
	int iRet = 0; //0表示成功
	mtxThread.acquire();
	//此处目前严格限制只能有一个开始模拟的任务包执行（后面的代码逻辑比较复杂，只是多个模拟包，暂时保留）
	if (m_vecHostCpuSimPar.size() > 0)   //当前有任务包
	{
		if (newSimPar.bIsCPUSim == true)
		{
			iRet = -1;   //当前有开始模拟的任务包
			mtxThread.release();
			return iRet;
		}
	}
	////如果是取消模拟，直接设置m_bIsStop为true
	if (newSimPar.bIsCPUSim == false)   //如果是取消模拟
	{
		m_bIsStop = true;
		m_vecHostCpuSimPar.clear();
		mtxThread.release();
		return iRet;
	}


	if (newSimPar.bIsCPUSim == true && newSimPar.tvBeginTime >= newSimPar.tvEndTime)  //检查下时间参数
	{
		//TODO  上报错误日志
		iRet = -2;
		mtxThread.release();
		return iRet;
	}

	
	//当前正在执行的任务包也包括在m_vecHostSimPar中
	if (m_vecHostCpuSimPar.size() == 0)   //一个任务包也没有时，直接添加
	{
		m_vecHostCpuSimPar.push_back(newSimPar);
	}
	else  //有任务包
	{
		HostCpuSimPar hostsimPar = m_vecHostCpuSimPar[0];

		//如果有任务包在执行，判断新任务包是否和正在执行的任务包冲突（时间）
		bool bIsExecute = false;
		if (m_bCpuExeJob == true)  // 已存在的任务正在执行
		{
			bIsExecute = true;
			if (newSimPar.tvBeginTime < hostsimPar.tvEndTime)   //新任务的开始时间小于正在执行任务的结束时间
			{
				//TODO  上报错误日志
				iRet = -2;
				mtxThread.release();
				return iRet;
			}
		}

		int iStartIndex = 0;   //新任务包和已有任务包数组比较的开始索引
		if (bIsExecute == true)  //有任务包执行，并且不冲突，新任务包只需和后面的任务包比较即可
		{
			iStartIndex = 1;
		}

		for (int iIndex = iStartIndex;iIndex < m_vecHostCpuSimPar.size();iIndex++)
		{
			HostCpuSimPar hostsimPar = m_vecHostCpuSimPar[iIndex];
			if (iIndex == iStartIndex)
			{
				if (newSimPar.tvEndTime <= hostsimPar.tvBeginTime) //满足要求
				{
					m_vecHostCpuSimPar.insert(m_vecHostCpuSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
			else if (iIndex == m_vecHostCpuSimPar.size() - 1)
			{
				if (newSimPar.tvBeginTime >= hostsimPar.tvEndTime) //满足要求
				{
					m_vecHostCpuSimPar.insert(m_vecHostCpuSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
			else
			{
				HostCpuSimPar proHostsimPar = m_vecHostCpuSimPar[iIndex - 1];
				if (newSimPar.tvBeginTime >= proHostsimPar.tvEndTime && 
					newSimPar.tvEndTime <= hostsimPar.tvBeginTime) //满足要求
				{
					m_vecHostCpuSimPar.insert(m_vecHostCpuSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
		}
		if (iRet != 0)
		{
			//TODO  上报错误日志
		}
	}
	mtxThread.release();
	return iRet;
}

int CUsageManager::SetHostMemSimParm(HostMemSimPar & newSimPar)
{
	int iRet = 0; //0表示成功
	mtxThread.acquire();
	//此处目前严格限制只能有一个开始模拟的任务包执行（后面的代码逻辑比较复杂，只是多个模拟包，暂时保留）
	if (m_vecHostMemSimPar.size() > 0)   //当前有任务包
	{
		if (newSimPar.bIsMemSim == true)
		{
			iRet = -1;   //当前有开始模拟的任务包
			mtxThread.release();
			return iRet;
		}
	}
	////如果是取消模拟，直接设置m_bIsStop为true
	if (newSimPar.bIsMemSim == false)   //如果是取消模拟
	{
		m_bIsStop = true;
		m_vecHostMemSimPar.clear();
		mtxThread.release();
		return iRet;
	}

	if (newSimPar.tvBeginTime >= newSimPar.tvEndTime)  //检查下时间参数
	{
		//TODO  上报错误日志
		mtxThread.release();
		return iRet;
	}
	//当前正在执行的任务包也包括在m_vecHostSimPar中
	if (m_vecHostMemSimPar.size() == 0)   //一个任务包也没有时，直接添加
	{
		m_vecHostMemSimPar.push_back(newSimPar);
	}
	else  //有任务包
	{
		HostMemSimPar hostsimPar = m_vecHostMemSimPar[0];

		//如果有任务包在执行，判断新任务包是否和正在执行的任务包冲突（时间）
		bool bIsExecute = false;
		if (m_bMemExeJob == true)  // 已存在的任务正在执行
		{
			bIsExecute = true;
			if (newSimPar.tvBeginTime < hostsimPar.tvEndTime)   //新任务的开始时间小于正在执行任务的结束时间
			{
				//TODO  上报错误日志
				mtxThread.release();
				return iRet;
			}
		}

		int iStartIndex = 0;   //新任务包和已有任务包数组比较的开始索引
		if (bIsExecute == true)  //有任务包执行，并且不冲突，新任务包只需和后面的任务包比较即可
		{
			iStartIndex = 1;
		}

		for (int iIndex = iStartIndex;iIndex < m_vecHostMemSimPar.size();iIndex++)
		{
			HostMemSimPar hostsimPar = m_vecHostMemSimPar[iIndex];
			if (iIndex == iStartIndex)
			{
				if (newSimPar.tvEndTime <= hostsimPar.tvBeginTime) //满足要求
				{
					m_vecHostMemSimPar.insert(m_vecHostMemSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
			else if (iIndex == m_vecHostMemSimPar.size() - 1)
			{
				if (newSimPar.tvBeginTime >= hostsimPar.tvEndTime) //满足要求
				{
					m_vecHostMemSimPar.insert(m_vecHostMemSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
			else
			{
				HostMemSimPar proHostsimPar = m_vecHostMemSimPar[iIndex - 1];
				if (newSimPar.tvBeginTime >= proHostsimPar.tvEndTime && 
					newSimPar.tvEndTime <= hostsimPar.tvBeginTime) //满足要求
				{
					m_vecHostMemSimPar.insert(m_vecHostMemSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
		}
		if (iRet != 0)
		{
			//TODO  上报错误日志
		}
	}
	mtxThread.release();
	return iRet;
}


int CUsageManager::SetHostIOSimParm(HostIOSimPar & newSimPar)
{
	int iRet = 0; //0表示成功
	mtxThread.acquire();
	//此处目前严格限制只能有一个开始模拟的任务包执行（后面的代码逻辑比较复杂，只是多个模拟包，暂时保留）
	if (m_vecHostIOSimPar.size() > 0)   //当前有任务包
	{
		if (newSimPar.bIsIOSim == true)
		{
			iRet = -1;   //当前有开始模拟的任务包
			mtxThread.release();
			return iRet;
		}
	}

	////如果是取消模拟，直接设置m_bIsStop为true
	if (newSimPar.bIsIOSim == false)   //如果是取消模拟
	{
		m_bIsStop = true;
		m_vecHostIOSimPar.clear();
		mtxThread.release();
		return iRet;
	}


	if (newSimPar.tvBeginTime >= newSimPar.tvEndTime)  //检查下时间参数
	{
		//TODO  上报错误日志
		mtxThread.release();
		return iRet;
	}
	//当前正在执行的任务包也包括在m_vecHostSimPar中
	if (m_vecHostIOSimPar.size() == 0)   //一个任务包也没有时，直接添加
	{
		m_vecHostIOSimPar.push_back(newSimPar);
	}
	else  //有任务包
	{
		HostIOSimPar hostsimPar = m_vecHostIOSimPar[0];

		//如果有任务包在执行，判断新任务包是否和正在执行的任务包冲突（时间）
		bool bIsExecute = false;
		if (m_bIOExeJob == true)  // 已存在的任务正在执行
		{
			bIsExecute = true;
			if (newSimPar.tvBeginTime < hostsimPar.tvEndTime)   //新任务的开始时间小于正在执行任务的结束时间
			{
				//TODO  上报错误日志
				mtxThread.release();
				return iRet;
			}
		}

		int iStartIndex = 0;   //新任务包和已有任务包数组比较的开始索引
		if (bIsExecute == true)  //有任务包执行，并且不冲突，新任务包只需和后面的任务包比较即可
		{
			iStartIndex = 1;
		}

		for (int iIndex = iStartIndex;iIndex < m_vecHostMemSimPar.size();iIndex++)
		{
			HostIOSimPar hostsimPar = m_vecHostIOSimPar[iIndex];
			if (iIndex == iStartIndex)
			{
				if (newSimPar.tvEndTime <= hostsimPar.tvBeginTime) //满足要求
				{
					m_vecHostIOSimPar.insert(m_vecHostIOSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
			else if (iIndex == m_vecHostIOSimPar.size() - 1)
			{
				if (newSimPar.tvBeginTime >= hostsimPar.tvEndTime) //满足要求
				{
					m_vecHostIOSimPar.insert(m_vecHostIOSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
			else
			{
				HostIOSimPar proHostsimPar = m_vecHostIOSimPar[iIndex - 1];
				if (newSimPar.tvBeginTime >= proHostsimPar.tvEndTime && 
					newSimPar.tvEndTime <= hostsimPar.tvBeginTime) //满足要求
				{
					m_vecHostIOSimPar.insert(m_vecHostIOSimPar.begin() + iIndex,newSimPar);
					iRet = 0;
					break;
				}
			}
		}
		if (iRet != 0)
		{
			//TODO  上报错误日志
		}
	}
	mtxThread.release();
	return iRet;
}

int CUsageManager::svc(void)   //线程执行体
{
	int iRet = 0;
	while (1)
	{
		printf("CUsageManager::svc");
		if (m_bIsStop == true)
		{
			break;  //退出循环
		}
		mtxThread.acquire();
		if (m_vecHostCpuSimPar.size() > 0)
		{
			HostCpuSimPar hostCpuSimPar = m_vecHostCpuSimPar[0];
			time_t tvCurrentTime = time(NULL);
			//有任务包判断是否到了停止时间
			if (m_bCpuExeJob == true)
			{
				if (tvCurrentTime >= hostCpuSimPar.tvEndTime)  //结束任务包
				{
					//TODO 上报任务xxx任务结束
					m_bCpuExeJob = false;
					SetCpuUsage(false,0);
					m_vecHostCpuSimPar.erase(m_vecHostCpuSimPar.begin());
				}
			}
			else  //没有任务包判断是否将执行第一个任务包
			{
				if (hostCpuSimPar.tvBeginTime <= tvCurrentTime)// && hostsimPar.tvEndTime > tvCurrentTime)
				{
					//TODO 上报任务xxx任务开始
					m_bCpuExeJob = true;
					SetCpuUsage(hostCpuSimPar.bIsCPUSim,hostCpuSimPar.iCPUUsage);
				}
			}
		}

		if (m_vecHostMemSimPar.size() > 0)
		{
			HostMemSimPar hostMemSimPar = m_vecHostMemSimPar[0];
			time_t tvCurrentTime = time(NULL);
			//有任务包判断是否到了停止时间
			if (m_bMemExeJob == true)
			{
				if (tvCurrentTime >= hostMemSimPar.tvEndTime)  //结束任务包
				{
					//TODO 上报任务xxx任务结束
					m_bMemExeJob = false;
					SetRamUsage(false,0,0);
					m_vecHostMemSimPar.erase(m_vecHostMemSimPar.begin());
				}
			}
			else  //没有任务包判断是否将执行第一个任务包
			{
				if (hostMemSimPar.tvBeginTime <= tvCurrentTime)// && hostsimPar.tvEndTime > tvCurrentTime)
				{
					//TODO 上报任务xxx任务开始
					m_bMemExeJob = true;
					SetRamUsage(hostMemSimPar.bIsMemSim,hostMemSimPar.iMemUsage,0);
				}
			}
		}

		if (m_vecHostIOSimPar.size() > 0)
		{
			HostIOSimPar hostIOSimPar = m_vecHostIOSimPar[0];
			time_t tvCurrentTime = time(NULL);
			//有任务包判断是否到了停止时间
			if (m_bIOExeJob == true)
			{
				if (tvCurrentTime >= hostIOSimPar.tvEndTime)  //结束任务包
				{
					//TODO 上报任务xxx任务结束
					m_bIOExeJob = false;
					SetIoCount(false,0,0);
					m_vecHostIOSimPar.erase(m_vecHostIOSimPar.begin());
				}
			}
			else  //没有任务包判断是否将执行第一个任务包
			{
				if (hostIOSimPar.tvBeginTime <= tvCurrentTime)// && hostsimPar.tvEndTime > tvCurrentTime)
				{
					//TODO 上报任务xxx任务开始
					m_bIOExeJob = true;
					SetIoCount(hostIOSimPar.bIsIOSim,hostIOSimPar.iIOWSim,hostIOSimPar.iIOWSim);
				}
			}
		}
		mtxThread.release();
		ACE_OS::sleep(1);
	}
	return iRet;
}

void CUsageManager::StopThread()
{
	m_bIsStop = true;
}
int CUsageManager::SetCpuUsage(bool bIsSet,float fCPU)
{
	int iRet = 0;
	if (m_pCpuUsage != NULL)
	{
		iRet = m_pCpuUsage->SetCpuUsage1(bIsSet,fCPU);
	}
	else
	{
		iRet = -1;
	}
	return iRet;
}
int CUsageManager::SetRamUsage(bool bIsSet,unsigned long ulUsage,unsigned long ulVirUsage)
{
	int iRet = 0;
	if (m_pMemUsage != NULL)
	{
		iRet = m_pMemUsage->SetMemUsage1(bIsSet,ulUsage,ulVirUsage);
	}
	else
	{
		iRet = -1;
	}
	return iRet;
}
int CUsageManager::SetIoUsage(bool bIsSet,unsigned long ulUsage)
{
	int iRet = 0;
	if (m_IoUsage != NULL)
	{
		iRet = m_IoUsage->SetIoUsage1(bIsSet,ulUsage);
	}
	else
	{
		iRet = -1;
	}
	return iRet;
}
int CUsageManager::SetIoCount(bool bIsSet,unsigned long ulIOCount,unsigned long ulRIOCount)
{
	int iRet = 0;
	if (m_IoUsage != NULL)
	{
		iRet = m_IoUsage->SetIoCount1(bIsSet,ulIOCount,ulRIOCount);
	}
    double CPUUsage;
    double MemUsage;
    unsigned long IOKByte;
    GetSysResUseage(CPUUsage,MemUsage,IOKByte);
	return iRet;
}
int CUsageManager::GetSysResUseage(double &CPUUsage,double &MemUsage,unsigned long &IOKByte)
{
	int iRet = 0;
	if (m_pCpuUsage != NULL && m_pMemUsage != NULL && m_IoUsage != NULL)
	{
		CPUUsage = m_pCpuUsage->GetCpuUsage1();
		MemUsage = m_pMemUsage->GetRamUsage1();
		IOKByte = m_IoUsage->GetIoCount1();
	}
	else
	{
		iRet = -1;
	}
	return iRet;
}
