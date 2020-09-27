extern int fitpair(char *, int);
extern void  putpair(char *, datum, datum);
extern datum	getpair(char *, datum);
extern int  delpair(char *, datum);
extern int  chkpage(char *);
extern datum getnkey(char *, int);
extern void splpage(char *, char *, long);
#ifdef SEEDUPS
extern int duppair(char *, datum);
#endif
