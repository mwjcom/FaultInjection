
#ifndef _DBFAULTSIMPUBLIC_H_
#define _DBFAULTSIMPUBLIC_H_

#ifdef _WINDOWS
#ifdef  DBFAULTSIM_EXPORTS
#define DBFAULTSIM_API __declspec(dllexport)
#else
#define DBFAULTSIM_API __declspec(dllimport)
#endif
#else
#define DBFAULTSIM_API
#endif

#endif //!_DBFAULTSIMPUBLIC_H_