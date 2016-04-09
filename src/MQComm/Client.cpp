#include "Client.h"
#include "zmq.h"


CClient::CClient(std::string strIP)
{
	m_strIP = strIP;
	m_bConnected = false;
}

CClient::~CClient(void)
{
	zmq_close(m_socket);
	zmq_term (m_context);
}

//! 发送任务
int CClient::SendJob(std::string strXMLJob, std::string& strRet)
{	
	if ( Connect() != 0 )
	{
		return -1;
	}

	//! send
	std::string strMessage = std::string("Jb:") + strXMLJob;
	int rc = zmq_send (m_socket, strMessage.c_str(), strMessage.length(),0);
	if (rc <= 0)
	{
		return -2;
	}

	//! receive
	char pchRet[1024];
	rc = zmq_recv (m_socket, pchRet, 1024, 0);
	if (rc <= 0)
	{
		return -3;
	}
	strRet = pchRet;

	return 0;
}

//! 获取状态
int CClient::getStatus(std::string& strXMLStatus)
{
	if ( Connect() != 0 )
	{
		return -1;
	}

	//! send
	std::string strMessage = std::string("St:");
	int rc = zmq_send (m_socket, strMessage.c_str(), strMessage.length(),0);
	if (rc <= 0)
	{
		return -2;
	}

	//! receive
	char pchRet[4096];
	rc = zmq_recv (m_socket, pchRet, 1024, 0);
	if (rc <= 0)
	{
		return -3;
	}
	strXMLStatus = pchRet;

	return 0;
}

int CClient::Connect()
{
	if ( !m_bConnected )
	{
		if ( (m_context = zmq_init (1)) == NULL )
			return -1;

		// Socket to talk to server
		// Connecting to server¡­
		if( (m_socket = zmq_socket (m_context, ZMQ_REQ)) == NULL )
		{
			zmq_close(m_socket);
			m_socket = NULL;
			return -2;
		}

		char pchUrl[128];
		sprintf(pchUrl, "tcp://%s:%s", m_strIP.c_str(), ZMQ_PORTNUM);
		int iRet = zmq_connect ( m_socket, pchUrl );
		if ( iRet < 0 )
		{
			zmq_close(m_socket);
			m_socket = NULL;
			return -3;
		}
		m_bConnected = true;
	}

	return 0;
}
