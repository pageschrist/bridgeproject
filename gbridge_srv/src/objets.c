#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "distribution.h"
#include "arbitre.h"
coord_t tab_cartes[cA + 1][pique + 1];
void mark(void){
  printf("mark\n");
}
void affiche_tabjeu(position_t position);
void init_list_best(l_best_t *l_best){
  l_best->first=NULL;
  l_best->last=NULL;

}
void add_list_l(l_best_t *l_best,best_t *best) {
  elem_best_t *elem_best=malloc(sizeof(elem_best_t));
  if(elem_best==NULL) {
    fprintf(stderr,"Pb avec malloc\n");
    exit(EXIT_FAILURE);
  }
  else {
    elem_best->best=best;
    elem_best->next=NULL;
    elem_best->prev=l_best->last;
    if(l_best->last) l_best->last->next = elem_best;
    else l_best->first = elem_best;
    l_best->last = elem_best; 
  }

}
best_t * rm_list_l(l_best_t *l_best) {
  best_t *best=malloc(sizeof(best_t));
  elem_best_t *tmp_elem_best=l_best->last;
  if(!tmp_elem_best) return NULL;
  best=tmp_elem_best->best;
  l_best->last=tmp_elem_best->prev;
  if(l_best->last) l_best->last->next = NULL;
  else l_best->first = NULL;
  free(tmp_elem_best);
  return(best);
}
best_t * rm_list_f(l_best_t *l_best) {
  best_t *best=malloc(sizeof(best_t));
  elem_best_t *tmp_elem_best=l_best->first;
  if(!tmp_elem_best) return NULL;
  best=tmp_elem_best->best;
  l_best->first=tmp_elem_best->next;
  if(l_best->first) l_best->first->prev = NULL;
  else l_best->last = NULL;
  free(tmp_elem_best);
  return(best);
}
void add_list_f(l_best_t *l_best,best_t *best) {
  elem_best_t *elem_best=malloc(sizeof(elem_best_t));
  if(elem_best==NULL) {
    fprintf(stderr,"Pb avec malloc\n");
    exit(EXIT_FAILURE);
  }
  else {
    elem_best->best=best;
    elem_best->prev=NULL;
    elem_best->next=l_best->first;
    if(l_best->first) l_best->first->prev = elem_best;
    else l_best->last = elem_best;
    l_best->first = elem_best; 
  }

}
  

void clear_list(l_best_t *l_list) {
  printf("Nettoyage de liste\n");
  elem_best_t *tmp;
  elem_best_t *elem_best=l_list->first;
  while (elem_best) {
    free(elem_best->best->carte);
    free(elem_best->best);
    tmp=elem_best;
    elem_best=elem_best->next;
    free(tmp);

  }
  l_list->first=NULL;
  l_list->last=NULL;

}
void init_tabjeu (game_t *game)
{
  position_t position;
  int index ,i;
  game->tabjeu = malloc ((sizeof (tablist_t *)) * (est + 1) * (pique + 1));
  game->tabjeuref = malloc ((sizeof (tablist_t *)) * (est + 1) * (pique + 1));
  couleur_t couleur;
  for (position = sud; position < est + 1; position++)
    {
      for (couleur = trefle; couleur < pique + 1; couleur++)
	{
	  index = INDEX (position, couleur);
          game->tabjeu[index]=malloc(sizeof (tablist_t));
          game->tabjeuref[index]=malloc(sizeof (tablist_t));
	  for (i = 0; i < NBPCOULEURS; i++)
	    game->tabjeuref[index]->tabcoul[i] = pdc;
	  game->tabjeuref[index]->nbcrt = 0;
	  game->tabjeuref[index]->poscartej = 0;
	  game->tabjeuref[index]->couleureval = aucune;
	  for (i = 0; i < NBPCOULEURS; i++)
	    game->tabjeu[index]->tabcoul[i] = pdc;
	  game->tabjeu[index]->nbcrt = 0;
	  game->tabjeu[index]->poscartej = 0;
	  game->tabjeu[index]->couleureval = aucune;
	}
    }
}
void clear_tabjeu (game_t *game)
{
  position_t position;
  int index ;
  couleur_t couleur;
  for (position = sud; position < est + 1; position++)
    {
      for (couleur = trefle; couleur < pique + 1; couleur++)
	{
	  index = INDEX (position, couleur);
          free(game->tabjeuref[index]);
          game->tabjeuref[index]=NULL;
          free(game->tabjeu[index]);
          game->tabjeu[index]=NULL;
	}
    }
  free(game->tabjeu);
  free(game->tabjeuref);
}
int sub_index (tablist_t **tmpjeu,position_t position, couleur_t couleur, valeur_t valeur)
{

  int pos_index=0;
  for(pos_index=tmpjeu[INDEX (position, couleur)]->nbcrt-1;pos_index>-1;pos_index--)  {

      if (tmpjeu[INDEX (position, couleur)]->tabcoul[pos_index] < valeur)
        return(pos_index);
    }
  return(0);
}

