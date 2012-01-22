ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
AM_MAKEFLAGS = --no-print-directory
AM_CFLAGS = \
	-Wall \
        -Wmissing-declarations \
        -Wmissing-prototypes \
        -Wnested-externs \
        -Wpointer-arith \
        -Wpointer-arith \
        -Wsign-compare \
        -Wchar-subscripts \
        -Wstrict-prototypes \
        -Wshadow \
        -Wformat-security \
        -Wtype-limits \
        -ffunction-sections \
        -fdata-sections
AM_LDFLAGS = \
	-Wl,--gc-sections \
	-Wl,--as-needed
EXTRA_DIST = src/libsc.sym

include_HEADERS = libsc.h
lib_LTLIBRARIES = libsc.la

libsc_la_SOURCES = libsc.c
libsc_la_LDFLAGS = $(AM_LDFLAGS) -version-info 0:0:0 -Wl,--version-script=$(top_srcdir)/libsc.sym

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsc.pc