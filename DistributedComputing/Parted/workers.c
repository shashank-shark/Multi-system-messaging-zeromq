/*
 * workers.c
 *
 *  Created on: 06-Sep-2019
 *      Author: shashank
 */

#include <czmq.h>
#define NBR_WORKERS 5
#define NBR_CLIENTS 10
#define WORKER_READY "\001"

static char *self;

static void *
worker_task (void *args)
{
	zctx_t *ctx = zctx_new();
	void *worker = zsocket_new (ctx, ZMQ_REQ);
	zsocket_connect (worker, "ipc://%s-localbe.ipc", self);

	// Tell the broker that we are ready for work
	zframe_t *frame = zframe_new (WORKER_READY, 1);
	zframe_send(&frame, worker, 0);

	// Process the messages as they arrive
	while (true)
	{
		zmsg_t *msg = zmsg_recv (worker);
		if (!msg)
			break;

		sleep (randof(2));
		zmsg_send (&msg, worker);
	}

	zctx_destroy(&ctx);
	return NULL;
}

int main ()
{
	int worker_nbr = 0;
	for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr ++)
	{
		zthread_new (worker_task, NULL);
	}

	while(1) sleep (10);

	return 0;
}
