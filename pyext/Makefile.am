
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=gnu99 $(DEPS_CFLAGS) $(PYTHON_CPPFLAGS)

DEFAULT_INCLUDES=-I$(top_srcdir)

pkgpython_PYTHON = __init__.py
pkgpython_LTLIBRARIES = tvpicklemodule.la
tvpicklemodule_la_SOURCES = tvpicklemodule.c
tvpicklemodule_la_LDFLAGS = -module $(PYTHON_LDFLAGS)
tvpicklemodule_la_LIBADD = -ltvpickle $(DEPS_LIBS)

bin_SCRIPTS=tvpsend

