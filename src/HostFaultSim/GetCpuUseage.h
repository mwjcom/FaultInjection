/*!
*	\file	CpuUsage.h
*
*	\brief	获取CPU使用率定义头文件
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
#ifndef GETCPUUSAGE_H 
#define GETCPUUSAGE_H
#include "ConstDef.h"
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>

#ifdef _WINDOWS
#include <psapi.h>
#include <pdh.h> 
#include "tlhelp32.h"
//#pragma comment(lib,"Psapi.lib")
#else
typedef struct PACKEDOCCUPY    //定义一个cpu occupy的结构体
{
	char name[20];      //定义一个char类型的数组名name有20个元素
	unsigned int user; //定义一个无符号的int类型的user
	unsigned int nice; //定义一个无符号的int类型的nice
	unsigned int system;//定义一个无符号的int类型的system
	unsigned int idle; //定义一个无符号的int类型的idle
	unsigned int iowait; //定义一个无符号的int类型的idle
	unsigned int irq ; //定义一个无符号的int类型的idle
	unsigned int softirq; //定义一个无符号的int类型的idle
	unsigned int stealstolen; //定义一个无符号的int类型的idle
	unsigned int guest; //定义一个无符号的int类型的idle
}CPU_OCCUPY;
#endif

#ifndef _WINDOWS
#define PROCESS_ITEM 14
typedef long long __int64_;
#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
typedef struct				
{
	__int64_ user;	
	__int64_ nice;	
	__int64_ system;	
	__int64_ idle;	
}total_cpu_occupy_t;

typedef struct
{
	pid_t pid;		
	__int64_ utime;	
	__int64_ stime;	
	__int64_ cutime;	
	__int64_ cstime;	
}process_cpu_occupy_t;
#endif



class CGetCpuUseage
{
public:
	CGetCpuUseage(void);
	~CGetCpuUseage(void);
	float GetCpuUsage();

#ifdef _WINDOWS
public:
	float WinCpuUseage();
	//!得到CPU的个数
	/*int GetProcessorNumber();*/
	__int64 CompareFileTime ( FILETIME time1, FILETIME time2);
	//uint64_t file_time_2_utc(const FILETIME* ftime);

private:
	FILETIME m_preidleTime;
	FILETIME m_prekernelTime;
	FILETIME m_preuserTime;
#else
	float get_cpu_usage_rate();
	float cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n);
	//对无类型get函数含有一个形参结构体类弄的指针O
	void get_cpuoccupy (CPU_OCCUPY *cpust);
#endif
};
#endif
