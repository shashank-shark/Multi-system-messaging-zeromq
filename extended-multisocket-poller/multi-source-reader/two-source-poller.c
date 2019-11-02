#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();

	void *sourceOneReciever = zmq_socket (context, ZMQ_SUB);
	int rc = zmq_connect (sourceOneReciever, "tcp://localhost:8888");
	assert (rc == 0);
	zmq_setsockopt (sourceOneReciever, ZMQ_SUBSCRIBE, "source-one", 10);

	void *sourceTwoReciever = zmq_socket (context, ZMQ_SUB);
	rc = zmq_connect (sourceTwoReciever, "tcp://localhost:8889");
	assert (rc == 0);
	zmq_setsockopt (sourceTwoReciever, ZMQ_SUBSCRIBE, "source-two", 10);

	zmq_pollitem_t items[] = {
		{ sourceOneReciever, 0, ZMQ_POLLIN, 0},
		{ sourceTwoReciever, 0, ZMQ_POLLIN, 0}
	};

	while (1)
	{

		zmq_poll (items, 2, -1);

		// poll source 1
		if (items[0].revents & ZMQ_POLLIN) {
			char *stringOne = s_recv (sourceOneReciever);
			printf ("[UPDATE] : %s\n", stringOne);
			free (stringOne);
		}

		// poll source 2
		if (items[1].revents & ZMQ_POLLIN) {
			char *stringTwo = s_recv (sourceTwoReciever);
			printf ("[UPDATE] : %s\n", stringTwo);
			free (stringTwo);
		}

	}

	zmq_close (sourceOneReciever);
	zmq_close (sourceTwoReciever);

	zmq_ctx_destroy (context);


	return 0;
}