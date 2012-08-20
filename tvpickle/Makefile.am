
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=gnu99 $(DEPS_CFLAGS)

#dist_man3_MANS = tvpickle_init.3 tvpickle_time.3 tvpickle_tipc_node.3 tvpickle_tipc_cuid.3

DEFAULT_INCLUDES=-I$(top_srcdir)

lib_LTLIBRARIES = libtvpickle.la

libtvpickle_la_LDFLAGS = -version-info $(SHARED_VERSION_INFO)
libtvpickle_la_LIBADD = $(DEPS_LIBS) -lm
libtvpickle_la_SOURCES =\
	version.c\
	common.c\
	pickle_stdarg_encoder.c\
	pickle_stdarg_decoder.c

library_includedir=$(includedir)/tvpickle-$(VERSION)/tvpickle
library_include_HEADERS =\
	tvpickle.h\
	version.h\
	common.h\
	pickle_length.h\
	pickle_encoder.h\
	pickle_decoder.h\
	pickle_stdarg_encoder.h\
	pickle_stdarg_decoder.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = tvpickle.pc

