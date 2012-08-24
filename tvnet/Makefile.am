
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=gnu99 $(DEPS_CFLAGS)

#dist_man3_MANS = tvnet_init.3 tvnet_time.3 tvnet_tipc_node.3 tvnet_tipc_cuid.3

DEFAULT_INCLUDES=-I$(top_srcdir)

lib_LTLIBRARIES = libtvnet.la

libtvnet_la_LDFLAGS = -version-info $(SHARED_VERSION_INFO)
libtvnet_la_LIBADD = $(DEPS_LIBS) -lm
libtvnet_la_SOURCES =\
	version.c\
	common.c\
	fmt_parser.c\
	pickle_print.c\
	pickle_stdarg_encoder.c\
	pickle_stdarg_decoder.c\
	log.c

check_PROGRAMS = pickle_print_test cuid_test
TESTS = pickle_print_test cuid_test

pickle_print_test_SOURCES = pickle_print_test.c
pickle_print_test_LDADD = $(top_builddir)/tvnet/libtvnet.la

cuid_test_SOURCES = cuid_test.c
cuid_test_LDADD = $(top_builddir)/tvnet/libtvnet.la

library_includedir=$(includedir)/tvnet-$(VERSION)/tvnet
library_include_HEADERS =\
	tvnet.h\
	version.h\
	common.h\
	target.h\
	service.h\
	fmt_parser.h\
	pickle_print.h\
	pickle_length.h\
	pickle_encoder.h\
	pickle_decoder.h\
	pickle_stdarg_encoder.h\
	pickle_stdarg_decoder.h\
	cuid.h\
	log_messages.h\
	log.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = tvnet.pc

