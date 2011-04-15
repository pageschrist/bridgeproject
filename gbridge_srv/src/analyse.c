#include <stdio.h>
#include <string.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "distribution.h"
#include "analyse.h"
#include "create_config.h"
position_t ligneia;

int small_condition(game_t *game,char *s_smallref) {

  int i,index,meml=0,memr=0,testcond=0,sum=0;
  couleur_t couleur;
  char *ptr;
  char *s_sec;
  char *s_small=malloc(sizeof(char)*(strlen(s_smallref)+1));
  memset (s_small,0,sizeof(char)*(strlen(s_smallref)+1));
  strncpy(s_small,s_smallref,strlen(s_smallref));
  printf("voici s_small:%s\n",s_small);
  switch (s_smallref[0]) {
    case 'p':
    index=INDEX(game->bid->position,pique);
    meml=game->tabjeuref[index]->nbcrt;
    break;
    case 'c':
    index=INDEX(game->bid->position,coeur);
    meml=game->tabjeuref[index]->nbcrt;
    break;
    case 'k':
    index=INDEX(game->bid->position,carreau);
    meml=game->tabjeuref[index]->nbcrt;
    break;
    case 't':
    index=INDEX(game->bid->position,trefle);
    meml=game->tabjeuref[index]->nbcrt;
    break;
    case 'h':
    for (couleur=trefle;couleur<pique+1;couleur++) {
      index=INDEX(game->bid->position,couleur);
      for (i=0;i<game->tabjeuref[index]->nbcrt;i++) {
        if(game->tabjeuref[index]->tabcoul[i]==cA)
          meml=meml+4;
        if(game->tabjeuref[index]->tabcoul[i]==cR)
          meml=meml+3;
        if(game->tabjeuref[index]->tabcoul[i]==cD)
          meml=meml+2;
        if(game->tabjeuref[index]->tabcoul[i]==cV)
          meml=meml+1;
      }
    }
    break;
  }
  if(s_smallref[2]<'A') {

    s_sec=strtok_r(s_small,&s_small[1],&ptr);
    s_sec=strtok_r(NULL,&s_small[1],&ptr);
    memr=atoi(s_sec);
  }
  else {
    switch (s_smallref[2]) {
      case 'p':
      index=INDEX(game->bid->position,pique);
      memr=game->tabjeuref[index]->nbcrt;
      break;
      case 'c':
      index=INDEX(game->bid->position,coeur);
      memr=game->tabjeuref[index]->nbcrt;
      break;
      case 'k':
      index=INDEX(game->bid->position,carreau);
      memr=game->tabjeuref[index]->nbcrt;
      break;
      case 't':
      index=INDEX(game->bid->position,trefle);
      memr=game->tabjeuref[index]->nbcrt;
      break;
      case 'h':
      for (couleur=trefle;couleur<pique+1;couleur++) {
        index=INDEX(game->bid->position,couleur);
        for (i=0;i<game->tabjeuref[index]->nbcrt;i++) {
          if(game->tabjeuref[index]->tabcoul[i]==cA)
            sum=sum+4;
          if(game->tabjeuref[index]->tabcoul[i]==cR)
            sum=sum+3;
          if(game->tabjeuref[index]->tabcoul[i]==cD)
            sum=sum+2;
          if(game->tabjeuref[index]->tabcoul[i]==cV)
            sum=sum+1;
      
        }
      }
      memr=sum;
      break;
      case 'd':
      for (couleur=trefle;couleur<pique+1;couleur++) {
        index=INDEX(game->bid->position,couleur);
        for (i=0;i<game->tabjeuref[index]->nbcrt;i++) {
          if(game->tabjeuref[index]->tabcoul[i]==cA)
            sum=sum+4;
          if(game->tabjeuref[index]->tabcoul[i]==cR)
            sum=sum+3;
          if(game->tabjeuref[index]->tabcoul[i]==cD)
            sum=sum+2;
          if(game->tabjeuref[index]->tabcoul[i]==cV)
            sum=sum+1;
      
      
        }
      }
      memr=sum;
      break;

    }

  }
  
  if (s_smallref[1]=='>') 
    testcond=meml-memr;
  else
    testcond=memr-meml;
  free(s_small);
  printf("testcondition:%d\n",testcond);
  return(testcond);

}