int
find_index (tablist_t **tmpjeu,position_t position, couleur_t couleur, valeur_t valeur)
{

  int pos_index=0;
    for(pos_index=0;pos_index<=tmpjeu[INDEX (position, couleur)]->nbcrt-1;pos_index++)  {
      
      if (tmpjeu[INDEX (position, couleur)]->tabcoul[pos_index] == valeur)
        return(pos_index);
    }
    printf("valeur=%d couleur=%d\n",valeur,couleur);
    return(-1);

}


int
remove_index (tablist_t **tmpjeu,position_t position, couleur_t couleur, int pos_index)
{
  int j=0;
  
  if(pos_index==-1) { 
    printf("Problem dans remove_index pos_index=%d\n",pos_index);
    exit(1);
  } 
  for(j=pos_index;j<tmpjeu[INDEX (position, couleur)]->nbcrt-1;j++)  {
      
    tmpjeu[INDEX (position, couleur)]->tabcoul[j] =tmpjeu[INDEX(position,couleur)]->tabcoul[j+1];
  }

  tmpjeu[INDEX (position, couleur)]->tabcoul[tmpjeu[INDEX (position, couleur)]->nbcrt-1]=pdc;
  tmpjeu[INDEX (position, couleur)]->nbcrt=tmpjeu[INDEX (position, couleur)]->nbcrt-1;

  return(pos_index);

}

int
insert_index (tablist_t **tmpjeu,position_t position, couleur_t couleur, valeur_t valeur,int pos_index)
{

  int j=0;
  if((pos_index==0) && (tmpjeu[INDEX (position, couleur)]->nbcrt==0)) {
    tmpjeu[INDEX (position, couleur)]->tabcoul[pos_index]=valeur;
    tmpjeu[INDEX (position, couleur)]->nbcrt=tmpjeu[INDEX (position, couleur)]->nbcrt+1;
    return(1);
  }
  if( (tmpjeu[INDEX (position, couleur)]->nbcrt==pos_index)) {
    tmpjeu[INDEX (position, couleur)]->tabcoul[pos_index]=valeur;
    tmpjeu[INDEX (position, couleur)]->nbcrt=tmpjeu[INDEX (position, couleur)]->nbcrt+1;
    return(1);
  }
  
  for(j=tmpjeu[INDEX (position, couleur)]->nbcrt;j>pos_index;j--)  {
    if(j==0 ) {
      printf("j =0 pos_index=%d tmpjeu->nbcrt=%d\n",pos_index,tmpjeu[INDEX (position, couleur)]->nbcrt);
      exit(1);
    } 

    tmpjeu[INDEX (position, couleur)]->tabcoul[j] =
                tmpjeu[INDEX(position,couleur)]->tabcoul[j-1];
  }
  tmpjeu[INDEX (position, couleur)]->tabcoul[pos_index]=valeur;
  tmpjeu[INDEX (position, couleur)]->nbcrt=tmpjeu[INDEX (position, couleur)]->nbcrt+1;
  return(1);

}

