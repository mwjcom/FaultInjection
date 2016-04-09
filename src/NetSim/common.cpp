#include <stdlib.h>
#include <memory.h>

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "windivert.h"
#endif

#include "common.h"
#include "NetSimulates.h"
#include "NetSimMgr.h"

#ifdef WIN32
#define DIVERT_PRIORITY 0
#define MAX_PACKETSIZE 0xFFFF
#define READ_TIME_PER_STEP 3
// FIXME does this need to be larger then the time to process the list?
#define CLOCK_WAITMS 40
#define QUEUE_LEN 2 << 10
#define QUEUE_TIME 2 << 9 

static HANDLE divertHandle;
static volatile short stopLooping;
static HANDLE loopThread, clockThread, mutex;
time_t rawtime;  
struct tm * tf;

static DWORD divertReadLoop(LPVOID arg);
static DWORD divertClockLoop(LPVOID arg);

//// not to put these in common.h since modules shouldn't see these
//extern PacketNode * const head;
//extern PacketNode * const tail;

static PacketNode headNode = {0}, tailNode = {0};
PacketNode * const head = &headNode, * const tail = &tailNode;

void initPacketNodeList() {
	if (head->next == NULL && tail->prev == NULL) {
		// first time initializing
		head->next = tail;
		tail->prev = head;
	} else {
		// have used before, then check node is empty
		assert(isListEmpty());
	}
}

// TODO  using malloc in the loop is not good for performance
//       just not sure I can write a better memory allocator
PacketNode* createNode(char* buf, UINT len, WINDIVERT_ADDRESS *addr) {
	PacketNode *newNode = (PacketNode*)malloc(sizeof(PacketNode));
	newNode->packet = (char*)malloc(len);
	memcpy(newNode->packet, buf, len);
	newNode->packetLen = len;
	memcpy(&(newNode->addr), addr, sizeof(WINDIVERT_ADDRESS));
	newNode->next = newNode->prev = NULL;
	return newNode;
}

void freeNode(PacketNode *node) {
	assert((node != head) && (node != tail));
	free(node->packet);
	free(node);
}

PacketNode* popNode(PacketNode *node) {
	assert((node != head) && (node != tail));
	node->prev->next = node->next;
	node->next->prev = node->prev;
	return node;
}

PacketNode* insertAfter(PacketNode *node, PacketNode *target) {
	assert(node && node != head && node != tail && target != tail);
	node->prev = target;
	node->next = target->next;
	target->next->prev = node;
	target->next = node;
	return node;
}

PacketNode* insertBefore(PacketNode *node, PacketNode *target) {
	assert(node && node != head && node != tail && target != head);
	node->next = target;
	node->prev = target->prev;
	target->prev->next = node;
	target->prev = node;
	return node;
}

PacketNode* appendNode(PacketNode *node) {
	return insertBefore(node, tail);
}

short isListEmpty() {
	return head->next == tail;
}

#ifdef _DEBUG
PWINDIVERT_IPHDR ip_header;
PWINDIVERT_IPV6HDR ipv6_header;
PWINDIVERT_TCPHDR tcp_header;
PWINDIVERT_UDPHDR udp_header;
PWINDIVERT_ICMPHDR icmp_header;
PWINDIVERT_ICMPV6HDR icmpv6_header;
UINT payload_len;
void dumpPacket(char *buf, int len, PWINDIVERT_ADDRESS paddr) {
    char *protocol;
    UINT16 srcPort = 0, dstPort = 0;
	char pLog[MAX_LOG_LEN];
    WinDivertHelperParsePacket(buf, len, &ip_header, &ipv6_header,
        &icmp_header, &icmpv6_header, &tcp_header, &udp_header,
        NULL, &payload_len);
    // need to cast byte order on port numbers
    if (tcp_header != NULL) {
        protocol = "TCP ";
        srcPort = ntohs(tcp_header->SrcPort);
        dstPort = ntohs(tcp_header->DstPort);
    } else if (udp_header != NULL) {
        protocol = "UDP ";
        srcPort = ntohs(udp_header->SrcPort);
        dstPort = ntohs(udp_header->DstPort);
    } else if (icmp_header || icmpv6_header) {
        protocol = "ICMP";
    } else {
        protocol = "???";
    }

    if (ip_header != NULL) {
        UINT8 *src_addr = (UINT8*)&ip_header->SrcAddr;
        UINT8 *dst_addr = (UINT8*)&ip_header->DstAddr;
        LOG(pLog,"%s.%s: %u.%u.%u.%u:%d->%u.%u.%u.%u:%d",
            protocol,
            paddr->Direction == WINDIVERT_DIRECTION_OUTBOUND ? "OUT " : "IN  ",
            src_addr[0], src_addr[1], src_addr[2], src_addr[3], srcPort,
            dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3], dstPort);
    } else if (ipv6_header != NULL) {
        UINT16 *src_addr6 = (UINT16*)&ipv6_header->SrcAddr;
        UINT16 *dst_addr6 = (UINT16*)&ipv6_header->DstAddr;
		LOG(pLog,"%s.%s: %x:%x:%x:%x:%x:%x:%x:%x:%d->%x:%x:%x:%x:%x:%x:%x:%x:%d",
			protocol,
			paddr->Direction == WINDIVERT_DIRECTION_OUTBOUND ? "OUT " : "IN  ",
			src_addr6[0], src_addr6[1], src_addr6[2], src_addr6[3],
			src_addr6[4], src_addr6[5], src_addr6[6], src_addr6[7], srcPort,
			dst_addr6[0], dst_addr6[1], dst_addr6[2], dst_addr6[3],
			dst_addr6[4], dst_addr6[5], dst_addr6[6], dst_addr6[7], dstPort);
    }
}
#else
#define dumpPacket(x, y, z)
#endif

