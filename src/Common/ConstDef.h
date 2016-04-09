#ifndef CONSTDEF_H_
#define CONSTDEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>

#include "../Common/CommonDef.h"
//#include"ace/Date_Time.h"
//#include "..\NetSim\common.h"
//#include <iostream> 
#ifdef WIN32
#ifndef _WINDOWS
#define _WINDOWS
#endif
#endif


#ifdef _WINDOWS
#include <Windows.h>
#define USAGEDLL_EXPORT __declspec(dllexport)
#define USAGEDLL_IMPORT __declspec(dllimport)
#else
// _SC_NPROCESSORS_CONF 定义在 unistd.h 文件中。
#include <unistd.h>
// 使用　#define __USE_GNU　是为了使用CPU_SET()等宏。
// 具体在 /usr/include/sched.h 中有定义。
#define __USE_GNU
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define USAGEDLL_EXPORT
#define USAGEDLL_IMPORT
#endif

using namespace std;
const double GAP_LINEAR = 100;  //线性函数时间间隔100ms  

#endif
