#include "VMManager.h"
#include <string>

/*
 * Certain arguments differ when using VIX with VMware Server 2.0
 * and VMware Workstation.
 *
 * Comment out this definition to use this code with VMware Server 2.0.
 */
#define USE_WORKSTATION

#ifdef USE_WORKSTATION

#define  CONNTYPE    VIX_SERVICEPROVIDER_VMWARE_WORKSTATION

#define  HOSTNAME ""
#define  HOSTPORT 0
#define  USERNAME ""
#define  PASSWORD ""

#define  VMPOWEROPTIONS   VIX_VMPOWEROP_LAUNCH_GUI   // Launches the VMware Workstaion UI
                                                     // when powering on the virtual machine.

#define VMXPATH_INFO "where vmxpath is an absolute path to the .vmx file " \
                     "for the virtual machine."

#else    // USE_WORKSTATION

/*
 * For VMware Server 2.0
 */

#define CONNTYPE VIX_SERVICEPROVIDER_VMWARE_VI_SERVER

#define HOSTNAME "https://192.2.3.4:8333/sdk"
/*
 * NOTE: HOSTPORT is ignored, so the port should be specified as part
 * of the URL.
 */
#define HOSTPORT 0
#define USERNAME "root"
#define PASSWORD "hideme"

#define  VMPOWEROPTIONS VIX_VMPOWEROP_NORMAL

#define VMXPATH_INFO "where vmxpath is a datastore-relative path to the " \
                     ".vmx file for the virtual machine, such as "        \
                     "\"[standard] ubuntu/ubuntu.vmx\"."

#endif    // USE_WORKSTATION

#define MAX_LOG_LEN 256

