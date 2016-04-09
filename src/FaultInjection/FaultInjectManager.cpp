#include "FaultInjectManager.h"
//#include "../HostFaultSim/HostFaultManager.h"
//using namespace NSFaultInjectionXML;
//CFaultInjectManager::CFaultInjectManager(void)
//{
//}
//
//
//CFaultInjectManager::~CFaultInjectManager(void)
//{
//}
//
//
//int CFaultInjectManager::DealWithSimData(char * pFilePaht)
//{
//	xercesc::XMLPlatformUtils::Initialize();
//	try
//	{
//		CFaultInjectionXMLDoc doc;
//		CFaultInjectXMLType root = doc.Load(pFilePaht);
//
//		xercesc::DOMNode * pCursor = root.GetStartingHostFaultCursor();
//		int iCount = root.GetHostFaultCount();
//		CHostFaultType host = root.GetHostFault();//GetAdvancedHostFaultCursor(pCursor);
//		if (root.HasHostFault())    //有主机故障
//		{
//			time_t tvBeginTime = time(NULL);
//			time_t tvEndTime = time(NULL) + 30*24*60*60;   //30天
//			if(host.HasStartTime()) //有开始时间
//			{
//				CSchemaDateTime tv1 = host.GetStartTime();
//				
//			}
//			CFaultItemType3 faultItem = host.GetFaultItem();
//			if (faultItem.HasCPUFault())   //有CPU故障
//			{
//				CCPUFaultType cpu = faultItem.GetCPUFault();
//				bool bIsSet = cpu.GetIsInject();
//				int iCpu = 0;
//				if (cpu.HasCPUUsage())
//				{
//					iCpu = cpu.GetCPUUsage();
//				}
//
//
//				HostCpuSimPar hostCpuSimPar;
//				hostCpuSimPar.tvBeginTime = time(NULL);
//				hostCpuSimPar.tvEndTime =  time(NULL) + 1*60;
//				hostCpuSimPar.bIsCPUSim = bIsSet;
//				hostCpuSimPar.iCPUUsage = iCpu;
//				g_pHostFaultManager->SetHostCpuSimParm(hostCpuSimPar);
//			}
//
//			if (faultItem.HasMEMFault())   //有Mem故障
//			{
//				CMEMFaultType mem = faultItem.GetMEMFault();
//				bool bIsSet = mem.GetIsInject();
//				int iMem = 0;
//				if (mem.HasMEMUsage())
//				{
//					iMem = mem.GetMEMUsage();
//				}
//				HostMemSimPar hostMemSimPar;
//				hostMemSimPar.tvBeginTime = time(NULL);
//				hostMemSimPar.tvEndTime =  time(NULL) + 1*60;
//				hostMemSimPar.bIsMemSim = bIsSet;
//				hostMemSimPar.iMemUsage = iMem;
//				g_pHostFaultManager->SetHostMemSimParm(hostMemSimPar);
//			}
//
//			if (faultItem.HasDiskIOFault())   //有IO故障
//			{
//				CDiskIOFaultType io = faultItem.GetDiskIOFault();
//				bool bIsSet = io.GetIsInject();
//				int iWIO = 0;
//				int iRIO = 0;
//				if (io.HasReadIOByte())
//				{
//					iRIO = io.GetReadIOByte();
//				}
//				if (io.HasWriteIOByte())
//				{
//					iWIO = io.GetWriteIOByte();
//				}
//				HostIOSimPar hostIOSimPar;
//				hostIOSimPar.tvBeginTime = time(NULL);
//				hostIOSimPar.tvEndTime =  time(NULL) + 1*60;
//				hostIOSimPar.bIsIOSim = bIsSet;
//				hostIOSimPar.iIORSim = iRIO;
//				hostIOSimPar.iIOWSim = iWIO;
//				g_pHostFaultManager->SetHostIOSimParm(hostIOSimPar);
//			}
//		}
//		
//		
//
//
//		
//	}
//	catch (...)
//	{
//
//	}
//
//	return 0;
//}
