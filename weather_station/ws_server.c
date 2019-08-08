#include "zhelpers.h"
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
	// prepare the context and publisher
	void *context = zmq_ctx_new ();
	void *publisher = zmq_socket (context, ZMQ_PUB);
	int rc = zmq_bind (publisher, "tcp://*:5556");

	assert (rc == 0);

	rc = zmq_bind (publisher, "ipc://weather.ipc");
	assert (rc == 0);

	// Initialize the random number generator
	srandom ((unsigned) time (NULL));

	while (1)
	{
		int zipcode, temperature, relhumidity;
		zipcode = randof (1000000);
		temperature = randof (251) - 80;
		relhumidity = randof (50) + 10;

		// send messages to all subscribers
		char update[20];
		sprintf (update, "%05d %d %d", zipcode, temperature, relhumidity);

		s_send (publisher, update);
	}

	zmq_close (publisher);
	zmq_ctx_destroy (context);

	return 0;
}