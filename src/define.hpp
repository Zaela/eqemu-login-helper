
#ifndef _DEFINE_HPP_
#define _DEFINE_HPP_

#ifdef _WIN32
#define _WIN32_WINNT 0x0500
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>
#endif

#include <cstring>
#include <cstdio>

#define toNetworkShort htons
#define toNetworkLong htonl
#define toHostShort ntohs
#define toHostLong ntohl

#ifndef _WIN32
#define closesocket close
#define INVALID_SOCKET -1
#else
typedef int socklen_t;
#endif

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned char byte;

#endif//_DEFINE_HPP_
