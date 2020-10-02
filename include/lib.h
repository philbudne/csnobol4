/* $Id$ */

/* prototypes for "lib" functions */

/* from bal.c */
int getbal(struct spec *,struct descr *);

/* from date.c */
void date(struct spec *,struct descr *);

/* from endex.c */
void endex(int);

/* from hash.c */
#ifndef STATIC_HASH
void hash(struct descr *,struct spec *);
#endif /* STATIC_HASH not defined */

/* from init.c */
int getparm(struct spec *);
void init_args(int, char *[]);
void init(void);

/* from io.c */
void io_initvars(void);
void io_backspace(int_t);
EXPORT(int) io_closeall(int);
EXPORT(int) io_skip(int);
EXPORT(void) io_input_file(char *);
#ifdef MEM_IO
EXPORT(void) io_input_string(char *);
#endif
EXPORT(int) io_mkfile(int, FILE *, char *);
EXPORT(int) io_mkfile_noclose(int, FILE *, char *);
EXPORT(int) io_attached(int);
#ifdef MEM_IO
EXPORT(int) io_output_string(int, char *, char *, int);
#endif
void io_printf(int_t,...);
void io_print(struct descr *,struct descr *,struct spec *);
int io_endfile(int_t);
/* io_read() in libret.h */
void io_rewind(int_t);
void io_ecomp(void);
int io_openi(struct descr *,struct spec *,struct spec *,struct descr *);
int io_openo(struct descr *,struct spec *,struct spec *);
/* io_include() in libret.h */
EXPORT(char *)io_fname(int);
int io_file(struct descr *,struct spec *);
int io_seek(struct descr *,struct descr *,struct descr *);
int io_sseek(int_t,int_t,int_t,int_t,int_t *);
int io_flushall(int);
EXPORT(int) io_findunit(void);
EXPORT(FILE *) io_getfp(int);
int io_pad(struct spec *,int);
int io_finish(void);
int io_add_lib_dir(char *);
int io_add_lib_path(char *);
char *io_lib_find(char *, char *, char *);
char *io_lib_dir(int n);
void io_preload(void);
void io_show_paths(void);
void io_fastpr(struct descr *,struct descr *,struct descr *,struct spec *,struct spec *);

/* from lexcmp.c */
int lexcmp(struct spec *,struct spec *);

/* from ordvst.c */
void ordvst(void);

/* from pair.c */
#ifndef STATIC_PAIR
int locapt(struct descr *,struct descr *,struct descr *);
int locapv(struct descr *,struct descr *,struct descr *);
#endif /* STATIC_PAIR not defined */

/* from pat.c */
#ifndef STATIC_PAT
void cpypat(struct descr *,struct descr *,struct descr *,struct descr *,struct descr *,struct descr *);
void linkor(struct descr *,struct descr *);
void lvalue(struct descr *,struct descr *);
void maknod(struct descr *,struct descr *,struct descr *,struct descr *,struct descr *,struct descr *);
#endif /* STATIC_PAT not defined */

/* from pml.c */
int getpmproto(struct spec *,struct descr *);

/* from realst.c */
void realst(struct spec *,struct descr *);

/* from replace.c */
void rplace(struct spec *,struct spec *,struct spec *);

/* from str.c */
int pad(struct descr *,struct spec *,struct spec *,struct spec *);
void raise1(struct spec *);
int raise2(struct spec *,struct spec *);
int reverse(struct spec *,struct spec *);
int substr(struct spec *,struct spec *,struct descr *);
void trimsp(struct spec *,struct spec *);
void spec2str(struct spec *,char *,int);
char *mspec2str(struct spec *);
void apdsp(struct spec *, struct spec *);
char *strjoin(char *str0, ...);
void mergsp(struct spec *,struct spec *,struct spec *);
void movblk2(struct descr *,struct descr *,int_t);

/* stream.c declarations in include/syntab.h */

