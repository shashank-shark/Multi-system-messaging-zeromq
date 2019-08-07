#include <stdio.h>
#include <zmq.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char **argv)
{
	void *context = zmq_ctx_new();
	printf ("Connecting to hello world server\n");
	void *requester = zmq_socket (context, ZMQ_REQ);

	zmq_connect (requester, "tcp://localhost:5555");


	int request_nbr = 0;
	for (request_nbr = 0; request_nbr != 10; request_nbr ++)
	{
		zmq_msg_t request;
		zmq_msg_init_size (&request, 5);
		memcpy (zmq_msg_data (&request), "Hello", 5);
		printf ("Sending Hello to server ... %d\n", request_nbr);
		zmq_msg_send (&request, requester, 0);
		zmq_msg_close (&request);

		zmq_msg_t reply;
		zmq_msg_init (&reply);
		zmq_msg_recv (&reply, requester, 0);
		printf ("Recieved World .. %d\n", request_nbr);
		zmq_msg_close (&reply);
	}

	sleep (2);
	zmq_close (requester);
	zmq_ctx_destroy (context);


	return 0;
}
