/* $Id$ */

/* dummy getenv() function for snolib/host.c */

char *
getenv( str )
    char *str;
{
    return (char *)0;			/* NULL */
}
