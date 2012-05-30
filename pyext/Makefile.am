
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=gnu99 $(DEPS_CFLAGS) $(PYTHON_CPPFLAGS)
#AX_SWIG_PYTHON_OPT=-python -includeall -I..

DEFAULT_INCLUDES=-I$(top_srcdir)

#BUILT_SOURCES = tvpickle_wrap.c
#SWIG_SOURCES = tvpickle.i

pkgpython_PYTHON = __init__.py
pkgpython_LTLIBRARIES = tvpicklemodule.la
tvpicklemodule_la_SOURCES = tvpicklemodule.c
tvpicklemodule_la_LDFLAGS = -module $(PYTHON_LDFLAGS)
tvpicklemodule_la_LIBADD = -ltvpickle $(DEPS_LIBS)

#tvpickle_wrap.c: $(SWIG_SOURCES)
#	$(SWIG) $(AX_SWIG_PYTHON_OPT) -o $@ $<

#CLEANFILES=tvpickle_wrap.c tvpickle.py
