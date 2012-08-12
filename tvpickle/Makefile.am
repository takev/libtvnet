
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=gnu99 $(DEPS_CFLAGS)

#dist_man3_MANS = tvpickle_init.3 tvpickle_time.3 tvpickle_tipc_node.3 tvpickle_tipc_cuid.3

DEFAULT_INCLUDES=-I$(top_srcdir)

lib_LTLIBRARIES = libtvpickle.la

libtvpickle_la_LDFLAGS = -version-info $(SHARED_VERSION_INFO)
libtvpickle_la_SOURCES = version.c stdarg_encoder.c stdarg_decoder.c common.c decompress.c
libtvpickle_la_LIBADD = $(DEPS_LIBS) -lm

library_includedir=$(includedir)/tvpickle-$(VERSION)/tvpickle
library_include_HEADERS = tvpickle.h version.h encoder.h length.h decoder.h stdarg_encoder.h stdarg_decoder.h common.h decompress.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = tvpickle.pc