int divertStart(const char *filter, char buf[]) {
    int ix;
	char pLog[MAX_LOG_LEN];
    divertHandle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, DIVERT_PRIORITY, 0);
    if (divertHandle == INVALID_HANDLE_VALUE) {
        DWORD lastError = GetLastError();
        if (lastError == ERROR_INVALID_PARAMETER) {
            strcpy(buf, "Failed to start filtering : filter syntax error.");
        } else {
            sprintf(buf, "Failed to start filtering : failed to open device (code:%lu).\n"
                "Make sure you run clumsy as Administrator.", lastError);
        }
        return FALSE;
    }
    LOG(pLog,"Divert opened handle.");

    WinDivertSetParam(divertHandle, WINDIVERT_PARAM_QUEUE_LEN, QUEUE_LEN);
    WinDivertSetParam(divertHandle, WINDIVERT_PARAM_QUEUE_TIME, QUEUE_TIME);
    LOG(pLog, "WinDivert internal queue Len: %d, queue time: %d", QUEUE_LEN, QUEUE_TIME);

    // init package link list
    initPacketNodeList();

    // reset module
    for (ix = 0; ix < g_pNetSimMgr->modules.size(); ++ix) {
        g_pNetSimMgr->modules[ix]->lastEnabled = 0;
    }

    // kick off the loop
    LOG(pLog, "Creating threads and mutex...");
    stopLooping = FALSE;
    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        sprintf(buf, "Failed to create mutex (%lu)", GetLastError());
        return FALSE;
    }

    loopThread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)divertReadLoop, NULL, 0, NULL);
    if (loopThread == NULL) {
        sprintf(buf, "Failed to create recv loop thread (%lu)", GetLastError());
        return FALSE;
    }
    clockThread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)divertClockLoop, NULL, 0, NULL);
    if (clockThread == NULL) {
        sprintf(buf, "Failed to create clock loop thread (%lu)", GetLastError());
        return FALSE;
    }

    LOG(pLog, "Threads created");

    return TRUE;
}

