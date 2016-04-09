#ifndef NET_MANAGER_H
#define NET_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include "ConstDef.h"
#include "NetworkSim.h"

#ifdef NETSIM_EXPORTS
#define NETSIM_API USAGEDLL_EXPORT
#else
#define NETSIM_API USAGEDLL_IMPORT
#endif

typedef enum TcpOrUdp
{
	TcpType,
	UdpType
}TcpOrUdp;
typedef struct _portInfo
{
	TcpOrUdp nType;
	unsigned int iLocalPort;
	unsigned int iRemotePort;
	unsigned int uiPid;
	std::string strPname;
}portInfo;

class NETSIM_API CNetManager
{
public:
	CNetManager(void);
	~CNetManager(void);

	//! 获取系统当前被占用的端口信息
	int GetPortInfo(
		std::map<unsigned int, portInfo*>& mapTcpPorts,
		std::map<unsigned int, portInfo*>& mapUdpPorts);

	int PortsOccupy(
		std::vector<int> vtPorts);

	int PortsRelease(
		std::vector<int> vtPorts);

	int StartNetSim(NetSimParam& simParam);
	int StopNetSim(NetSimParam& simParam);

private:
#ifdef WIN32
	std::map<int, SOCKET> socketList;
#else
	std::map<int, int> socketList;
#endif
	bool m_bIsSimulating;
};

extern NETSIM_API CNetManager* g_pNetManager; //网络故障注入管理全局对象

#endif

