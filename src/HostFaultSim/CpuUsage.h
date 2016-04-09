/*!
*	\file	CpuUsage.h
*
*	\brief	CPU故障注入定义头文件
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
#ifndef CPUUSAGE_H 
#define CPUUSAGE_H
#include "ConstDef.h"
#include "GetCpuUseage.h"
class CCpuUsage
{
public:
	CCpuUsage(void);
	~CCpuUsage(void);
	int SetCpuUsage1(bool bIsSet,double dCpu); //设置CPU使用率
	float GetCpuUsage1();                      //获取CPU使用率
private:
	bool m_bIsStop;   //是否停止模拟故障注入线程
	int m_iCpuNum;    //CPU个数
	double m_dRatio;  
	CGetCpuUseage * m_pGetCpuUseage;  //获取CPU当前使用率
#ifdef _WINDOWS
	void Run();   //执行模拟
	static DWORD WINAPI ThreadProc (LPVOID pParam);  //模拟线程执行体
#else
   void Run_Linux();  //执行模拟
   static void * ThreadProc_Linux( void* pvParameter );//模拟线程执行体
#endif


	//int GetCpuUsage();
};
#endif

