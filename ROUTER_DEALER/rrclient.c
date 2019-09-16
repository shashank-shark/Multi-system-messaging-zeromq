#include <czmq.h>

int main (void) 
{
    zctx_t *ctx = zctx_new();

    // connect to the talk server
    void *requester = zsocket_new (ctx, ZMQ_REQ);
    zsocket_bind (requester, "tcp://localhost:5559");

    int request_nbr = 0;
    for (request_nbr = 0; request_nbr != 10; request_nbr ++)
    {
        zstr_sendm (requester, "Hello");
        char *str = zstr_recv (requester);
        printf ("Recieved Reply for request %d : [%s] \n",request_nbr, str );
        free (str);
    }

    zctx_destroy (&ctx);

    return 0;
}