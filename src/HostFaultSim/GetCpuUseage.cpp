#include "GetCpuUseage.h"


CGetCpuUseage::CGetCpuUseage(void)
{
}


CGetCpuUseage::~CGetCpuUseage(void)
{
}

float CGetCpuUseage::GetCpuUsage()
{
	float fCpuRate = 0;
#ifdef _WINDOWS
	fCpuRate = WinCpuUseage();
#else
	fCpuRate = get_cpu_usage_rate();
#endif
	printf("******=========Cpu=%d==========****\n",fCpuRate); 
	return fCpuRate;
}
#ifdef _WINDOWS
float CGetCpuUseage::WinCpuUseage()
{
	GetSystemTimes( &m_preidleTime, &m_prekernelTime, &m_preuserTime);
	::Sleep(1000);
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetSystemTimes( &idleTime, &kernelTime, &userTime );

	__int64 idle =   CompareFileTime( m_preidleTime,idleTime);
	__int64 kernel = CompareFileTime( m_prekernelTime, kernelTime);
	__int64 user =   CompareFileTime(m_preuserTime, userTime);

	if(kernel+user == 0)
		return 0.0;
	//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
	float fCpu = (float)(kernel +user - idle) *100/(kernel+user);

	m_preidleTime = idleTime;
	m_prekernelTime = kernelTime;
	m_preuserTime = userTime ;
	return fCpu;
}
__int64  CGetCpuUseage::CompareFileTime ( FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime ;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime ;
	return   (b - a);
}
#else
#define  PROC_STAT_PATH "/proc/stat"

float CGetCpuUseage::get_cpu_usage_rate()
{
	CPU_OCCUPY cpu_stat1;
	CPU_OCCUPY cpu_stat2;
	//第一次获取cpu使用情况
	get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);
	//printf("cpu name: %s\n",cpu_stat1.name);
	//printf("cpu user: %u\n",cpu_stat1.user);
	//printf("cpu nice: %u\n",cpu_stat1.nice);
	//printf("cpu system: %u\n",cpu_stat1.system);
	//printf("cpu idle: %u\n",cpu_stat1.idle);
	//printf("cpu iowait: %u\n",cpu_stat1.iowait);
	//printf("cpu irq: %u\n",cpu_stat1.irq);
	//printf("cpu softirq: %u\n",cpu_stat1.softirq);
	//printf("cpu stealstolen: %u\n",cpu_stat1.stealstolen);
	//printf("cpu guest: %u\n",cpu_stat1.guest);

    sleep(1);
	//第二次获取cpu使用情况
	get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2);
	//计算cpu使用率
	float cup = cal_cpuoccupy ((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);

	return cup;
} 
float CGetCpuUseage::cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n) 
{   

	//unsigned long od, nd;    
	//unsigned long id, sd;
	unsigned long all_1, all_2;
	unsigned long idle_2 , idle_1;
	unsigned long m_cpu_1, m_cpu_2;
	int cpu_use = 0;   
	//第一次(用户+优先级+系统+空闲)的时间再赋给od
	all_1 = (unsigned long) (o->user + o->nice + o->system +o->idle + o->iowait+o->irq+o->softirq+o->stealstolen+o->guest);
	//第二次(用户+优先级+系统+空闲)的时间再赋给od
	all_2 = (unsigned long) (n->user + n->nice + n->system +n->idle+ n->iowait+o->irq+ n->softirq+ n->stealstolen+n->guest);
	//用户第一次和第二次的时间之差再赋给id
	m_cpu_1= o->user + o->nice + o->system;
	m_cpu_2= n->user + n->nice + n->system;
	idle_1 = o->idle;
	idle_2 = n->idle;
	//id = (unsigned long) (n->user - o->user);
	////系统第一次和第二次的时间之差再赋给sd
	//sd = (unsigned long) (n->system - o->system);
	//((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
	//if((nd-od) != 0)
	// cpu_use = (int)((sd+id)*10000)/(nd-od); 
	//else cpu_use = 0;
	float cpu_use_rate = 100*( (float)(all_2 - all_1) - (float)(idle_2 - idle_1)) / (float)(all_2 - all_1);    
	printf("%04.2f%%\n",cpu_use_rate);
	cpu_use_rate = (float)(m_cpu_2 - m_cpu_1) / (float)(all_2 - all_1) * 100;
	printf("%04.2f%%\n",cpu_use_rate); 
	return cpu_use_rate;
}
//对无类型get函数含有一个形参结构体类弄的指针O
void CGetCpuUseage::get_cpuoccupy (CPU_OCCUPY *cpust) 
{   
	FILE *fd = NULL;         
	char buff[256]; 
	CPU_OCCUPY *cpu_occupy;
	cpu_occupy=cpust;

	fd = fopen ("/proc/stat", "r"); 
	if (NULL != fd)
	{
		fgets (buff, sizeof(buff), fd);
		//printf("buf=%s",buff);
		sscanf (buff, "%s%u%u%u%u%u%u%u%u%u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle,
			&cpu_occupy->iowait,&cpu_occupy->irq ,&cpu_occupy->softirq ,&cpu_occupy->stealstolen ,&cpu_occupy->guest);

		fclose(fd);
	}
}
#endif
