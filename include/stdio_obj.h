struct stdio_obj {
    struct io_obj io;
    FILE *f;
    enum { LAST_NONE, LAST_OUTPUT, LAST_INPUT } last;
};

#define MAXMODE 8
void flags2mode __P((int flags, char *mode, char dir));

struct io_obj *stdio_wrap __P((char *path, FILE *, size_t, const struct io_ops *, int flags));
struct io_obj *stdio_open __P((char *path, int flags, int dir));
struct io_obj *pipeio_open __P((char *path, int flags, int dir));
struct io_obj *ptyio_open __P((char *path, int flags, int dir));

extern const struct io_ops stdio_ops;
