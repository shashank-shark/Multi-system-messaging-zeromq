#include "zhelpers.h"

int main (int argc, char **argv)
{
	void *context = zmq_ctx_new ();

	// prepare the socket to send messages
	void *sender = zmq_socket (context, ZMQ_PUSH);
	zmq_bind (sender, "tcp://*:5557");

	// socket to send batch messages on
	void *sink = zmq_socket (context, ZMQ_PUSH);
	zmq_connect (sink, "tcp://*.5558");

	printf ("Press Enter when the worker threads are ready\n");
	getchar();
	printf ("Sending tasks to workers ... \n");

	// the first message is 0 and signals the start of batch
	s_send (sink, 0);

	// initializing a random number generator
	srandom ((unsigned) time (NULL));

	// send 100 tasks
	int task_nbr;
	int total_msec = 0;

	for (task_nbr = 0; task_nbr < 100; task_nbr ++)
	{
		int workload;
		workload = randof (100) + 1;
		total_msec = total_msec + workload;
		
		char str[10];
		sprintf (str,"%d", workload);
		s_send (sender, str);
	}

	printf ("Total expected cost : %d msec\n", total_msec);

	zmq_close (sink);
	zmq_close (sender);
	zmq_ctx_destroy (context);

	return 0;
}
