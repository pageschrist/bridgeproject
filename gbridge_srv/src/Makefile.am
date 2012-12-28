
bin_PROGRAMS = gbridge_srv


gbridge_srv_SOURCES = \
		analyse.c analyse.h \
		create_config.c create_config.h \
		file.c file.h \
		ia.c ia.h \
		stack.c stack.h \
		traffic.c traffic.h \
		objets.c objets.h \
		gbridge_srv.c gbridge_srv.h \
		distribution.c distribution.h \
		arbitre.c arbitre.h

gbridge_srv_CPPFLAGS = ${GLIB_CFLAGS} ${GTK_CFLAGS} -DDATADIR="\"@datadir@/${PACKAGE_NAME}\""
gbridge_srv_LDADD =-lm -lpthread ${GLIB_LIBS} ${GTK_LIBS}
