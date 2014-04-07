#include "udp_comm.h"


/**********************************************************
 * Abstract:
 *     Initializes the libraries need for socket 
 *     communication.
 *********************************************************/
int initialize_udp_comm()
{
#if PLATFORM == PLATFORM_WINDOWS
	WSADATA WsaData;
	/* error = 0				  --> Successfully Completed
	 *       = WSASYSNOTREADY     --> The underlying network subsystem is not ready for network communication.
	 *       = WSAVERNOTSUPPORTED --> The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.
	 *       = WSAEINPROGRESS	  --> A blocking Windows Sockets 1.1 operation is in progress.
	 *       = WSAEPROCLIM		  --> A limit on the number of tasks supported by the Windows Sockets implementation has been reached.
	 *       = WSAEFAULT		  --> The lpWSAData parameter is not a valid pointer.
	 */
	int error = WSAStartup( MAKEWORD(2,2), &WsaData );
	switch (error) {
		case WSASYSNOTREADY:
			perror("The underlying network subsystem is not ready for network communication!");
			break;
		case WSAVERNOTSUPPORTED:
			perror("The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation!");
			break;
		case WSAEINPROGRESS:
			perror("A blocking Windows Sockets 1.1 operation is in progress!");
			break;
		case WSAEPROCLIM:
			perror("A limit on the number of tasks supported by the Windows Sockets implementation has been reached!");
			break;
		case WSAEFAULT:
			perror("The lpWSAData parameter is not a valid pointer!");
			break;
		default:
			// No error occured
			break;
	}
	return error == 0; // Completed Successfully

#else
	return 1;

#endif
}



/**********************************************************
 * Abstract:
 *     Cleans up the libraries need for socket 
 *     communication.
 *********************************************************/
int terminate_udp_comm()
{
#if PLATFORM == PLATFORM_WINDOWS
	return !WSACleanup();
#else
	return 1;
#endif
}



/**********************************************************
 * Abstract:
 *     This makes sure that the socket is not already open
 *     then, attempts to create a socket on the port 
 *     specified that accepts messages only from the listed
 *     address.  To bind to all ip addresses assigned to 
 *     this machine, set address to 0.  To bind to an 
 *     arbitrary port, set port to 0.
 *********************************************************/
int open_socket( unsigned int *s, unsigned int net_local_ip, unsigned short net_local_port )
{
	SOCKADDR_IN address;
	unsigned long nonBlocking;

	assert( !is_socket_open(s) );

	// create socket
	*s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( *s <= 0 )
	{ // Failed to create the socket
		perror("The socket passed is not open!");
		*s = 0;
		return 0;
	}

	// bind to port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = net_local_ip; //INADDR_ANY;
	address.sin_port = net_local_port;

	if ( bind( *s, (SOCKADDR *)&address, sizeof(SOCKADDR_IN) ) < 0 )
	{ // Failed to bind to the port
		perror("Failed to bind to the IP address and port!");
		close_socket(s);
		return 0;
	}

	// set non-blocking io
	/*nonBlocking = 1;
	#if PLATFORM == PLATFORM_UNIX

		if ( fcntl( *s, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
		{ // failed to set non-blocking io
			perror("Failed to set non-blocking io!");
			close_socket(s);
			return 0;
		}
	
	#elif PLATFORM == PLATFORM_WINDOWS

		if ( ioctlsocket( *s, FIONBIO, &nonBlocking ) != 0 )
		{ // failed to set non-blocking io
			perror("Failed to set non-blocking io!");
			close_socket(s);
			return 0;
		}

	#endif*/

	// successfully created and configured the socket
	return 1;
}



/**********************************************************
 * Abstract:
 *     Closes the socket 
 *********************************************************/
int close_socket( unsigned int *s)
{
	int error_code = 0;

	if ( *s != 0 )
	{
		#if PLATFORM == PLATFORM_UNIX
			error_code = close( *s );
		#elif PLATFORM == PLATFORM_WINDOWS
			error_code = closesocket( *s );
		#endif
		*s = 0;

		if ( error_code != 0 )
		{
			perror("Failed to close the socket!");
		}
	}

	return error_code == 0;
}



/**********************************************************
 * Abstract:
 *     Attempts to read a message from the socket interface
 *     specified.   The maximum number of bytes read is
 *     determined by the buffer_len parameter.  The buffer
 *     will be allocated during this function to the size
 *     specified.  The user of this function is expected
 *     to call free(buffer) to unallocate the memory.
 *     If no message is received or a message is denied by
 *     the filter then the buffer will be returned empty
 *     and the return value of read_message will be 0.
 *     The remote ip address and remote port values are 
 *     used to filter out unwanted messages from unknown
 *     senders.
 *     The remote ip address and remote port are used to 
 *     filter out messages from unkown senders.
 *     If the remote ip address is 0 then the ip filter is
 *     disabled.
 *     If the remote port is 0 then the port filter is 
 *     disabled.
 *********************************************************/
int read_message( unsigned int *socket, char **buffer, unsigned short buffer_len, unsigned int net_remote_ip_addr, unsigned short net_remote_port)
{
	int received_bytes;
	SOCKADDR_IN from;
	int from_len = sizeof( from );

	// allocate the buffer
	assert( buffer_len > 0 );
	*buffer = (char *)calloc( buffer_len, sizeof(char) );

	// Make sure it is pointing to a valid socket
	if ( !is_socket_open(socket) )
	{
		perror("The socket passed is not open!");
		return -1;
	}
	
	// read the message
	received_bytes = recvfrom( *socket, *buffer, buffer_len, 0, (SOCKADDR*)&from, &from_len );

	// Check if the message was read successfully
	// Apply the specified ip and port filters
	if ( ( received_bytes <= 0 ) ||
		 ( net_remote_ip_addr != 0 && net_remote_ip_addr != from.sin_addr.s_addr ) ||
		 (    net_remote_port != 0 &&    net_remote_port != from.sin_port ) )
	{
		// The message was not from an allowed ip address or port.
		// clear the buffer
		free(*buffer);
		*buffer = 0;
		received_bytes = 0;
	}

	return received_bytes;
}



/**********************************************************
 * Abstract:
 *     Attempts to send a message from the socket interface
 *     specified.   The message buffer contains the 
 *     information to be sent.  The memory for the message
 *     buffer is not clear here.  The user is expected to 
 *     manage the allocated memory.
 *     The remote ip and port point to the intended 
 *     recipient of the message.
 *     
 *********************************************************/
int send_message( unsigned int *socket, char *message, unsigned short msg_len, unsigned int net_remote_ip_addr, unsigned short net_remote_port)
{
	int sent_bytes;
	SOCKADDR_IN address;

	// Make sure the message length is greater than zero
	assert( msg_len > 0 );


	// Ensure the socket is open
	if ( !is_socket_open(socket) )
	{
		perror("The socket passed is not open!");
		return 0;
	}

	// Set the destination address
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = net_remote_ip_addr;
	address.sin_port = net_remote_port;

	// Send the message
	sent_bytes = sendto( *socket, message, msg_len, 0, (SOCKADDR*)&address, sizeof(SOCKADDR_IN) );

	// return success if the number of sent bytes equal the message length.
	return sent_bytes == msg_len;
}



/**********************************************************
 * Abstract:
 *     Returns whether or not the socket has been 
 *     initialized.
 *********************************************************/
int is_socket_open( unsigned int *s )
{
	return *s != 0;
}



unsigned short port_to_net( unsigned short port )
{
	return htons(port);
}