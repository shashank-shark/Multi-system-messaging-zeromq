#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();
	void *subscriber = zmq_socket (context, ZMQ_SUB);

	// connect to the publisher
	int rc = zmq_connect (subscriber, "tcp://localhost:8080");
	assert (rc == 0);

	char *bangaloreUpdate = "BANGALORE";
	char *chennaiUpdate = "CHENNAI";
	char *delhiUpdate = "DELHI";
	
	//  set the socket options saying that we are subscribing to certain topics

	// subscribe the updates from bangalore
	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, bangaloreUpdate, strlen (bangaloreUpdate));
	assert (rc == 0);

	// subscribe the updates from chennai
	rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, chennaiUpdate, strlen (chennaiUpdate));
	assert (rc == 0);

	// subscribe the updates from delhi
	// rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, delhiUpdate, strlen (delhiUpdate));
	// assert (rc == 0);


	int bTemp = 0, bSun = 0, bwind = 0;
	int cTemp = 0, cSun = 0, cwind = 0;
	int dTemp = 0, dSun = 0, dwind = 0;

	// start recieving the updates
	while (1)
	{
		char *recvUpdate = s_recv (subscriber);

		// test1
		printf ("%s\n", recvUpdate);
	}

	return 0;
}