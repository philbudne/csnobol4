/* $Id$ */

void
bzero( ptr, len )
    char *ptr;
    int len;
{
    while (len-- > 0) {
	*ptr++ = 0;
    }
}
