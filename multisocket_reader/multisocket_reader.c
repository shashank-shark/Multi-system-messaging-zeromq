#include "zhelpers.h"

int main (int argc, char **argv)
{
	void *context = zmq_ctx_new ();

	// connect to task ventilator
	void *reciever = zmq_socket (context, ZMQ_PULL);
	zmq_connect (reciever, "tcp://localhost:5557");

	// connect to weather server
	void *subscriber = zmq_socket (context, ZMQ_SUB);
	zmq_connect (subscriber, "tcp://localhost:5556");

	zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "1001", 6);

	// Process messages from both sockets
	while (1)
	{
		int rc = 0;
		for (rc = 0; !rc;)
		{
			zmq_msg_t task;
			zmq_msg_init (&task);

			if ((rc == zmq_msg_recv (&task, reciever, ZMQ_DONTWAIT)) != -1) {
				// process task
				printf ("wow\n");
			}
			zmq_msg_close (&task);
		}

		// process any waiting other updates
		for (rc = 0; !rc;)
		{
			zmq_msg_t update;
			zmq_msg_init (&update);
			if ((rc == zmq_msg_recv (&update, subscriber, ZMQ_DONTWAIT)) != -1) {
				// process weather update
				printf ("UPDATE\n");
			}
			zmq_msg_close (&update);
		}
		s_sleep (1);
	}

	zmq_close (reciever);
	zmq_close (subscriber);
	zmq_ctx_destroy (context);

	return 0;
}
