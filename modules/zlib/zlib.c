/*
 * $Id$
 * In-memory zlib compress/decompress/crc in the manner of Python zlib module
 * Phil Budne <phil@snobol4.org>
 * November 2020
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>			/* malloc/realloc/free */
#include <zlib.h>

#include "h.h"				/* EXPORT */
#include "snotypes.h"			/* struct descr */
#include "load.h"
#include "equ.h"
#include "str.h"
#include "macros.h"

#include "module.h"
SNOBOL4_MODULE(zlib)

/*
**=pea
**=sect NAME
**zlib library compression
**=sect SYNOPSYS
**=code
**B<-INCLUDE 'zlib.sno'>
**	B<ZLIB_COMPRESS(>I<string>,[I<level>]B<)>
**=ecode
**=cut
**=snobol4
**	LOAD("ZLIB_COMPRESS(STRING,)STRING", ZLIB_DL)
**=cut
*/
lret_t
ZLIB_COMPRESS( LA_ALIST ) {
    int level = Z_DEFAULT_COMPRESSION;
    z_stream z;
    unsigned int size;
    unsigned char *buf;
    int flush;

    bzero(&z, sizeof(z));
    z.avail_in = LA_STR_LEN(0);
    z.next_in = (unsigned char *)LA_STR_PTR(0);

    if (LA_TYPE(1) == I)
	level = LA_INT(1);

    z.avail_out = size = z.avail_in;
    z.next_out = buf = malloc(size);
    if (!z.next_out)
	RETFAIL;

    switch (deflateInit(&z, level)) {
    case Z_OK:
        break;
    case Z_MEM_ERROR:
    case Z_STREAM_ERROR:		/* bad compression level? */
	RETFAIL;
    default:
        deflateEnd(&z);
	RETFAIL;
    }

    do {
	if (z.avail_out == 0) {
	    unsigned int incr = size;
	    unsigned int nsize = size + incr;
	    unsigned char *nbuf = realloc(buf, nsize);
	    if (!nbuf) {
		free(buf);
		RETFAIL;
	    }
	    z.next_out = nbuf + size;
	    z.avail_out = incr;
	    buf = nbuf;
	    size = nsize;
	}
	flush = z.avail_in ? Z_NO_FLUSH : Z_FINISH;
	deflate(&z, flush);
    } while (flush != Z_FINISH);
    size -= z.avail_out;
    deflateEnd(&z);
    RETSTR2_FREE((char *)buf, size);
}

/*
**=code
**	B<ZLIB_UNCOMPRESS(>I<string>,[I<wbits>],[I<ibufsize>]B<)>
**=ecode
**=cut
**=snobol4
**	LOAD("ZLIB_UNCOMPRESS(STRING,,)STRING", ZLIB_DL)
**=cut
*/
lret_t
ZLIB_UNCOMPRESS( LA_ALIST ) {
    int wbits = MAX_WBITS;
    z_stream z;
    unsigned int size;
    unsigned char *buf;
    int flush;

    bzero(&z, sizeof(z));
    z.avail_in = LA_STR_LEN(0);
    z.next_in = (unsigned char *)LA_STR_PTR(0);

    if (LA_TYPE(1) == I)
	wbits = LA_INT(1);

    size = z.avail_in * 2;
    if (LA_TYPE(2) == I)
	size = LA_INT(1);
    if (size == 0)
	size = 1024;

    z.avail_out = size;
    z.next_out = buf = malloc(size);
    if (!z.next_out)
	RETFAIL;

    switch (inflateInit2(&z, wbits)) {
    case Z_OK:
        break;
    case Z_MEM_ERROR:
    case Z_STREAM_ERROR:		/* bad compression level? */
	RETFAIL;
    default:
        inflateEnd(&z);
	RETFAIL;
    }

    do {
	if (z.avail_out == 0) {
	    unsigned int incr = size;
	    unsigned int nsize = size + incr;
	    unsigned char *nbuf = realloc(buf, nsize);
	    if (!nbuf) {
		free(buf);
		RETFAIL;
	    }
	    z.next_out = nbuf + size;
	    z.avail_out = incr;
	    buf = nbuf;
	    size = nsize;
	}
	flush = z.avail_in ? Z_NO_FLUSH : Z_FINISH;
	inflate(&z, flush);		/* XXX check return */
    } while (flush != Z_FINISH);
    size -= z.avail_out;
    inflateEnd(&z);
    RETSTR2_FREE((char *)buf, size);
}


/*
**=code
**	B<ZLIB_CRC32(>I<string>B<)>
**=ecode
**=cut
**=snobol4
**	LOAD("ZLIB_CRC32(STRING)INTEGER", ZLIB_DL)
**=cut
*/

lret_t
ZLIB_CRC32( LA_ALIST ) {
    unsigned int val = 0;
    int_t len = LA_STR_LEN(0);
    unsigned char *buf = (unsigned char *)LA_STR_PTR(0);

    if (LA_TYPE(1) == I)
	val = LA_INT(1);

    while (len > 0) {
	int tlen;
	if (len > 0x7fffffff)
	    tlen = 0x7fffffff;
	else
	    tlen = len;
	val = crc32(val, buf, tlen);
	buf += tlen;
	len -= tlen;
    }
    RETINT(val & 0xffffffff);
}

/*
**=code
**	B<ZLIB_ADLER32(>I<string>[,I<starting_value>]B<)>
**=ecode
**=cut
**=snobol4
**	LOAD("ZLIB_ADLER32(STRING)INTEGER", ZLIB_DL)
**=cut
*/

lret_t
ZLIB_ADLER32( LA_ALIST ) {
    unsigned int val = 1;
    int_t len = LA_STR_LEN(0);
    unsigned char *buf = (unsigned char *)LA_STR_PTR(0);

    if (LA_TYPE(1) == I)
	val = LA_INT(1);

    while (len > 0) {
	int tlen;
	if (len > 0x7fffffff)
	    tlen = 0x7fffffff;
	else
	    tlen = len;
	val = adler32(val, buf, tlen);
	buf += tlen;
	len -= tlen;
    }
    RETINT(val & 0xffffffff);
}

/*
**=sect DESCRIPTION
**The zlib module implements direct (in memory) access to zlib compression
**and checksum routines in the manner of the Python zlib module.
**
**For compressed file I/O see B<snobol4io>(1).
**
**=sect COMPATIBILITY
**L<http://tools.ietf.org/html/rfc1950> (zlib format),
**L<http://tools.ietf.org/html/rfc1951> (deflate format),
**L<http://tools.ietf.org/html/rfc1952> (gzip format)
**
**=sect SEE ALSO
**B<snobol4>(1), B<snobol4io>(1), L<http://zlib.net/>
**
**=sect AUTHOR
**Phil Budne
**=cut
*/
