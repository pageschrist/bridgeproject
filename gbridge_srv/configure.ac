#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(gbridge_srv, 0.9, pageschrist@gmail.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/gbridge_srv.c])
AC_CONFIG_HEADERS([config.h])
AM_PATH_GLIB_2_0(2.6.0,[ ])
AM_PATH_GTK_2_0(2.6.0,[ ])
# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([bzero gethostbyname gettimeofday inet_ntoa memset socket strchr strdup uname])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	config/Makefile
	])
AC_OUTPUT
