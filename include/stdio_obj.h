/*
 * $Id$
 * I/O object using stdio
 * Phil Budne
 * 9/11/2020
 */

struct stdio_obj {
    struct io_obj io;
    FILE *f;
    enum { LAST_NONE, LAST_OUTPUT, LAST_INPUT } last;
};

#define MAXMODE 8			/* X+bex<NUL> */
void flags2mode(int flags, char *mode, char dir);

struct io_obj *stdio_wrap(char *path, FILE *, size_t objsize,
			  const struct io_ops *, int flags);
struct io_obj *stdio_open(char *path, int flags, int dir);
struct io_obj *pipeio_open(char *path, int flags, int dir);
struct io_obj *ptyio_open(char *path, int flags, int dir);

extern const struct io_ops stdio_ops;	/* for use as SUPER */
