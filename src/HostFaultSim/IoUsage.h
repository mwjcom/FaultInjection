/*!
*	\file	IoUsage.h
*
*	\brief	硬盘IO故障注入定义头文件
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
#ifndef IOUSAGE_H 
#define IOUSAGE_H
#include "ace/Task.h"
#include "ace/OS.h"
#include "ConstDef.h"
// #ifdef _WINDOWS
// #include "Qt\QThread.h"
// #include "Qt\qmutex.h"
// #else
// #include <qthread.h>
// #include <qmutex.h>
// #endif
class CIoUsage: public ACE_Task_Base
{
public:
	CIoUsage(void);
	~CIoUsage(void);
	int SetIoUsage1(bool bIsSet,unsigned long Usage); //设置硬盘IO使用率
	int SetIoCount1(bool bIsSet,unsigned long ulIOCount,unsigned long ulRIOCount); //设置硬盘IO吞吐量
	unsigned long GetIoCount1();   //获取硬盘IO吞吐量
private:
	unsigned long m_IoWCount; //每s写入文件的字节数
	unsigned long m_IoRCount; //每s从文件读取的字节数
	unsigned long m_IoUsage;  //硬盘IO的使用率（实际功能未被使用）
	//HANDLE m_IoThreadHandle;
	//bool m_bEndThread;
	//QMutex m_QM;
	bool m_bStop;     //是否停止模拟故障注入线程
	//void run();

	/*!
	*	\brief	线程执行体
	*/
	int svc(void);

	void StopThread();
	//void run();       //模拟故障注入线程实现体

    char *GetRandomBuffer(unsigned long Len);   //创建读写缓冲区
};

#endif
