#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();

	void *sourcePublisher = zmq_socket (context, ZMQ_PUB);
	int rc = zmq_bind (sourcePublisher, "tcp://*:8889");
	assert (rc == 0);

	printf ("Source - 2 started publishing image updates ... \n");

	int i = 1;

	while (1)
	{
		char update[30];

		sprintf (update, "source-two IMAGE UPDATES %d", i);
		i = i + 1;

		s_send (sourcePublisher, update);
		s_sleep (1000);
	}

	zmq_close (sourcePublisher);
	zmq_ctx_destroy (context);

	return 0;
}