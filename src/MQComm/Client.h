#ifndef MQCOMM_CLIENT_H
#define MQCOMM_CLIENT_H

#include <string>
#include <map>
#include <vector>
#include "PubFile.h"

class MQCOMM_API CClient
{
public:
	CClient(std::string strIP);
	~CClient(void);

	//! 连接
	int Connect();

	//! 发送任务
	int SendJob(std::string strXMLJob, std::string& strRet);

	//! 获取状态
	int getStatus(std::string& strXMLStatus);

private:
	std::string m_strIP;
	void *m_context;
	void *m_socket;
	bool m_bConnected;
};

#endif

