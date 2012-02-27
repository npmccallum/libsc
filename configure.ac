dnl Initialize autoconf
AC_PREREQ(2.60)
AC_INIT([libsc], [0.2], [nathaniel@themccallums.org], [libsc],
        [https://github.com/npmccallum/libsc])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])
AC_PROG_CC_STDC
AC_PROG_LIBTOOL

dnl Initialize automake
AM_INIT_AUTOMAKE([check-news foreign 1.10 silent-rules subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_PROG_CC_C_O

dnl Initialize libtool
LT_INIT

dnl Output files
AC_CONFIG_FILES([Makefile tests/Makefile libsc.pc])
AC_OUTPUT

dnl Print details
AC_MSG_RESULT([
        $PACKAGE $VERSION
        =====

        prefix:                 ${prefix}
        sysconfdir:             ${sysconfdir}
        libdir:                 ${libdir}
        includedir:             ${includedir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
        ldflags:                ${LDFLAGS}
])