static int sendAllListPackets() {
    // send packet from tail to head and remove sent ones
    int sendCount = 0;
    UINT sendLen;
    PacketNode *pnode;
	char pLog[MAX_LOG_LEN];
#ifdef _DEBUG
    // check the list is good
    // might go into dead loop but it's better for debugging
    PacketNode *p = head;
    do {
        p = p->next;
    } while (p->next);
    assert(p == tail);
#endif

    while (!isListEmpty()) {
        pnode = popNode(tail->prev);
        sendLen = 0;
        assert(pnode != head);
        // FIXME inbound injection on any kind of packet is failing with a very high percentage
        //       need to contact windivert auther and wait for next release
        if (!WinDivertSend(divertHandle, pnode->packet, pnode->packetLen, &(pnode->addr), &sendLen)) {
            PWINDIVERT_ICMPHDR icmp_header;
            PWINDIVERT_ICMPV6HDR icmpv6_header;
            PWINDIVERT_IPHDR ip_header;
            PWINDIVERT_IPV6HDR ipv6_header;
            LOG(pLog, "Failed to send a packet. (%lu)", GetLastError());
            dumpPacket(pnode->packet, pnode->packetLen, &(pnode->addr));
            // as noted in windivert help, reinject inbound icmp packets some times would fail
            // workaround this by resend them as outbound
            // TODO not sure is this even working as can't find a way to test
            //      need to document about this
            WinDivertHelperParsePacket(pnode->packet, pnode->packetLen, &ip_header, &ipv6_header,
                &icmp_header, &icmpv6_header, NULL, NULL, NULL, NULL);
            if ((icmp_header || icmpv6_header) && IS_INBOUND(pnode->addr.Direction)) {
                BOOL resent;
                pnode->addr.Direction = WINDIVERT_DIRECTION_OUTBOUND;
                if (ip_header) {
                    UINT32 tmp = ip_header->SrcAddr;
                    ip_header->SrcAddr = ip_header->DstAddr;
                    ip_header->DstAddr = tmp;
                } else if (ipv6_header) {
                    UINT32 tmpArr[4];
                    memcpy(tmpArr, ipv6_header->SrcAddr, sizeof(tmpArr));
                    memcpy(ipv6_header->SrcAddr, ipv6_header->DstAddr, sizeof(tmpArr));
                    memcpy(ipv6_header->DstAddr, tmpArr, sizeof(tmpArr));
                }
                resent = WinDivertSend(divertHandle, pnode->packet, pnode->packetLen, &(pnode->addr), &sendLen);
                LOG(pLog, "Resend failed inbound ICMP packets as outbound: %s", resent ? "SUCCESS" : "FAIL");
                InterlockedExchange((LONG*)&sendState, SEND_STATUS_SEND);
            } else {
                InterlockedExchange((LONG*)&sendState, SEND_STATUS_FAIL);
            }
        } else {
            if (sendLen < pnode->packetLen) {
                // TODO don't know how this can happen, or it needs to be resent like good old UDP packet
                LOG(pLog, "Internal Error: DivertSend truncated send packet.");
                InterlockedExchange((LONG*)&sendState, SEND_STATUS_FAIL);
            } else {
                InterlockedExchange((LONG*)&sendState, SEND_STATUS_SEND);
            }
        }


        freeNode(pnode);
        ++sendCount;
    }
    assert(isListEmpty()); // all packets should be sent by now

    return sendCount;
}

// step function to let module process and consume all packets on the list
static void divertConsumeStep() {
#ifdef _DEBUG
    DWORD startTick = GetTickCount(), dt;
#endif
	char pLog[MAX_LOG_LEN];
    int ix, cnt;
    // use lastEnabled to keep track of module starting up and closing down
    for (ix = 0; ix < g_pNetSimMgr->modules.size(); ++ix) {
        NetworkSim *module = g_pNetSimMgr->modules[ix];
        if (module->enable()) {
            if (!module->lastEnabled) {
                module->startUp();
                module->lastEnabled = 1;
            }
            if (module->process(head, tail)) {
                InterlockedIncrement((LONG*)&(module->processTriggered));
            }
        } else {
            if (module->lastEnabled) {
                module->closeDown(head, tail);
                module->lastEnabled = 0;
            }
        }
    }
    cnt = sendAllListPackets();
#ifdef _DEBUG
    dt =  GetTickCount() - startTick;
    if (dt > CLOCK_WAITMS / 2) {
        LOG(pLog, "Costy consume step: %lu ms, sent %d packets", GetTickCount() - startTick, cnt);
    }
#endif
}

// periodically try to consume packets to keep the network responsive and not blocked by recv
static DWORD divertClockLoop(LPVOID arg) {
    DWORD startTick, stepTick, waitResult;
    int ix;
	char pLog[MAX_LOG_LEN];
    UNREFERENCED_PARAMETER(arg);

    for(;;) {
        // use acquire as wait for yielding thread
        startTick = GetTickCount();
        waitResult = WaitForSingleObject(mutex, CLOCK_WAITMS);
        switch(waitResult) {
            case WAIT_OBJECT_0:
                /***************** enter critical region ************************/
                divertConsumeStep();
                /***************** leave critical region ************************/
                if (!ReleaseMutex(mutex)) {
                    InterlockedIncrement((LONG*)&stopLooping);
                    LOG(pLog, "Fatal: Failed to release mutex (%lu)", GetLastError());
                    ABORT();
                }
                // if didn't spent enough time, we sleep on it
                stepTick = GetTickCount() - startTick;
                if (stepTick < CLOCK_WAITMS) {
                    Sleep(CLOCK_WAITMS - stepTick);
                }
                break;
            case WAIT_TIMEOUT:
                // read loop is processing, so we can skip this run
                LOG(pLog, "!!! Skipping one run");
                Sleep(CLOCK_WAITMS);
                break;
            case WAIT_ABANDONED:
                LOG(pLog, "Acquired abandoned mutex");
                InterlockedIncrement((LONG*)&stopLooping);
                break;
            case WAIT_FAILED:
                LOG(pLog, "Acquire failed (%lu)", GetLastError());
                InterlockedIncrement((LONG*)&stopLooping);
                break;
        }

        // need to get the lock here
        if (stopLooping) {
            int lastSendCount = 0;
            BOOL closed;

            waitResult = WaitForSingleObject(mutex, INFINITE);
            switch (waitResult)
            {
            case WAIT_ABANDONED:
            case WAIT_FAILED:
                LOG(pLog, "Acquire failed/abandoned mutex (%lu), will still try closing and return", GetLastError());
            case WAIT_OBJECT_0:
                /***************** enter critical region ************************/
                LOG(pLog, "Read stopLooping, stopping...");
                // clean up by closing all modules
                for (ix = 0; ix < g_pNetSimMgr->modules.size(); ++ix) {
                    NetworkSim *module = g_pNetSimMgr->modules[ix];
                    if (module->enable()) {
                        module->closeDown(head, tail);
                    } 
                }
                LOG(pLog, "Send all packets upon closing");
                lastSendCount = sendAllListPackets();
                LOG(pLog, "Lastly sent %d packets. Closing...", lastSendCount);

                // terminate recv loop by closing handler. handle related error in recv loop to quit
                closed = WinDivertClose(divertHandle);
                assert(closed);

                // release to let read loop exit properly
                /***************** leave critical region ************************/
                if (!ReleaseMutex(mutex)) {
                    LOG(pLog, "Fatal: Failed to release mutex (%lu)", GetLastError());
                    ABORT();
                }
                return 0;
                break;
            }
        }
    }
}

