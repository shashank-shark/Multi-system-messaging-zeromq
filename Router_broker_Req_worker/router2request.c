#include "zhelpers.h"
#include <pthread.h>

#define NBR_WORKERS 10

static void *worker_task (void *args)
{
	void *context = zmq_ctx_new ();
	void *worker = zmq_socket (context, ZMQ_REQ);

	s_set_id (worker);
	zmq_connect (worker, "tcp://localhost:5671");

	int total = 0;

	while (1)
	{
		s_send (worker, "HI BOSS");

		char *workload = s_recv (worker);
		int finished = (strcmp (workload, "Fired!") == 0);
		free (workload);

		if (finished) {
			printf ("Completed: %d tasks\n", total);
			break;
		}

		total ++;

		s_sleep (randof(500) + 1);
	}

	zmq_close (worker);
	zmq_ctx_destroy (context);

	return NULL;
}



int main (int argc, char **argv)
{
	void *context = zmq_ctx_new ();
	void *broker = zmq_socket (context, ZMQ_ROUTER);

	zmq_bind (broker, "tcp://*:5671");
	srandom ((unsigned) time (NULL));

	int worker_nbr = 0;

	for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr ++)
	{
		pthread_t worker;
		pthread_create (&worker, NULL, worker_task, NULL);
	}

	int64_t end_time = s_clock () + 5000;
	int workers_fired = 0;

	while (1)
	{
		// next message gives us the recently used worker
		char *identity = s_recv (broker);
		s_sendmore (broker, identity);
		free (identity);

		free (s_recv (broker));
		free (s_recv (broker));

		s_sendmore (broker, "");

		// encourage workers till they are fired
		if (s_clock () < end_time)
			s_send (broker, "Work Harder");
		else
		{
			s_send (broker, "Fired!");
			if (++workers_fired == NBR_WORKERS)
				break;
		}

	}

	zmq_close (broker);
	zmq_ctx_destroy (context);

	return 0;
}
