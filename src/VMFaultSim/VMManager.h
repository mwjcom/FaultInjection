#ifndef VM_MANAGER_H
#define VM_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include "ConstDef.h"
#include "vix.h"

#ifdef VMFAULTSIM_EXPORTS
#define VMFAULTSIM_API USAGEDLL_EXPORT
#else
#define VMFAULTSIM_API USAGEDLL_IMPORT
#endif
class VMFAULTSIM_API CVMManager
{
public:
	CVMManager(void);
	~CVMManager(void);

	int OpenVM(std::string vmPath);
	int CloseVM(std::string vmPath);
	int SuspendVM(std::string vmPath);

	VixHandle hostHandle;

};

extern VMFAULTSIM_API CVMManager* g_vmManager;

#endif

