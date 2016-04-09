#include "CpuUsage.h"
//#include<iostream>  
#include<cmath>
#ifndef _WINDOWS
typedef struct CPUThreadPar
{
    CCpuUsage * pCpuUsage;
    int iCpuNo;
}CPUThreadPar;

CPUThreadPar CpuThreadParStruct[32];
#endif
CCpuUsage::CCpuUsage()
{
	m_pGetCpuUseage = NULL;
	m_pGetCpuUseage = new CGetCpuUseage;

#ifndef _WINDOWS
    for(int i=0;i<32;i++)
    {
        CpuThreadParStruct[i].pCpuUsage = NULL;
    }
#endif
}
CCpuUsage::~CCpuUsage()
{
	if (m_pGetCpuUseage != NULL)
	{
		delete m_pGetCpuUseage;
		m_pGetCpuUseage = NULL;
	}
}
float CCpuUsage::GetCpuUsage1()
{
	float fCurCpuUsage = 0;
	if (m_pGetCpuUseage)
	{
		fCurCpuUsage = m_pGetCpuUseage->GetCpuUsage(); //当前CPU使用率
	}
	return fCurCpuUsage;
}
int CCpuUsage::SetCpuUsage1(bool bIsSet,double dCpu)
{
	int iRet = 0;
	double f100 = 100;
	m_dRatio = dCpu/f100;
	if (bIsSet == true)
	{
		float fCurCpuUsage = m_pGetCpuUseage->GetCpuUsage(); //当前CPU使用率
		if (fCurCpuUsage > dCpu)
		{
			iRet = -1;  //当前的CPU使用率大于设置值
// 			char pLog[MAX_LOG_LEN];
// 			LOG(pLog, "SetCpuUsage setval %.2f < curval %.2f", dCpu, fCurCpuUsage);
			return iRet;
		}
        m_bIsStop = false;
#ifdef _WINDOWS
		SYSTEM_INFO info;  
		GetSystemInfo(&info);   //得到cpu数目  
		m_iCpuNum = info.dwNumberOfProcessors; 
		Run();
#else
		// sysconf() 返回选项(变量)的当前值，
		// 这个值可配置的但也是受系统限制的。
		// _SC_NPROCESSORS_CONF 的值为 CPU 个数，基于零(0)开始编号
		// CPU 编号范围：0 ～ ( sysconf( _SC_NPROCESSORS_CONF ) - 1 )
		//uiCPUTotal = sysconf( _SC_NPROCESSORS_CONF ); 
		m_iCpuNum = sysconf( _SC_NPROCESSORS_CONF );
		printf( "Notice: current system has %i CPU(s). ~ thread: %lu\n", m_iCpuNum, pthread_self() );
        Run_Linux();
#endif

		
	}
	else
	{
		m_bIsStop = true; 
	}

	return iRet;
}
#ifdef _WINDOWS
void CCpuUsage::Run()  
{   
	HANDLE handle[32];    
	DWORD thread_id[32]; //线程id  

	for (int i = 0; i < m_iCpuNum; i++)  
	{  
		if ((handle[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc,   
			this, 0, &thread_id[i])) != NULL)  //创建新线程  
			SetThreadAffinityMask(handle[i], i+1); //限定线程运行在哪个cpu上  
	}  

}   
DWORD WINAPI CCpuUsage::ThreadProc (LPVOID pParam)
{
	CCpuUsage* ThisObj = (CCpuUsage*)pParam;
	if (ThisObj == NULL)
	{
		return 0;
	}
	unsigned BUSY =  ThisObj->m_dRatio * GAP_LINEAR;  
	unsigned IDLE = (1 - ThisObj->m_dRatio) * GAP_LINEAR;  
	while(1)  
	{  
 		if (true == ThisObj->m_bIsStop)
 		{
 			break;
 		}
		unsigned ta = GetTickCount(); //操作系统启动到现在所经过的时间ms;  
		while(GetTickCount() - ta < BUSY); 
		Sleep(IDLE);
	}
	return 0;
}
#else

