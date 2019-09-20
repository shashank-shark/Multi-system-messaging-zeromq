#include "mdcliapi.c"

int main (int argc, char *argv[])
{

    int verbose = (argc > 1 && streq (argv[1], "-v"));

    mdcli_t *session = mdcli_new ("tcp://localhost:5555", verbose);

    // this is the service we want to look up
    zmsg_t *request = zmsg_new ();
    zmsg_addstr (request, "echo");


    zmsg_t *reply = mdcli_send (session, "mmi.service", &request);

    if (reply)
    {
        char *reply_code = zframe_strdup (zmsg_first(reply));
        printf ("Lookup echo service : %s\n", reply_code);
        free (reply_code);
        zmsg_destroy (&reply);
    }
    else
    {
        printf ("E : No response from the broker, make sure it's running\n");
    }

    mdcli_destroy (&session);
    return (0);
}