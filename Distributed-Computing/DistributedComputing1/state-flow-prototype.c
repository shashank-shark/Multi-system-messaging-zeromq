/*
 * state-flow-prototype.c
 *
 *  Created on: 04-Sep-2019
 *      Author: shashank
 */

#include <czmq.h>

int main (int argc, char *argv[])
{
	// first argument is the broker's name
	// other argument are our peer names

	if (argc < 2) {
		printf ("syntax invalid : peering1 me {you} ...\n");
		return 0;
	}

	char *self = argv[1];
	printf ("I : Preparing broker at %s ...\n", self);
	srandom ((unsigned int)time(NULL));

	// create the context
	zctx_t *ctx = zctx_new();

	// Bind the state backend to endpoint
	void *statebe = zsocket_new(ctx, ZMQ_PUB);
	zsocket_bind(statebe, "ipc://%s-state.ipc", self);


	// connect statefe to all peers
	void *statefe = zsocket_new (ctx, ZMQ_SUB);
	zsocket_set_subscribe(statefe, "");


	int argn = 0;
	for (argn = 2; argn < argc; argn ++) {
		char *peer = argv[argn];
		printf ("I : Connecting to state backend at '%s' \n", peer);
		zsocket_connect(statefe, "ipc://%s-state.ipc", peer);
	}

	// now the main while loop sends out the status messages to the peers and
	// collects the status messages back from the peers. The zmq_poll timeout
	// defines our own heartbeat

	while (true) {
		zmq_pollitem_t items[] = {
				{statefe, 0, ZMQ_POLLIN, 0}
		};

		int rc = zmq_poll (items, 1, 1000 * ZMQ_POLL_MSEC);
		if (rc == -1)	break;

		if (items[0].revents & ZMQ_POLLIN) {
			char *peer_name = zstr_recv (statefe);
			char *available = zstr_recv(statefe);
			printf ("%s - %s workers free \n", peer_name, available);
			free (peer_name);
			free (available);
		}
		else {
			zstr_sendm(statefe, self);
			zstr_sendf(statebe, "%d", randof(10));
		}
	}

	zctx_destroy(&ctx);
	return 0;
}
