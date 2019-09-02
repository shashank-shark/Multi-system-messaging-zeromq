#include <czmq.h>

// main function here
int main (int argc, char *argv[])
{
	if (argc < 2) {
		printf ("syntax : peering me {you} ...\n");
		return 0;
	}

	char *self = argv[1];

	printf("preparing broker at %s ...\n",self);
	srandom ((unsigned int) time(NULL));

	void *ctx = zctx_new ();
	
	// bind state backend to endpoint
	void *statebe = zsocket_new (ctx, ZMQ_PUB);
	zsocket_bind (statebe, "ipc://%s-state.ipc", self);

	// connect statefe to all peers
	void *statefe = zsocket_new (ctx, ZMQ_SUB);
	zsockopt_set_subscribe (statefe, "");

	int argn;

	for (argn = 2; argn < argc; argn ++) {
		char *peer = argv[argn];
		printf ("I : connecting to state backend at '%s'\n", peer);
		zsocket_connect (statefe, "ipc://%s-state.ipc", peer);
	}

	// main loop sends out status messages to peers and collects status messages back
	// from the peers

	while (1) {
		zmq_pollitem_t items[] = {
			{ statefe, 0, ZMQ_POLLIN, 0}
		};

		int rc = zmq_poll (items, 1, 1000*ZMQ_POLL_MSEC);
		if (rc == -1)
			break;

		// handle incoming status messages
		if (items[0].revents & ZMQ_POLLIN) {
			char *peer_name = zstr_recv (statefe);
			char *available = zstr_recv (statefe);

			printf ("%s - %s workers free\n", peer_name, available);
			free (peer_name);
			free (available);
		}
		else {
			zstr_sendm (statebe, self);
			zstr_sendm (statebe, "%d", randof(10));
		}

	}

	zctx_destroy (&ctx);

	return 0;
}