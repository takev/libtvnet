
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=gnu99 $(DEPS_CFLAGS) $(PYTHON_CPPFLAGS)

DEFAULT_INCLUDES=-I$(top_srcdir)

pkgpython_PYTHON = __init__.py
pkgpython_LTLIBRARIES = tvrpcmodule.la
tvrpcmodule_la_SOURCES = tvrpcmodule.c
tvrpcmodule_la_LDFLAGS = -module $(PYTHON_LDFLAGS) -L../tvrpc
tvrpcmodule_la_LIBADD = -ltvrpc $(DEPS_LIBS)

bin_SCRIPTS=tvpsend

