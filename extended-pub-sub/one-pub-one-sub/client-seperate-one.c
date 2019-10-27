#include "zhelpers.h"

int main (int argc, char *argv[])
{

	void *context = zmq_ctx_new ();
	void *subscriber = zmq_socket (context, ZMQ_SUB);

	// connect to socket
	int rc = zmq_connect (subscriber, "tcp://localhost:8080");
	assert (rc == 0);

	// set socket options
	char *bangaloreUpdate = "BANGALORE";

	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, bangaloreUpdate, strlen (bangaloreUpdate));
	assert (rc == 0);

	// aim is to read temp, sunlight, wind-velocity updates
	int bTemp = 0, bSunligh = 0, bWind = 0;
	char dummy[10];
	char cityDummy[10];

	while (1)
	{
		// start recieving the updates
		char *bangRecvUpdate = s_recv (subscriber);



		sscanf (bangRecvUpdate, "%s %s %d %d %d",cityDummy, dummy ,&bTemp, &bSunligh, &bWind);
		printf ("%s\n", bangRecvUpdate);
		printf ("Temp - %d Sunlight - %d WindVelocoty - %d\n", bTemp, bSunligh, bWind);
		free (bangRecvUpdate);
	}

	free (bangaloreUpdate);


	zmq_close (subscriber);
	zmq_ctx_destroy (context);

	return 0;
}