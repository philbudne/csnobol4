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
    struct acts *actions;		/* pointer to action table */
};
