#include <stdio.h>

#include "udp_comm.h"

#define MAC_WORD_1 0x60
#define MAC_WORD_2 0xa4
#define MAC_WORD_3 0x4c
#define MAC_WORD_4 0xb0
#define MAC_WORD_5 0xbf
#define MAC_WORD_6 0xa1

void wait_for_sol()
{
	// initialize the socket handle
	unsigned int socket_receive = 0;
	char *msg_buf;
	int msg_buf_len;
	int bytes_read;
	int i;
	char *magic_packet;

	msg_buf_len = 103;	// 102 + 1 (magic packet + string terminating null)
	//initialize the magic packet that is expected to be received
	magic_packet = (char *)calloc(msg_buf_len, 1);

	*((unsigned int *)magic_packet) = 0xffffffff;
	*((unsigned int *)(magic_packet+4)) = 0xffff;
	for ( i = 1; i < 17; i++ )
	{
		*((unsigned int *)(magic_packet+(i*6)))   = MAC_WORD_1;
		*((unsigned int *)(magic_packet+(i*6)+1)) = MAC_WORD_2;
		*((unsigned int *)(magic_packet+(i*6)+2)) = MAC_WORD_3;
		*((unsigned int *)(magic_packet+(i*6)+3)) = MAC_WORD_4;
		*((unsigned int *)(magic_packet+(i*6)+4)) = MAC_WORD_5;
		*((unsigned int *)(magic_packet+(i*6)+5)) = MAC_WORD_6;
	}
	

	// initialize the socket interface
	if ( !initialize_udp_comm() ) return;

	// Create a socket to receive messages from port 25001 on any interface
	if ( !open_socket( &socket_receive, 0, port_to_net(19374) ) ) return;

	// Read the message
	while ( 1 )
	{
		bytes_read = read_message(&socket_receive, &msg_buf, msg_buf_len, 0, 0);
		if ( bytes_read < 0 ) return;
		if ( bytes_read > 0 )
		{
			//for ( i=0; i < bytes_read; i++ )
			//	printf("%02X ", 0x000000ff & msg_buf[i]);
			//printf("\n");

			// verify that the magic packet is for this computer
			i = strcmp(magic_packet, msg_buf);
			if ( i == 0 )
			{
				printf("SOL Magic Packet received\n");
				system("sudo shutdown -P 0");
				break;
			}

			free(msg_buf);
		}
	}

	free(magic_packet);

	// Close the sockets
	if ( !close_socket( &socket_receive ) ) return;

	// Clean up the socket interface
	if ( !terminate_udp_comm() ) return;

#if PLATFORM == PLATFORM_WINDOWS
	system("pause");
#endif
}


int main()
{
	wait_for_sol();

	return 0;
}
