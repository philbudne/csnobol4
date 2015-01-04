/* $Id$ */

/* prototypes for "lib" functions */

/* from bal.c */
int getbal __P((struct spec *,struct descr *));

/* from date.c */
void date __P((struct spec *,struct descr *));

/* from endex.c */
void endex __P((int));

/* from hash.c */
#ifndef STATIC_HASH
void hash __P((struct descr *,struct spec *));
#endif /* STATIC_HASH not defined */

/* from init.c */
int getparm __P((struct spec *));
void init_args __P((int, char *[]));
void init __P((void));

/* from io.c */
void io_initvars __P((void));
void io_backspace __P((int_t));
EXPORT(int) io_closeall __P((int));
EXPORT(int) io_skip __P((int));
EXPORT(void) io_input_file __P((char *));
#ifdef MEM_IO
EXPORT(void) io_input_string __P((char *));
#endif
EXPORT(int) io_mkfile __P((int, FILE *, char *));
EXPORT(int) io_mkfile_noclose __P((int, FILE *, char *));
EXPORT(int) io_attached __P((int));
#ifdef MEM_IO
EXPORT(int) io_output_string __P((int, char *, char *, int));
#endif
void io_printf __P((int_t,...));
void io_print __P((struct descr *,struct descr *,struct spec *));
int io_endfile __P((int_t));
/* io_read() in libret.h */
void io_rewind __P((int_t));
void io_ecomp __P((void));
int io_openi __P((struct descr *,struct spec *,struct spec *,struct descr *));
int io_openo __P((struct descr *,struct spec *,struct spec *));
/* io_include() in libret.h */
EXPORT(char *)io_fname __P((int));
int io_file __P((struct descr *,struct spec *));
int io_seek __P((struct descr *,struct descr *,struct descr *));
int io_sseek __P((int_t,int_t,int_t,int_t,int_t *));
int io_flushall __P((int));
EXPORT(int) io_findunit __P((void));
EXPORT(FILE *) io_getfp __P((int));
int io_pad __P((struct spec *,int));
int io_finish __P((void));
int io_add_lib_dir __P((char *));
int io_add_lib_path __P((char *));
char *io_lib_find __P((char *, char *, char *));
char *io_lib_dir __P((int n));
void io_preload __P((void));
void io_show_paths __P((void));
void io_fastpr __P((struct descr *,struct descr *,struct descr *,struct spec *,struct spec *));

/* from lexcmp.c */
int lexcmp __P((struct spec *,struct spec *));

/* from ordvst.c */
void ordvst __P((void));

/* from pair.c */
#ifndef STATIC_PAIR
int locapt __P((struct descr *,struct descr *,struct descr *));
int locapv __P((struct descr *,struct descr *,struct descr *));
#endif /* STATIC_PAIR not defined */

/* from pat.c */
#ifndef STATIC_PAT
void cpypat __P((struct descr *,struct descr *,struct descr *,struct descr *,struct descr *,struct descr *));
void linkor __P((struct descr *,struct descr *));
void lvalue __P((struct descr *,struct descr *));
void maknod __P((struct descr *,struct descr *,struct descr *,struct descr *,struct descr *,struct descr *));
#endif /* STATIC_PAT not defined */

/* from pml.c */
int getpmproto __P((struct spec *,struct descr *));

/* from realst.c */
void realst __P((struct spec *,struct descr *));

/* from replace.c */
void rplace __P((struct spec *,struct spec *,struct spec *));

/* from str.c */
int pad __P((struct descr *,struct spec *,struct spec *,struct spec *));
void raise1 __P((struct spec *));
int raise2 __P((struct spec *,struct spec *));
int reverse __P((struct spec *,struct spec *));
int substr __P((struct spec *,struct spec *,struct descr *));
void trimsp __P((struct spec *,struct spec *));
void spec2str __P((struct spec *,char *,int));
char *mspec2str __P((struct spec *));
void apdsp __P((struct spec *, struct spec *));
char *strjoin __P((char *str0, ...));
void mergsp __P((struct spec *,struct spec *,struct spec *));
void movblk2 __P((struct descr *,struct descr *,int_t));

/* stream.c declarations in include/syntab.h */

/* from top.c */
#ifndef STATIC_TOP
void top __P((struct descr *,struct descr *,struct descr *));
#endif /* STATIC_TOP not defined */

/* from tree.c */
void addsib __P((struct descr *,struct descr *));
void addson __P((struct descr *,struct descr *));
void insert __P((struct descr *,struct descr *));

/* from version.c */
void version __P((void));

/****************************************************************
 * system dependant functions
 */

/* from spcint.c */
int spcint __P((struct descr *,struct spec *));

/* from intspc.c */
void intspc __P((struct spec *, struct descr *));

/* from spreal.c */
int spreal __P((struct descr *,struct spec *));

/* from dynamic.c */
char *dynamic __P((size_t));
void vm_gc_advise __P((int));

/* from execute.c */
void execute __P((char *));

/* from exists.c */
int exists __P((char *));
int isdir __P((char *));
int abspath __P((char *));

/* from expops.c */
int expint __P((struct descr *,struct descr *,struct descr *));
int exreal __P((struct descr *,struct descr *,struct descr *));

/* from load.c */
void unload __P((struct spec *));
#ifdef LOAD_PROTO
typedef int (loadable_func_t)(LOAD_PROTO); /* function entry point */
loadable_func_t *os_load __P((char *, char *));
#endif

/* from loadx.c (or load.c) */
int callx __P((struct descr *,struct descr *,struct descr *,struct descr *));
int load __P((struct descr *,struct spec *,struct spec *));

/* from mstime.c */
real_t mstime __P((void));

/* from syspend.c */
void proc_suspend __P((void));

/* from sys.c */
void hwname __P((char *));
void osname __P((char *));

/* from term.c */
FILE * term_input __P((void));

/* from inet(6).c */
FILE *tcp_open __P((char *, char *, int, int));
FILE *udp_open __P((char *, char *, int, int));
void inet_cleanup __P((void));
int inet_close __P((FILE *));

#define INET_PRIV	01
#define INET_BROADCAST	02
#define INET_REUSEADDR	04
#define INET_DONTROUTE	010
#define INET_OOBINLINE	020
#define INET_KEEPALIVE	040
#define INET_NODELAY	0100

/* from tty.c */
int fisatty __P((FILE *, char *));
void tty_mode __P((FILE *, int, int, int));
void tty_close __P((FILE *));		/* advisory! */
void tty_suspend __P((void));		/* SIG_TSTP */
/* if TTY_READ_RAW or TTY_READ_COOKED defined; */
int tty_read __P((FILE *,char *,int,int,int,int,char *));

/*
 * other functions we provide on some systems:
 * bcmp bcopy bzero (see str.h)
 * isnan
 */

/* from getredirect.c */
#ifdef vms
int getredirection __P((int, char **));
#endif /* vms defined */

/* from popen.c */
#ifdef NEED_POPEN
extern FILE *popen __P((char *, char *)); /* from {generic,vms}/popen.c */
extern int pclose __P((FILE *));
#endif /* NEED_POPEN defined */

#ifdef OSDEP_OPEN
extern int osdep_open __P((const char *, const char *, FILE **));
#endif /* OSDEP_OPEN defined */

/* from sleep.c */
extern int sleepf __P((real_t));

/* from break.c */
extern int chk_break __P((int));