void init_game(game_t *game) {
  
  game->contrat=malloc(sizeof(contrat_t));
  game->bid=malloc(sizeof(bid_t));
  game->cur_bid[0]='\0';
  init_tabjeu(game);
  game->status='c';

}
void clear_game(game_t *game) {
  free(game->contrat);
  free(game->bid);
  game->cur_bid[0]='\0';
  clear_tabjeu(game);

}

int char_to_int(char *c) {
  char *atout="TKCPS";
  int i;
  for (i=0;i<(int) strlen(atout);i++) {
    if(atout[i]==*c) {
      return(i);

    }
  }
  return(0);
}



void
affiche_tabjeuref (game_t *game,position_t position)
{

  couleur_t couleur;
  char *affp,*affco,*affca;
  int i;
  printf ("\n%s ", affp=affichage (position, POSITION));
  free(affp);
  for (couleur = trefle; couleur < aucune; couleur++)
    {
      printf ("\n%s ", affco=affichage (couleur, COULEUR));
      free(affco);
/* Prints data in list */
      for(i=0;i<game->tabjeuref[INDEX (position, couleur)]->nbcrt;i++) {
	  printf ("%s", affca=affichage (game->tabjeuref[INDEX(position,couleur)]->tabcoul[i], CARTE));
          free(affca);

      }
         
    }

}

void affiche_tabjeu_c (tablist_t **t_jeu)
{

  couleur_t couleur;
  position_t position;
  char *affca,*affco,*affp;
  int i;
  for (position = sud; position < est+1; position++)
  { 
    printf ("\n%s ", affp=affichage (position, POSITION));
    free(affp);
    for (couleur = trefle; couleur < aucune; couleur++)
      {
        printf ("\n%s ", affco=affichage (couleur, COULEUR));
        free(affco);
/* Prints data in list */
        for(i=0;i<t_jeu[INDEX (position, couleur)]->nbcrt;i++) {
	  printf ("%s", affca=affichage (t_jeu[INDEX(position,couleur)]->tabcoul[i], CARTE));
          free(affca);
        }
        printf(" Nb de cartes:%d\n",t_jeu[INDEX (position, couleur)]->nbcrt);
         
    }
  }
}


void
init_cartes (void)
{
  valeur_t i;
  couleur_t j;
  for (i = 0; i < cA + 1; i++)
    {
      for (j = 0; j < pique + 1; j++)
	{
	  tab_cartes[i][j].nol = aucun;
	  tab_cartes[i][j].noc = aucun;
	  tab_cartes[i][j].detenteur = aucun;
	}
    }

}
void *
duplique_pli (void *data)
{
  pli_t *plicopie;
  position_t i;
  ligne_t j;
  plicopie = malloc (sizeof (pli_t));
  for (i = sud; i < est + 1; i++)
    {
      plicopie->carte[i].nocarte = ((pli_t *) data)->carte[i].nocarte;
      plicopie->carte[i].clcarte = ((pli_t *) data)->carte[i].clcarte;
      plicopie->defausse[i].nocarte = ((pli_t *) data)->defausse[i].nocarte;
      plicopie->defausse[i].clcarte = ((pli_t *) data)->defausse[i].clcarte;
    }
  plicopie->entame = ((pli_t *) data)->entame;
  plicopie->nextpos = ((pli_t *) data)->nextpos;
  plicopie->nopli = ((pli_t *) data)->nopli;
  plicopie->noj = ((pli_t *) data)->noj;
  plicopie->atout = ((pli_t *) data)->atout;
  plicopie->phcarte.nocarte = ((pli_t *) data)->phcarte.nocarte;
  plicopie->phcarte.clcarte = ((pli_t *) data)->phcarte.clcarte;
  plicopie->lastcarte.nocarte = ((pli_t *) data)->lastcarte.nocarte;
  plicopie->lastcarte.clcarte = ((pli_t *) data)->lastcarte.clcarte;
  //plicopie->ligne = ((pli_t *) data)->ligne;
  for (j = ns; j < eo + 1; j++)
    {
      plicopie->nbpli_ligne[j] = ((pli_t *) data)->nbpli_ligne[j];
    }
  return ((void *) plicopie);

}
void affiche_thread_jeu(thread_jeu_t *thread_jeu) {
  position_t position;
  couleur_t couleur;
  char *affca,*affco;
  int index;
  for (position=sud;position<est+1;position++) {
    for (couleur=trefle;couleur<pique+1;couleur++) {
      index=INDEX(position,couleur);  
      
    }
  }
  //affiche_pli(thread_jeu->pli);
  affiche_tabjeu_c(thread_jeu->t_jeu);
  printf("carte:%s%s\n",affca=affichage(thread_jeu->carte->nocarte,CARTE),affco=affichage(thread_jeu->carte->clcarte,COULEUR));
  free(affca);
  free(affco);
  printf("best_cartepot:%s%s\n",affca=affichage(thread_jeu->best_cartepot->nocarte,CARTE),affco=affichage(thread_jeu->best_cartepot->clcarte,COULEUR));
  free(affca);
  free(affco);
  printf("status=%d score=%d prof=%d prof_max=%d\n",thread_jeu->status,thread_jeu->score,thread_jeu->prof,thread_jeu->prof_max);

}

