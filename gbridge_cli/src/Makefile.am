
bin_PROGRAMS = gbridge_cli


gbridge_cli_SOURCES = \
		alloc.c alloc.h \
                callbacks.c callbacks.h \
		client.c client.h \
 		create_config.c create_config.h \
		draw.c draw.h \
		file.c file.h \
		find.c find.h \
		free.c free.h \
		gbridge_cli.c gbridge_cli.h \
		ihmbridge.c ihmbridge.h \
		jeu.c jeu.h \
		load.c load.h \
		objets.c objets.h \
		outils.c outils.h \
		traffic.c traffic.h 
		


gbridge_cli_CPPFLAGS = ${GLIB_CFLAGS} ${GTK_CFLAGS}
gbridge_cli_LDADD =-lm  ${GLIB_LIBS} ${GTK_LIBS}
