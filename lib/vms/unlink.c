/*
 * $Id$
 * Needed under VAXC v3
 */

int
unlink(char *fname) {
    return delete(fname);
}
