#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char **argv)
{
	void *context = zmq_ctx_new();

	// socket to talk to clinets
	void *responder = zmq_socket (context, ZMQ_REP);
	zmq_bind (responder, "tcp://*:5555");


	while (1)
	{
		// wait for the next request from the client
		zmq_msg_t request;
		zmq_msg_init (&request);
		zmq_msg_recv (&request, responder, 0);
		printf ("Recieved Hello\n");
		zmq_msg_close (&request);

		sleep (1);

		// send reply back to the client
		zmq_msg_t reply;
		zmq_msg_init_size (&reply, 5);
		memcpy (zmq_msg_data (&reply), "World", 5);
		zmq_msg_send (&reply, responder, 0);
		zmq_msg_close (&reply);
	}

	zmq_close(responder);
	zmq_ctx_destroy (context);

	return 0;
}
