/* $Id$ */

/* dummy inet interface */

#include <stdio.h>

FILE *
tcp_open( host, service, port, priv )
    char *host, *service;
    int port, priv;
{
    return NULL;
}


FILE *
udp_open( host, service, port, priv )
    char *host, *service;
    int port, priv;
{
    return NULL;
}

void
inet_cleanup() {
}
