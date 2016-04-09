#include "FaultInjectManager.h"
#include "../DBFaultSim/DBFaultSim.h"
#include "../HostFaultSim/HostFaultManager.h"
#include "ace/OS.h"
#include "ace/Event.h"
#include "ace/Signal.h"
const char STOP_SERVER[] = ACE_TEXT("/STOP_SERVER_RTDB");
int main(int argc, char *argv[])
{
	ACE_Event stopEvent( 1, 0, USYNC_PROCESS, STOP_SERVER );
	stopEvent.reset();
	CTestTask g_TestTask;
	if (g_pHostFaultManager == NULL)
	{
		g_pHostFaultManager = new CHostFaultManager;
	}

	
	char filePath[] = "D:\\99se\\1.xml";



// 	CFaultInjectManager aa;
// 	aa.DealWithSimData(filePath);
	//xercesc::XMLPlatformUtils::Terminate();
	
// 	HostCpuSimPar hostCpuSimPar;
// 	hostCpuSimPar.tvBeginTime = time(NULL);
// 	hostCpuSimPar.tvEndTime =  time(NULL) + 1*60;
// 	hostCpuSimPar.bIsCPUSim = true;
// 	hostCpuSimPar.iCPUUsage = 75;
// 	g_pHostFaultManager->SetHostCpuSimParm(hostCpuSimPar);
// 
// 	HostMemSimPar hostMemSimPar;
// 	hostMemSimPar.tvBeginTime = time(NULL) + 30;
// 	hostMemSimPar.tvEndTime =  time(NULL) + 2*60;
// 	hostMemSimPar.bIsMemSim = true;
// 	hostMemSimPar.iMemUsage = 85;
// 	g_pHostFaultManager->SetHostMemSimParm(hostMemSimPar);
// 
// 	HostIOSimPar hostIOSimPar;
// 	hostIOSimPar.tvBeginTime = time(NULL) + 1*60;
// 	hostIOSimPar.tvEndTime =  time(NULL) + 3*60;
// 	hostIOSimPar.bIsIOSim = true;
// 	hostIOSimPar.iIORSim = 1024;
// 	hostIOSimPar.iIOWSim = 1024;
// 	g_pHostFaultManager->SetHostIOSimParm(hostIOSimPar);
// 	while (1)
// 	{
// 		Sleep(1000);
// 	}
	//! 等待StartServer通知进程终止运行
	stopEvent.wait();
	stopEvent.signal();	//!< 需要将信号保持有效状态	
	return 0;
}