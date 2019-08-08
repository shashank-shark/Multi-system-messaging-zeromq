#include "zhelpers.h"
#include <unistd.h>

int main (int argc, char **argv)
{
	void *context = zmq_ctx_new ();

	printf("Collecting updates from weather station\n");
	void *subscriber = zmq_socket (context, ZMQ_SUB);
	int rc = zmq_connect (subscriber, "tcp://localhost:5556");

	// subscriber to zipcode
	char *filter = "10001";
	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, filter, strlen (filter));
	assert (rc == 0);

	// Process 100 updates
	int update_nbr;
	long total_temp = 0;
	// for (update_nbr = 0; update_nbr < 100; update_nbr ++)
	while (1)
	{
		char *string = s_recv (subscriber);
		int zipcode, temperature, relhumidity;
		sscanf (string, "%d %d %d", &zipcode, &temperature, &relhumidity);
		printf ("Temperature = %d\n", temperature);
		total_temp = total_temp + temperature;
		free (string);
	}

	printf ("Average temperature of zipcode was %d ", (int)(total_temp / update_nbr));
	zmq_close (subscriber);
	zmq_ctx_destroy (context);

	return 0;
}