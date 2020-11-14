/*
 * $Id$
 * Message Digests using OpenSSL high level "Envelope" API
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>			/* sprintf */
#include <stdlib.h>			/* free */

#include <openssl/evp.h>
#include <openssl/opensslv.h>

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

static int once;			/* global */
static VAR handle_handle_t digest_handles;

/*
**=snobol4
**	LOAD("DIGEST_INIT(STRING)EXTERNAL", DIGEST_DL)
**	LOAD("DIGEST_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_FINAL(EXTERNAL)STRING", DIGEST_DL)
**=cut
*/

#if OPENSSL_VERSION_NUMBER < 0x00090700L /* create added in 0.9.7? */
static EVP_MD_CTX *
EVP_MD_CTX_new(void) {
    EVP_MD_CTX *ctx = malloc(sizeof(EVP_MD_CTX));
    /* new in 0.9.7? changed to _reset in the great renaming of 1.1.0?? */
    EVP_MD_CTX_init(ctx);
    return ctx;
}

void
EVP_MD_CTX_free(EVP_MD_CTX *ctx) {
    EVP_MD_CTX_cleanup(ctx);
    free(ctx);
}
#elif OPENSSL_VERSION_NUMBER < 0x10100000L /* renamed in 1.1.0 */
#define EVP_MD_CTX_new EVP_MD_CTX_create
#define EVP_MD_CTX_free EVP_MD_CTX_destroy
#endif

#ifdef DEBUG_DIGEST
#define DEBUGF(x) printf x
#else
#define DEBUGF(x)
#endif

static void
free_ctx(void *ctx) {
    EVP_MD_CTX_free(ctx);
}

lret_t
DIGEST_INIT( LA_ALIST ) {
    char *alg;
    snohandle_t h;
    const EVP_MD *md;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    if (!once) {
	/* needed w/ 0.9.8b, but not 1.1.1d */
	OpenSSL_add_all_digests();
	once = 1;
    }
    DEBUGF(("line %d\n", __LINE__));
    if (!ctx)
	RETFAIL;

    DEBUGF(("line %d\n", __LINE__));
    alg = mgetstring(LA_PTR(0));
    if (!alg)
	goto fail;
    
    DEBUGF(("line %d: %s\n", __LINE__, alg));
    md = EVP_get_digestbyname(alg);
    free(alg);
    DEBUGF(("line %d: md %p\n", __LINE__, md));
    if (!md || !EVP_DigestInit(ctx, md))
	goto fail;

    DEBUGF(("line %d\n", __LINE__));
    h = new_handle2(&digest_handles, ctx, "DIGEST", free_ctx, modinst);
    if (!OK_HANDLE(h)) {
    fail:
	EVP_MD_CTX_free(ctx);
	RETFAIL;
    }
    DEBUGF(("line %d\n", __LINE__));
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
