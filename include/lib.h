/* $Id$ */

/* prototypes for "lib" functions */

/* from bal.c */
void bal __P((struct spec *,struct descr *));

/* from date.c */
void date __P((struct spec *,struct descr *));

/* from endex.c */
void endex __P((int));

/* from hash.c */
void hash __P((struct descr *,struct spec *));

/* from init.c */
int getparm __P((struct spec *));
void init_args __P((int, char *[]));
void init __P((void));

/* from intspc.c */
int intspc __P((struct spec *,struct descr *));

/* from io.c */
void io_backspace __P((int));
void io_ecomp __P((void));
void io_endfile __P((int));
int io_file __P((struct descr *,struct spec *));
int io_flushall __P((int));
int io_include __P((struct descr *,struct spec *));
void io_init __P((void));
int io_openi __P((struct descr *,struct spec *,struct spec *,struct descr *));
int io_openo __P((struct descr *,struct spec *,struct spec *));
int io_pad __P((struct spec *,int));
void io_print __P((struct descr *,struct descr *,struct spec *));
void io_printf __P((int,...));
void io_rewind __P((int));
int io_seek __P((struct descr *,struct descr *,struct descr *));

/* from lexcmp.c */
int lexcmp __P((struct spec *,struct spec *));

/* from ordvst.c */
void ordvst __P((void));

/* from pair.c */
int locapt __P((struct descr *,struct descr *,struct descr *));
int locapv __P((struct descr *,struct descr *,struct descr *));

/* from pat.c */
void cpypat __P((struct descr *,struct descr *,struct descr *,struct descr *,struct descr *,struct descr *));
void linkor __P((struct descr *,struct descr *));
void lvalue __P((struct descr *,struct descr *));
void maknod __P((struct descr *,struct descr *,struct descr *,struct descr *,struct descr *,struct descr *));

/* from pml.c */
void getpmproto __P((struct spec *,struct descr *));

/* from realst.c */
void realst __P((struct spec *,struct descr *));

/* from replace.c */
void rplace __P((struct spec *,struct spec *,struct spec *));

/* from str.c */
void pad __P((struct descr *,struct spec *,struct spec *,struct spec *));
void raise1 __P((struct spec *));
void raise2 __P((struct spec *,struct spec *));
void reverse __P((struct spec *,struct spec *));
void substr __P((struct spec *,struct spec *,struct descr *));
void trimsp __P((struct spec *,struct spec *));
void spec2str __P((struct spec *,char *,int));

/* stream.c declarations in include/syntab.h */

/* from top.c */
void top __P((struct descr *,struct descr *,struct descr *));

/* from tree.c */
void addsib __P((struct descr *,struct descr *));
void addson __P((struct descr *,struct descr *));
void insert __P((struct descr *,struct descr *));

/****************************************************************
 * system dependant functions
 */

/* from convert.c */
int spcint __P((struct descr *,struct spec *));
int spreal __P((struct descr *,struct spec *));

/* from dynamic.c */
char *dynamic __P((int));
void vm_gc_advise __P((int));

/* from expops.c */
int expint __P((struct descr *,struct descr *,struct descr *));
int exreal __P((struct descr *,struct descr *,struct descr *));

/* from load.c */
int callx __P((struct descr *,struct descr *,struct descr *,struct descr *));
int load __P((struct descr *,struct spec *,struct spec *));
void unload __P((struct spec *));

/* from mstime.c */
int_t mstime __P((void));

/* XXX missing; sys.c term.c tty.c */

/*
 * functions we provide on some systems:
 * bcmp bcopy bzero isnan popen pclose
 */
