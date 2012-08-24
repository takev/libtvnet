
DEFAULT_INCLUDES=-I$(top_srcdir)

AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=gnu99 $(DEPS_CFLAGS) $(DEFAULT_INCLUDES)

#dist_man3_MANS = tvlog_init.3 tvlog_time.3 tvlog_tipc_node.3 tvlog_tipc_cuid.3

sbin_PROGRAMS = tvlogd

tvlogd_SOURCES = main.c main.h loop.c loop.h
tvlogd_LDADD = $(DEPS_LIBS) $(top_builddir)/tvnet/libtvnet.la



