/* $Id$ */

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

enum io_include_ret {
    INC_FAIL,
    INC_SKIP,
    INC_OK
};

enum stream_ret stream();
enum io_read_ret io_read();
enum io_include_ret io_include();
