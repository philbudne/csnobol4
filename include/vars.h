/* $Id$ */

/*
 * all static variables gathered in allocated "vars" struct
 * used only when NO_STATIC_VARS defined
 */

#include "equ.h"
#include "res.h"

struct vars {
    /* SIL resident data; */
    struct res v_res;

    /* for SIL code; */
    volatile int v_math_error;
    struct descr *v_cstack;
    struct descr v_ostack[1];

    /* lib/init.c */
    int v_rflag;
    int v_pmstack;
    int v_ndynamic;

    char *v_params;
    char **v_argv;
    int v_firstarg;
    int v_argc;

    /* lib/date.c, lib/intspc.c, lib/realst.c */
    char v_strbuf[64];			/* return buffer */

    /* lib/io.c */
    struct iovars *v_iov;

    /* lib/snolib/retstring.c */
    struct spec v_retspec[1];
    char *v_retbuf;
    int v_retbuflen;

    /* lib/X/tty.c */
    void *v_ttylist;

    /* lib/X/load.c */
    void *v_loadptr;

    /* lib/X/mstime.c */
    void *v_timeptr;

#ifdef ENDEX_LONGJMP
    void *v_endex_jmpbuf;
#endif /* ENDEX_LONGJMP defined */

#ifdef USE_WINSOCK_H
    int v_wsock_init;
#endif /* USE_WINSOCK_H */
};

#define res	(varp->v_res)
#define math_error (varp->v_math_error)
#define cstack	(varp->v_cstack)
#define ostack	(varp->v_ostack)
#define rflag	(varp->v_rflag)
#define pmstack	(varp->v_pmstack)
#define ndynamic (varp->v_ndynamic)
#define params	(varp->v_params)
#define argv	(varp->v_argv)
#define firstarg (varp->v_firstarg)
#define argc	(varp->v_argc)
#define ndynamic (varp->v_ndynamic)
#define strbuf	(varp->v_strbuf)
#define rflag	(varp->v_rflag)
#define iov	(*varp->v_iov)
#define retspec	(varp->v_retspec)
#define retbuf	(varp->v_retbuf)
#define retbuflen (varp->v_retbuflen)
#define ttylist	(varp->v_ttylist)
#define loadptr (varp->v_loadptr)
#define timeptr	(varp->v_timeptr)
#define endex_jmpbuf (varp->v_endex_jmpbuf)
#define wsock_init (varp->v_wsock_init)

extern struct vars *varp;
