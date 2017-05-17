#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([fast5], [0.01], [bobmittmann@gmail.com])
AC_CONFIG_SRCDIR([src/fast5.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(hdf5, H5Fopen)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([strerror strrchr strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
