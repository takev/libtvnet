ACLOCAL_AMFLAGS = -I m4
SUBDIRS = tvnet pyext tvlogd tvlog

include am/doxygen.am

MOSTLYCLEANFILES = $(DX_CLEANFILES)
EXTRA_DIST = autogen.sh $(DX_CONFIG)

doc: doxygen-doc

clean: clean-recursive doxygen-clean

doxygen-clean:
	@rm -f $(DX_CLEANFILES)
