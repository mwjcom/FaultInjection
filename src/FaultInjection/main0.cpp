#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <iostream>
#include <string>

#include "Altova.h"
#include "SchemaTypes.h"
#include "AltovaException.h"
#include "SchemaTypeString.h"
#include "SchemaTypeNumber.h"
#include "SchemaTypeCalendar.h"
#include "SchemaTypeBinary.h"
#include "XmlException.h"
#include "Doc.h"
#include "Node.h"

using namespace altova;
#include "../Common/FaultInjectionXML/FaultInjectionXML.h"
#include "../DBFaultSim/DBFaultSim.h"
#include "../HostFaultSim/HostFaultManager.h"

#ifdef _DEBUG
#pragma comment(lib, "xerces-c_2D")
#pragma comment(lib, "Altova_Dll_Xerces_nomfcD")
#pragma comment(lib, "AltovaXML_Dll_Xerces_nomfcD")
#else
#pragma comment(lib, "xerces-c_2")
#pragma comment(lib, "Altova_Dll_Xerces_nomfc")
#pragma comment(lib, "AltovaXML_Dll_Xerces_nomfc")
#endif


int main(int argc, char *argv[])
{
// #ifdef _XERCES_VERSION
	xercesc::XMLPlatformUtils::Initialize();
//#else
	//CoInitialize(NULL);
//#endif

	NSFaultInjectionXML::CFaultInjectionXMLDoc doc ;

	CTestTask g_TestTask;
	CHostFaultManager aa;
	

	xercesc::XMLPlatformUtils::Terminate();

	return 0;

}

