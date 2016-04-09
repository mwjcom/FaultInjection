#ifndef COMMONDEF_H_
#define COMMONDEF_H_
typedef struct HostSimPar
{
	std::string sJobName;         //任务包的名称
	time_t tvBeginTime;    //模拟开始时间
	time_t tvEndTime;      //模拟结束时间  结束时间必须有
	bool bIsCPUSim;               //是否CPU模拟
	bool bIsMemSim;               //是否Mem模拟
	bool bIsIOSim;                //是否IO模拟
	int iCPUUsage;                //CPU设定值
	int iMemUsage;                //内存设定值
	int iIORSim;                   //硬盘读设定值
	int iIOWSim;                   //硬盘写设定值
}HostSimPar;
#endif