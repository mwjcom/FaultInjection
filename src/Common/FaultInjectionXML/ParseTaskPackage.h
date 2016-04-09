#ifndef _PARSETASKPACKAGE_H_
#define _PARSETASKPACKAGE_H_

#ifndef _USRDLL
#define FaultInjectionXML_DECLSPECIFIER
#else
#ifdef FaultInjectionXML_EXPORTS
#define FaultInjectionXML_DECLSPECIFIER __declspec(dllexport)
#else
#define FaultInjectionXML_DECLSPECIFIER __declspec(dllimport)
#endif
#endif

class FaultInjectionXML_DECLSPECIFIER CParseTaskPackage
{
public:
	CParseTaskPackage(void);
	virtual ~CParseTaskPackage(void);	
};

#endif //!_PARSETASKPACKAGE_H_