static DWORD divertReadLoop(LPVOID arg) {
    char packetBuf[MAX_PACKETSIZE];
    WINDIVERT_ADDRESS addrBuf;
    UINT readLen;
    PacketNode *pnode;
    DWORD waitResult;
	char pLog[MAX_LOG_LEN];
    UNREFERENCED_PARAMETER(arg);

    for(;;) {
        // each step must fully consume the list
        assert(isListEmpty()); // FIXME has failed this assert before. don't know why
        if (!WinDivertRecv(divertHandle, packetBuf, MAX_PACKETSIZE, &addrBuf, &readLen)) {
            DWORD lastError = GetLastError();
            if (lastError == ERROR_INVALID_HANDLE || lastError == ERROR_OPERATION_ABORTED) {
                // treat closing handle as quit
                LOG(pLog, "Handle died or operation aborted. Exit loop.");
                return 0;
            }
            LOG(pLog, "Failed to recv a packet. (%lu)", GetLastError());
            continue;
        }
        if (readLen > MAX_PACKETSIZE) {
            // don't know how this can happen
            LOG(pLog, "Internal Error: DivertRecv truncated recv packet."); 
        }

        //dumpPacket(packetBuf, readLen, &addrBuf);  

        waitResult = WaitForSingleObject(mutex, INFINITE);
        switch(waitResult) {
            case WAIT_OBJECT_0:
                /***************** enter critical region ************************/
                if (stopLooping) {
                    LOG(pLog, "Lost last recved packet but user stopped. Stop read loop.");
                    /***************** leave critical region ************************/
                    if (!ReleaseMutex(mutex)) {
                        LOG(pLog, "Fatal: Failed to release mutex on stopping (%lu). Will stop anyway.", GetLastError());
                    }
                    return 0;
                }
                // create node and put it into the list
                pnode = createNode(packetBuf, readLen, &addrBuf);
                appendNode(pnode);
                divertConsumeStep();
                /***************** leave critical region ************************/
                if (!ReleaseMutex(mutex)) {
                    LOG(pLog, "Fatal: Failed to release mutex (%lu)", GetLastError());
                    ABORT();
                }
                break;
            case WAIT_TIMEOUT:
                LOG(pLog, "Acquire timeout, dropping one read packet");
                continue;
                break;
            case WAIT_ABANDONED:
                LOG(pLog, "Acquire abandoned.");
                return 0;
            case WAIT_FAILED:
                LOG(pLog, "Acquire failed.");
                return 0;
        }
    }
}

void divertStop() {
    HANDLE threads[2];
    threads[0] = loopThread;
    threads[1] = clockThread;
	char pLog[MAX_LOG_LEN];
    LOG(pLog, "Stopping...");
    InterlockedIncrement((LONG*)&stopLooping);
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    LOG(pLog, "Successfully waited threads and stopped.");
}

short calcChance(short chance) {
	// notice that here we made a copy of chance, so even though it's volatile it is still ok
	return (chance == 10000) || ((rand() % 10000) < chance);
}

static short resolutionSet = 0;

void startTimePeriod() {
	if (!resolutionSet) {
		// begin only fails when period out of range
		timeBeginPeriod(TIMER_RESOLUTION);
		resolutionSet = 1;
	}
}

void endTimePeriod() {
	if (resolutionSet) {
		timeEndPeriod(TIMER_RESOLUTION);
		resolutionSet = 0;
	}
}

#endif //WIN32
