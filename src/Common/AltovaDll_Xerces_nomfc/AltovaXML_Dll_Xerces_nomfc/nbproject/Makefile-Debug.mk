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
OBJECTDIR=build/Debug/GNU-Generic

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Doc.o \
	${OBJECTDIR}/StdAfx.o \
	${OBJECTDIR}/Node.o \
	${OBJECTDIR}/XmlException.o

# C Compiler Flags
CFLAGS=-fPIC

# CC Compiler Flags
CCFLAGS=-fPIC
CXXFLAGS=-fPIC

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} ../../../PublicProject/Debug/bin/libAltovaXML_Dll_Xerces_nomfc.so

../../../PublicProject/Debug/bin/libAltovaXML_Dll_Xerces_nomfc.so: ${OBJECTFILES}
	${MKDIR} -p ../../../PublicProject/Debug/bin
	${LINK.cc} -shared -o ../../../PublicProject/Debug/bin/libAltovaXML_Dll_Xerces_nomfc.so ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Doc.o: Doc.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Doc.o Doc.cpp

${OBJECTDIR}/StdAfx.o: StdAfx.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -o ${OBJECTDIR}/StdAfx.o StdAfx.cpp

${OBJECTDIR}/Node.o: Node.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Node.o Node.cpp

${OBJECTDIR}/XmlException.o: XmlException.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -o ${OBJECTDIR}/XmlException.o XmlException.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} ../../../PublicProject/Debug/bin/libAltovaXML_Dll_Xerces_nomfc.so

# Subprojects
.clean-subprojects:
