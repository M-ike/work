#include "common.h"

#ifdef unix

     #include <unistd.h>
     void sleepMS(unsigned int ms)  { usleep(ms*1000); }

#else

     #include <windows.h>
     #include <winbase.h>

     void sleepMS(unsigned int ms)  { Sleep(ms); }

#endif
