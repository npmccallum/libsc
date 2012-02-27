dnl Initialize autoconf
AC_PREREQ(2.60)
AC_INIT([libsc], [0.2], [nathaniel@themccallums.org], [libsc],
        [https://github.com/npmccallum/libsc])
AC_CONFIG_AUX_DIR([build-aux])
AC_CANONICAL_SYSTEM
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])
AC_PROG_CC_STDC
AC_PROG_LIBTOOL

dnl Initialize automake
AM_INIT_AUTOMAKE([check-news foreign 1.10 -Wall -Wno-portability silent-rules subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AM_PROG_CC_C_O 

dnl Initialize libtool
LT_INIT([disable-static pic-only])

dnl Enable debug mode
AC_ARG_ENABLE([debug],
        AS_HELP_STRING([--enable-debug], [enable debug messages @<:@default=disabled@:>@]),
        [], [enable_debug=no])
AS_IF([test "x$enable_debug" = "xyes"], [
        AC_DEFINE(ENABLE_DEBUG, [1], [Debug messages.])
])

my_CFLAGS="-Wall \
-Wmissing-declarations -Wmissing-prototypes \
-Wnested-externs -Wpointer-arith \
-Wpointer-arith -Wsign-compare -Wchar-subscripts \
-Wstrict-prototypes -Wshadow \
-Wformat-security -Wtype-limits"
AC_SUBST([my_CFLAGS])

AC_CONFIG_FILES([Makefile tests/Makefile libsc.pc])

AC_OUTPUT
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

        debug:                  ${enable_debug}
])
