#include "zhelpers.h"
#include <unistd.h>

int main (void)
{
	void *context = zmq_ctx_new ();

	// first connect to our subscriber socket
	void *subscriber = zmq_socket (context, ZMQ_SUB);
	zmq_connect (subscriber, "tcp://192.168.43.136:5561");
	zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "", 0);

	sleep (1);

	// secondly synchronize with the publisher
	void *syncclient = zmq_socket (context, ZMQ_REQ);
	zmq_connect (syncclient, "tcp://192.168.43.136:5562");

	// send a sync request
	s_send (syncclient, "");

	// wait for syncronization replay
	char *string = s_recv (syncclient);
	free (string);

	// third get our updates and report how many we want
	int update_nbr = 0;
	while (1)
	{
		char *string = s_recv (subscriber);
		if (strcmp (string, "END") == 0)
		{
			free (string);
			break;
		}

		free (string);
		update_nbr ++;
	}

	printf ("Recieved %d updates .. \n", update_nbr);

	zmq_close (subscriber);
	zmq_close (syncclient);
	zmq_ctx_destroy (context);

	return 0;
}
