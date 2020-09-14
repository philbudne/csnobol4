/* $Id$ */

/* dummy inet interface */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>
#include "h.h"				/* TRUE */

int
tcp_socket( host, service, port, priv )
    char *host, *service;
    int port, priv;
{
    return -1;
}


FILE *
udp_socket( host, service, port, priv )
    char *host, *service;
    int port, priv;
{
    return -1;
}

void
inet_cleanup() {
}

int
inet_close( f )
    FILE *f;
{
    return TRUE;
}
