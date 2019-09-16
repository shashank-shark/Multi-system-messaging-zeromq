#include <czmq.h>

int main (void)
{
    zctx_t *ctx = zctx_new();

    void *frontend = zsocket_new (ctx, ZMQ_ROUTER);
    zsocket_bind (frontend, "tcp://*:5559");
   

    void *backend = zsocket_new (ctx, ZMQ_DEALER);
   zsocket_bind (backend, "tcp://*:5560");


    zmq_proxy (frontend, backend, NULL);

    zmq_close (frontend);
    zmq_close (backend);
    zmq_ctx_destroy (&ctx);

    return 0;
}