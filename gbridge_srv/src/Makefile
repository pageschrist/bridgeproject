all:

	#cc `pkg-config glib-2.0 --cflags` -g -c server.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c traffic.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c create_config.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c distribution.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c objets.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c gbridge_srv.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c analyse.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c ia.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c stack.c
	cc `pkg-config glib-2.0 --cflags` -g -W -Wall -c arbitre.c
	cc `pkg-config glib-2.0 --cflags --libs` -W -Wall -g -o gbridge_srv traffic.o gbridge_srv.o ia.o stack.o arbitre.o create_config.o objets.o distribution.o analyse.o

clean:

	rm -f *.o gbridge_srv
