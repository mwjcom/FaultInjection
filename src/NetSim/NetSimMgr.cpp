#include "NetSimMgr.h"
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#else
#include <errno.h>
#endif

NetSimMgr* g_pNetSimMgr = NULL;
#ifndef WIN32
int NetSim(const char* pNetCard, const char* pQdisc, const char* filter)
{
    char pLog[MAX_LOG_LEN];
    //! create bands
    char pchCmd[256];
    sprintf(pchCmd, "tc qdisc add dev %s root handle 1: prio bands 4", pNetCard);
    int iRet = system(pchCmd);
    LOG(pLog, "%s", pchCmd);
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return iRet;
    }
    else {
        LOG(pLog, "system Successed!");
    }

    //! add qdisc
    iRet = system(pQdisc);
    LOG(pLog, "%s", pQdisc);
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return iRet;
    }
    else {
        LOG(pLog, "system Successed!");
    }

    //! add filter
    char pchFilter[MSG_BUFSIZE];
    sprintf(pchFilter, "tc filter add dev %s protocol ip parent 1:0 prio 4 u32 %s flowid 1:4", pNetCard,filter);
    iRet = system(pchFilter);
    LOG(pLog, "%s", pchFilter);
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return iRet;
    }
    else {
        LOG(pLog, "system Successed!");
    }

    return 0;
}

int StopNetSim(const char* pNetCard)
{
    char pLog[MAX_LOG_LEN];
    char pchCmd[256];
    //! delete filter
    sprintf(pchCmd, "tc filter del dev %s pref 4", pNetCard);
    int iRet = system(pchCmd);
    LOG(pLog, "%s", pchCmd);
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
		return iRet;
    }
    else {
        LOG(pLog, "system Successed!");
    }

    //! delete qdisc
    sprintf(pchCmd, "tc qdisc del dev %s root", pNetCard);
    iRet = system(pchCmd);
    LOG(pLog, "%s", pchCmd);
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
		return iRet;
    }
    else {
        LOG(pLog, "system Successed!");
    }
	return 0;
}
#endif

NetSimMgr::NetSimMgr(void)
{
#ifdef WIN32
	modules.push_back(new NetLag);
	modules.push_back(new NetDrop);
	modules.push_back(new NetThrottle);
	modules.push_back(new NetDuplicate);
	modules.push_back(new NetOOD);
	modules.push_back(new NetTamper);
#endif
}


NetSimMgr::~NetSimMgr(void)
{
	stop();
#ifdef WIN32
	for (int i = 0; i < modules.size(); i++)
	{
		if ( modules[i] )
		{
			delete modules[i];
			modules[i] = NULL;
		}

		modules.clear();
	}
#endif
}

