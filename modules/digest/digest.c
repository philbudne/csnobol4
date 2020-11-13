#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* sprintf */
#include <stdlib.h>			/* malloc/free */

#include <openssl/md5.h>
#include <openssl/sha.h>

#include "h.h"
#include "equ.h"
#include "str.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "module.h"
#include "handle.h"

SNOBOL4_MODULE(digest)

#define DIGEST(ALG,CTX,BITS) \
lret_t \
DIGEST_##ALG( LA_ALIST ) { \
    unsigned char out[CTX##_DIGEST_LENGTH]; \
    ALG((unsigned char *)LA_STR_PTR(0), LA_STR_LEN(0), out); \
    RETSTR2((char *)out, BITS/8); \
} \
static handle_handle_t ALG##_handles; \
lret_t \
DIGEST_##ALG##_INIT( LA_ALIST ) { \
    snohandle_t h; \
    CTX##_CTX *ctx = malloc(sizeof(CTX##_CTX)); \
    if (!ctx) RETFAIL; \
    bzero(ctx, sizeof(CTX##_CTX)); \
    if (!ALG##_Init(ctx)) { free(ctx); RETFAIL; } \
    h = new_handle2(&ALG##_handles, ctx, #ALG, free, modinst); \
    if (!OK_HANDLE(h)) { free(ctx); RETFAIL; } \
    RETHANDLE(h); \
} \
lret_t \
DIGEST_##ALG##_UPDATE( LA_ALIST ) { \
    CTX##_CTX *ctx = lookup_handle(&ALG##_handles, LA_HANDLE(0)); \
    if (!ctx) RETFAIL; \
    if (!ALG##_Update(ctx, LA_STR_PTR(1), LA_STR_LEN(1))) \
	RETFAIL; \
    RETNULL; \
} \
lret_t \
DIGEST_##ALG##_FINAL( LA_ALIST ) { \
    int ret; \
    snohandle_t h; \
    unsigned char out[CTX##_DIGEST_LENGTH]; \
    CTX##_CTX *ctx = lookup_handle(&ALG##_handles, LA_HANDLE(0)); \
    if (!ctx) RETFAIL; \
    ret = ALG##_Final(out, ctx); \
    remove_handle(&ALG##_handles, h); \
    if (!ret) RETFAIL; \
    RETSTR2((char *)out, BITS/8); \
}

/*
**=snobol4
**	LOAD("DIGEST_MD5(STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_MD5_INIT()MD5", DIGEST_DL)
**	LOAD("DIGEST_MD5_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_MD5_FINAL(EXTERNAL)STRING", DIGEST_DL)
**
**	LOAD("DIGEST_SHA1(STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA1_INIT()EXTERNAL", DIGEST_DL)
**	LOAD("DIGEST_SHA1_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA1_FINAL(EXTERNAL)STRING", DIGEST_DL)
**
**	LOAD("DIGEST_SHA224(STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA224_INIT()EXTERNAL", DIGEST_DL)
**	LOAD("DIGEST_SHA224_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA224_FINAL(EXTERNAL)STRING", DIGEST_DL)
**
**	LOAD("DIGEST_SHA256(STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA256_INIT()EXTERNAL", DIGEST_DL)
**	LOAD("DIGEST_SHA256_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA256_FINAL(EXTERNAL)STRING", DIGEST_DL)
**
**	LOAD("DIGEST_SHA384(STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA384_INIT()EXTERNAL", DIGEST_DL)
**	LOAD("DIGEST_SHA384_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA384_FINAL(EXTERNAL)STRING", DIGEST_DL)
**
**	LOAD("DIGEST_SHA512(STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA512_INIT()EXTERNAL", DIGEST_DL)
**	LOAD("DIGEST_SHA512_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_SHA512_FINAL(EXTERNAL)STRING", DIGEST_DL)
**=cut
*/

DIGEST(MD5, MD5, 128)
DIGEST(SHA1, SHA, 160)
DIGEST(SHA224, SHA256, 224)
DIGEST(SHA256, SHA256, 256)
DIGEST(SHA384, SHA512, 384)
DIGEST(SHA512, SHA512, 512)

/*
**=snobol4
**	LOAD("DIGEST_HEX(STRING)", DIGEST_DL)
**=cut
*/
lret_t
DIGEST_HEX( LA_ALIST ) {
    char out[512/4 + 1];
    int i = LA_STR_LEN(0);
    unsigned char *cp = (unsigned char *)LA_STR_PTR(0);
    char *op = out;
    while (i-- > 0) {
	sprintf(op, "%02x", *cp++);
	op += 2;
    }
    RETSTR2(out, op-out);
}
