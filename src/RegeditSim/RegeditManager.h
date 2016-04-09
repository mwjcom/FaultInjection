#ifndef REGEDIT_MANAGER_H
#define REGEDIT_MANAGER_H

#include "ConstDef.h"
#include <atlbase.h>
#include <vector>

#ifdef REGEDITSIM_EXPORTS
#define REGSIM_API USAGEDLL_EXPORT
#else
#define REGSIM_API USAGEDLL_IMPORT
#endif

typedef struct RegItem
{
	HKEY mainPath;
	std::string strPath;
	DWORD dwType;
	std::string strKey;
	std::string strValue;
}RegItem;

class REGSIM_API CRegeditManager
{
public:
	CRegeditManager(void);
	~CRegeditManager(void);

	int RegeditOccupy(std::vector<RegItem>& vtReg);

	int RegeditRelease(std::vector<RegItem>& vtReg);

	//std::vector<RegItem> m_vtReg;
};

extern REGSIM_API CRegeditManager* g_regeditManager;

#endif

