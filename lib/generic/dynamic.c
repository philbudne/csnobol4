/* $Id$ */

/* allocate dynamic region */

char *malloc();

char *
dynamic( size )
    int size;
{
    return malloc(size);
}

void
vm_gc_advise(gc)
    int gc;
{
}
