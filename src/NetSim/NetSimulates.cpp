#include "NetSimulates.h"

#define LAG_MIN "0"
#define LAG_MAX "3000"
#define KEEP_AT_MOST 2000
// send FLUSH_WHEN_FULL packets when buffer is full
#define FLUSH_WHEN_FULL 800
#define LAG_DEFAULT 50

#ifdef WIN32

PacketNode NetLag::lagHeadNode = {0};
PacketNode NetLag::lagTailNode = {0};
PacketNode* NetLag::bufHead = &NetLag::lagHeadNode;
PacketNode* NetLag::bufTail = &NetLag::lagTailNode;
int NetLag::bufSize = 0;

extern NetSimParam* g_simParam = NULL;
NetLag::NetLag(void)
{
	shIn = 1;
	shOut = 1;
	lagTime = 50;
}
NetLag::~NetLag(void)
{
}

bool NetLag::enable()
{
	if ( g_simParam )
	{
		return g_simParam->lag.bEnable;
	}
	else
		return false;
}

void NetLag::UpdateParams()
{
	if ( g_simParam )
	{
		shIn = g_simParam->lag.shIn;
		shOut = g_simParam->lag.shIn;
		lagTime = g_simParam->lag.shIn;
	}
}
void NetLag::startUp()
{
	if (NetLag::bufHead->next == NULL && NetLag::bufTail->next == NULL) {
		NetLag::bufHead->next = NetLag::bufTail;
		NetLag::bufTail->prev = bufHead;
		NetLag::bufSize = 0;
	} else {
		assert(isBufEmpty());
	}
	startTimePeriod();
}
void NetLag::closeDown(PacketNode *head, PacketNode *tail)
{
	char pLog[MAX_LOG_LEN];
	PacketNode *oldLast = tail->prev;
	UNREFERENCED_PARAMETER(head);
	// flush all buffered packets
	LOG(pLog, "Closing down lag, flushing %d packets", bufSize);
	while(!isBufEmpty()) {
		insertAfter(popNode(NetLag::bufTail->prev), oldLast);
		--bufSize;
	}
	endTimePeriod();
}
short NetLag::process(PacketNode *head, PacketNode *tail)
{
	char pLog[MAX_LOG_LEN];
	DWORD currentTime = timeGetTime();
	PacketNode *pac = tail->prev;
	//! 更新界面配置
	UpdateParams();
	// pick up all packets and fill in the current time
	while (bufSize < KEEP_AT_MOST && pac != head) {
		if (checkDirection(pac->addr.Direction, shIn, shOut)) {
			insertAfter(popNode(pac), bufHead)->timestamp = timeGetTime();
			++bufSize;
			pac = tail->prev;
		} else {
			pac = pac->prev;
		}
	}

	// try sending overdue packets from buffer tail
	while (!isBufEmpty()) {
		PacketNode *pac = bufTail->prev;
		if (currentTime > pac->timestamp + lagTime) {
			insertAfter(popNode(bufTail->prev), head); // sending queue is already empty by now
			--bufSize;
			LOG(pLog, "Send lagged packets.");
		} else {
			LOG(pLog, "Sent some lagged packets, still have %d in buf", bufSize);
			break;
		}
	}

	// if buffer is full just flush things out
	if (bufSize >= KEEP_AT_MOST) {
		int flushCnt = FLUSH_WHEN_FULL;
		while (flushCnt-- > 0) {
			insertAfter(popNode(bufTail->prev), head);
			--bufSize;
		}
	}

	return bufSize > 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! NetDrop
NetDrop::NetDrop()
{
	shIn = 1;
	shOut = 1;
	iChance = 1000;
}
NetDrop::~NetDrop()
{

}
void NetDrop::startUp()			// called when starting up the module
{
	char pLog[MAX_LOG_LEN];
	LOG(pLog, "drop enabled");
}
void NetDrop::closeDown(PacketNode *head, PacketNode *tail) // called when starting up the module
{
	char pLog[MAX_LOG_LEN];
	UNREFERENCED_PARAMETER(head);
	UNREFERENCED_PARAMETER(tail);
	LOG(pLog, "drop disabled");
}
short NetDrop::process(PacketNode *head, PacketNode *tail)
{
	int dropped = 0;
	char pLog[MAX_LOG_LEN];
	//! 更新界面配置
	UpdateParams();
	while (head->next != tail) {
		PacketNode *pac = head->next;
		// chance in range of [0, 10000]
		if (checkDirection(pac->addr.Direction, shIn, shOut)
			&& calcChance(iChance)) {
				LOG(pLog, "dropped with chance %.1f%%, direction %s",
					iChance/100.0, BOUND_TEXT(pac->addr.Direction));
				freeNode(popNode(pac));
				++dropped;
		} else {
			head = head->next;
		}
	}

	return dropped > 0;
}
bool NetDrop::enable()
{
	if ( g_simParam )
	{
		return g_simParam->drop.bEnable;
	}
	else
		return false;
}
void NetDrop::UpdateParams()
{
	if ( g_simParam )
	{
		shIn = g_simParam->drop.shIn;
		shOut = g_simParam->drop.shOut;
		iChance = g_simParam->drop.iChance;	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PacketNode throttleHeadNode = {0}, throttleTailNode = {0};
//! NetThrottle
NetThrottle::NetThrottle()
{
	shIn = 1;
	shOut = 1;
	iChance = 1000;
	iTime = 30;

	bufSize = 0;
	throttleStartTick = 0; 
	bufHead = &throttleHeadNode;
	bufTail = &throttleTailNode;
	dropThrottled = 0;
}
NetThrottle::~NetThrottle()
{

}
short NetThrottle::isBufEmpty() {
	short ret = bufHead->next == bufTail;
	if (ret) assert(bufSize == 0);
	return ret;
}
void NetThrottle::startUp()			// called when starting up the module
{
	if (bufHead->next == NULL && bufTail->next == NULL) {
		bufHead->next = bufTail;
		bufTail->prev = bufHead;
		bufSize = 0;
	} else {
		assert(isBufEmpty());
	}
	throttleStartTick = 0;
	startTimePeriod();
}
void NetThrottle::clearBufPackets(PacketNode *tail) {
	char pLog[MAX_LOG_LEN];
	PacketNode *oldLast = tail->prev;
	LOG(pLog, "Throttled end, send all %d packets. Buffer at max: %s", bufSize, bufSize == KEEP_AT_MOST ? "YES" : "NO");
	while (!isBufEmpty()) {
		insertAfter(popNode(bufTail->prev), oldLast);
		--bufSize;
	}
	throttleStartTick = 0;
}

void NetThrottle::dropBufPackets() {
	char pLog[MAX_LOG_LEN];
	LOG(pLog, "Throttled end, drop all %d packets. Buffer at max: %s", bufSize, bufSize == KEEP_AT_MOST ? "YES" : "NO");
	while (!isBufEmpty()) {
		freeNode(popNode(bufTail->prev));
		--bufSize;
	}
	throttleStartTick = 0;
}
void NetThrottle::closeDown(PacketNode *head, PacketNode *tail) // called when starting up the module
{
	UNREFERENCED_PARAMETER(tail);
	UNREFERENCED_PARAMETER(head);
	clearBufPackets(tail);
	endTimePeriod();
}
short NetThrottle::process(PacketNode *head, PacketNode *tail)
{	
	char pLog[MAX_LOG_LEN];
	short throttled = FALSE;
	UpdateParams();
	UNREFERENCED_PARAMETER(head);
	if (!throttleStartTick) {
		if (!isListEmpty() && calcChance(iChance)) {
			LOG(pLog, "Start new throttling w/ chance %.1f, time frame: %d", iChance/10.0, iTime);
			throttleStartTick = timeGetTime();
			throttled = TRUE;
			goto THROTTLE_START; // need this goto since maybe we'll start and stop at this single call
		}
	} else {
THROTTLE_START:
		// start a block for declaring local variables
	{
		// already throttling, keep filling up
		PacketNode *pac = tail->prev;
		DWORD currentTick = timeGetTime();
		while (bufSize < KEEP_AT_MOST && pac != head) {
			if (checkDirection(pac->addr.Direction, shIn, shOut)) {
				insertAfter(popNode(pac), bufHead);
				++bufSize;
				pac = tail->prev;
			} else {
				pac = pac->prev;
			}
		}

		// send all when throttled enough, including in current step
		if (bufSize >= KEEP_AT_MOST || (currentTick - throttleStartTick > (unsigned int)iTime)) {
			// drop throttled if dropThrottled is toggled
			if (dropThrottled) {
				dropBufPackets();
			} else {
				clearBufPackets(tail);
			}
		}
	}
	}

	return throttled;
}
bool NetThrottle::enable()
{
	if ( g_simParam )
	{
		return g_simParam->throttle.bEnable;
	}
	
	return false;
}
void NetThrottle::UpdateParams()
{
	if ( g_simParam )
	{
		shIn = g_simParam->throttle.shIn;
		shOut = g_simParam->throttle.shOut;
		iChance = g_simParam->throttle.iChance;
		iTime = g_simParam->throttle.iTime;	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! NetDuplicate
NetDuplicate::NetDuplicate()
{
	shIn = 1;
	shOut = 1;
	iChance = 1000;
	iCount = 2;
}
NetDuplicate::~NetDuplicate()
{

}
void NetDuplicate::startUp()			// called when starting up the module
{
	char pLog[MAX_LOG_LEN];
	LOG(pLog, "dup enabled");
}
void NetDuplicate::closeDown(PacketNode *head, PacketNode *tail) // called when starting up the module
{
	char pLog[MAX_LOG_LEN];
	UNREFERENCED_PARAMETER(head);
	UNREFERENCED_PARAMETER(tail);
	LOG(pLog, "dup disabled");
}
short NetDuplicate::process(PacketNode *head, PacketNode *tail)
{
	char pLog[MAX_LOG_LEN];
	short duped = FALSE;
	PacketNode *pac = head->next;
	UpdateParams();
	while (pac != tail) {
		if (checkDirection(pac->addr.Direction, shIn, shOut)
			&& calcChance(iChance)) {
				short copies = iCount - 1;
				LOG(pLog, "duplicating w/ chance %.1f%%, cloned additionally %d packets", iChance/100.0, copies);
				while (copies--) {
					PacketNode *copy = createNode(pac->packet, pac->packetLen, &(pac->addr));
					insertBefore(copy, pac); // must insertBefore or next packet is still pac
				}
				duped = TRUE;
		}
		pac = pac->next;
	}
	return duped;
}
bool NetDuplicate::enable()
{
	if ( g_simParam )
	{
		return g_simParam->duplicate.bEnable;		
	}
	else
		return false;
}
void NetDuplicate::UpdateParams()
{
	if ( g_simParam )
	{
		shIn = g_simParam->duplicate.shIn;
		shOut = g_simParam->duplicate.shOut;
		iChance = g_simParam->duplicate.iChance;
		iCount = g_simParam->duplicate.iCount;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define KEEP_TURNS_MAX 10 
//! NetOOD
NetOOD::NetOOD()
{
	shIn = 1;
	shOut = 1;
	iChance = 1000;
	oodPacket = NULL;
}
NetOOD::~NetOOD()
{

}
void NetOOD::startUp()			// called when starting up the module
{
	char pLog[MAX_LOG_LEN];
	LOG(pLog, "ood enabled");
	giveUpCnt = KEEP_TURNS_MAX;
	// assert on the issue that repeatly enable/disable abort the program
	assert(oodPacket == NULL);
}
void NetOOD::closeDown(PacketNode *head, PacketNode *tail) // called when starting up the module
{
	char pLog[MAX_LOG_LEN];
	UNREFERENCED_PARAMETER(tail);
	LOG(pLog, "ood disabled");
	if (oodPacket != NULL) {
		insertAfter(oodPacket, head);
		oodPacket = NULL; // ! need to empty the ood packet
	}
}

// find the next packet fits the direction check or null
PacketNode* NetOOD::nextCorrectDirectionNode(PacketNode *p) {
	if (p == NULL) {
		return NULL;
	}

	do {
		p = p->next;
	} while (p->next != NULL && !checkDirection(p->addr.Direction, shIn, shOut));

	return p->next == NULL ? NULL : p;
}

// not really perfect swap since it assumes a is before b
void NetOOD::swapNode(PacketNode *a, PacketNode *b) {
	assert(a->prev && a->next && b->prev && b->next); // not accidentally swapping head/tail
	assert(a != b); // treat swap self as error here since we shouldn't really be doing it
	if (a->next == b) {
		// adjacent nodes need special care
		a->prev->next = b;
		b->next->prev = a;
		a->next = b->next;
		b->prev = a->prev;
		a->prev = b;
		b->next = a;
	} else {
		PacketNode *pa = a->prev,
			*na = a->next,
			*pb = b->prev,
			*nb = b->next;
		pa->next = na->prev = b;
		b->prev = pa;
		b->next = na;
		pb->next = nb->prev = a;
		a->prev = pb;
		a->next = nb;
	}
}

short NetOOD::process(PacketNode *head, PacketNode *tail)
{
	char pLog[MAX_LOG_LEN];
	UpdateParams();
	if (oodPacket != NULL) {
		if (!isListEmpty() || --giveUpCnt == 0) {
			LOG(pLog, "Ooo sent direction %s, is giveup %s", BOUND_TEXT(oodPacket->addr.Direction), giveUpCnt ? "NO" : "YES");
			insertAfter(oodPacket, head);
			oodPacket = NULL;
			giveUpCnt = KEEP_TURNS_MAX;
		} // skip picking packets when having oodPacket already
	} else if (!isListEmpty()) {
		PacketNode *pac = head->next;
		if (pac->next == tail) {
			// only contains a single packet, then pick it out and insert later
			if (checkDirection(pac->addr.Direction, shIn, shOut) && calcChance(iChance)) {
				oodPacket = popNode(pac);
				LOG(pLog, "Ooo picked packet w/ chance %.1f%%, direction %s", iChance/100.0, BOUND_TEXT(pac->addr.Direction));
				return TRUE;
			}
		} else if (calcChance(iChance)) {
			// since there's already multiple packets in the queue, do a reorder will be enough
			PacketNode *first = head, *second;
			do {
				first = nextCorrectDirectionNode(first);
				second = nextCorrectDirectionNode(first);
				// calculate chance per swap
				if (first && second && calcChance(iChance)) {
					swapNode(first, second);
					LOG(pLog, "Multiple packets OOD swapping");
				} else {
					// move forward first to progress
					first = second;
				}
			} while (first && second);
			return TRUE;
		}
	}

	return FALSE;

}
bool NetOOD::enable()
{
	if ( g_simParam)
	{
		return g_simParam->ood.bEnable;
	}
	else
		return false;
}
void NetOOD::UpdateParams()
{
	if ( g_simParam )
	{
		shIn = g_simParam->ood.shIn;
		shOut = g_simParam->ood.shOut;
		iChance = g_simParam->ood.iChance;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char patterns[] = {
	0x64,
	0x13,
	0x88,

	0x40,
	0x1F,
	0xA0,

	0xAA,
	0x55
};
//! NetTamper
NetTamper::NetTamper()
{
	shIn = 1;
	shOut = 1;
	iChance = 1000;
	shReSum = 1;
}
NetTamper::~NetTamper()
{

}
void NetTamper::tamper_buf(char *buf, UINT len)
{
	UINT ix;
	for (ix = 0; ix < len; ++ix) {
		buf[ix] ^= patterns[patIx++ & 0x7];
	}
}
void NetTamper::startUp()			// called when starting up the module
{
	char pLog[MAX_LOG_LEN];
	LOG(pLog, "tamper enabled");
	patIx = 0;
}
void NetTamper::closeDown(PacketNode *head, PacketNode *tail) // called when starting up the module
{
	char pLog[MAX_LOG_LEN];
	UNREFERENCED_PARAMETER(head);
	UNREFERENCED_PARAMETER(tail);
	LOG(pLog, "tamper disabled");
}
short NetTamper::process(PacketNode *head, PacketNode *tail)
{
	char pLog[MAX_LOG_LEN];
	short tampered = FALSE;
	PacketNode *pac = head->next;
	UpdateParams();
	while (pac != tail) {
		if (checkDirection(pac->addr.Direction, shIn, shOut)
			&& calcChance(iChance)) {
				char *data = NULL;
				UINT dataLen = 0;
				if (WinDivertHelperParsePacket(pac->packet, pac->packetLen, NULL, NULL, NULL,
					NULL, NULL, NULL, (PVOID*)&data, &dataLen) 
					&& data != NULL && dataLen != 0) {
						// try to tamper the central part of the packet,
						// since common packets put their checksum at head or tail
						if (dataLen <= 4) {
							// for short packet just tamper it all
							tamper_buf(data, dataLen);
							LOG(pLog, "tampered w/ chance %.1f, dochecksum: %d, short packet changed all", iChance/100.0, shReSum);
						} else {
							// for longer ones process 1/4 of the lens start somewhere in the middle
							UINT len = dataLen;
							UINT len_d4 = len / 4;
							tamper_buf(data + len/2 - len_d4/2 + 1, len_d4);
							LOG(pLog, "tampered w/ chance %.1f, dochecksum: %d, changing %d bytes out of %u", iChance/100.0, shReSum, len_d4, len);
						}
						// FIXME checksum seems to have some problem
						if (shReSum) {
							WinDivertHelperCalcChecksums(pac->packet, pac->packetLen, 0);
						}
						tampered = TRUE;
				}

		}
		pac = pac->next;
	}
	return tampered;
}
bool NetTamper::enable()
{
	if (g_simParam)
	{
		return g_simParam->tamper.bEnable;
	}
	else
		return false;
}
void NetTamper::UpdateParams()
{
	if ( g_simParam )
	{
		shIn = g_simParam->tamper.shIn;
		shOut = g_simParam->tamper.shOut;
		iChance = g_simParam->tamper.iChance;
		shReSum = g_simParam->tamper.shReSum;	
	}
}

#endif
