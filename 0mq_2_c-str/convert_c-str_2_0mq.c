#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

static int s_send (void *socket, char *string)
{
	zmq_msg_t message;
	zmq_msg_init_size (&message, strlen(string));

	memcpy (zmq_msg_data (&message), string, strlen (string));
	int size = zmq_msg_send (&message, socket, 0);
	zmq_msg_close (&message);
	return (size);
}
