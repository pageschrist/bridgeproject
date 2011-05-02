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
#include <unistd.h>		/* fork(), write(), close() */
#include <glib.h>		/* For Glib functions */
#include <glib/gtypes.h>		/* For Glib functions */

#include "create_config.h"
#include "objets.h"
#include "ia.h"
#include "arbitre.h"
#include "analyse.h"
#include "distribution.h"
#include "gbridge_srv.h"
void debug_info(void);

void print_usage (void);
extern int flag_debug;
extern GHashTable *configHash;
extern carte_t best_carte;
/*
 * prototypes
 */
int _gethostname (char *buffer, int length);

/*
 * constants
 */
void debug_info(void) {
 printf("Debug\n");
}
void print_usage (void) {
  printf("usage gbride_srv -r random\n");
  printf("\t-p profondeur\n");

}



const int BACK_LOG = 5;

void signal_handler ( void)
{
   printf("Signal trappé\n");
  while(0<waitpid(-1,NULL,WNOHANG));
}


gboolean newgame(game_t *game) {
  int random=game->transfert->random, i;
  int prof=game->transfert->level*4;
  position_t rotation;
  carte_t *best_coup=NULL;
  l_best_t *l_best=NULL;
  int nb_best=0;
  struct timeval *timeav=malloc(sizeof(struct timeval));
  struct timeval *timeap=malloc(sizeof(struct timeval));
  pli_t *pli;
  int t,notour=0;
  pli = malloc (sizeof (pli_t));
  init_pli(pli,INIT);
  printf("On est dans newgame random=%d level=%d\n",random,prof);

  //rotation=init_distrib (game,random,pli);
  rotation=0;
  printf("Voici la rotation%d\n",rotation);

  /*envoi_jeu (game, sockslv_id);
  do {
    read(sockslv_id,transfert,sizeof(transfert_t));
    if(transfert->status==BID) {
      read(sockslv_id,bid,sizeof(bid_t));
      
    
    } 

  } while(transfert->status==BID); */
  game->contrat->declarant=sud;
  for(i=strlen(game->cur_bid)-1;i>0;i=i-2) {

    if(game->cur_bid[i-1]!='P') {
      game->contrat->atout=game->cur_bid[i];
      break;

    }
  }
  envoi_jeu ((game->contrat->declarant + 2) % NBJOUEURS, game);

  // On positionne le premier pli à 
  printf ("Voici le declarant:%d",game->contrat->declarant);
  pli->entame=(game->contrat->declarant+1)%4;
  pli->nextpos=(game->contrat->declarant+1)%4;
  pli->atout= char_to_int(&game->contrat->atout);
  printf("On joue atout:%d\n",pli->atout);
  //Debut de la partie ia
          
  for (notour=0;notour<13;notour++) {
    for(t=0;t<NBJOUEURS;t++) {
      printf("Voici le tour %d ,nojoueur %d\n",notour ,t);
      if((pli->nextpos)%2 == (game->contrat->declarant+1)%2) {
      printf("IA joue\n");
        if(NULL==(l_best=malloc(sizeof(l_best_t)))) {
          fprintf(stderr,"Pb avec malloc\n");
          exit (EXIT_FAILURE);
        }
        else {
          init_list_best(l_best);
        }
        gettimeofday(timeav,NULL);
        if(notour>=5)
          prof=28;
	first_explore ( pli, prof-pli->noj,&nb_best,l_best,game);
        gettimeofday(timeap,NULL);
        printf("Voici le temps:%d\n", (int)  timeap->tv_sec-(int) timeav->tv_sec);
        best_coup=choix_best(&nb_best,l_best,game);
        nb_best=0;
        clear_list(l_best);
        free(l_best);
        l_best=NULL;
        pli->lastcarte.nocarte=best_coup->nocarte;
        pli->lastcarte.clcarte=best_coup->clcarte;
        printf("Joue coup pli,best_coup\n");
        affiche_carte(best_coup);
	if(joue_coup( pli,best_coup,game)==0) 
          { 
           free(best_coup);
           game->transfert->status =WAITING;
           return ( TRUE);
        }
        free(best_coup);
      }
      else {
        printf("Joueur  joue\n");
        read(game->sockslv_id,game->transfert,sizeof(transfert_t));
        if(game->transfert->status==PLI )
           read (game->sockslv_id,   pli, sizeof (pli_t));
        else
         return(game->transfert);
        printf("Joue coup pli,NULL\n");
	joue_coup( pli,NULL,game); 
      }
      pli->nextpos=evaluation_pli(pli);

      write (game->sockslv_id,  pli, sizeof (pli_t));
    }
  }
  for (t=0;t<NBJOUEURS;t++) 
    envoi_jeu ((game->contrat->declarant + t) % NBJOUEURS, game);
  printf("4 jeux envoyé\n");
  game->transfert->status=WAITING;
  free(pli);
  return (TRUE);
}

