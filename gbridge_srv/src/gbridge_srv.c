#include <stdio.h>		/* */
#include <stdlib.h>		/* exit() */
#include <string.h>		/* memset(), memcpy() */
#include <sys/utsname.h>	/* uname() */
#include <sys/types.h>
#include <sys/socket.h>		/* socket(), bind(), listen(),
				   accept() */
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <mcheck.h>
#include <unistd.h>		/* fork(), write(), close() */
#include <glib.h>		/* For Glib functions */
#include <glib/gtypes.h>	/* For Glib functions */

#include "create_config.h"
#include "objets.h"
#include "analyse.h"
#include "ia.h"
#include "arbitre.h"
#include "traffic.h"
#include "distribution.h"
#include "gbridge_srv.h"
void debug_info(void);

void print_usage(void);
extern GHashTable *configHash;
/*
 * prototypes
 */
int _gethostname(char *buffer, int length);

/*
 * constants
 */
void debug_info(void)
{
    printf("Debug\n");
}
void print_usage(void)
{
    printf("usage gbride_srv -r random\n");
    printf("\t-p profondeur\n");

}



void end_session(game_t * game)
{


    fprintf(stdout, "End of the Session\n");
    clear_game(game);
    shutdown(game->sockslv_id, SHUT_RDWR);
    close(game->sockslv_id);
    exit(0);
}

void signal_handler(int sig)
{
    sig = sig;
    printf("Signal trappé\n");
    while (0 < waitpid(-1, NULL, WNOHANG));
}

void main_game(game_t * game)
{
    gboolean status;
    couleur_t couleur;
    position_t position;
    int index;
    hopestat_t **hopestat = NULL;
    init_game(game);
    do {
	if ('e' == read_header(game, game->bid, 'b'))
	    end_session(game);
	if (game->status == 'n') {
	    clear_game(game);
	    init_game(game);
	    init_distrib(game);
	    envoi_jeu(0, game);
	    status = 1;
	} else {
	    status = analyse_bid(game);
	    if (game->status == 'b') {
		if('e'==write_data(game, game->cur_bid, 'u'))
                  end_session(game);
            }
	    if (!hopestat)
		hopestat = analyse_tabjeu(game);
	}
    } while (status);
    game->status = 'g';
    rotation(game, hopestat);
    newgame(game, hopestat);
    clear_game(game);
    for (couleur = trefle; couleur < pique + 1; couleur++) {
	for (position = sud; position < est + 1; position++) {
	    index = INDEX(position, couleur);
	    if (hopestat[index]->best_card)
		free(hopestat[index]->best_card);
	    free(hopestat[index]);
	}
    }
    free(hopestat);
    hopestat = NULL;

}



