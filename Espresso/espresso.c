#include <czmq.h>

static void
subscriber_thread (void *args, zctx_t *ctx, void *pipe)
{
    // subscribe to A and B
    void *subscriber = zsocket_new (ctx, ZMQ_SUB);
    zmq_connect (subscriber, "tcp://localhost:6001");

    zsocket_set_subscribe (subscriber, "A");
    zsocket_set_subscribe (subscriber, "B");

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

static void
publisher_thread (void *args, zctx_t *ctx, void *pipe)
{
    void *publisher = zsocket_new (ctx, ZMQ_PUB);
    zsocket_bind (publisher, "tcp://*:6000");

    while (!zctx_interrupted)
    {
        char string[10];
        sprintf (string, "%c-%05d", randof (10) + 'A', randof (100000));

        if (zstr_send (publisher, string) == -1)
        {
            break;
        }

        zclock_sleep (100);
    }
}

static void
listener_thread (void *args, zctx_t *ctx, void *pipe)
{
    while (true)
    {
        zframe_t *frame = zframe_recv (pipe);

        if (!frame) break;

        zframe_print (frame, NULL);
        zframe_destroy (&frame);
    }
}

int main (void)
{
    zctx_t *ctx = zctx_new ();
    zthread_fork (ctx, publisher_thread, NULL);
    zthread_fork (ctx, subscriber_thread, NULL);

    void *subscriber = zsocket_new (ctx, ZMQ_XSUB);
    zsocket_connect (subscriber, "tcp://localhost:6000");
    void *publisher = zsocket_new (ctx, ZMQ_XPUB);
    zsocket_bind (publisher, "tcp://*:6001");

    void *listener = zthread_fork (ctx, listener_thread, NULL);

    zmq_proxy (subscriber, publisher, listener);

    puts (" Interrupted");

    zctx_destroy (&ctx);

    return 0;
}
