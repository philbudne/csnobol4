/*
 * $Id$
 * functions from from inet(6).c
 * Phil Budne
 * 2020-09-15
*/

#ifndef sock_t
#define sock_t int
#define close_socket close
#define SETSOCKOPT_ARG_CAST
#endif

sock_t tcp_socket(char *host, char *service, int port, int flags);
sock_t udp_socket(char *host, char *service, int port, int flags);
void inet_cleanup(void);

/*
 * flags returned by inet_parse (in io.h)
 * used by tcp_socket, udp_socket
 */

#define INET_PRIV	01
#define INET_BROADCAST	02
#define INET_REUSEADDR	04
#define INET_DONTROUTE	010
#define INET_OOBINLINE	020
#define INET_KEEPALIVE	040
#define INET_NODELAY	0100
#define INET_CLOEXEC	0200
#define INET_VERIFY	0400
