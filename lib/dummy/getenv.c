/* $Id$ */

/* dummy getenv() function for snolib/host.c */

char *
getenv(char *str) {
    return (char *)0;			/* NULL */
}
