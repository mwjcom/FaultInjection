#ifndef _NETWORKSIM_H_
#define _NETWORKSIM_H_

#include <stdio.h>
#ifdef WIN32
#include <assert.h>
#include "windivert.h"
#include <vector>

// package node
typedef struct _NODE {
	char *packet;
	UINT packetLen;
	WINDIVERT_ADDRESS addr;
	DWORD timestamp; // ! timestamp isn't filled when creating node since it's only needed for lag
	struct _NODE *prev, *next;
} PacketNode;

//! init
void initPacketNodeList();
PacketNode* createNode(char* buf, UINT len, WINDIVERT_ADDRESS *addr);
void freeNode(PacketNode *node);
PacketNode* popNode(PacketNode *node);
PacketNode* insertBefore(PacketNode *node, PacketNode *target);
PacketNode* insertAfter(PacketNode *node, PacketNode *target);
PacketNode* appendNode(PacketNode *node);
short isListEmpty();


typedef struct LagParam
{
	bool bEnable;
	short shIn;
	short shOut;
	int lagTime;
}LagParam;

typedef struct DropParam
{
	bool bEnable;
	short shIn;
	short shOut;
	int iChance;
}DropParam;

typedef struct ThrottleParam
{
	bool bEnable;
	short shIn;
	short shOut;
	short dropThrottled;
	int iChance;
	int iTime;
}ThrottleParam;

typedef struct DuplicateParam
{
	bool bEnable;
	short shIn;
	short shOut;
	int iChance;
	int iCount;
}DuplicateParam;

typedef struct OODParam
{
	bool bEnable;
	short shIn;
	short shOut;
	int iChance;
}OODParam;

typedef struct TamperParam
{
	bool bEnable;
	short shIn;
	short shOut;
	int iChance;
	short shReSum;
}TamperParam;

typedef struct NetSimParam
{
	std::string strFilter;
	LagParam lag;
	DropParam drop;
	ThrottleParam throttle;
	DuplicateParam duplicate;
	OODParam ood;
	TamperParam tamper;
}NetSimParam;

// network simulate
class NetworkSim
{
public:
	NetworkSim(){
		//enabledFlag = 0;
		lastEnabled = 0;
		processTriggered = 0;
	};
	virtual ~NetworkSim(void){};

	//short enabledFlag; // volatile short flag to determine enabled or not
    virtual void startUp() = 0; // called when starting up the module
    virtual void closeDown(PacketNode *head, PacketNode *tail) = 0; // called when starting up the module
    virtual short process(PacketNode *head, PacketNode *tail) = 0;
	virtual bool enable() = 0;
	virtual void UpdateParams() = 0;

    /*
     * Flags used during program excution. Need to be re initialized on each run
     */
    short lastEnabled; // if it is enabled on last run
    short processTriggered; // whether this module has been triggered in last step 
};

static volatile short sendState;

#endif  //WIN32

#endif

