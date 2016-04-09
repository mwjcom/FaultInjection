#
# Gererated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add custumized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Release/GNU-Solaris-x86

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Doc.o \
	${OBJECTDIR}/StdAfx.o \
	${OBJECTDIR}/Node.o \
	${OBJECTDIR}/XmlException.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} dist/Release/GNU-Solaris-x86/libAltovaXML_Dll_Xerces_nomfc.so

dist/Release/GNU-Solaris-x86/libAltovaXML_Dll_Xerces_nomfc.so: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Solaris-x86
	${LINK.cc} -shared -o dist/Release/GNU-Solaris-x86/libAltovaXML_Dll_Xerces_nomfc.so ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Doc.o: Doc.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/Doc.o Doc.cpp

${OBJECTDIR}/StdAfx.o: StdAfx.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/StdAfx.o StdAfx.cpp

${OBJECTDIR}/Node.o: Node.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/Node.o Node.cpp

${OBJECTDIR}/XmlException.o: XmlException.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/XmlException.o XmlException.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/GNU-Solaris-x86/libAltovaXML_Dll_Xerces_nomfc.so

# Subprojects
.clean-subprojects:
