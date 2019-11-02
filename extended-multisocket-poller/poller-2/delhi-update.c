#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();
	int rc;

	void *bangaloreReciever = zmq_socket (context, ZMQ_SUB);
	rc = zmq_connect (bangaloreReciever, "tcp://localhost:8000");
	assert (rc == 0);
	zmq_setsockopt (bangaloreReciever, ZMQ_SUBSCRIBE, "BANGALORE", 8);

	void *delhiReciever = zmq_connect (context, ZMQ_SUB);
	rc = zmq_connect (delhiReciever, "tcp://localhost:8000");
	assert (rc == 0);
	zmq_setsockopt (delhiReciever, ZMQ_SUBSCRIBE, "DELHI", 5);

	void *chennaiReciever = zmq_connect (context, ZMQ_SUB);
	rc = zmq_connect (chennaiReciever, "tcp://localhost:8000");
	assert (rc == 0)
	zmq_setsockopt (chennaiReciever, ZMQ_SUBSCRIBE, "CHENNAI", 6);

	zmq_pollitem_t items[] {
		{ bangaloreReciever, 0, ZMQ_POLLIN, 0 },
		{ delhiReciever, 0, ZMQ_POLLIN, 0 },
		{ chennaiReciever, 0, ZMQ_POLLIN, 0 }
	};

	



	return 0;
}