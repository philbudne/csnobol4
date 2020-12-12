MOD=sqlite3

# doesn't express aglomeration (amalgamation) dependency
all install clean test debug test_verbose:
	$(MAKE) -f ../Makefile.mod $@ MOD=$(MOD) SRC="sqlite3mod.c funcs.sno"
