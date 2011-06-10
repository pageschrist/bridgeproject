GETTEXT_PACKAGE = gbridge
all:

	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c traffic.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c create_config.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c draw.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c alloc.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c client.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c load.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c callbacks.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c objets.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c gbridge_cli.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c ihmbridge.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c jeu.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c free.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c find.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags` -g -Wall -W -c arbitre.c
	cc `pkg-config glib-2.0 gtk+-2.0 --cflags --libs` -g -Wall -W -o gbridge_cli traffic.o gbridge_cli.o load.o find.o  free.o create_config.o callbacks.o objets.o arbitre.o  ihmbridge.o client.o jeu.o draw.o alloc.o

clean:

	rm -f *.o gbridge_cli
