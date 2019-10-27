#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();
	void *subscriber = zmq_socket (context, ZMQ_SUB);

	// connect to the publisher
	int rc = zmq_connect (subscriber, "tcp://localhost:8080");
	assert (rc == 0);

	char *bangaloreUpdate = "BANGALORE";
	
	//  set the socket options saying that we are subscribing to certain topics

	// subscribe the updates from bangalore
	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, bangaloreUpdate, strlen (bangaloreUpdate));
	assert (rc == 0);

	int bTemp = 0, bSun = 0, bwind = 0;

	// start recieving the updates
	while (1)
	{
		char *recvUpdate = s_recv (subscriber);

		// test1
		printf ("%s\n", recvUpdate);
	}

	return 0;
}