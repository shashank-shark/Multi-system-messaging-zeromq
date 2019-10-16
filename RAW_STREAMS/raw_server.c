#include <czmq.h>

int main (void)
{
    zctx_t *ctx = zctx_new();

    void *router = zsocket_new (ctx, ZMQ_ROUTER);

    zsocket_set_router_raw (router, 1);

    int rc = zsocket_bind (router, "tcp://*:5060");
    assert (rc != -1);

    while (true)
    {
        zframe_t *mess = zframe_recv (router);
        printf ("Message obtained\n");
        zframe_print (mess, "");

        if (!mess)  break;

        char *message = zstr_recv (router);
        printf ("Message recieved\n");
        printf ("%s", message);

    }

    zctx_destroy (&ctx);

    return 0;
}