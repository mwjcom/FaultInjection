////////////////////////////////////////////////////////////////////////
//
// XmlException.h
//
// This file was generated by XMLSpy 2005 Enterprise Edition.
//
// YOU SHOULD NOT MODIFY THIS FILE, BECAUSE IT WILL BE
// OVERWRITTEN WHEN YOU RE-RUN CODE GENERATION.
//
// Refer to the XMLSpy Documentation for further details.
// http://www.altova.com/xmlspy
//
////////////////////////////////////////////////////////////////////////


#ifndef ALTOVA_XMLEXCEPTION_H_INCLUDED
#define ALTOVA_XMLEXCEPTION_H_INCLUDED

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "../Altova_Dll_Xerces_nomfc/AltovaException.h"

namespace altova {


class ALTOVA_DECLSPECIFIER CXmlException : public CAltovaException
{
public:
	CXmlException(int nCode, const tstring& sMessage);
};


} // namespace altova

#endif // ALTOVA_XMLEXCEPTION_H_INCLUDED
