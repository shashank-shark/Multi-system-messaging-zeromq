#include <czmq.h>

static void
subscriber_thread (void *args, zctx_t *ctx, void *pipe)
{
    // subscribe to A and B
    void *subscriber = zsocket_new (ctx, ZMQ_SUB);
    zmq_connect (subscriber, "tcp://localhost:6001");

    zsockopt_set_subscriber (subscriber, "A");
    zsockopt_set_subscriber (subscriber, "B");

    int count = 0;

    while (count < 5)
    {
        char *string = zstr_recv (subscriber);
        if (!string)    break;

        free (string);
        count ++;
    }
    
    zsocket_destroy (ctx, subscriber);
}