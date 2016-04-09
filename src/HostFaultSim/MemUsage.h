/*!
*	\file	MemUsage.h
*
*	\brief	内存故障注入定义头文件
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
#ifndef MEMUSAGE_H 
#define MEMUSAGE_H
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
typedef struct RamChunkList
{
	RamChunkList* Next;
	//CHAR RamChunk[0x400000-8];
	char RamChunk[4*1024*1024];
}RamChunkList;

//class QMutex;
class CMemUsage: public ACE_Task_Base

{
public:
	CMemUsage(void);
	~CMemUsage(void);
    int SetMemUsage1(bool bIsSet,unsigned long Usage,unsigned long VirUsage = 0); //设置内存使用率
	int GetRamUsage1(); //获取内存使用率
private:
	//QMutex m_QM;
	unsigned long m_RamUsage;    //内存使用率
	unsigned long m_VirUsage;    //z虚拟内存使用率
	RamChunkList* m_ListHead;    //内存申请列表
	RamChunkList* m_VirListHead;    //内存申请列表
	bool m_bStop;

	//void SetFlg(bool flg);

	//void run();   //模拟故障注入线程实现体

	/*!
	*	\brief	线程执行体
	*/
	int svc(void);

	void StopThread();

#ifdef _WINDOWS
	int GetRamCurUsage(); //获取内存使用率
	int GetVirCurUsage(); //获取虚拟内存使用率
#else
	typedef long long __int64_;
	typedef struct PACKEDMEM         //定义一个mem occupy的结构体
	{
		unsigned long free;          //定义一个char类型的数组名name有20个元素
		unsigned long total; 
		unsigned long buffers;   
		unsigned long cached;                       
	}MEM_OCCUPY;
	int GetRamCurUsage();//获取内存使用率
	int GetVirCurUsage(); //获取虚拟内存使用率
	void Get_Memoccupy (MEM_OCCUPY *mem);
#endif
};
#endif

