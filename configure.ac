#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([ELepHant Reasoner], [0.4.21], [sertkaya.baris@gmail.com])
AM_INIT_AUTOMAKE(ELepHant-reasoner, 0.4.21)
AC_CONFIG_SRCDIR([src/elephant-reasoner.c])
AC_CONFIG_HEADERS([config.h])
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile 
	src/Makefile 
	src/parser/Makefile
	src/model/Makefile
	src/index/Makefile
	src/saturation/Makefile
	src/hierarchy/Makefile
	src/reasoner/Makefile
	src/hashing/Makefile
	src/preprocessing/Makefile
	src/test/Makefile
	src/utils/Makefile])
AC_OUTPUT