/* from top.c */
#ifndef STATIC_TOP
void top(struct descr *,struct descr *,struct descr *);
#endif /* STATIC_TOP not defined */

/* from tree.c */
void addsib(struct descr *,struct descr *);
void addson(struct descr *,struct descr *);
void insert(struct descr *,struct descr *);

/* from version.c */
void version(void);

/****************************************************************
 * system dependant functions
 */

/* from spcint.c */
int spcint(struct descr *,struct spec *);

/* from intspc.c */
void intspc(struct spec *, struct descr *);

/* from spreal.c */
int spreal(struct descr *,struct spec *);

/* from dynamic.c */
char *dynamic(size_t);
void vm_gc_advise(int);

/* from execute.c */
void execute(char *);

/* from exists.c */
int exists(char *);
int isdir(char *);
int abspath(char *);

/* from expops.c */
int expint(struct descr *,struct descr *,struct descr *);
int exreal(struct descr *,struct descr *,struct descr *);

/* from loadx.c */
int load(struct descr *, struct spec *, struct spec *);
void unload(struct spec *);

/* from load.c */
void *os_load_library(const char *name);
void os_unload_library(void *lib);
void *os_find_symbol(void *lib, const char *name, void **stash);
void os_unload_function(const char *name, void *stash);

/* from pml.c */
#ifdef LOAD_PROTO
typedef int (loadable_func_t)(LOAD_PROTO); /* function entry point */
loadable_func_t *pml_find(char *);
#endif /* LOAD_PROTO defined */

/* from loadx.c (or load.c) */
int callx(struct descr *,struct descr *,struct descr *,struct descr *);
int load(struct descr *,struct spec *,struct spec *);

/* from mstime.c */
real_t mstime(void);

/* from syspend.c */
void proc_suspend(void);

/* from sys.c */
void hwname(char *);
void osname(char *);

/* from term.c */
FILE * term_input(void);

/* {tc,ud}p_socket moved to inet.h */

/* from tty.c */
int fisatty(FILE *);		/* or fisatty.c */
/*
 * cbreak means don't wait for CR (binary/raw)
 * recl only meaningful in cbreak mode
 */
void tty_mode(FILE *, int cbreak, int noecho, int recl);
void tty_close(FILE *);		/* advisory! */
void tty_suspend(void);		/* SIG_TSTP */
/*
 * if TTY_READ_RAW or TTY_READ_COOKED defined (Windows, VMS)
 * cbreak means don't wait for CR (binary/raw)
 * keepeol only meaningful if !cbreak
 */
int tty_read(FILE *,char *buf,int len, int cbreak,int noecho,int keepeol,char * fname);

/*
 * other functions we provide on some systems:
 * bcmp bcopy bzero (see str.h)
 * isnan
 */

#ifdef vms
/* from getredirect.c */
int getredirection(int, char **);
#endif /* vms defined */

#ifdef NEED_POPEN
/* from popen.c */
extern FILE *popen(char *, char *); /* from {generic,vms}/popen.c */
extern int pclose(FILE *);
#endif /* NEED_POPEN defined */

#ifdef OSDEP_OPEN
/* from osdep.c */
extern int osdep_open(const char *, const char *, FILE **);
#endif /* OSDEP_OPEN defined */

/* from sleep.c */
extern int sleepf(real_t);

/* from break.c */
extern int chk_break(int);

#ifndef HAVE_CLOSEFROM
/* from closefrom.c */
extern void closefrom(int minfd);
#endif

#ifndef HAVE_GETDTABLESIZE
/* from getdtablesize.c */
extern int getdtablesize(void);
#endif

#ifndef HAVE_GETLINE
/* from getline.c */
extern ssize_t getline(char **bufp, size_t *lenp, FILE *fp);
#endif

#ifndef HAVE_GETOPT
/* from getopt.c */
extern int getopt(int argc, char **argv, char *opts);
#endif
