// LOAD BALANCER 2

#include <pthread>
#include "zhelpers.h"

#define NBR_CLIENTS 30
#define NBR_WORKERS 3

/* client_work */
static void *
client_work (void *args)
{
	void *context = zmq_ctx_new ();
	void *client = zmq_socket (context, ZMQ_REQ);

	s_set_id (client);
	zmq_connect (client, "ipc://frontend.ipc");

	// Send requests and get reply
	s_send (client, "HELLO");
	char *reply = s_recv (client);
	printf ("Client : %s\n", reply);
	free (reply);

	zmq_close (client);
	zmq_ctx_destroy (context);

	return NULL;
}

/* worker thread jobs */
static void *
worker_work (void *args)
{
	void *context = zmq_ctx_new ();
	void *worker = zmq_socket (context, ZMQ_REQ);

	s_set_id (worker);
	zmq_connect (worker, "ipc://backend.ipc");

	s_send (worker, "READY");

	while (1)
	{
		char *identity = s_recv (worker);
		char *empty = s_recv (worker);	

		assert (*empty == 0);
		free (empty);

		// Get request and send reply
		char *request = s_recv (worker);
		printf ("Worker : %s\n", request);
		free (request);

		s_sendmore (worker, identity);
		s_sendmore (worker, "");
		
		s_send (worker, "OK");
		free (identity);
	}

	zmq_close (worker);
	zmq_ctx_destroy (context);

	return NULL;
}



// main driver function
int main (int argc, char **argv)
{
	void *context = zmq_ctx_new ();
	void *frontend = zmq_socket (context, ZMQ_ROUTER);
	void *backend = zmq_socket (context, ZMQ_ROUTER);

	// bind the sockets frontend and backend
	zmq_bind (frontend, "ipc://frontend.ipc");
	zmq_bind (backend, "ipc://backend.ipc");

	// start creating client threads
	int client_nbr = 0;
	for (client_nbr = 0; client_nbr < NBR_CLIENTS; client_nbr ++)
	{
		// start generating threads
		pthread_t client;
		pthread_create (&client, NULL, client_work, (void *) (intptr_t) client_nbr);
	}

	// start creating worker threads
	int worker_nbr = 0;
	for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr ++)
	{
		pthread_t worker;
		pthread_create (&worker, NULL, worker_work, (void *)(intptr_t) worker_work);
	}

	return 0;
}