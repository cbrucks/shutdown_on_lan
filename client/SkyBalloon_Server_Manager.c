#include <stdio.h>

#include "udp_comm.h"
#include "logger_cbrucks.h"

void send_magic_packet(unsigned int port)
{
	// initialize the socket handles
	//unsigned int socket_send = 0;
	unsigned int socket_send = 0;
	int msg_buf_len;
	int i;
	char *magic_packet;

	printf("Creating the magic packet...\n");
	msg_buf_len = 102;	// 102 + 1 (magic packet + string terminating null)
	//initialize the magic packet that is expected to be received
	magic_packet = (char *)calloc(msg_buf_len+1, sizeof(char));
	*((unsigned int *)magic_packet) = 0xffffffff;
	*((unsigned int *)(magic_packet+4)) = 0xffff;
	for ( i = 1; i < 17; i++ )
	{
		*((unsigned int *)(magic_packet+(i*6)))   = 0x60;	// 60
		*((unsigned int *)(magic_packet+(i*6)+1)) = 0xA4;	// A4
		*((unsigned int *)(magic_packet+(i*6)+2)) = 0x4C;	// 4C
		*((unsigned int *)(magic_packet+(i*6)+3)) = 0xB0;	// B0
		*((unsigned int *)(magic_packet+(i*6)+4)) = 0xBF;	// BF
		*((unsigned int *)(magic_packet+(i*6)+5)) = 0xA1;	// A1
	}
	
	printf("Initializing UDP resources...\n");
	// initialize the socket interface
	if ( !initialize_udp_comm() )
	{
		printf("!!!! Failed to initialize UDP resources !!!!\n");
		return;
	}

	printf("Opening a socket...\n");
	// Create a socket to receive messages from port 25001 on any interface
	if ( !open_socket( &socket_send, 0, 0 ) )
	{
		printf("!!!! Failed to open the socket !!!!\n");
		return;
	}

	printf("Sending the WOL message...\n");
	// Send the message
	if ( !send_message(&socket_send, magic_packet, msg_buf_len, ip_string_to_net("192.168.1.255"), port_to_net(port)) )
	{
		char *error_msg = NULL;
		printf("!!!! Failed to send the message !!!!\n");
		get_last_error_message(error_msg);
		printf("%s\n", error_msg);
		free(error_msg);
		return;
	}
	else
		printf("Message successfully sent.\n");


	//printf("Cleaning up memory...\n");
	//free(magic_packet);

	printf("Closing the socket...\n");
	// Close the sockets
	if ( !close_socket( &socket_send ) )
	{
		printf("!!!! Failed to close the socket !!!!\n");
		return;
	}

	printf("Cleaning up the UDP resources...\n");
	// Clean up the socket interface
	if ( !terminate_udp_comm() )
	{
		printf("!!!! Failed to clean up UDP resources !!!!\n");
		return;
	}
}


int main()
{
	// Shutdown on LAN
	//send_magic_packet(19374);

	// Wake on LAN
	send_magic_packet(9);

	printf("Done\n");

#if PLATFORM == PLATFORM_WINDOWS
	system("pause");
#endif

	return 0;
}
