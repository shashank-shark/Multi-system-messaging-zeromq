#include "zhelpers.h"

int main (int argc, char *argv[])
{

	void *context = zmq_ctx_new ();
	void *subscriber = zmq_socket (context, ZMQ_SUB);

	// connect to socket
	int rc = zmq_connect (subscriber, "tcp://localhost:8080");
	assert (rc == 0);

	// set socket options - subscribe to bangalore update
	char *bangaloreUpdate = "BANGALORE";

	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, bangaloreUpdate, strlen (bangaloreUpdate));
	assert (rc == 0);

	// set socket options - subscribe to bangalore update
	char *chennaiUpdate = "CHENNAI";

	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, chennaiUpdate, strlen (chennaiUpdate));
	assert (rc == 0);

	// set socket options - subscribe to bangalore update
	char *delhiUpdate = "DELHI";

	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, delhiUpdate, strlen (delhiUpdate));
	assert (rc == 0);


	// aim is to read temp, sunlight, wind-velocity updates
	int recvTemp = 0, recvSunligh = 0, recvWind = 0;
	char dummyChar[10];
	char cityName[10];

	while (1)
	{
		// recv bangalore update
		char *bangaloreUpdate = s_recv (subscriber);

		sscanf (bangaloreUpdate, "%s %s %d %d %d", cityName, dummyChar, &recvTemp, &recvSunligh, &recvWind);
		printf("CITY = %s | TEMPERATURE = %d | SUNLIGHT = %d | WINDSPEED = %d\n", cityName, recvTemp, recvSunligh, recvWind);

		// recv chennai update
		char *chennaiUpdate = s_recv (subscriber);

		sscanf (chennaiUpdate, "%s %s %d %d %d", cityName, dummyChar, &recvTemp, &recvSunligh, &recvWind);
		printf("CITY = %s | TEMPERATURE = %d | SUNLIGHT = %d | WINDSPEED = %d\n", cityName, recvTemp, recvSunligh, recvWind);

		// recv delhi update
		char *delhiUpdate = s_recv (subscriber);

		sscanf (delhiUpdate, "%s %s %d %d %d", cityName, dummyChar, &recvTemp, &recvSunligh, &recvWind);
		printf("CITY = %s | TEMPERATURE = %d | SUNLIGHT = %d | WINDSPEED = %d\n", cityName, recvTemp, recvSunligh, recvWind);

	}

	free (bangaloreUpdate);
	free (chennaiUpdate);
	free (delhiUpdate);


	zmq_close (subscriber);
	zmq_ctx_destroy (context);

	return 0;
}