#ifndef _NETSIMMGR_H_
#define _NETSIMMGR_H_

#include "NetSimulates.h"

#define CONFIG_FILE "config.txt"
#define CONFIG_MAX_RECORDS 64
#define CONFIG_BUF_SIZE 40960
typedef struct {
	char* filterName;
	char* filterValue;
} filterRecord;


class NetSimMgr
{
public:
	NetSimMgr(void);
	~NetSimMgr(void);

	// loading up filters and fill in
	void loadConfig();
    int start(const char* filter, char* buf, const char* inbuf = NULL, const char* outRC = NULL);
	int stop();
	void startup();
	void cleanup();
#ifdef WIN32
    std::vector<NetworkSim*> modules;
#endif
    unsigned int filtersSize;
	filterRecord filters[CONFIG_MAX_RECORDS];
	char configBuf[CONFIG_BUF_SIZE+2]; // add some padding to write \n
};

extern NetSimMgr* g_pNetSimMgr;

#endif

