/*
 * local-and-cloud-flow.c
 *
 *  Created on: 04-Sep-2019
 *      Author: shashank
 */


#include <czmq.h>

#define NBR_CLIENTS 10
#define NBR_WORKERS 3
#define WORKER_READY "\001"	// this is to signal that worker is ready

static char *self;

static void *
client_task (void *args) {
	zctx_t *ctx = zctx_new();

	void *client = zsocket_new (ctx, ZMQ_REQ);
	zsocket_connect(ctx, "ipc://%s-localfe.ipc", self);

	while (true) {
		// send request's and get reply
		zstr_send (client, "HELLO");
		char *reply = zstr_recv (client);

		if (!reply)
			break;

		printf ("Client: %s\n", reply);
		free (reply);
		sleep (1);
	}

	zctx_destroy(&ctx);
	return NULL;
}

// the worker task plugs into the load-balancer using a REQ socket

static void *
worker_task (void *args) {

	zctx_t *ctx = zctx_new();
	void *worker = zsocket_new (ctx, ZMQ_REQ);
	zsocket_connect(worker, "ipc://%s-localbe.ipc", self);

	// Tell broker that we are ready for work
	while (true) {
		zmsg_t *msg = zmsg_recv(worker);
		if (!msg)
			break;

		zframe_print(zmsg_last(msg), "Worker: ");
		zframe_reset(zmsg_last(msg), "OK", 2);
		zmsg_send (&msg, worker);
	}

	zctx_destroy(&ctx);
	return NULL;
}

int main (int argc, char *argv[]) {

	if (argc < 2) {
		printf ("syntax invalid : peering1 me {you} ...\n");
		return 0;
	}

	self = argv[1];
	printf ("I : Preparing broker at %s...\n", self);
	srandom ((unsigned)time(NULL));

	zctx_t *ctx = zctx_new();

	// bind cloudfe to endpoint
	void *cloudfe = zsocket_new (ctx, ZMQ_ROUTER);
	zsocket_set_identity (cloudfe, self);
	zsocket_bind (cloudfe, "ipc://%s-cloud.ipc", self);

	// connect cloud backend to all peers
	void *cloudbe = zsocket_new (ctx, ZMQ_ROUTER);
	zsocket_set_identity(cloudbe, self);
	int argn;
	for (argn = 2; argn < argc; argn++) {
		char *peer = argv[argn];
		printf ("I : Connecting to cloud frontend '%s' \n", peer);
		zsocket_connect(cloudbe, "ipc://%s-cloud.ipc", peer);
	}

	// prepare local frontend and backend
	void *localfe = zsocket_new (ctx, ZMQ_ROUTER);
	zsocket_bind (localfe, "ipc://%s-localfe.ipc", self);

	void *localbe = zsocket_new (ctx, ZMQ_ROUTER);
	zsocket_bind (localbe, "ipc://%s-localbe.ipc", self);

	// get users to tell us when we can start
	int worker_nbr = 0;
	for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr ++) {
		zthread_new (worker_task, NULL);
	}

	// start local clients
	int client_nbr = 0;
	for (client_nbr = 0; client_nbr < NBR_CLIENTS; client_nbr ++) {
		zthread_new (client_task, NULL);
	}

	// least recently used queue of available workers
	int capacity = 0;
	zlist_t *workers = zlist_new();


	while (true) {
		zmq_pollitem_t backends[] = {
				{localfe, 0, ZMQ_POLLIN, 0},
				{cloudbe, 0, ZMQ_POLLIN, 0}
		};

		// if we have no workers then wait indefinitely
		int rc = zmq_poll(backends, 2, capacity? 1000 * ZMQ_POLL_MSEC: -1);
		if (rc == -1)
			break;

		// handle reply from local worker
		zmsg_t *msg = NULL;
		// start polling

		if (backends[0].revents & ZMQ_POLLIN) {
			msg = zmsg_recv (localbe);

			if (!msg)
				break;

			zframe_t *frame = zmsg_first(msg);
			if (memcmp(zframe_data(frame), WORKER_READY, 1) == 0) {
				zmsg_destroy(&msg);
			}
		}
		else {
			if (backends[1].revents & ZMQ_POLLIN) {
				msg = zmsg_recv (cloudbe);
				if (!msg)
					break;

				// We dont use peer broker identity for anything
				zframe_t *identity = zmsg_unwrap(msg);
				zframe_destroy(&identity);
			}
		}

		// Route reply to broker if it is addresses to a broker
		for (argn = 2; msg && argn < argc; argn ++) {
			// route to random peer
			char *data = (char *) zframe_data (zmsg_first (msg));
			size_t size = zframe_size (zmsg_first (msg));

			if (size == strlen (argv[argn]) && memcmp (data, argv[argn], size) == 0) {
				zmsg_send (&msg, cloudfe);
			}
		}

		if (msg)
			zmsg_send (&msg, localfe);

		// now we will route as many client requests we have worker capacity
		while (capacity) {
			zmq_pollitem_t frontends[] = {
					{ localfe, 0, ZMQ_POLLIN, 0},
					{ cloudfe, 0, ZMQ_POLLIN, 0}
			};

			rc = zmq_poll (frontends, 2, 0);
			assert (rc >= 0);

			int reroutable = 0;
			// we will do peer brokers first to prevent starvation
			if (frontends[1].revents & ZMQ_POLLIN) {
				msg = zmsg_recv (cloudfe);
				reroutable = 0;
			}
			else if (frontends[0].revents & ZMQ_POLLIN) {
				msg = zmsg_recv (localfe);
				reroutable = 1;
			}
			else
			{
				break;
			}

			if (reroutable && argc > 2 && randof (5) == 0) {
				// Route to random broker peer
				int peer = randof (argc - 2) + 2;
				zmsg_pushmem(msg, argv[peer], strlen (argv[peer]));
				zmsg_send (&msg, cloudbe);
			}
			else
			{
				zframe_t *frame = (zframe_t *) zlist_pop (workers);
				zmsg_wrap (msg, frame);
				zmsg_send (&msg, localbe);
				capacity --;
			}

		}

	}

	// when we are done wind up properly
	while (zlist_size(workers)) {
		zframe_t *frame = (zframe_t *) zlist_pop(workers);
		zframe_destroy(&frame);
	}

	zlist_destroy(&workers);
	zctx_destroy(&ctx);

	return 0;
}


