int test_condition(game_t *game,char *s_condition) {
  char *s_small;
  char *saveptr1;
  s_small=strtok_r(s_condition,"&,",&saveptr1);
  if (small_condition(game,s_small)<=0) {
    return 0;
  }
  s_small=strtok_r(NULL,"&,",&saveptr1);
  printf("s_small:%s\n",s_small);
  while(s_small!=NULL){
  //while((s_small=strtok(NULL,"&,"))!=NULL){
    if (small_condition(game,s_small)<=0){
      return 0;
    }
    s_small=strtok_r(NULL,"&,",&saveptr1);
  
  }
  return 1;
} 


void analyse_bid(game_t *game) {
  FILE *obidhomeconf;
  char bidhomeconf[1024];
  gboolean found=FALSE;
  char *home,tmpstring[2];
  char *ptr;
  char *s_bidref,*s_newbid,*s_condition;
  char buf[16384];
  home = getenv ("HOME");
  sprintf (bidhomeconf, "%s%s", home, BIDCONFHOME);
  obidhomeconf=fopen(bidhomeconf,"r");
  tmpstring[0]=(char ) (game->bid->nombre+48);
  tmpstring[1]='\0';
  game->bid->position=(game->bid->position+2)%4;
  strcat(game->cur_bid,tmpstring);
  strcat(game->cur_bid,affichage(game->bid->couleur,COULEUR));
  strcat(game->cur_bid,"PP");
  while (fgets(buf, 16380, obidhomeconf)) {
    if (!strncmp(buf, game->cur_bid, strlen(game->cur_bid))) {
      buf[strlen(buf)-1] = '\0';
      s_bidref = strtok_r(buf, ":",&ptr);
      s_newbid = strtok_r(NULL, ":",&ptr);
      s_condition = strtok_r(NULL, ":",&ptr);
      if(test_condition(game,s_condition)) {
        found=TRUE;
        break;
      }
    }

    

  }
  if(found==FALSE) 
    strcat(game->cur_bid,"PP"); 
  else
    strcat(game->cur_bid,s_newbid);
   // For player 
    strcat(game->cur_bid,"PP"); 
  
  fclose(obidhomeconf);
}



/*Cette fonction essaie de trouver un fit en majeur, sinon elle teste l'arret a sans atout , sinon elle est essaie un fit en mineur, enfin elle retourne la couleur la plus longue */
couleur_t
fit (main_t * mainjoueur, int position)
{
  int  nbmaxcarte = 0,i;
  couleur_t  refcouleur ;
  if (mainjoueur[position].nbcartes[pique] +
      mainjoueur[position + 2].nbcartes[pique] >= FIT)
    return (pique);

  if (mainjoueur[position].nbcartes[coeur] +
      mainjoueur[position + 2].nbcartes[coeur] >= FIT)
    return (coeur);

  if ((mainjoueur[position].nbpointshonneurs[pique] +
       mainjoueur[position + 2].nbpointshonneurs[pique] >= ARRET)
      && (mainjoueur[position].nbpointshonneurs[coeur] +
	  mainjoueur[position + 2].nbpointshonneurs[coeur] >= ARRET)
      && (mainjoueur[position].nbpointshonneurs[carreau] +
	  mainjoueur[position + 2].nbpointshonneurs[carreau] >= ARRET)
      && (mainjoueur[position].nbpointshonneurs[trefle] +
	  mainjoueur[position + 2].nbpointshonneurs[trefle] >= ARRET))
    return (aucune);

  if (mainjoueur[position].nbcartes[carreau] +
      mainjoueur[position + 2].nbcartes[carreau] >= FIT)
    return (carreau);
  if (mainjoueur[position].nbcartes[trefle] +
      mainjoueur[position + 2].nbcartes[trefle] >= FIT)
    return (trefle);
  for (i = pique; i >= trefle; i--)
    {
      if (mainjoueur[position].nbcartes[i] +
	  mainjoueur[position + 2].nbcartes[i] >= nbmaxcarte)
	{
	  nbmaxcarte =
	    mainjoueur[position].nbcartes[i] + mainjoueur[position +
								2].
	    nbcartes[i];
	  refcouleur = i;
	}

    }
  return (refcouleur);
}

