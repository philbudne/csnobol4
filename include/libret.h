/* return values for library routines */

enum stream_ret {
    ST_STOP,
    ST_EOS,
    ST_ERROR
};

enum io_read_ret {
    IO_OK,
    IO_EOF,
    IO_ERR
};

enum stream_ret stream();
enum io_read_ret io_read();
/* $Id$ */

