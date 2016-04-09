#ifndef MQCOMM_PUBFILE_H
#define MQCOMM_PUBFILE_H

#include "ConstDef.h"

#ifdef MQCOMM_EXPORTS
#define MQCOMM_API USAGEDLL_EXPORT
#else
#define MQCOMM_API USAGEDLL_IMPORT
#endif

#define ZMQ_PORTNUM "5555"

#endif