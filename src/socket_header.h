#pragma once

#include "macros.h"

#if YG_PLATFORM == YG_PLATFORM_WIN32
	#include <winsock2.h>
	#include <In6addr.h>
	#include <Ws2tcpip.h>
	
	# define SOCKET_FD   SOCKET

//#define errno WSAGetLastError()
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <errno.h>
	# define SOCKET_FD   int
#endif




#define INVALID_FD  ((SOCKET_FD)-1)