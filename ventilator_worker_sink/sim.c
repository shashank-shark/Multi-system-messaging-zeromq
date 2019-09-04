#include <zmq.h>
int main()
{
int major, minor, patch;
zmq_version (&major, &minor, &patch); printf ("Current ØMQ version is %d.%d.%d\n", major, minor, patch);
return 0;
}
