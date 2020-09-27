/*
 * $Id$
 *
 * language extensions
 */

#ifndef TRUE
#define TRUE	1
#endif /* TRUE not defined */

#ifndef FALSE
#define FALSE	0
#endif /* FALSE not defined */

#ifndef NULL
#define NULL	0
#endif /* NULL not defined */

/*
 * For Windoze DLL's
 * MSC and Borland want the decl on different sides of the type!!
 */

#ifndef EXPORT
#define EXPORT(TYPE) TYPE
#endif /* EXPORT not defined */

#ifndef IMPORT
#define IMPORT(TYPE) TYPE
#endif /* IMPORT not defined */
