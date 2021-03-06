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

#include "create_config.h"
#include "objets.h"
#include "analyse.h"
#include "ia.h"
#include "stack.h"
#include "arbitre.h"
#include "traffic.h"
#include "distribution.h"
#include "gbridge_srv.h"
#include "file.h"
void debug_info(void);

void print_usage(void);
extern GHashTable *configHash;
int _gethostname(char *buffer, int length);

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
    gboolean bidstatus,diststatus=FALSE;
    color_t couleur;
    position_t position;
    int index;
    hopestat_t **hopestat = NULL;
    init_game(game);
    do {
	if ('e' == read_header(game, game->bid, 'b'))
	    end_session(game);
	if (game->status == 'n'|| game->status == 'f') {
	    clear_game(game);
            if(game->status == 'n') {
	      init_game(game);
	      init_distrib(game);
              diststatus=TRUE;
            }
            else {
              init_cardplayed(game);
              if(file_parse(game))
                diststatus=check_tabjeu(game);
            }
            if(diststatus ) {
	      envoi_jeu(0, game);
              bidstatus=TRUE;
            }
            
	} else {
	    bidstatus = analyse_bid(game);
	    if (game->status == 'b') {
		if('e'==write_data(game, game->cur_bid, 'u'))
                  end_session(game);
            }
	    if (!hopestat &&(game->nbcard == NBPCOULEURS))
		hopestat = analyse_tabjeu(game,NULL);
	}
    } while  (diststatus==FALSE || bidstatus==TRUE);
    game->status = 'g';
    rotation(game, hopestat);
    printf("###################################################\n");
    affiche_tabjeu_c(game->tabjeu);
    printf("###################################################\n");
    newgame(game, hopestat);
    clear_game(game);
    if(hopestat) {
      for (couleur = club; couleur < spade + 1; couleur++) {
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

}



gboolean newgame(game_t * game, hopestat_t ** hopestat)
{
    int random = game->random, i;
    int prof = game->level * 4;
    position_t position;
    color_t couleur;
    choice_color_t *choice_color=malloc(sizeof(choice_color_t));
    memset(choice_color,0,sizeof(choice_color_t));
    int index;
    card_t *best_coup = NULL;
    l_best_t *l_best = NULL;
    int nb_best = 0;
    struct timeval *timeav = malloc(sizeof(struct timeval));
    struct timeval *timeap = malloc(sizeof(struct timeval));
    trick_t *trick;
    int t, notour = 0 ;
    trick = malloc(sizeof(trick_t));
    init_trick(trick, INIT);
    if (game->debug)
	printf("We are in newgame random=%d level=%d\n", random, prof);


    if (game->debug && hopestat) {
	for (couleur = club; couleur < spade + 1; couleur++) {

	    for (position = sud; position < est + 1; position++) {
		index = INDEX(position, couleur);
		printf
		    ("position=%d,color=%d,nbline[IALINE]=%d,nbline[IALINE+1]=%d,aff=%d \t",
		     hopestat[index]->position, hopestat[index]->couleur,
		     hopestat[index]->nbline[IALINE],
		     hopestat[index]->nbline[(IALINE + 1) % 2],
		     hopestat[index]->aff);
		if (hopestat[index]->best_card)
		    affiche_card(hopestat[index]->best_card);
                printf("\n");
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
    trick->entame = (game->contrat->declarant + 1) % 4;
    trick->nextpos = (game->contrat->declarant + 1) % 4;
    trick->atout = game->contrat->atout;
    if(game->nbcard <10)
      prof = game->nbcard  * 4;
    for (notour = 0; notour < game->nbcard; notour++) {
	for (t = 0; t < NBJOUEURS; t++) {
            display_cardplayed(game);
	    if ((trick->nextpos) % 2 == (game->contrat->declarant + 1) % 2) {
                if(game->debug)
		  printf("IA play\n");
		if (NULL == (l_best = malloc(sizeof(l_best_t)))) {
		    fprintf(stderr, "Pb avec malloc\n");
		    exit(EXIT_FAILURE);
		} else {
		    init_list_best(l_best);
		}
		gettimeofday(timeav, NULL);
		if (notour > 5) {
		    prof = (game->nbcard - notour) * 4;
		}
		if ((notour == 155555 && t == 0)&&hopestat && game->nbcard == NBPCOULEURS) {	// On est à l'entame
		    search_best_color(trick->nextpos, hopestat,choice_color);
                    printf("color->toavoid=%d,color->interessant=%d\n",choice_color->toavoid,choice_color->interessant);
		    best_coup = malloc(sizeof(card_t));
		    memcpy(best_coup,
			   hopestat[INDEX(trick->nextpos, choice_color->interessant)]->best_card,
			   sizeof(card_t));
		    gettimeofday(timeap, NULL);
		} else {
		    if (t != 0 && game->tabjeu[INDEX(trick->nextpos, trick->card[trick->entame].clcard)]->nbcrt != 0) {	// On a entame et on a de la couleur d'entame
			if (game->debug)
                          printf("look in analyse_hand\n");
			best_coup =
			    analyse_hand(game, trick,
					 trick->card[trick->entame].clcard);
                        if(!best_coup)
                          printf("ERROR: best_coup empty  coming from analyse_hand!!\n");
 
		    } else {
			first_explore(trick, prof - trick->noj, &nb_best,
					  l_best, game);
			gettimeofday(timeap, NULL);
			if (game->debug)
			    printf("Voici le temps:%d\n",
				   (int) timeap->tv_sec -
				   (int) timeav->tv_sec);
			best_coup =
			    best_choice(&nb_best, l_best, game, NULL,trick);
                        if(!best_coup)
                          printf("ERROR: best_coup empty  coming from best_choice!!\n");
		    }
		    nb_best = 0;
		    clear_list(l_best);
		    free(l_best);
		    l_best = NULL;
		}
                if(best_coup) {
		  trick->lastcard.nocard = best_coup->nocard;
		  trick->lastcard.clcard = best_coup->clcard;
		  if (game->debug || TRUE) {
		    printf("Joue coup trick,best_coup\n");
		    affiche_card(best_coup);
		  }
		  if (joue_coup(trick, best_coup, game) == 0) {
		    free(best_coup);
		    game->transfert->status = WAITING;
		    return (TRUE);
		  }
		  free(best_coup);
                }
                else 
                  printf("ERROR: best_coup empty !!\n");
	    } else {
                if(game->debug)
		  printf("Player  \n");
		if ('e' == read_header(game, trick, 'p'))
		    end_session(game);
		joue_coup(trick, NULL, game);
	    }
	    trick->nextpos = evaluation_trick(trick);

	    if('e'==write_data(game, trick, 'p'))
              end_session(game);
	}
    }
    for (t = 0; t < NBJOUEURS; t++)
	envoi_jeu((game->contrat->declarant + t) % NBJOUEURS, game);
    if (game->debug)
	printf("4 games sent\n");
    free(trick);
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
    fill_config();
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
    (void) memset(&svrname, 0, sizeof(svrname));
    (void) memcpy(&svrname.sin_addr, phost->h_addr, phost->h_length);


    svrname.sin_addr.s_addr = htonl(INADDR_ANY);	// Pour permettre d'ecouter sur toutes les adresses

    svrname.sin_family = AF_INET;
    svrname.sin_port = htons(port);	/* network-order */

    status =
	bind(socksrv_id, (struct sockaddr *) &svrname, sizeof(svrname));
    if (-1 == status) {
	perror("bind()");
	exit(1);
    }
        status=listen(socksrv_id,5);
	if (-1 == status) {
	    perror("listen()");
            printf("errno=%d",errno);
	    exit(1);
	}
    while (1) {
	struct sockaddr_in cltname;
	socklen_t clientlength = sizeof(cltname);

	(void) memset(&cltname, 0, sizeof(cltname));

	game->sockslv_id = accept(socksrv_id,
				  (struct sockaddr *) &cltname,
				  &clientlength);
	if (-1 == game->sockslv_id) {
           if(EINTR!=errno) { 
	    game->sockslv_id = accept(socksrv_id,
				  (struct sockaddr *) &cltname,
				  &clientlength);
	    if (-1 == game->sockslv_id) {
	      perror("accept()");
              exit(1);
            }
          }
          else {
            printf ("The errno value is EINTR\n");
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