void CCpuUsage::Run_Linux()
{
	for (int i = 0; i < m_iCpuNum; i++)  
	{
		pthread_t sThreadID; 
        CpuThreadParStruct[i].pCpuUsage = this;
        CpuThreadParStruct[i].iCpuNo = i;
		if ( 0 != pthread_create( &sThreadID,       // 线程ID
			NULL,             // 线程属性 - NULL 使用缺少值
            ThreadProc_Linux, // 线程函数地址
            &CpuThreadParStruct[i] ) )   // 线程函数参数 - 指定线程用哪个 CPU
		{
// 			char pLog[MAX_LOG_LEN];
// 			LOG(pLog, "create %d CpuThread  error",i+1);
			//printf( "!!! Error: create thread failed !!!\n" ); 
		}
        /*else
		{
			pthread_join( sThreadID, // 线程ID
				NULL );    // 线程的返回值 - NULL 不获得返回值
        }*/
	}
}
// gcc main.c -o runatcpu -lpthread
void * CCpuUsage::ThreadProc_Linux( void* pvParameter )
{
    CPUThreadPar pTempPar = *(CPUThreadPar *)pvParameter;
    unsigned int uiWhichCPU = pTempPar.iCpuNo;
    CCpuUsage * pThis = pTempPar.pCpuUsage;
	//unsigned int uiCPUTotal = 0;
	unsigned int j          = 0; 
	unsigned int i          = 0;

	// CPU 亲和力(CPU Affinity)的概念：
	// CPU 亲合力就是指在 Linux 系统中能够将一个或多个进程绑定到一个或多个处理器上运行。
	// 进程可以通过 CPU 集合决定将在哪个或哪几个 CPU 上运行。
	// cpu_set_t 结构体来表示一个 CPU 集合。
	cpu_set_t sCPUSet;

	//// sysconf() 返回选项(变量)的当前值，
	//// 这个值可配置的但也是受系统限制的。
	//// _SC_NPROCESSORS_CONF 的值为 CPU 个数，基于零(0)开始编号
	//// CPU 编号范围：0 ～ ( sysconf( _SC_NPROCESSORS_CONF ) - 1 )
	//uiCPUTotal = sysconf( _SC_NPROCESSORS_CONF );   
	//printf( "Notice: current system has %i CPU(s). ~ thread: %lu\n", 
	//	uiCPUTotal, 
	//	pthread_self() );

	// 清空一个 CPU 集合
	CPU_ZERO( &sCPUSet );
	// 将一个给定的 CPU 编号向一个 CPU 集合中增加
	CPU_SET( uiWhichCPU, &sCPUSet );
	// 将一个给定的 CPU 编号从一个 CPU 集合中移除
	// CPU_CLR( uiWhichCPU, &sCPUSet );

	// 将指定 线程ID 绑定指定的 CPU
	if ( -1 == pthread_setaffinity_np( pthread_self(),    // 线程ID
		sizeof( sCPUSet ), // CPU 集合结构体大小
		&sCPUSet ) )       // CPU 集合结构体变量指针
	{
		printf( "!!! Error: bind current thread to specified CPU failed !!! ~ thread: %lu\n", 
			pthread_self() );
	}
	else
	{
		int busyTime = 10;   //忙的时间
		int idleTime = 0;    //空闲时间
		long timeuse = 0;    //实际循环用的时间
		//int cpucoe = 0;      //CPU占用率
		struct timeval tpstart,tpend; 
		while(1)  
		{  
            if (true == pThis->m_bIsStop)
			{
				break;
			}
			gettimeofday(&tpstart,NULL); //得到当前的系统时间
			while (timeuse <= busyTime)
			{
				gettimeofday(&tpend,NULL);
				timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + (tpend.tv_usec - tpstart.tv_usec);  
				timeuse /= 1000;               //转换成ms                
			}

            idleTime = ((100 * busyTime) / pThis->m_dRatio) - busyTime;

			sleep(idleTime / 1000);    //转化成ms
		}
	}
}


#endif


