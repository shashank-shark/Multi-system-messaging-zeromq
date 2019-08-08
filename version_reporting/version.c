#include <stdio.h>
#include "zhelpers.h"

int main (int argc, char **argv)
{
	int major, minor, patch;
	zmq_version (&major, &minor, &patch);
	printf ("Current ZMQ versions is : %d %d %d\n", major, minor, patch);
	return 0;
}
