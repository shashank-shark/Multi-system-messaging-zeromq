#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();
	void *subscriber = zmq_socket (context, ZMQ_SUB);

	int rc = zmq_connect (subscriber, "tcp://localhost:8080");
	assert (rc == 0);


	// set socket options for prespective subscribers
	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "BANGALORE", strlen ("BANGALORE"));
	assert (rc == 0);

	int temperature = 0;
	int windSpeed = 0;
	int pressure = 0;
	char city[10];

	while (1)
	{
		char *recvUpdate = s_recv (subscriber);

		sscanf (recvUpdate, "%s %d %d %d", city, &temperature, &windSpeed, &pressure);
		printf ("CITY = %s | TEMP = %d | WINDSPEED = %d | PRESSURE = %d\n", city, temperature, windSpeed, pressure);

		free (recvUpdate);
	}

	return 0;
}