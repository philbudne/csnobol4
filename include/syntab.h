/* $Id$ */

enum action { AC_CONTIN, AC_STOP, AC_STOPSH, AC_ERROR, AC_GOTO };

struct acts {
    int put;				/* int or spec! */
    enum action act;
    struct syntab *go;
};

struct syntab {
    char *name;				/* table name */
    char chrs[128];			/* index into actions */
    /* XXX include count of entries in actions array? */
    struct acts *actions;		/* pointer to action table */
};
