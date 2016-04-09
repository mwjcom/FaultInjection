/*!
*	\file	UsageManager.h
*
*	\brief	系统资源故障注入管理定义头文件
*
*	\Author 
*
*	\date	2016-2-1 10:30
*
*	\version	1.0
*
*	\attention	Copyright (c) 2016, 
*	\attention	All rights reserved.
*/
#ifndef USAGEMANAGER_H_
#define USAGEMANAGER_H_
#include "ace/Task.h"
#include "ConstDef.h"
#include <vector>
// #ifdef USAGEMANAGER_EXPORTS
// #define USAGEMANAGER_API  USAGEDLL_EXPORT
// #else
// #define USAGEMANAGER_API USAGEDLL_IMPORT
// #endif
class CCpuUsage;
class CMemUsage;
class CIoUsage;
class CUsageManager: public ACE_Task_Base
{
public:
	CUsageManager(void);
	~CUsageManager(void);
	int SetHostCpuSimParm(HostCpuSimPar & newSimPar);
	int SetHostMemSimParm(HostMemSimPar & newSimPar);
	int SetHostIOSimParm(HostIOSimPar & newSimPar);
	int GetSysResUseage(double &CPUUsage,double &MemUsage,unsigned long &IOKByte);

private:
	CCpuUsage * m_pCpuUsage;  //CPU故障注入对象
	CMemUsage * m_pMemUsage;  //内存故障注入对象
	CIoUsage * m_IoUsage;     //IO故障注入对象

	vector<HostCpuSimPar> m_vecHostCpuSimPar;   //CPU任务包数组
	vector<HostMemSimPar> m_vecHostMemSimPar;   //Mem任务包数组
	vector<HostIOSimPar> m_vecHostIOSimPar;     //IO任务包数组
	//std::string m_sJobName;               //正在执行的任务包名称
	bool m_bCpuExeJob;                       //正在执行的任务包
	bool m_bMemExeJob;                       //正在执行的任务包
	bool m_bIOExeJob;                       //正在执行的任务包
	bool m_bIsStop;                       //线程停止标志
	ACE_Thread_Mutex mtxThread;           //互斥


	
	//设置CPU使用率  返回值：0，正常；-1
	int SetCpuUsage(bool bIsSet,float fCPU);
	//设置内存使用率  返回值：0，正常；-1，设置的内存比当前实际内存低
	int SetRamUsage(bool bIsSet,unsigned long ulUsage,unsigned long ulVirUsage);
	//设置IO使用率  返回值：0，正常；-1  （功能暂时保留）
	int SetIoUsage(bool bIsSet,unsigned long ulUsage);

	//设置IO吞吐量  返回值：0，正常；-1  
	//参数ulIOCount：写入字节数
	//参数ulRIOCount：读取字节数
	int SetIoCount(bool bIsSet,unsigned long ulIOCount = 0,unsigned long ulRIOCount = 0);
	
	/*!
	*	\brief	线程执行体
	*/
	int svc(void);

	void StopThread();
};
//extern USAGEMANAGER_API CUsageManager* g_pUsageManager; //系统资源故障注入管理全局对象
#endif

