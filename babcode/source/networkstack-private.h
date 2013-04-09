
#if PLATFORM == PLATFORM_WINDOWS
#   include <winsock2.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#   include <string.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <fcntl.h>
#   include <unistd.h>
#   include <errno.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
#   pragma comment( lib, "wsock32.lib" )
#endif

#include <stdlib.h>
#include <stdio.h>


#define Network_GetLastError()	(errno)
