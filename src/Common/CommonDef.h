#ifndef COMMONDEF_H_
#define COMMONDEF_H_
typedef struct HostCpuSimPar
{
	time_t tvBeginTime;
	time_t tvEndTime;
	bool bIsCPUSim;
	int iCPUUsage;
}HostCpuSimPar;


typedef struct HostMemSimPar
{
	time_t tvBeginTime;
	time_t tvEndTime;
	bool bIsMemSim;
	int iMemUsage;
}HostMemSimPar;

typedef struct HostIOSimPar
{
	time_t tvBeginTime;
	time_t tvEndTime;
	bool bIsIOSim;
	int iIORSim;
	int iIOWSim;
}HostIOSimPar;
#endif