gboolean newgame(game_t * game, hopestat_t ** hopestat)
{
    int random = game->random, i;
    int prof = game->level * 4;
    position_t position;
    couleur_t couleur;
    int index;
    carte_t *best_coup = NULL;
    l_best_t *l_best = NULL;
    int nb_best = 0;
    struct timeval *timeav = malloc(sizeof(struct timeval));
    struct timeval *timeap = malloc(sizeof(struct timeval));
    pli_t *pli;
    int t, notour = 0, n;
    pli = malloc(sizeof(pli_t));
    init_pli(pli, INIT);
    if (game->debug)
	printf("On est dans newgame random=%d level=%d\n", random, prof);


    if (game->debug) {
	for (couleur = trefle; couleur < pique + 1; couleur++) {

	    for (position = sud; position < est + 1; position++) {
		index = INDEX(position, couleur);
		printf
		    ("position=%d,color=%d,nbline[IALINE]=%d,nbline[IALINE+1]=%d\n",
		     hopestat[index]->position, hopestat[index]->couleur,
		     hopestat[index]->nbline[IALINE],
		     hopestat[index]->nbline[(IALINE + 1) % 2]);
		if (hopestat[index]->best_card)
		    affiche_carte(hopestat[index]->best_card);
	    }
	}

    }

    game->contrat->declarant = sud;
    for (i = strlen(game->cur_bid) - 1; i > 0; i = i - 2) {

	if (game->cur_bid[i - 1] != 'P') {
	    game->contrat->atout = char_to_int(&game->cur_bid[i]);
	    break;

	}
    }
    envoi_jeu((game->contrat->declarant) % NBJOUEURS, game);
    envoi_jeu((game->contrat->declarant + 2) % NBJOUEURS, game);
    couleur_t color;
    pli->entame = (game->contrat->declarant + 1) % 4;
    pli->nextpos = (game->contrat->declarant + 1) % 4;
    pli->atout = game->contrat->atout;

    for (notour = 0; notour < 13; notour++) {
	for (t = 0; t < NBJOUEURS; t++) {
	    printf("Voici le tour %d ,nojoueur %d\n", notour, t);
	    if ((pli->nextpos) % 2 == (game->contrat->declarant + 1) % 2) {
		printf("IA play\n");
		if (NULL == (l_best = malloc(sizeof(l_best_t)))) {
		    fprintf(stderr, "Pb avec malloc\n");
		    exit(EXIT_FAILURE);
		} else {
		    init_list_best(l_best);
		}
		gettimeofday(timeav, NULL);
		if (notour > 5) {
		    prof = (13 - notour) * 4;
		}
		if (notour == 0 && t == 0) {	// On est à l'entame
		    color = search_best_color(pli->nextpos, hopestat);
		    best_coup = malloc(sizeof(carte_t));
		    memcpy(best_coup,
			   hopestat[INDEX(pli->nextpos, color)]->best_card,
			   sizeof(carte_t));
		    gettimeofday(timeap, NULL);
		} else {
		    if (t != 0 && game->tabjeu[INDEX(pli->nextpos, pli->carte[pli->entame].clcarte)]->nbcrt != 0) {	// On a entame et on a de la couleur d'entame
			best_coup =
			    analyse_hand(game, pli,
					 pli->carte[pli->entame].clcarte);
		    } else {
			n = first_explore(pli, prof - pli->noj, &nb_best,
					  l_best, game);
			gettimeofday(timeap, NULL);
			if (game->debug)
			    printf("Voici le temps:%d\n",
				   (int) timeap->tv_sec -
				   (int) timeav->tv_sec);
			best_coup =
			    choix_best(&nb_best, l_best, game, NULL);
		    }
		    nb_best = 0;
		    clear_list(l_best);
		    free(l_best);
		    l_best = NULL;
		}
		pli->lastcarte.nocarte = best_coup->nocarte;
		pli->lastcarte.clcarte = best_coup->clcarte;
		if (game->debug) {
		    printf("Joue coup pli,best_coup\n");
		    affiche_carte(best_coup);
		}
		if (joue_coup(pli, best_coup, game) == 0) {
		    free(best_coup);
		    game->transfert->status = WAITING;
		    return (TRUE);
		}
		free(best_coup);
	    } else {

		printf("Player  \n");
		if ('e' == read_header(game, pli, 'p'))
		    end_session(game);
		joue_coup(pli, NULL, game);
	    }
	    pli->nextpos = evaluation_pli(pli);

	    if('e'==write_data(game, pli, 'p'))
              end_session(game);
	}
    }
    for (t = 0; t < NBJOUEURS; t++)
	envoi_jeu((game->contrat->declarant + t) % NBJOUEURS, game);
    if (game->debug)
	printf("4 games sent\n");
    free(pli);
    return (TRUE);
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    mtrace();
    game_t *game = malloc(sizeof(game_t));
    memset(game, 0, sizeof(game_t));
    int socksrv_id = 0, on = 0, port, status = 0, childpid = 0;
    int c, random, prof;
    struct hostent *phost = NULL;
    char hostname[80] = "";
    //struct sockaddr_in svrname = { 0 };
    struct sockaddr_in svrname;
    game->level = DEFAULTLEVEL;
    game->random = DEFAULTRANDOM;
    create_config();
    rempli_config();
    verifie_config();
    random = atoi(g_hash_table_lookup(configHash, "random"));
    prof = atoi(g_hash_table_lookup(configHash, "profondeur"));
    while (1) {
	c = getopt(argc, argv, "p:r:");
	if (c < 0)
	    break;


	switch (c) {
	case 'p':
	    prof = atoi(optarg);
	    prof = prof - (prof % 4);
	    printf("depth=%d\n", prof);
	    break;
	case 'r':
	    random = atoi(optarg);
	    printf("random=%d\n", random);
	    break;
	default:
	    print_usage();
	    exit(1);
	}
    }
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&(sa.sa_mask));
    if (sigaction(SIGCHLD, &sa, NULL) != 0) {
    	fprintf(stderr, "Problem with sigaction()\n");
    	exit(EXIT_FAILURE);
        }

    port = atoi(g_hash_table_lookup(configHash, "port"));
    prof = prof - (prof % 4);

    socksrv_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == socksrv_id) {
	perror("socket()");
	exit(1);
    }

    /*
     * turn off bind address checking, and allow port numbers
     * to be reused - otherwise the TIME_WAIT phenomenon will
     * prevent binding to these address.port combinations for
     * (2 * MSL) seconds.
     */

    on = 1;

    status = setsockopt(socksrv_id, SOL_SOCKET, SO_REUSEADDR,
			(const char *) &on, sizeof(on));

    if (-1 == status) {
	perror("setsockopt(...,SO_REUSEADDR,...)");
    }

    /*
     * when connection is closed, there is a need to linger to ensure
     * all data is transmitted, so turn this on also
     */
    {
	struct linger linger = { 0, 0 };

	linger.l_onoff = 1;
	linger.l_linger = 30;
	status = setsockopt(socksrv_id, SOL_SOCKET, SO_LINGER,
			    (const char *) &linger, sizeof(linger));

	if (-1 == status) {
	    perror("setsockopt(...,SO_LINGER,...)");
	}
    }

    /*
     * find out who I am
     */

    status = _gethostname(hostname, sizeof(hostname));
    if (-1 == status) {
	perror("_gethostname()");
	exit(1);
    }

    phost = gethostbyname(hostname);
    if (NULL == phost) {
	perror("gethostbyname()");
	exit(1);
    }
    printf("Voici l'addresse%s", phost->h_addr);
    (void) memset(&svrname, 0, sizeof(svrname));
    (void) memcpy(&svrname.sin_addr, phost->h_addr, phost->h_length);


    svrname.sin_addr.s_addr = htonl(INADDR_ANY);	// Pour permettre d'ecouter sur toutes les adresses

    svrname.sin_family = AF_INET;
    svrname.sin_port = htons(port);	/* network-order */

    status =
	bind(socksrv_id, (struct sockaddr *) &svrname, sizeof(svrname));
    printf("We are in bind\n");
    if (-1 == status) {
	perror("bind()");
	exit(1);
    }
        status=listen(socksrv_id,5);
	printf("On vient de passer listen\n");
	if (-1 == status) {
	    perror("listen()");
            printf("errno=%d",errno);
	    exit(1);
	}
    while (1) {
	//struct sockaddr_in cltname = { 0 };
	struct sockaddr_in cltname;
	socklen_t clientlength = sizeof(cltname);

	(void) memset(&cltname, 0, sizeof(cltname));

	game->sockslv_id = accept(socksrv_id,
				  (struct sockaddr *) &cltname,
				  &clientlength);
	if (-1 == game->sockslv_id) {
        
	    game->sockslv_id = accept(socksrv_id,
				  (struct sockaddr *) &cltname,
				  &clientlength);
	    if (-1 == game->sockslv_id) {
	      perror("accept()");
              exit(1);
            }
	}

	childpid = fork();

	switch (childpid) {
	case -1:		/* ERROR */
	    perror("fork()");
	    exit(1);

	case 0:		/* child process */
	    do {
		main_game(game);
	    } while (game->status != 'e');
	    close(socksrv_id);

	    if (-1 == getpeername(game->sockslv_id,
				  (struct sockaddr *) &cltname,
				  &clientlength)) {
		perror("getpeername()");
	    } else {
		printf("Connection request from %s\n",
		       inet_ntoa(cltname.sin_addr));
	    }


            clear_game(game);
            free(game);
	    close(game->sockslv_id);

	default:		/* parent process */
	    printf("Parent Process\n");
	    close(game->sockslv_id);
	}
    }
    exit(0);
}

/*
 * Local replacement of gethostname() to aid portability
 */
int _gethostname(char *buffer, int length)
{
    struct utsname sysname;
    int status = 0;

    status = uname(&sysname);
    if (-1 != status) {
	strncpy(buffer, sysname.nodename, length);
    }

    return (status);
}
