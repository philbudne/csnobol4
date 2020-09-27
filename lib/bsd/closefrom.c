/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <unistd.h>
#include <stdio.h>			/* for lib.h */

#include "h.h"
#include "snotypes.h"
#include "lib.h"

void
closefrom(int minfd) {
    int i;
    for (i = getdtablesize(); i >= minfd; i--)
	close(i);
}
