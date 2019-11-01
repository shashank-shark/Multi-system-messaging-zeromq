#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();

	void *publisher = zmq_socket (context, ZMQ_PUB);
	
	int rc = zmq_bind (publisher, "tcp://*:8080");
	assert (rc == 0);

	srandom ((unsigned int)time(NULL));

	while (1)
	{
		char bangaloreUpdate[20];
		int temperature, windSpeed, sunLight;

		temperature = randof (100) + 10;
		windSpeed = randof (30);
		sunLight = randof (40);

		sprintf (bangaloreUpdate, "BANGALORE - %d %d %d", temperature, windSpeed, sunLight);
		s_send (publisher, bangaloreUpdate);
		s_sleep (1000);
	}

	zmq_close (publisher);
	zmq_ctx_destroy (context);

	return 0;
}