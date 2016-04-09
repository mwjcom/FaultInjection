#ifndef MQCOMM_SERVER_H
#define MQCOMM_SERVER_H

#include <string>
#include <map>
#include <vector>
#include "PubFile.h"
#include "ace/Task.h"
#include "ace/OS.h"

class MQCOMM_API CMsgProcess
{
public:
	CMsgProcess(void){};
	~CMsgProcess(void){};

	//! 处理任务
	virtual int ProcJob(std::string strXMLJob, std::string& strRet) = 0;

	//! 获取状态
	virtual int getStatus(std::string& strXMLStatus) = 0;
};

class MQCOMM_API CServer : public ACE_Task_Base
{
public:
	CServer(void);
	~CServer(void);

	/*!
	*	\brief	线程执行体
	*/
	int svc(void);

	void Stop();

	void Start();

	void SetProcess(CMsgProcess* process);

private:
	bool m_bStop;
	void *m_context;
	void* m_socket;

	CMsgProcess* m_process;
};

#endif