int main (int argc, char *argv[])
{
  struct sigaction sa;
  game_t *game=malloc(sizeof(game_t));
  memset(game,0,sizeof(game_t));
  int socksrv_id = 0, on = 0, port, status = 0, childpid = 0;
  transfert_t *transfert=NULL;
  int c,random,prof;
  struct hostent *phost = NULL;
  char hostname[80] = "";
  //struct sockaddr_in svrname = { 0 };
  struct sockaddr_in svrname ;
  transfert=malloc(sizeof(transfert_t));
  create_config ();
  rempli_config ();
  verifie_config ();
  random = atoi (g_hash_table_lookup (configHash, "random"));
  prof = atoi (g_hash_table_lookup (configHash, "profondeur"));
  while (1) { 
    c = getopt(argc, argv, "p:r:");
    if (c<0) 
      break;


    switch( c) {
      case 'p' :
        prof=atoi(optarg);
        prof= prof-(prof%4);
        printf("Voici profondeur=%d\n",prof);
        break;
      case 'r' :
        random=atoi(optarg);
        printf("Voici random=%d\n",random);
        break;
      default:
        print_usage();
        exit(1);
    }
  }
  memset (&sa,0,sizeof (struct sigaction));
  sa.sa_handler = signal_handler;
  sa.sa_flags =0;
  sigemptyset ( &(sa.sa_mask));
  if (sigaction (SIGCHLD,&sa,NULL )!= 0 ) {
    fprintf (stderr, "Problème avec sigaction()\n");
    exit (EXIT_FAILURE);
  }

  port = atoi (g_hash_table_lookup (configHash, "port"));
  prof= prof-(prof%4);
  printf("Voici random=%d\n",random);
  printf("Voici profondeur=%d\n",prof);

  socksrv_id = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == socksrv_id)
    {
      perror ("socket()");
      exit (1);
    }

  /*
   * turn off bind address checking, and allow port numbers
   * to be reused - otherwise the TIME_WAIT phenomenon will
   * prevent binding to these address.port combinations for
   * (2 * MSL) seconds.
   */

  on = 1;

  status = setsockopt (socksrv_id, SOL_SOCKET, SO_REUSEADDR,
		       (const char *) &on, sizeof (on));

  if (-1 == status)
    {
      perror ("setsockopt(...,SO_REUSEADDR,...)");
    }

  /*
   * when connection is closed, there is a need to linger to ensure
   * all data is transmitted, so turn this on also
   */
  {
    struct linger linger = { 0, 0 };

    linger.l_onoff = 1;
    linger.l_linger = 30;
    status = setsockopt (socksrv_id, SOL_SOCKET, SO_LINGER,
			 (const char *) &linger, sizeof (linger));

    if (-1 == status)
      {
	perror ("setsockopt(...,SO_LINGER,...)");
      }
  }

  /*
   * find out who I am
   */

  status = _gethostname (hostname, sizeof (hostname));
  if (-1 == status)
    {
      perror ("_gethostname()");
      exit (1);
    }

  phost = gethostbyname (hostname);
  if (NULL == phost)
    {
      perror ("gethostbyname()");
      exit (1);
    }
  printf ("Voici l'addresse%s", phost->h_addr);
  (void) memset (&svrname, 0, sizeof (svrname));
  (void) memcpy (&svrname.sin_addr, phost->h_addr, phost->h_length);

  
  svrname.sin_addr.s_addr = htonl(INADDR_ANY); // Pour permettre d'ecouter sur toutes les adresses

  svrname.sin_family = AF_INET;
  svrname.sin_port = htons (port);	/* network-order */

  status = bind (socksrv_id, (struct sockaddr *) &svrname, sizeof (svrname));
  printf ("We are in bind\n");
  if (-1 == status)
    {
      perror ("bind()");
      exit (1);
    }

  while(1) 
    {
    status = listen (socksrv_id, BACK_LOG);
    printf("On vient de passer listen\n");
    if (-1 == status)
    {
      perror ("listen()");
      exit (1);
    }

      //struct sockaddr_in cltname = { 0 };
      struct sockaddr_in cltname; 
       socklen_t clientlength = sizeof (cltname);
      
      (void) memset (&cltname, 0, sizeof (cltname));

      game->sockslv_id = accept (socksrv_id,
			   (struct sockaddr *) &cltname, &clientlength);
      if (-1 == game->sockslv_id)
	{
	  perror ("accept()");
	  exit (1);
	}

      childpid = fork ();

      switch (childpid)
	{
	case -1:		/* ERROR */
	  perror ("fork()");
	  exit (1);

	case 0:		/* child process */

	  close (socksrv_id);

	  if (-1 == getpeername (game->sockslv_id,
				 (struct sockaddr *) &cltname, &clientlength))
	    {
	      perror ("getpeername()");
	    }
	  else
	    {
	      printf ("Connection request from %s\n",
		      inet_ntoa (cltname.sin_addr));
	    }
	  printf ("Clear, Init\n");
          //clear_game(game);
	  printf ("Clear, Init 2\n");
          init_game(game);
	  printf ("Clear, Init 3\n");
          init_distrib(game,random);
	  printf ("Envoi\n");
	  envoi_jeu (0, game);
	  printf ("Envoi apres\n");
          game->transfert=malloc(sizeof(transfert_t));
          read(game->sockslv_id,game->transfert,sizeof(transfert_t));
          do {
              while(game->transfert->status==BID) {
                    read ( game->sockslv_id, game->bid ,sizeof(bid_t));
                    analyse_bid(game); 
                    write ( game->sockslv_id, game->cur_bid ,sizeof(game->cur_bid));
                    read(game->sockslv_id,game->transfert,sizeof(transfert_t));
              }
              if (game->transfert->status == NEWGAME){ 
                  transfert=newgame (game);
                  read(game->sockslv_id,game->transfert,sizeof(transfert_t));
                  printf("transfert->random\n%d",game->transfert->random);
                  printf("transfert->level\n%d",game->transfert->level);
                  printf("transfert->status\n%d",game->transfert->status);
              }
              if (game->transfert->status == NEWDIST){ 
                  clear_game(game);
                  init_game(game);
                  printf("transfert->random\n%d",game->transfert->random);
                  init_distrib(game,game->transfert->random);
	          envoi_jeu (0, game);
                  read(game->sockslv_id,game->transfert,sizeof(transfert_t));
              }
 
          } while (game->transfert->status!=GAMEOVER);
            
            
	  close (game->sockslv_id);

	default:		/* parent process */
          printf("Parent Process\n");
	  close (game->sockslv_id);
	}
    }
        clear_game(game);
        free(game->transfert);
        free(game);
        exit(0);
}

/*
 * Local replacement of gethostname() to aid portability
 */
int
_gethostname (char *buffer, int length)
{
  struct utsname sysname ;
  int status = 0;

  status = uname (&sysname);
  if (-1 != status)
    {
      strncpy (buffer, sysname.nodename, length);
    }

  return (status);
}