/*

#include<iostream>
#include<cmath>
#include<windows.h>

static int PERIOD = 60 * 1000; //周期ms
const int COUNT = 300;  //一个周期计算次数
const double GAP_LINEAR = 100;  //线性函数时间间隔100ms
const double PI = 3.1415926535898; //PI
const double GAP = (double)PERIOD / COUNT; //周期函数时间间隔
const double FACTOR = 2 * PI / PERIOD;  //周期函数的系数
static double Ratio = 0.5;  //线性函数的值 0.5即50%
static double Max=0.9; //方波函数的最大值
static double Min=0.1; //方波函数的最小值

typedef double Func(double);  //定义一个函数类型 Func*为函数指针
typedef void Solve(Func *calc);//定义函数类型，参数为函数指针Func*
inline DWORD get_time() 
{ 
	return GetTickCount(); //操作系统启动到现在所经过的时间ms
}
double calc_sin(double x)  //调用周期函数solve_period的参数
{  
	return (1 + sin(FACTOR * x)) / 2; //y=1/2(1+sin(a*x))
}
double calc_fangbo(double x)  //调用周期函数solve_period的参数
{
	//方波函数
	if(x<=PERIOD/2) return Max;
	else return Min;
}

void solve_period(Func *calc) //线程函数为周期函数
{
	double x = 0.0;
	double cache[COUNT];
	for (int i = 0; i < COUNT; ++i, x += GAP) 
		cache[i] = calc(x); 
	int count = 0;
	while(1)
	{
		unsigned ta = get_time();
		if (count >= COUNT) count = 0;
		double r = cache[count++];
		DWORD busy = r * GAP;
		while(get_time() - ta < busy) {}
		Sleep(GAP - busy);
  }
}

void solve_linear(Func*)  //线程函数为线性函数，参数为空 NULL
{
	const unsigned BUSY =  Ratio * GAP_LINEAR;
	const unsigned IDLE = (1 - Ratio) * GAP_LINEAR;
	while(1)
	{
		unsigned ta = get_time();
		while(get_time() - ta < BUSY) {}
		Sleep(IDLE);
	}
}
//void solve_nonperiod(Func *calc) //非周期函数的处理，暂没实验
//{
//  double tb = 0;
//  while(1)
//  {
//    unsigned ta = get_time();
//    double r = calc(tb);
//    if (r < 0 || r > 1) r = 1;
//    DWORD busy = r * GAP;
//    while(get_time() - ta < busy) {}
//    Sleep(GAP - busy);
//    //tb += GAP;
//    tb += get_time() - ta;
//  }
//}

void run(int i=1,double R=0.5,double T=60000,double max=0.9,double min=0.1)
     //i为输出状态，R为直线函数的值，T为周期函数的周期，max方波最大值，min方波最小值
{
	Ratio=R; PERIOD=T; Max=max; Min=min;
	Func *func[] = {NULL ,calc_sin,calc_fangbo};  //传给Solve的参数，函数指针数组
	Solve *solve_func[] = { solve_linear, solve_period};  //Solve函数指针数组
	const int NUM_CPUS = 2;  //双核，通用的可以用下面GetSystemInfo得到cpu数目
	HANDLE handle[NUM_CPUS];  
	DWORD thread_id[NUM_CPUS]; //线程id
	//SYSTEM_INFO info;
	//GetSystemInfo(&info);   //得到cpu数目
	//const int num = info.dwNumberOfProcessors;
	switch(i)
	{
	case 1: //cpu1 ,cpu2都输出直线
		{
			for (int i = 0; i < NUM_CPUS; ++i)
			{
				Func *calc = func[0];
				Solve *solve = solve_func[0];
				if ((handle[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)solve, 
									(VOID*)calc, 0, &thread_id[i])) != NULL)  //创建新线程
				SetThreadAffinityMask(handle[i], i+1); //限定线程运行在哪个cpu上
			}
			WaitForSingleObject(handle[0],INFINITE);   //等待线程结束
			break;
		}
	case 2: //cpu1直线，cpu2正弦
		{
			for (int i = 0; i < NUM_CPUS; ++i)
			{
				Func *calc = func[i];
				Solve *solve = solve_func[i];
				if ((handle[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)solve, 
									(VOID*)calc, 0, &thread_id[i])) != NULL)  //创建新线程
				SetThreadAffinityMask(handle[i], i+1); //限定线程运行在哪个cpu上
			}
			WaitForSingleObject(handle[0],INFINITE);   //等待线程结束
			break;
		}
		case 3: //cpu1直线，cpu2方波
		{

			//Func *calc = func[0];
			//Solve *solve = solve_func[0];
			if ((handle[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)solve_func[0], 
								(VOID*)func[0], 0, &thread_id[0])) != NULL)  //创建新线程
			SetThreadAffinityMask(handle[0], 1); //限定线程运行在哪个cpu上
			Func *calc = func[2];
			Solve *solve = solve_func[1];
			if ((handle[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)solve, 
								(VOID*)calc, 0, &thread_id[1])) != NULL)  //创建新线程
			SetThreadAffinityMask(handle[1], 2); //限定线程运行在哪个cpu上
			WaitForSingleObject(handle[0],INFINITE);   //等待线程结束
			break;
		}
		case 4: //cpu1正弦，cpu2方波
		{
			if ((handle[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)solve_func[1], 
								(VOID*)func[1], 0, &thread_id[0])) != NULL)  //创建新线程
			SetThreadAffinityMask(handle[0], 1); //限定线程运行在哪个cpu上
			Func *calc = func[2];
			Solve *solve = solve_func[1];
			if ((handle[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)solve, 
								(VOID*)calc, 0, &thread_id[1])) != NULL)  //创建新线程
			SetThreadAffinityMask(handle[1], 2); //限定线程运行在哪个cpu上
			WaitForSingleObject(handle[0],INFINITE);   //等待线程结束
			break;
		}
		default: break;
	}
}

void main()
{
	run(1,0.5);  //cpu1 ,cpu2都输出50%的直线
	//run(2,0.5,30000); //cpu1 0.5直线，cpu2正弦周期30000
	//run(3);  //cpu1直线，cpu2方波
	//run(4,0.8,30000,0.95,0.5); //cpu1正弦，cpu2 0.95-0.5的方波
}*/