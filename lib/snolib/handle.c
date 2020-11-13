/* $Id$ */

/*
 * manage lists of "handles" for loadable code (SNOBOL4 EXTERNAL types)
 * entries from each handle table are assigned a different
 * EXTERNAL datatype, validated on lookup.
 *
 * Also module management
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H defined */

#include <stdlib.h>                     /* for malloc */
#include <stdio.h>			/* for NULL */

#include "snotypes.h"
#include "h.h"
#include "load.h"			/* SNOLOAD_API */
#include "handle.h"			/* prototypes */
#include "module.h"			/* struct module */
#include "str.h"			/* bzero */

#define HANDLE_HASH_SIZE (1<<8)		/* power of two */

typedef unsigned int handle_datatype_t;	/* must fit in vfld */
static VAR handle_datatype_t next_handle_datatype; /* TLS?? */
static TLS char in_handle_cleanup;

typedef int_t handle_number_t;

struct handle_entry {
    struct handle_entry *next;
    handle_number_t handle_number;
    void *value;
};

struct handle_table {
    long entries;
    const char *name;
    handle_datatype_t datatype;		/* SNOBOL4 EXTERNAL datatype */
    handle_number_t next;		/* next handle to hand out */
    void (*release)(void *value);
    struct handle_table *next_table;
    struct handle_entry *hash[HANDLE_HASH_SIZE];
};

#define HANDLE_HASH(H) (((int)(H)) & (HANDLE_HASH_SIZE-1))

/* private to this file (pointer to it in module_instance_data)
 * allows per-instance data to grow without recompiling modules
 */
struct module_instance_private {
    struct handle_table *htlist;
};

static const struct descr bad_handle;

/* for lazy allocation! never fetch mip->priv!!! */
static struct module_instance_private *
get_mip_priv(struct module_instance *mip) {
    if (!mip->priv) {
	mip->priv = malloc(sizeof(struct module_instance_private));
	if (mip->priv)
	    bzero(mip->priv, sizeof(struct module_instance_private));
    }
    return mip->priv;
}

SNOLOAD_API(void *)
lookup_handle(handle_handle_t *hhp, snohandle_t h) {
    struct handle_table *htp = *hhp;
    struct handle_entry *hp;

    if (!htp)
	return NULL;

#ifdef DEBUG_HANDLE2
    fprintf(stderr, "lookup_handle %s %d\n", htp->name, (long)h.a.i);
#endif
    if (h.v != htp->datatype)
	return NULL;

    for (hp = htp->hash[HANDLE_HASH(h.a.i)]; hp; hp = hp->next) {
	if (hp->handle_number == h.a.i)
	    return hp->value;
    }
    return NULL;
}

/*
 * user passes "handle handle", a pointer to a place for us to store a
 * pointer to a handle table (on the first lookup attempt).  Assign
 * datatype numbers from high to low (DATA types are assigned from low
 * to high)
 */
SNOLOAD_API(snohandle_t)
new_handle2(handle_handle_t *hhp, void *vp,
	    const char *tname, void (*release)(void *),
	    struct module_instance *mip) {
    struct handle_table *htp = *hhp;
    struct handle_entry *hp;
    struct descr h;
    int hash;

    if (!htp) {
	/* first time thru? create hash table */
	htp = malloc(sizeof(struct handle_table));
	if (!htp)
	    return bad_handle;
	bzero(htp, sizeof(struct handle_table));
	if (next_handle_datatype == 0)	/* first call? */
	    next_handle_datatype = SIZLIM;
	htp->datatype = --next_handle_datatype; /* assign datatype */
	htp->name = tname;
	htp->release = release;
	*hhp = htp;

	/* link into list of tables to pass to cleanup */
	if (mip) {
	    struct module_instance_private *priv = get_mip_priv(mip);
	    htp->next_table = priv->htlist;
	    priv->htlist = htp;
	}
    }

    /*
     * no longer hashing on pointer/value; an error to double enter?
     * would have to search entire table!!!
     */
    
    /* allocate block */
    hp = malloc(sizeof(struct handle_entry));
    if (!hp)
	return bad_handle;

    hp->handle_number = htp->next++;
    hash = HANDLE_HASH(hp->handle_number);
    hp->next = htp->hash[hash];
    hp->value = vp;

    htp->hash[hash] = hp;
    htp->entries++;

    h.f = 0;
    h.v = htp->datatype;
    h.a.i = hp->handle_number;

#ifdef DEBUG_HANDLES
    fprintf(stderr, "new_handle2 %s dt %d %p => %ld\n",
	    htp->name, h.v, vp, (long)h.a.i);
#endif

    return h;
}

