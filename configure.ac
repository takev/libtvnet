
AC_PREREQ(2.61)
AC_INIT(tvnet, 0.4, take.vos@vosgames.nl)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([buildutils])
AM_INIT_AUTOMAKE()

dnl    Here are a set of rules to help you update your library version information:
dnl
dnl    * Start with version information of `0:0:0' for each libtool library.
dnl    * Update the version information only immediately before a public release of your software.
dnl      More frequent updates are unnecessary, and only guarantee that the current interface
dnl      number gets larger faster.
dnl    * If the library source code has changed at all since the last update, then increment revision
dnl      (`c:r:a' becomes `c:r+1:a').
dnl    * If any interfaces have been added, removed, or changed since the last update, increment current,
dnl      and set revision to 0.
dnl    * If any interfaces have been added since the last public release, then increment age.
dnl    * If any interfaces have been removed since the last public release, then set age to 0.
SHARED_VERSION_INFO=1:0:0
AC_SUBST(SHARED_VERSION_INFO)

AC_PROG_CC([clang gcc cc])
AC_PROG_INSTALL
AC_PROG_LIBTOOL

DX_DOT_FEATURE(OFF)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(tvnet, doxygen.cfg)


AX_PKG_SWIG(1.3.17, [], [ AC_MSG_ERROR([SWIG is required to build..]) ])
dnl AX_SWIG_PYTHON
AX_PYTHON_DEVEL

AM_PATH_PYTHON

PKG_CHECK_MODULES([DEPS], [tvutils >= 0.4])

AC_CHECK_HEADER(
    [linux/tipc.h],
    AC_SUBST([TVN_HAVE_TIPC], [1]),
    AC_SUBST([TVN_HAVE_TIPC], [0])
)

AC_CONFIG_HEADERS([tvnet/config.h])
AC_CONFIG_FILES([Makefile tvnet/Makefile tvnet/tvnet.pc tvnet/version.h tvnet/target.h  pyext/Makefile tvlogd/Makefile tvlog/Makefile])
AC_OUTPUT

