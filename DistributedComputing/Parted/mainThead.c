/*
 * mainPart.c
 *
 *  Created on: 06-Sep-2019
 *      Author: shashank
 */


#include <czmq.h>

#define NBR_CLIENTS 10
#define NBR_WORKERS 5
#define WORKER_READY "\001"

static char *self;

int main (int argc, char *argv[])
{
	if (argc < 2) {
		printf ("syntax invalid : peering1 cluster-name {you} ...\n");
		return 0;
	}

	self = argv[1];
	printf ("I : Preparing  broker at %s ... \n", self);
	srandom ((unsigned)time(NULL));


	// prepare the local frontend and backend
	zctx_t *ctx = zctx_new ();

	void *localfe = zsocket_new (ctx, ZMQ_ROUTER);
	zsocket_bind (localfe, "ipc://%s-localfe.ipc", self);

	void *localbe = zsocket_new (ctx, ZMQ_ROUTER);
	zsocket_bind (localbe, "ipc://%s-localbe.ipc", self);

	// bind cloud frontend to endpoint
	void *cloudfe = zsocket_new (ctx, ZMQ_ROUTER);
	zsocket_set_identity(cloudfe, self);
	zsocket_bind (cloudfe, "ipc://%s-cloud.ipc", self);

	// connect cloud backend to all peers
	void *cloudbe = zsocket_new (ctx, ZMQ_ROUTER);
	zsocket_set_identity(cloudbe, self);

	int argn;
	for (argn = 2; argn < argc; argn ++)
	{
		char *peer = argv[argn];
		printf ("I: Connecting to cloud frontend at '%s' \n", peer);
		zsocket_connect (cloudbe, "ipc://%s-cloud.ipc", peer);
	}

	// bind state backend to endpoint
	void *statebe = zsocket_new (ctx, ZMQ_PUB);
	zsocket_bind (statebe, "ipc://%s-state.ipc", self);

	// connect state frontend to all peers
	void *statefe = zsocket_new (ctx, ZMQ_SUB);
	zsocket_set_subscribe(statefe, "");

	for (argn = 2; argn < argc; argn ++)
	{
		char *peer = argv[argn];
		printf ("I: Connecting to state backend at '%s' \n", peer);
		zsocket_connect (statefe, "ipc://%s-state.ipc", peer);
	}


//	// bind state backend to endpoint
//	void *statebe = zsocket_new (ctx, ZMQ_PUB);
//	zsocket_bind (statebe, "ipc://%s-state.ipc", self);
//
//	// connect state frontend to all peers
//	void *statefe = zsocket_new (ctx, ZMQ_SUB);
//	zsocket_set_subscribe (statefe, "");
//	for (argn = 2; argn < argc; argn ++)
//	{
//		char *peer = argv[argn];
//		printf ("I: connecting to state backend at '%s' \n", peer);
//		zsocket_connect (statefe, "ipc://%s-state.ipc", peer);
//	}

	// prepare monitor
	void *monitor = zsocket_new (ctx, ZMQ_PULL);
	zsocket_bind (monitor, "ipc://%s-monitor.ipc", self);

	// after binding and connecting all our sockets we connect the childs

	// Queue available workers
	int local_capacity = 0;
	int cloud_capacity = 0;

	zlist_t *workers = zlist_new ();

	while (true)
	{
		zmq_pollitem_t primary[] = {
				{ localbe, 0, ZMQ_POLLIN, 0},
				{ cloudbe, 0, ZMQ_POLLIN, 0},
				{ statefe, 0, ZMQ_POLLIN, 0},
				{ monitor, 0, ZMQ_POLLIN, 0}
		};

		// if we have no workers ready wait indefinitely
		int rc = zmq_poll (primary, 4, local_capacity ? 1000 * ZMQ_POLL_MSEC : -1);
		if (rc == -1)
			break;

		// track if capacity changes during this iteration
		int previous = local_capacity;
		zmsg_t *msg = NULL;


		if (primary[0].revents & ZMQ_POLLIN)
		{
			msg = zmsg_recv (localbe);
			if (!msg)
				break;

			zframe_t *identity = zmsg_unwrap (msg);
			zlist_append (workers, identity);
			local_capacity ++;

			// If it;s ready don't route the messages furthur
			zframe_t *frame = zmsg_first (msg);
			if (memcpy (zframe_data (frame), WORKER_READY, 1) == 0)
				zmsg_destroy(&msg);

		}
		else
			if (primary[1].revents & ZMQ_POLLIN)
		{
			msg = zmsg_recv (cloudbe);
			if (!msg)
				break;

			zframe_t *identity = zmsg_unwrap (msg);
			zframe_destroy(&identity);
		}

		for (argn = 2; msg && argn < argc; argn ++)
		{
			char *data= (char *) zframe_data (zmsg_first (msg));
			size_t size = zframe_size (zmsg_first(msg));

			if (size == strlen (argv[argn]) && memcmp (data, argv[argn], size) == 0)
				zmsg_send (&msg, cloudfe);
		}

		if (msg)
			zmsg_send (&msg, localfe);

		if (primary[2].revents & ZMQ_POLLIN)
		{
			char *peer = zstr_recv (statefe);
			char *status = zstr_recv (statefe);
			cloud_capacity = atoi (status);
			free (peer);
			free (status);
		}

		if (primary[3].revents & ZMQ_POLLIN)
		{
			char *status = zstr_recv (monitor);
			printf ("%s\n", status);
			free (status);
		}

		while (local_capacity + cloud_capacity)
		{
			zmq_pollitem_t secondary[] = {
					{ localfe, 0, ZMQ_POLLIN, 0},
					{ cloudfe, 0, ZMQ_POLLIN, 0}
			};

			if (local_capacity)
			{
				rc = zmq_poll (secondary, 2, 0);
			}
			else
			{
				rc = zmq_poll (secondary, 1, 0);
			}
			assert (rc > 0);

			if (secondary[0].revents & ZMQ_POLLIN)
			{
				msg = zmsg_recv (localfe);
			}
			else
			{
				if (secondary[1].revents & ZMQ_POLLIN)
				{
					msg = zmsg_recv (cloudfe);
				}
				else
				{
					break;
				}
			}

			if (local_capacity)
			{
				zframe_t *frame = (zframe_t *) zlist_pop (workers);
				zmsg_wrap (msg, frame);
				zmsg_send (&msg, localbe);
				local_capacity --;
			}
			else
			{
				int peer = randof (argc - 2) + 2;
				zmsg_pushmem (msg, argv[peer], strlen (argv[peer]));
				zmsg_send (&msg, cloudbe);
			}
		}
	}

	while (zlist_size(workers)) {
		zframe_t *frame = (zframe_t *) zlist_pop (workers);
		zframe_destroy (&frame);
	}

	zlist_destroy (&workers);
	zctx_destroy(&ctx);

	return 0;
}
