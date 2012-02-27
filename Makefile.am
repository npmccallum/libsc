ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
SUBDIRS = . tests

AM_MAKEFLAGS = --no-print-directory
AM_CFLAGS = -g \
            -Wall \
            -Wmissing-declarations \
            -Wmissing-prototypes \
            -Wnested-externs \
            -Wpointer-arith \
            -Wsign-compare \
            -Wchar-subscripts \
            -Wstrict-prototypes \
            -Wshadow \
            -Wformat-security \
            -ffunction-sections \
            -fdata-sections
EXTRA_DIST = src/libsc.sym

include_HEADERS = libsc.h
lib_LTLIBRARIES = libsc.la

libsc_la_SOURCES = libsc.c
libsc_la_LDFLAGS = -version-info 0:0:0 -export-symbols-regex '^_?sc_'

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsc.pc