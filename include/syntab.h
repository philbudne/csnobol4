/* $Id$ */

enum action { AC_CONTIN, AC_STOP, AC_STOPSH, AC_ERROR, AC_GOTO };

struct acts {
    int_t put;				/* int or spec! */
    enum action act;
    struct syntab *go;
};

#define CHARSET 256			/* XXX */
struct syntab {
    char *name;				/* table name */
    char chrs[CHARSET];			/* index into actions */
    /* XXX include count of entries in actions array? */
    const struct acts *actions;		/* pointer to action table */
};

enum stream_ret {
    ST_STOP,
    ST_EOS,
    ST_ERROR
};

/* from lib/stream.c */
enum stream_ret stream(struct spec *,struct spec *,struct syntab *);
void clertb(struct syntab *,enum action);
void plugtb(struct syntab *,enum action,struct spec *);
int any(struct spec *,struct descr *);
