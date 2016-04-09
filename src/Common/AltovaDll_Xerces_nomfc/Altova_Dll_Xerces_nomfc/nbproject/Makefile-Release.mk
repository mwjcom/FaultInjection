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
	${OBJECTDIR}/AltovaException.o \
	${OBJECTDIR}/SchemaTypeString.o \
	${OBJECTDIR}/SchemaTypeNumber.o \
	${OBJECTDIR}/SchemaTypeCalendar.o \
	${OBJECTDIR}/StdAfx.o \
	${OBJECTDIR}/SchemaTypeBinary.o \
	${OBJECTDIR}/SchemaTypes.o

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
.build-conf: ${BUILD_SUBPROJECTS} dist/Release/GNU-Solaris-x86/libAltova_Dll_Xerces_nomfc.so

dist/Release/GNU-Solaris-x86/libAltova_Dll_Xerces_nomfc.so: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Solaris-x86
	${LINK.cc} -shared -o dist/Release/GNU-Solaris-x86/libAltova_Dll_Xerces_nomfc.so ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/AltovaException.o: AltovaException.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/AltovaException.o AltovaException.cpp

${OBJECTDIR}/SchemaTypeString.o: SchemaTypeString.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/SchemaTypeString.o SchemaTypeString.cpp

${OBJECTDIR}/SchemaTypeNumber.o: SchemaTypeNumber.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/SchemaTypeNumber.o SchemaTypeNumber.cpp

${OBJECTDIR}/SchemaTypeCalendar.o: SchemaTypeCalendar.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/SchemaTypeCalendar.o SchemaTypeCalendar.cpp

${OBJECTDIR}/StdAfx.o: StdAfx.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/StdAfx.o StdAfx.cpp

${OBJECTDIR}/SchemaTypeBinary.o: SchemaTypeBinary.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/SchemaTypeBinary.o SchemaTypeBinary.cpp

${OBJECTDIR}/SchemaTypes.o: SchemaTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/SchemaTypes.o SchemaTypes.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/GNU-Solaris-x86/libAltova_Dll_Xerces_nomfc.so

# Subprojects
.clean-subprojects:
