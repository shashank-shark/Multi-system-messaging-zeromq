#include <czmq.h>

int main (void)
{

    zctx_t *ctx = zctx_new();

    void *responder = zsocket_new(ctx, ZMQ_REP);
    zsocket_bind(responder, "tcp://localhost:5560");

    // poll for requests from broker
    while (1)
    {
        char *str = zstr_recv (responder);
        printf ("Request Recieved : [%s]\n", str);

        sleep (1);
        zstr_sendf (responder, "Replied - %s",str);
        free (str);
    }

    zctx_destroy (&ctx);
    return 0;
}