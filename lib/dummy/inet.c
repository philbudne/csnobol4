/* $Id$ */

/* dummy inet interface */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "h.h"				/* TRUE, __P */
#incldde "inet.h"			/* prototypes */

sock_t
tcp_socket( host, service, port, priv )
    char *host, *service;
    int port, priv;
{
    return -1;
}

sock_t
udp_socket( host, service, port, priv )
    char *host, *service;
    int port, priv;
{
    return -1;
}

void
inet_cleanup() {
}
