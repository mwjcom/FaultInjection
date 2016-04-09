#ifndef GETSYSUSAGE_H 
#define GETSYSUSAGE_H
#include "ace/Task.h"
#include "ace/OS.h"
#include "ConstDef.h"

//#ifdef _WINDOWS
//#include "Qt\QThread.h"
//#include "Qt\qmutex.h"
//#else
//#include <qthread.h>
//#include <qmutex.h>
//#endif
//class QMutex;
class CGetSysUseage: public ACE_Task_Base
{
public:
	CGetSysUseage(void);
	~CGetSysUseage(void);
	void GetSysUseage(double &CPUUsage,double &MemUsage,unsigned long &IOKByte);
private:
	//QMutex m_QM;
	double m_CPUUsage;   //CPU使用率
	double m_MemUsage;   //内存使用率
	unsigned long m_IOKByte;    //IO读写字节数每s
	bool m_bStop;
	//void run();

	/*!
	*	\brief	线程执行体
	*/
	int svc(void);

	void StopThread();

};
#endif
