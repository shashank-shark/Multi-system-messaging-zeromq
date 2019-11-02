#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();

	void *sourcePublisher = zmq_socket (context, ZMQ_PUB);
	int rc = zmq_bind (sourcePublisher, "tcp://*:8888");
	assert (rc == 0);

	int i = 1;

	printf ("Source - 1 is started publishing image upadates ... \n");

	while (1)
	{
		char update[20];
		sprintf (update, "source-one IMAGE %d", i);
		i = i + 1;

		s_send (sourcePublisher, update);

		s_sleep (1000);
	}

	zmq_close (sourcePublisher);
	zmq_ctx_destroy (context);

	return 0;
}