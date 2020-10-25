#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "snotypes.h"
#include "h.h"
#include "lib.h"

int
sleepf(real_t t) {
    if (t > 0.0 && t < 1.0)
	t = 1.0;
    sleep((int)t);
    return 0;
}