#ifndef WIN32
#define LOG(op, fmt, ...)   \
	sprintf(op,/*"%04d-%02d-%02d %02d:%02d:%02d    "*/": " fmt , \
	/*tf->tm_year, tf->tm_mon, tf->tm_yday, tf->tm_hour, tf->tm_min, tf->tm_sec,*/ ##__VA_ARGS__); //\
	//if(g_pFaultInjection) g_pFaultInjection->AddLog(op);
#else
#define LOG(op, fmt, ...)   \
	(sprintf(op,/*"%04d-%02d-%02d %02d:%02d:%02d    "*/__FUNCTION__ ": " fmt , \
	/*tf->tm_year, tf->tm_mon, tf->tm_yday, tf->tm_hour, tf->tm_min, tf->tm_sec,*/ ##__VA_ARGS__)); //\
	//if(g_pFaultInjection) g_pFaultInjection->AddLog(op);
#endif


extern VMFAULTSIM_API CVMManager* g_vmManager = NULL;

CVMManager::CVMManager(void)
{
	char pLog[MAX_LOG_LEN];
	VixError err;
	VixHandle jobHandle = VIX_INVALID_HANDLE;

	jobHandle = VixHost_Connect(VIX_API_VERSION,
		CONNTYPE,
		HOSTNAME, // *hostName,
		HOSTPORT, // hostPort,
		USERNAME, // *userName,
		PASSWORD, // *password,
		0, // options,
		VIX_INVALID_HANDLE, // propertyListHandle,
		NULL, // *callbackProc,
		NULL); // *clientData);

	err = VixJob_Wait(jobHandle, 
		VIX_PROPERTY_JOB_RESULT_HANDLE, 
		&hostHandle,
		VIX_PROPERTY_NONE);
	if (VIX_FAILED(err)) {
		LOG(pLog, "Init VixHost error!!");
	}

	Vix_ReleaseHandle(jobHandle);
}


CVMManager::~CVMManager(void)
{
	VixHost_Disconnect(hostHandle);	
}

int CVMManager::OpenVM(std::string vmxPath)
{
	char pLog[MAX_LOG_LEN];
	VixHandle vmHandle;
	//! release before
	if (vmHandle != VIX_INVALID_HANDLE)
	{
		Vix_ReleaseHandle(vmHandle);
		vmHandle = VIX_INVALID_HANDLE;
	}

	//! get path
	VixHandle jobHandle = VIX_INVALID_HANDLE;
	jobHandle = VixVM_Open(hostHandle,
		vmxPath.c_str(),
		NULL, // VixEventProc *callbackProc,
		NULL); // void *clientData);
	//jobHandle = VixHost_OpenVM(hostHandle,
	//	vmxPath,
	//	VIX_VMOPEN_NORMAL,
	//	VIX_INVALID_HANDLE,
	//	NULL, // callbackProc
	//	NULL); // clientData
	VixError err = VixJob_Wait(jobHandle, 
		VIX_PROPERTY_JOB_RESULT_HANDLE, 
		&vmHandle,
		VIX_PROPERTY_NONE);
	if (VIX_FAILED(err)) {
		LOG(pLog, "Open VixVM %s error!!", vmxPath.c_str());
		Vix_ReleaseHandle(jobHandle);

		return -1;
	}

	Vix_ReleaseHandle(jobHandle);

	int iCPUCount = 0;
	int iMemSize = 0;
	VixPowerState vxState;
	err = Vix_GetProperties(vmHandle,
		VIX_PROPERTY_VM_NUM_VCPUS,
		&iCPUCount,
		VIX_PROPERTY_VM_MEMORY_SIZE,
		&iMemSize,
		VIX_PROPERTY_VM_POWER_STATE,
		&vxState,
		VIX_PROPERTY_NONE);

	if (VIX_OK != err) {
		LOG(pLog, "Get properties of %s error!!", vmxPath.c_str());
		return -1;
	}

	if (vxState == VIX_POWERSTATE_POWERED_OFF
		|| vxState == VIX_POWERSTATE_SUSPENDED
		|| vxState == VIX_POWERSTATE_PAUSED)
	{
		jobHandle = VIX_INVALID_HANDLE;
		jobHandle = VixVM_PowerOn(vmHandle,
			VMPOWEROPTIONS,
			VIX_INVALID_HANDLE,
			NULL, // *callbackProc,
			NULL); // *clientData);
		VixError err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			LOG(pLog, "Power on: %s error!!", vmxPath.c_str());;
		}

		Vix_ReleaseHandle(jobHandle);
	}
	else
	{
		return -1;
	}

	return 0;
}
int CVMManager::CloseVM(std::string vmxPath)
{
	char pLog[MAX_LOG_LEN];
	VixHandle vmHandle;
	//! release before
	if (vmHandle != VIX_INVALID_HANDLE)
	{
		Vix_ReleaseHandle(vmHandle);
		vmHandle = VIX_INVALID_HANDLE;
	}

	//! get path
	VixHandle jobHandle = VIX_INVALID_HANDLE;
	jobHandle = VixVM_Open(hostHandle,
		vmxPath.c_str(),
		NULL, // VixEventProc *callbackProc,
		NULL); // void *clientData);
	//jobHandle = VixHost_OpenVM(hostHandle,
	//	vmxPath,
	//	VIX_VMOPEN_NORMAL,
	//	VIX_INVALID_HANDLE,
	//	NULL, // callbackProc
	//	NULL); // clientData
	VixError err = VixJob_Wait(jobHandle, 
		VIX_PROPERTY_JOB_RESULT_HANDLE, 
		&vmHandle,
		VIX_PROPERTY_NONE);
	if (VIX_FAILED(err)) {
		LOG(pLog, "Open VixVM %s error!!", vmxPath.c_str());
		Vix_ReleaseHandle(jobHandle);

		return -1;
	}

	Vix_ReleaseHandle(jobHandle);

	int iCPUCount = 0;
	int iMemSize = 0;
	VixPowerState vxState;
	err = Vix_GetProperties(vmHandle,
		VIX_PROPERTY_VM_NUM_VCPUS,
		&iCPUCount,
		VIX_PROPERTY_VM_MEMORY_SIZE,
		&iMemSize,
		VIX_PROPERTY_VM_POWER_STATE,
		&vxState,
		VIX_PROPERTY_NONE);

	if (VIX_OK != err) {
		LOG(pLog, "Get properties of %s error!!", vmxPath.c_str());
		return -1;
	}

	if (vxState == VIX_POWERSTATE_POWERED_ON)
	{
		jobHandle = VIX_INVALID_HANDLE;
		jobHandle = VixVM_PowerOff(vmHandle,
			VIX_VMPOWEROP_NORMAL,
			NULL, // *callbackProc,
			NULL); // *clientData);
		VixError err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			LOG(pLog, "Power Off: %s error!!", vmxPath.c_str());;
		}

		Vix_ReleaseHandle(jobHandle);
	}
	else
	{
		return -1;
	}

	return 0;
}
int CVMManager::SuspendVM(std::string vmxPath)
{
	char pLog[MAX_LOG_LEN];
	VixHandle vmHandle;
	//! release before
	if (vmHandle != VIX_INVALID_HANDLE)
	{
		Vix_ReleaseHandle(vmHandle);
		vmHandle = VIX_INVALID_HANDLE;
	}

	//! get path
	VixHandle jobHandle = VIX_INVALID_HANDLE;
	jobHandle = VixVM_Open(hostHandle,
		vmxPath.c_str(),
		NULL, // VixEventProc *callbackProc,
		NULL); // void *clientData);
	//jobHandle = VixHost_OpenVM(hostHandle,
	//	vmxPath,
	//	VIX_VMOPEN_NORMAL,
	//	VIX_INVALID_HANDLE,
	//	NULL, // callbackProc
	//	NULL); // clientData
	VixError err = VixJob_Wait(jobHandle, 
		VIX_PROPERTY_JOB_RESULT_HANDLE, 
		&vmHandle,
		VIX_PROPERTY_NONE);
	if (VIX_FAILED(err)) {
		LOG(pLog, "Open VixVM %s error!!", vmxPath.c_str());
		Vix_ReleaseHandle(jobHandle);

		return -1;
	}

	Vix_ReleaseHandle(jobHandle);

	int iCPUCount = 0;
	int iMemSize = 0;
	VixPowerState vxState;
	err = Vix_GetProperties(vmHandle,
		VIX_PROPERTY_VM_NUM_VCPUS,
		&iCPUCount,
		VIX_PROPERTY_VM_MEMORY_SIZE,
		&iMemSize,
		VIX_PROPERTY_VM_POWER_STATE,
		&vxState,
		VIX_PROPERTY_NONE);

	if (VIX_OK != err) {
		LOG(pLog, "Get properties of %s error!!", vmxPath.c_str());
		return -1;
	}

	if (vxState == VIX_POWERSTATE_POWERED_ON)
	{
		jobHandle = VIX_INVALID_HANDLE;
		jobHandle = VixVM_Suspend(vmHandle,
			VIX_VMPOWEROP_NORMAL,
			NULL, // *callbackProc,
			NULL); // *clientData);
		VixError err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			LOG(pLog, "Suspend: %s error!!", vmxPath.c_str());;
		}

		Vix_ReleaseHandle(jobHandle);
	}
	else
	{
		return -1;
	}

	return 0;
}