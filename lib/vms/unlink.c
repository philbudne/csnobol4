/*
 * $Id$
 * Needed under VAXC v3
 */

int
unlink(fname)
    char *fname;
{
    return delete(fname);
}
