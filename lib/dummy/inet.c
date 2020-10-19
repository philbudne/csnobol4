/* $Id$ */

/* dummy inet interface */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdio.h>

#include "inet.h"			/* prototypes, sock_t */

sock_t
tcp_socket(char *host, char *service, int port, int priv) {
    return -1;
}

sock_t
udp_socket(char *host, char *service, int port, int priv) {
    return -1;
}

void
inet_cleanup() {
}
