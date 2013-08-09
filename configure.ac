#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# known to work with autconf version: autoconf (GNU Autoconf) 2.69
#
# @configure_input@
#

AC_PREREQ([2.68])
AC_INIT([getdns], [0.320], [melinda.shore@nomountain.net])
AC_CONFIG_SRCDIR([src/common/getdns_general.c])
AM_INIT_AUTOMAKE
LT_INIT
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([event_core], [event_base_new])
AC_CHECK_LIB([getdns], [getdns_context_create])
AC_CHECK_LIB([ldns], [ldns_dname_new_frm_str])

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h netinet/in.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_CONFIG_FILES([Makefile src/Makefile src/common/Makefile src/example/Makefile src/test/Makefile])
AC_CONFIG_SUBDIRS([src/ src/common/ src/example/ src/test/])
AC_OUTPUT