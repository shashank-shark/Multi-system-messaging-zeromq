#include "zhelpers.h"

int main (int argc, char **argv)
{
	// connect to task ventilator
	void *context = zmq_ctx_new ();
	void *reciever = zmq_socket (context, ZMQ_PULL);
	zmq_connect (reciever, "tcp://localhost:5557");

	// connect to weather station
	void *subscriber = zmq_socket (context, ZMQ_SUB);
	zmq_connect (subscriber, "tcp://localhost:5556");
	zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "10001", 6);

	int i = 0;

	// process message from both sockets
	while (1)
	{
		i ++;
		char msg[256];

		zmq_pollitem_t items[] = {
			{ reciever, 0, ZMQ_POLLIN, 0},
			{ subscriber, 0, ZMQ_POLLIN, 0}
		};
		
		zmq_poll (items, 2, -1);

		if (items[0].revents & ZMQ_POLLIN)
		{
			int size = zmq_recv (reciever, msg, 255, 0);
			if (size != -1) {
				printf ("TASK : %d\n", i);
			}
		}
		if (items[1].revents & ZMQ_POLLIN)
		{
			int size = zmq_recv (subscriber, msg, 255, 0);
			if (size != -1)
			{
				printf ("UPDATE : %d\n", i);
			}
		}
	}

	zmq_close (subscriber);
	zmq_ctx_destroy (context);

	return 0;
}
