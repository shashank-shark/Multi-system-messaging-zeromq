#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();
	void *publisher = zmq_socket (context, ZMQ_PUB);

	int rc = zmq_bind (publisher, "tcp://*:8080");
	assert (rc == 0);

	char city[10];
	int temperature = 0;
	int windSpeed = 0;
	int pressure = 0;

	char bangaloreUpdate[30];
	char chennaiUpdate[30];
	char delhiUpdate[30];

	srandom ((unsigned int)time(NULL));

	while (1)
	{
		// bangaloreUpdate
		temperature = randof (1000);
		windSpeed = randof (89);
		pressure = randof (50);

		sprintf (bangaloreUpdate, "%s %d %d %d", "BANGALORE", temperature, windSpeed, pressure);
		s_send (publisher, bangaloreUpdate);


		// cheanniUpdate
		temperature = randof (1000);
		windSpeed = randof (89);
		pressure = randof (50);

		sprintf (chennaiUpdate, "%s %d %d %d", "CHENNAI", temperature, windSpeed, pressure);
		s_send (publisher, chennaiUpdate);

		// delhiUpdate
		temperature = randof (1000);
		windSpeed = randof (89);
		pressure = randof (50);

		sprintf (delhiUpdate, "%s %d %d %d", "DELHI", temperature, windSpeed, pressure);
		s_send (publisher, delhiUpdate);

	}

	free (bangaloreUpdate);
	free (delhiUpdate);
	free (chennaiUpdate);

	zmq_close (publisher);
	zmq_ctx_destroy (context);

	return 0;
}