// loading up filters and fill in
void NetSimMgr::loadConfig() {
	char path[MSG_BUFSIZE];	
	FILE *f;
	char pLog[MAX_LOG_LEN];
#ifdef WIN32
    char *p;
	GetModuleFileName(NULL, (LPWSTR)path, MSG_BUFSIZE);
	LOG(pLog, "Executable path: %s", path);
	p = strrchr(path, '\\');
	if (p == NULL) p = strrchr(path, '/'); // holy shit
	strcpy(p+1, CONFIG_FILE);
#else
    sprintf(path, "%s", CONFIG_FILE);
#endif
	LOG(pLog, "Config path: %s", path);
	f = fopen(path, "r");
	if (f) {
		size_t len;
		char *p, *last;
		len = fread(configBuf, sizeof(char), CONFIG_BUF_SIZE, f);
		if (len == CONFIG_BUF_SIZE) {
			LOG(pLog, "Config file is larger than %d bytes, get truncated.", CONFIG_BUF_SIZE);
		}
		// always patch in a newline at the end to ease parsing
		configBuf[len] = '\n';
		configBuf[len+1] = '\0';

		// parse out the kv pairs. isn't quite safe
		filtersSize = 0;
		last = p = configBuf;
		do {
			// eat up empty lines
EAT_SPACE:  while (isspace(*p)) { ++p; }
			if (*p == '#') {
				p = strchr(p, '\n');
				if (!p) break;
				p = p + 1;
				goto EAT_SPACE;
			}

			// now we can start
			last = p;
			p = strchr(last, ':');
			if (!p) break;
			*p = '\0';
			filters[filtersSize].filterName = last;
			p += 1;
			while (isspace(*p)) { ++p; } // eat potential space after :
			last = p;
			p = strchr(last, '\n');
			if (!p) break;
			filters[filtersSize].filterValue = last;
			*p = '\0';
			if (*(p-1) == '\r') *(p-1) = 0;
			last = p = p + 1;
			++filtersSize;
		} while (last && last - configBuf < CONFIG_BUF_SIZE);
		LOG(pLog, "Loaded %u records.", filtersSize);
	}

	if (!f || filtersSize == 0)
	{
		filtersSize = 0;
		LOG(pLog,"Failed to load from config. Fill in a simple one.");
#ifdef WIN32
		// config is missing or ill-formed. fill in some simple ones
		filters[filtersSize].filterName = "loopback packets";
		filters[filtersSize].filterValue = "outbound and ip.DstAddr >= 127.0.0.1 and ip.DstAddr <= 127.255.255.255";
#else
        filters[filtersSize].filterName = "loopback packets";
        filters[filtersSize].filterValue = "match ip dst 127.0.0.1/24";
#endif
		filtersSize = 1;
	}
}

void NetSimMgr::startup() {
#ifdef WIN32
	// initialize seed
	srand((unsigned int)time(NULL));
#else
    char pLog[MAX_LOG_LEN];
    //! load ifb
    int iRet = system("modprobe ifb");
    LOG(pLog, "modprobe ifb");
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return;
    }
    else {
        LOG(pLog, "system Successed!");
    }
    //! up ifb0
    iRet = system("ip link set ifb0 up");
    LOG(pLog, "ip link set ifb0 up");
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return;
    }
    else {
        LOG(pLog, "system Successed!");
    }
    //! add eth0 ingress
    iRet = system("tc qdisc add dev eth0 ingress");
    LOG(pLog, "tc qdisc dev eth0 ingress");
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return;
    }
    else {
        LOG(pLog, "system Successed!");
    }
    //! redirect eth0 mirror ifb
    iRet = system("tc filter add dev eth0 parent ffff: protocol ip u32 match u32 0 0 flowid 1:1 action mirred egress redirect dev ifb0");
    LOG(pLog, "tc filter add dev eth0 parent ffff: protocol ip u32 match u32 0 0 flowid 1:1 action mirred egress redirect dev ifb0");
    if( iRet != 0 ) {
        LOG(pLog, "system Failed, errmsg:%s!!(%d)", strerror(errno),errno);
        return;
    }
    else {
        LOG(pLog, "system Successed!");
    }
#endif
}

int NetSimMgr::start(const char *filter, char* buf, const char* inbuf, const char* outRC) {
#ifdef WIN32
	if (divertStart(filter, buf) == FALSE) {
		return -1;
	}
#else
    char pLog[MAX_LOG_LEN];
    // out
    int iRet = NetSim("eth0", buf, filter);
    if( 0 != iRet )
    {
        sprintf(buf, "%s", strerror(errno));
        return iRet;
    }

    //! in
    iRet = NetSim("ifb0", inbuf, filter);
    if( 0 != iRet )
    {
        sprintf(buf, "%s", strerror(errno));
        return iRet;
    }
#endif

	return 0;
}

int NetSimMgr::stop() {
#ifdef WIN32
	divertStop();
#else
    // out
    int iRet = StopNetSim("eth0");
    if( 0 != iRet )
        return iRet;

    //! in
    iRet = StopNetSim("ifb0");
    if( 0 != iRet )
        return iRet;

#endif
	return 0;
}

void NetSimMgr::cleanup() {
#ifdef WIN32
	endTimePeriod(); // try close if not closing
#endif
}