/* Nouvelle Fonction de calcul du contrat a jouer en attendant les fonctions d'bid */
position_t
eval_contrat (game_t *game)
{
  int nbpointsns, nbpointseo, pointsadditionnels, ref, refpts, jouepar;
  couleur_t color,i;
  nbpointsns = game->mainjoueur[nord].nbpoints + game->mainjoueur[sud].nbpoints;
  nbpointseo = game->mainjoueur[est].nbpoints + game->mainjoueur[ouest].nbpoints;
  /* on cherche l'atout dans le camp majoritaire en point */
  if (nbpointsns >= nbpointseo)
    {
      color = fit (game->mainjoueur, SUD);
      printf ("Couleur de reference:%d\n", color);
      ref = SUD;
      refpts = nbpointsns;
    }
  else
    {
      color = fit (game->mainjoueur, OUEST);
      ref = OUEST;
      refpts = nbpointseo;
    }
  printf ("Points: %d", refpts);
  pointsadditionnels = 0;
  if (color != aucune)
    {
      for (i = 0; i < NBCOULEURS; i++)
	{
	  if (color != i)
	    {
	      if (game->mainjoueur[ref].nbcartes[i] <= 2)
		pointsadditionnels =
		  pointsadditionnels + 3 - game->mainjoueur[ref].nbcartes[i];
	      if (game->mainjoueur[ref + 2].nbcartes[i] <= 2)
		pointsadditionnels =
		  pointsadditionnels + 3 - game->mainjoueur[ref].nbcartes[i];
	    }

	}
      if (game->mainjoueur[ref].nbcartes[color] >=
	  game->mainjoueur[ref + 2].nbcartes[color])
	{
	  jouepar = ref;
	}
      else
	{
	  jouepar = ref + 2;
	}
      refpts = refpts + pointsadditionnels;
    }
  else
    {
      for (i = 0; i < NBCOULEURS; i++)
	{
	  if ((game->mainjoueur[ref].nbpointshonneurs[i] +
	       game->mainjoueur[ref + 2].nbpointshonneurs[i]) == ARRET)
	    {

	      if (game->mainjoueur[ref].nbpointshonneurs[i] >=
		  game->mainjoueur[ref + 2].nbpointshonneurs[i])
		{
		  jouepar = ref;
		}
	      else
		{
		  jouepar = ref + 2;
		}
	    }
	}
    }
  if ((refpts >= 20) && (refpts < 23))
    game->contrat->nom = 1;
  if ((refpts >= 23) && (refpts < 25))
    game->contrat->nom = 2;
  if ((refpts >= 25) && (refpts < 27))
    game->contrat->nom = 3;
  if ((refpts >= 27) && (refpts < 30))
    game->contrat->nom = 4;
  if ((refpts >= 30) && (refpts < 33))
    game->contrat->nom = 5;
  if ((refpts >= 33) && (refpts < 37))
    game->contrat->nom = 6;
  if (refpts >= 37)
    game->contrat->nom = 7;
  game->contrat->atout = color;
  game->contrat->nbplis = 6 + game->contrat->nom;
  game->contrat->declarant = 0; //modification pour faire pivoter le jeu   =jouepar
  ligneia = (  1) % 2;
  printf ("Dans eval_contrat\n");
  return(4-jouepar);
}