void
affiche_pli (pli_t * pli)
{
  position_t i;
  char *affca,*affco;
  ligne_t j;
  /*return(0); */
  for (i = sud; i < est + 1; i++)
    {
      if (pli->carte[i].nocarte==pdc) {
      printf("probleme dans affiche_pli pour %d\n",i);
      
      }
      printf ("pos %d,  %s%s\n", i, affca=affichage (pli->carte[i].nocarte, CARTE),
	      affco=affichage (pli->carte[i].clcarte, COULEUR));
      free(affca);
      free(affco);
    }
  printf ("Vci  pli.noj %d\n", pli->noj);
  printf ("Vci  pli.phcarte %d\n", pli->phcarte.nocarte);
  printf ("Vci  pli.lastcarte %d\n", pli->lastcarte.nocarte);
  printf ("Vci  pli.entame %d\n", pli->entame);
  printf ("Vci  pli.nextpos %d\n", pli->nextpos);
  printf ("Vci  pli.nopli %d\n", pli->nopli);
  for (j = ns; j < eo + 1; j++)
    {
      printf ("Vci pli.nbpli_ligne[%d] %d\n", j, pli->nbpli_ligne[j]);
    }

}

void
affiche_carte (carte_t * carte)
{
  printf ("carte:%d", carte->nocarte);
  printf ("couleur:%d", carte->clcarte);
}

void
affiche_tab_cartes (void)
{
  extern coord_t tab_cartes[cA + 1][pique + 1];
  position_t i;
  valeur_t v;
  couleur_t c;
  for (i = sud; i < est + 1; i++)
    {
      for (c = trefle; c < pique + 1; c++)
	{
	  for (v = c2; v < cA + 1; v++)
	    {
	      if (tab_cartes[v][c].detenteur == i)
		printf ("tab_cartes[%s][%s].det=%s\n", affichage (v, CARTE),
			affichage (c, COULEUR),
			affichage (tab_cartes[v][c].detenteur, POSITION));
	    }
	}
    }
}

void
init_pli (pli_t * pli, int maniere)
{
  position_t i;
  ligne_t j;
  if (maniere == INIT)
    {
      pli->atout = aucune;
      pli->entame = aucun;
      pli->nextpos = aucun;
      pli->nopli = 0;
      pli->noj = 0;
      for (j = ns; j < eo + 1; j++)
	{
	  pli->nbpli_ligne[j] = 0;
	}
      pli->lastcarte.nocarte = pdc;
      pli->lastcarte.clcarte = aucune;
    }
    pli->phcarte.nocarte = pdc;
    pli->phcarte.clcarte = aucune;
    for (i=sud;i<est+1;i++) {
      pli->carte[i].nocarte = pdc;
      pli->carte[i].clcarte = aucune;
      pli->defausse[i].nocarte = pdc;
      pli->defausse[i].clcarte = aucune;
    }
}

