#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();

	// handle ventilator
	void *taskReciever = zmq_socket (context, ZMQ_PULL);
	int rc = zmq_connect (taskReciever, "tcp://localhost:5557");

	// handle weather updates
	void *weatherUpdates = zmq_socket (context, ZMQ_SUB);
	rc = zmq_connect (weatherUpdates, "tcp://localhost:8080");
	zmq_setsockopt (weatherUpdates, ZMQ_SUBSCRIBE, "BANGALORE", 3);

	// initlialize the poll set
	zmq_pollitem_t items[] = {
		{ taskReciever, 0, ZMQ_POLLIN, 0},
		{ weatherUpdates, 0, ZMQ_POLLIN, 0}
	};

	// start polling from these sockets

	while (1)
	{
		zmq_msg_t message;

		// -1 for ZMQ_DONTWAIT
		zmq_poll (items, 2, -1);

		if (items[0].revents & ZMQ_POLLIN) {
			int size = 0;

			zmq_msg_init (&message);
			size = zmq_msg_recv (&message, taskReciever, 0);

			if (size == -1)	continue;

			char *string = malloc (size + 1);
			memcpy (string, zmq_msg_data(&message), size);
			string[size] = 0;

			printf ("FROM TASK - %s\n", string);

			zmq_msg_close (&message);
		}

		if (items[1].revents & ZMQ_POLLIN) {

			int size = 0;

			zmq_msg_init  (&message);
			size = zmq_msg_recv (&message, weatherUpdates, 0);

			if (size == -1)	continue;

			char *string = malloc (size + 1);
			memcpy (string, zmq_msg_data(&message), size);
			string[size] = 0;

			printf ("FROM Weather - %s\n", string);

			zmq_msg_close (&message);
		}
	}

	zmq_close (taskReciever);
	zmq_close (weatherUpdates);

	zmq_ctx_destroy (context);

	return 0;
}