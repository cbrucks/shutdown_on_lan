// include guard
#ifndef __UDP_RECEIVE__
#define __UDP_RECEIVE__

// Custom script used to detect the platform
#include "get_platform.h"

// Platform specific includes for sockets
#if PLATFORM == PLATFORM_WINDOWS
	#include <winsock2.h>
	#pragma comment( lib, "wsock32.lib" )
	#pragma comment(lib, "Ws2_32.lib")
#elif PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <stdlib.h>
	#include <arpa/inet.h>

	#define SOCKADDR_IN struct sockaddr_in
	#define SOCKADDR struct sockaddr
#else
	#error unknown platform!
#endif

// General includes
#include <assert.h>
#include <stdio.h>

// Global defines
#define ip_string_to_net(ip_address) inet_addr(ip_address)

// Global variables

// Global functions
int initialize_udp_comm();
int terminate_udp_comm();
int open_socket( unsigned int *socket, unsigned int local_ip_address, unsigned short local_port );
int close_socket( unsigned int *socket );
int read_message( unsigned int *socket, char **buffer, unsigned short buffer_len,
				  unsigned int net_remote_ip_addr, unsigned short net_remote_port );
int send_message( unsigned int *socket, char *buffer, unsigned short buffer_len,
				  unsigned int net_remote_ip_addr, unsigned short net_remote_port );

int is_socket_open( unsigned int *socket );

unsigned short port_to_net( unsigned short port );

// Private functions

#endif /* __UDP_RECEIVE__ */