#include "zhelpers.h"

int main (int argc, char *argv[])
{
	void *context = zmq_ctx_new ();

	void *taskDistributer = zmq_socket (context, ZMQ_PUSH);
	int rc = zmq_bind (taskDistributer, "tcp://*:5557");
	assert (rc == 0);

	printf ("Press ENTER when the workers are ready : \n");
	getchar();
	printf ("Distributing the tasks to workers ... \n");

	srandom ((unsigned int)time (NULL));

	int nbr_tasks = 0;
	for (nbr_tasks = 0; nbr_tasks < 100; nbr_tasks ++)
	{
		int workload;
		workload = randof (100) + 1;

		char workLoadSend[4];
		sprintf (workLoadSend, "%d", workload);
		s_send (taskDistributer, workLoadSend);
	}

	printf ("Workload successfully distributed - FAIR Distribution ... \n");

	zmq_close (taskDistributer);
	zmq_ctx_destroy (context);

	return 0;
}