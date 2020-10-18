/*
 * $Id$
 * I/O object using stdio
 * Phil Budne
 * 9/11/2020
 */

enum last_direction { LAST_NONE, LAST_OUTPUT, LAST_INPUT };

struct stdio_obj {
    struct io_obj io;
    FILE *f;
    enum last_direction last;
};

#define MAXMODE 8			/* X+bex<NUL> */
void flags2mode(int flags, char *mode, char dir);

struct io_obj *stdio_wrap(const char *path, FILE *, size_t objsize,
			  const struct io_ops *, int flags);
struct io_obj *stdio_open(const char *path, int flags, int dir);
struct io_obj *pipeio_open(const char *path, int flags, int dir);
struct io_obj *ptyio_open(const char *path, int flags, int dir);

extern const struct io_ops stdio_ops;	/* for use as SUPER */
