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

static VAR handle_handle_t digest_handles;

/*
**=snobol4
**	LOAD("DIGEST_INIT(STRING)EXTERNAL", DIGEST_DL)
**	LOAD("DIGEST_UPDATE(EXTERNAL,STRING)STRING", DIGEST_DL)
**	LOAD("DIGEST_FINAL(EXTERNAL)STRING", DIGEST_DL)
**=cut
*/

#if OPENSSL_VERSION_NUMBER < 0x00907000L /* create added in 0.9.7? */
static EVP_MD_CTX *
EVP_MD_CTX_new(void) {
    EVP_MD_CTX *ctx = malloc(sizeof(EVP_MD_CTX));
    bzero(ctx, sizeof(EVP_MD_CTX));
    return ctx;
}

void
EVP_MD_CTX_free(EVP_MD_CTX *ctx) {
    /* cleanup?? */
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
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    static int once;			/* global */

    if (!once) {
	OpenSSL_add_all_digests();
	once = 1;
    }
#endif

    if (!ctx)
	RETFAIL;

    alg = mgetstring(LA_PTR(0));
    if (!alg)
	goto fail;
    
    md = EVP_get_digestbyname(alg);
    free(alg);

    if (!md)
	goto fail;
#if OPENSSL_VERSION_NUMBER < 0x00907000L
    EVP_DigestInit(ctx, md);		/* void in 0.9.6 */
#else
    if (!EVP_DigestInit(ctx, md))
	goto fail;
#endif

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
#if OPENSSL_VERSION_NUMBER < 0x00907000L
    EVP_DigestUpdate(ctx, LA_STR_PTR(1), LA_STR_LEN(1)); /* void */
#else
    if (!EVP_DigestUpdate(ctx, LA_STR_PTR(1), LA_STR_LEN(1)))
	RETFAIL;
#endif
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

#if OPENSSL_VERSION_NUMBER < 0x00907000L
    EVP_DigestFinal(ctx, out, &s);	/* void */
    ret = 1;
#else
    ret = EVP_DigestFinal(ctx, out, &s);
#endif
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

/*
*=snobol4
*
*	DEFINE("DIGEST(ALG,STR)CTX")	:(DIGEST.END)
*DIGEST	CTX = DIGEST_INIT(ALG)		:F(FRETURN)
*	DIGEST_UPDATE(CTX, STR)		:F(FRETURN)
*	DIGEST = DIGEST_FINAL(CTX)	:F(FRETURN)S(RETURN)
*DIGEST.END
*=cut
*/
