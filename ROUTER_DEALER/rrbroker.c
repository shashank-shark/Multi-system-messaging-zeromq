#include <czmq.h>

int main (void)
{
    zctx_t *ctx = zctx_new();

    void *frontend = zsocket_new (ctx, ZMQ_ROUTER);
    int rc = zsocket_bind (frontend, "tcp://*:5559");
    assert (rc == 0);

    void *backend = zsocket_new (ctx, ZMQ_DEALER);
    rc = zsocket_bind (backend, "tcp://*:5560");
    assert (rc == 0);

    zmq_proxy (frontend, backend, NULL);

    zmq_ctx_destroy (&ctx);

    return 0;
}