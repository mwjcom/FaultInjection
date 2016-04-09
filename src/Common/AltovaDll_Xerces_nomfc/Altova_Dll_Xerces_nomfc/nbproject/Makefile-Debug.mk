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
	${OBJECTDIR}/AltovaException.o \
	${OBJECTDIR}/SchemaTypeString.o \
	${OBJECTDIR}/SchemaTypeNumber.o \
	${OBJECTDIR}/SchemaTypeCalendar.o \
	${OBJECTDIR}/StdAfx.o \
	${OBJECTDIR}/SchemaTypeBinary.o \
	${OBJECTDIR}/SchemaTypes.o

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
.build-conf: ${BUILD_SUBPROJECTS} ../../../PublicProject/Debug/bin/libAltova_Dll_Xerces_nomfc.so

../../../PublicProject/Debug/bin/libAltova_Dll_Xerces_nomfc.so: ${OBJECTFILES}
	${MKDIR} -p ../../../PublicProject/Debug/bin
	${LINK.cc} -shared -o ../../../PublicProject/Debug/bin/libAltova_Dll_Xerces_nomfc.so ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/AltovaException.o: AltovaException.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../OORTDB_Design/Server/PublicFile -o ${OBJECTDIR}/AltovaException.o AltovaException.cpp

${OBJECTDIR}/SchemaTypeString.o: SchemaTypeString.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../OORTDB_Design/Server/PublicFile -o ${OBJECTDIR}/SchemaTypeString.o SchemaTypeString.cpp

${OBJECTDIR}/SchemaTypeNumber.o: SchemaTypeNumber.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../OORTDB_Design/Server/PublicFile -o ${OBJECTDIR}/SchemaTypeNumber.o SchemaTypeNumber.cpp

${OBJECTDIR}/SchemaTypeCalendar.o: SchemaTypeCalendar.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../OORTDB_Design/Server/PublicFile -o ${OBJECTDIR}/SchemaTypeCalendar.o SchemaTypeCalendar.cpp

${OBJECTDIR}/StdAfx.o: StdAfx.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../OORTDB_Design/Server/PublicFile -o ${OBJECTDIR}/StdAfx.o StdAfx.cpp

${OBJECTDIR}/SchemaTypeBinary.o: SchemaTypeBinary.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../OORTDB_Design/Server/PublicFile -o ${OBJECTDIR}/SchemaTypeBinary.o SchemaTypeBinary.cpp

${OBJECTDIR}/SchemaTypes.o: SchemaTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../OORTDB_Design/Server/PublicFile -o ${OBJECTDIR}/SchemaTypes.o SchemaTypes.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} ../../../PublicProject/Debug/bin/libAltova_Dll_Xerces_nomfc.so

# Subprojects
.clean-subprojects:
