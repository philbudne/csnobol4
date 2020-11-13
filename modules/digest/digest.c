#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* sprintf */
#include <stdlib.h>			/* free */

#include <openssl/evp.h>

#include "h.h"
#include "equ.h"
#include "str.h"
#include "snotypes.h"
#include "macros.h"
#include "load.h"
#include "module.h"
#include "handle.h"

SNOBOL4_MODULE(digest)

#define MAX_DIGEST_LENGTH (512/8)

static VAR handle_handle_t digest_handles;

/*
**=snobol4
**	LOAD("DIGEST_INIT(STRING)EXTERNAL", DIGEST_DL)
**	LOAD("DIGEST_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_FINAL(EXTERNAL)STRING", DIGEST_DL)
**=cut
*/

static void
free_ctx(void *ctx) {
    EVP_MD_CTX_free(ctx);
}

lret_t
DIGEST_INIT( LA_ALIST ) {
    snohandle_t h;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    char *alg = mgetstring(LA_PTR(0));
    const EVP_MD *md;

    ctx = EVP_MD_CTX_new();
    if (!ctx)
	RETFAIL;

    alg = mgetstring(LA_PTR(0));
    if (!alg)
	goto fail;
    
    md = EVP_get_digestbyname(alg);
    free(alg);
    if (!md || !EVP_DigestInit(ctx, md))
	goto fail;

    h = new_handle2(&digest_handles, ctx, "DIGEST", free_ctx, modinst);
    if (!OK_HANDLE(h)) {
    fail:
	EVP_MD_CTX_free(ctx);
	RETFAIL;
    }
    RETHANDLE(h);
}

lret_t
DIGEST_UPDATE( LA_ALIST ) {
    EVP_MD_CTX *ctx = lookup_handle(&digest_handles, LA_HANDLE(0));
    if (!ctx) RETFAIL;
    if (!EVP_DigestUpdate(ctx, LA_STR_PTR(1), LA_STR_LEN(1)))
	RETFAIL;
    RETNULL;
}

lret_t
DIGEST_FINAL( LA_ALIST ) {
    EVP_MD_CTX *ctx = lookup_handle(&digest_handles, LA_HANDLE(0));
    unsigned char out[MAX_DIGEST_LENGTH];
    unsigned int s;
    int ret;

    if (!ctx)
	RETFAIL;

    ret = EVP_DigestFinal(ctx, out, &s);
    EVP_MD_CTX_free(ctx);
    remove_handle(&digest_handles, LA_HANDLE(0));

    if (ret)
	RETSTR2((char *)out, s);

    RETFAIL;
}


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
