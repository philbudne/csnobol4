/* $Id$ */

/* dummy functions for EXPINT/EXREAL
 * (attempts to execute code using exponentiation will
 * cause fatal errors (as specified in SIL reference))
 */

int
expint(struct descr *res, struct descr *x, struct descr *y) {
    UNDF();
}

int
exreal(struct descr *res, struct descr *x, struct descr *y) {
    INTR10();
}
