
#ifndef _BASICSVCFAULTSIMPUBLIC_H_
#define _BASICSVCFAULTSIMPUBLIC_H_

#ifdef _WINDOWS
#ifdef  BASICSVCFAULTSIM_EXPORTS
#define BASICSVCFAULTSIM_API __declspec(dllexport)
#else
#define BASICSVCFAULTSIM_API __declspec(dllimport)
#endif
#else
#define BASICSVCFAULTSIM_API
#endif

#endif //!_BASICSVCFAULTSIMPUBLIC_H_