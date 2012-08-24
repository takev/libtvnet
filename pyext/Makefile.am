
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=gnu99 $(DEPS_CFLAGS) $(PYTHON_CPPFLAGS)

DEFAULT_INCLUDES=-I$(top_srcdir)

pkgpython_PYTHON = __init__.py
pkgpython_LTLIBRARIES = tvnetmodule.la
tvnetmodule_la_SOURCES = tvnetmodule.c
tvnetmodule_la_LDFLAGS = -module $(PYTHON_LDFLAGS) -L../tvnet
tvnetmodule_la_LIBADD = -ltvnet $(DEPS_LIBS)

dist_bin_SCRIPTS=tvnsend

