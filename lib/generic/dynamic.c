/* $Id$ */

/* allocate dynamic region */

char *malloc();

char *
dynamic( size )
    int size;
{
    return malloc(size);
}
