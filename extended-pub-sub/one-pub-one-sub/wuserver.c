#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();
	void *publisher = zmq_socket (context, ZMQ_PUB);

	// binding as it is the update server
	int rc = zmq_bind (publisher, "tcp://*:8080");
	assert (rc == 0);
	
	// rc = zmq_bind (publisher, "ipc://weather.ipc");
	// assert (rc == 0);

	// initialize the random function
	srandom ((unsigned int)time(NULL));

	// start publishing the updates
	while (1)
	{

		/* UPDATE - 1*/
		char bangaloreUpdate[20];
		int temperature, sunlight, windSpeed;
		temperature = randof (1000);
		sunlight = randof (300);
		windSpeed = randof (20);

		sprintf (bangaloreUpdate, "BANGALORE - %d %d %d", temperature, sunlight, windSpeed);
		s_send (publisher, bangaloreUpdate);
	}

	// close what's opened
	zmq_close (publisher);
	zmq_ctx_destroy (context);

	return 0;
}