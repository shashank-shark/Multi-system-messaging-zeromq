#include "zhelpers.h"

int main (int argc, char **argv)
{
	void *context = zmq_ctx_new ();

	// connect to task ventilator
	void *reciever = zmq_socket (context, ZMQ_PULL);
	zmq_connect (reciever, "tcp://localhost:5557");

	// connect to weather station
	void *subscriber = zmq_socket (context, ZMQ_SUB);
	zmq_connect (subscriber, "tcp://localhost:5556");
	zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "10001", 6);

	// initliaize the poll set
	zmq_pollitem_t items[] = {
		{ reciever, 0, ZMQ_POLLIN, 0},
		{ subscriber, 0, ZMQ_POLLIN, 0}
	};

	int i = 0;

	while (1)
	{
		zmq_msg_t message;
		zmq_poll (items, 2, -1);

		if (items[0].revents & ZMQ_POLLIN)
		{
			zmq_msg_init (&message);
			zmq_msg_recv (&message, reciever, 0);

			// process task
			i ++;
			printf ("TASK : %d\n", i);

			zmq_msg_close (&message);
		}

		printf ("%d", items[1].revents & ZMQ_POLLIN);

		if (items[1].revents & ZMQ_POLLIN)
		{
			zmq_msg_init (&message);
			zmq_msg_recv (&message, subscriber, 0);

			// process update
			printf ("UPDATE FROM WEATHER STATION : %d\n", i);

			zmq_msg_close (&message);
		}
	}

	zmq_close (reciever);
	zmq_close (subscriber);
	zmq_ctx_destroy (context);
	
	return 0;
}