SNOLOAD_API(void)
remove_handle(handle_handle_t *hhp, snohandle_t h) {
    struct handle_table *htp = *hhp;
    struct handle_entry *hp, *pp;
    int hash = HANDLE_HASH(h.a.i);

    if (!htp || in_handle_cleanup)
	return;

#ifdef DEBUG_HANDLES
    fprintf(stderr, "remove_handle %s %ld\n", htp->name, (long)h.a.i);
#endif

    pp = NULL;
    for (hp = htp->hash[hash]; hp; pp = hp, hp = hp->next) {
	if (hp->handle_number == h.a.i) {
#ifdef DEBUG_HANDLES
	    fprintf(stderr, " found %ld => %p\n",
		   (long)hp->handle_number, hp->value);
#endif
	    if (pp)
		pp->next = hp->next;
	    else
		htp->hash[hash] = hp->next;
	    free(hp);
	    htp->entries--;
	    return;
	}
    }
}

/* call to cleanup one table */
static void
handle_cleanup_table(struct handle_table *htp) {
    int i;

    if (!htp->release)
	return;

    /* XXX keep per-table? nah */
    in_handle_cleanup = 1;	/* guard against remove_handle! */
    for (i = 0; i < HANDLE_HASH_SIZE; i++) {
	struct handle_entry *hp, *next;
	for (hp = htp->hash[i]; hp; hp = next) {
	    next = hp->next;
	    /* SHOULD NOT call remove_handle! */
	    (htp->release)(hp->value);
	    free(hp);
	}
	htp->hash[i] = NULL;
    }
    htp->entries = 0;
    in_handle_cleanup = 0;
    free(htp);
}

static void
handle_cleanup_tables(struct handle_table *htp) {
    while (htp) {
	struct handle_table *next = htp->next_table;
	handle_cleanup_table(htp);
	htp = next;
    }
}

/* for EXTERNAL_DATATYPE */
const char *
handle_table_name(struct descr *dp, struct module_instance *mip) {
    struct handle_table *htp;

    /* direct access! avoid lazy alloc */
    if (!mip->priv)
	return NULL;

    htp = mip->priv->htlist;
    while (htp) {
	if (dp->v == htp->datatype)
	    return htp->name;
	htp = htp->next_table;
    }
    return NULL;
}


/*****************
 * module support
 * belongs in module.c
 * here to avoid private data getters/setters
 * (or letting struct module_instance_private outside this file)
 */

/*
 * called for each thread entering (on load)
 * called via module->call_module_instance_init
 */
int
module_instance_init(struct module *mp) {
#ifdef DEBUG_MODULES
    fprintf(stderr, "module_instance_init %s\n", mp->name);
#endif

    /*
     * NOTE!!
     * check abi version & module struct size before touching anything else!
     * could return false if invalid ABI version:
     * too old (no longer supported)
     * too new (change in major version means incompatible change)
     */

    /* XXX increment mip->module->refcount (while holding global lock?)?? */

    return 1;
}

void
module_instance_cleanup(struct module *mp) {
    struct module_instance *mip = (mp->get_module_instance)();

#ifdef DEBUG_MODULES
    fprintf(stderr, "module_instance_cleanup %s\n", mp->name);
#endif

    /* XXX decrement mip->module->refcount?? */
    
    if (mip->priv) {
	handle_cleanup_tables(mip->priv->htlist);
	/* add other private data cleanup here */
	free(mip->priv);
	mip->priv = NULL;
    }
}
