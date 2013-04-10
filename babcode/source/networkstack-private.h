
#if PLATFORM == PLATFORM_WINDOWS

#   include <winsock2.h>
#   define Network_GetLastError()	(WSAGetLastError())

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

#   include <string.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <fcntl.h>
#   include <unistd.h>
#   include <errno.h>
#   define Network_GetLastError()	(errno)

#endif

#if PLATFORM == PLATFORM_WINDOWS

#   pragma comment( lib, "wsock32.lib" )

#endif

#include <stdlib.h>
#include <stdio.h>

