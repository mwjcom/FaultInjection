#ifndef _COMMON_H_
#define _COMMON_H_

#include <time.h>
#include "NetworkSim.h"
//#include "faultinjection.h"

#define MSG_BUFSIZE 512
#define FILTER_BUFSIZE 1024
#define NAME_SIZE 16
#define MAX_LOG_LEN 256

#define CONTROLS_HANDLE "__CONTROLS_HANDLE"
#define SYNCED_VALUE "__SYNCED_VALUE"
#define INTEGER_MAX "__INTEGER_MAX"
#define INTEGER_MIN "__INTEGER_MIN"
#define FIXED_MAX "__FIXED_MAX"
#define FIXED_MIN "__FIXED_MIN"
#define FIXED_EPSILON 0.01

// workaround stupid vs2012 runtime check.
// it would show even when seeing explicit "(short)(i);"
#define I2S(x) ((short)((x) & 0xFFFF))

#ifndef WIN32
#define LOG(op, fmt, ...)   \
    sprintf(op,/*"%04d-%02d-%02d %02d:%02d:%02d    "*/": " fmt , \
    /*tf->tm_year, tf->tm_mon, tf->tm_yday, tf->tm_hour, tf->tm_min, tf->tm_sec,*/ ##__VA_ARGS__); //\
    //if(g_pFaultInjection) g_pFaultInjection->AddLog(op);
#else
#define LOG(op, fmt, ...)   time ( &rawtime );  tf = localtime ( &rawtime );\
    (sprintf(op,/*"%04d-%02d-%02d %02d:%02d:%02d    "*/__FUNCTION__ ": " fmt , \
    /*tf->tm_year, tf->tm_mon, tf->tm_yday, tf->tm_hour, tf->tm_min, tf->tm_sec,*/ ##__VA_ARGS__)); //\
    //if(g_pFaultInjection) g_pFaultInjection->AddLog(op);

#ifdef __MINGW32__
#define INLINE_FUNCTION __inline__
#else
#define INLINE_FUNCTION __inline
#endif

#ifdef _DEBUG
#define ABORT() assert(0)

extern time_t rawtime;  
extern struct tm * tf;
// check for assert
#ifndef assert
// some how vs can't trigger debugger on assert, which is really stupid
#define assert(x) do {if (!(x)) {DebugBreak();} } while(0)
#endif


#else
#define LOG(op, fmt, ...)
#define ABORT()
//#define assert(x)
#endif

// status for sending packets, 
#define SEND_STATUS_NONE 0
#define SEND_STATUS_SEND 1
#define SEND_STATUS_FAIL -1

// WinDivert
int divertStart(const char * filter, char buf[]);
void divertStop();

// utils
// STR to convert int macro to string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

short calcChance(short chance);

#define BOUND_TEXT(b) ((b) == WINDIVERT_DIRECTION_INBOUND ? "IN" : "OUT")
#define IS_INBOUND(b) ((b) == WINDIVERT_DIRECTION_INBOUND)
#define IS_OUTBOUND(b) ((b) == WINDIVERT_DIRECTION_OUTBOUND)
// inline helper for inbound outbound check
static INLINE_FUNCTION
BOOL checkDirection(UINT8 dir, short inbound, short outbound) {
    return (inbound && IS_INBOUND(dir))
                || (outbound && IS_OUTBOUND(dir));
}



// wraped timeBegin/EndPeriod to keep calling safe and end when exit
#define TIMER_RESOLUTION 4
void startTimePeriod();
void endTimePeriod();

// elevate
BOOL IsElevated();
BOOL IsRunAsAdmin();
BOOL tryElevate(HWND hWnd, BOOL silent);

// parameterized
//extern BOOL parameterized;
//void setFromParameter(Ihandle *ih, const char *field, const char *key);
//BOOL parseArgs(int argc, char* argv[]);
#endif //WIN32

#endif
