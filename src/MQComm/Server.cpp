#include "Server.h"
#include "zmq.h"

CServer::CServer(void)
{
	m_bStop = true;
	m_process = NULL;

	if ( (m_context = zmq_init (1)) == NULL )
		return;
}

CServer::~CServer(void)
{
	if ( !m_bStop )
	{
		m_bStop = true;
	}
	wait();

	zmq_term (m_context);
}

/*!
*	\brief	线程执行体
*/
int CServer::svc(void)
{
	if( (m_socket = zmq_socket(m_context, ZMQ_REP)) == NULL )
	{
		zmq_close(m_socket);
		m_socket = NULL;
		return -2;
	}

	//!也就是使用tcp协议进行通信
	char pchUrl[128];
	sprintf(pchUrl, "tcp://*:%s", ZMQ_PORTNUM);
	int rc = zmq_bind (m_socket, pchUrl);
	if (rc != 0) {
		zmq_close(m_socket);
		m_socket = NULL;
		return -3;
	}

	while ( !m_bStop )
	{
		char pMsg[4096] = {0};
		int rc = zmq_recv(m_socket,pMsg,sizeof(pMsg),0);
		if (rc > 0)
		{
			std::string strMsg = pMsg;
			//! 收到任务
			if (memcmp(pMsg,"J",1) == 0)
			{
				std::string strJob = strMsg.substr( 3, string::npos );	
				if ( m_process )
				{
					m_process->ProcJob(strJob, strMsg);
					rc = zmq_send(m_socket, strMsg.c_str(), strMsg.size(), 0);
					if (rc <= 0)
					{
						continue;
					}				
				}
			}
			//! 获取状态
			else if (memcmp(pMsg,"S",1) == 0)
			{
				if ( m_process )
				{
					m_process->getStatus(strMsg);
					rc = zmq_send(m_socket, strMsg.c_str(), strMsg.size(), 0);
					if (rc <= 0)
					{
						continue;
					}
				}
			}
		}
	}

	if (m_socket)
	{
		zmq_close(m_socket);
		m_socket = NULL;
	}

	return 0;
}

void CServer::Stop()
{
	m_bStop = true;
}

void CServer::Start()
{
	m_bStop = false;
	this->activate();	
}

void CServer::SetProcess(CMsgProcess* process)
{
	m_process = process;
}
