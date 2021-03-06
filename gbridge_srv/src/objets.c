#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "distribution.h"
#include "analyse.h"
#include "ia.h"
#include "stack.h"
#include "arbitre.h"
coord_t tab_cards[cA + 1][spade + 1];
void mark(void)
{
    printf("mark\n");
}

void init_list_best(l_best_t * l_best)
{
    l_best->first = NULL;
    l_best->last = NULL;

}

int max(int a,int b) {

  if(a>b) 
    return(a);
  else
    return(b);
}

void add_list_l(l_best_t * l_best, best_t * best)
{
    elem_best_t *elem_best = malloc(sizeof(elem_best_t));
    if (elem_best == NULL) {
	fprintf(stderr, "Pb avec malloc\n");
	exit(EXIT_FAILURE);
    } else {
	elem_best->best = best;
	elem_best->next = NULL;
	elem_best->prev = l_best->last;
	if (l_best->last)
	    l_best->last->next = elem_best;
	else
	    l_best->first = elem_best;
	l_best->last = elem_best;
    }

}
best_t *rm_list_l(l_best_t * l_best)
{
    best_t *best = malloc(sizeof(best_t));
    elem_best_t *tmp_elem_best = l_best->last;
    if (!tmp_elem_best)
	return NULL;
    best = tmp_elem_best->best;
    l_best->last = tmp_elem_best->prev;
    if (l_best->last)
	l_best->last->next = NULL;
    else
	l_best->first = NULL;
    free(tmp_elem_best);
    return (best);
}

best_t *rm_list_f(l_best_t * l_best)
{
    best_t *best = malloc(sizeof(best_t));
    elem_best_t *tmp_elem_best = l_best->first;
    if (!tmp_elem_best)
	return NULL;
    best = tmp_elem_best->best;
    l_best->first = tmp_elem_best->next;
    if (l_best->first)
	l_best->first->prev = NULL;
    else
	l_best->last = NULL;
    free(tmp_elem_best);
    return (best);
}

void add_list_f(l_best_t * l_best, best_t * best)
{
    elem_best_t *elem_best = malloc(sizeof(elem_best_t));
    if (elem_best == NULL) {
	fprintf(stderr, "Pb avec malloc\n");
	exit(EXIT_FAILURE);
    } else {
	elem_best->best = best;
	elem_best->prev = NULL;
	elem_best->next = l_best->first;
	if (l_best->first)
	    l_best->first->prev = elem_best;
	else
	    l_best->last = elem_best;
	l_best->first = elem_best;
    }

}


void clear_list(l_best_t * l_list)
{
    elem_best_t *tmp;
    elem_best_t *elem_best = l_list->first;
    while (elem_best) {
	free(elem_best->best->card);
	free(elem_best->best);
	tmp = elem_best;
	elem_best = elem_best->next;
	free(tmp);

    }
    l_list->first = NULL;
    l_list->last = NULL;

}

void init_tabjeu(game_t * game)
{
    position_t position;
    int index, i;
    game->tabjeu = malloc((sizeof(tablist_t *)) * (est + 1) * (spade + 1));
    game->tabjeuref =
	malloc((sizeof(tablist_t *)) * (est + 1) * (spade + 1));
    color_t couleur;
    for (position = sud; position < est + 1; position++) {
	for (couleur = club; couleur < spade + 1; couleur++) {
	    index = INDEX(position, couleur);
	    game->tabjeu[index] = malloc(sizeof(tablist_t));
	    game->tabjeuref[index] = malloc(sizeof(tablist_t));
	    for (i = 0; i < NBPCOULEURS; i++)
		game->tabjeuref[index]->tabcoul[i] = pdc;
	    game->tabjeuref[index]->nbcrt = 0;
#ifdef DEBUG
	    game->tabjeuref[index]->debug = TRUE;
	    game->tabjeu[index]->debug = TRUE;
#else
	    game->tabjeuref[index]->debug = FALSE;
	    game->tabjeu[index]->debug = FALSE;
#endif
	    game->tabjeuref[index]->couleureval = aucune;
	    for (i = 0; i < NBPCOULEURS; i++)
		game->tabjeu[index]->tabcoul[i] = pdc;
	    game->tabjeu[index]->nbcrt = 0;
	    game->tabjeu[index]->couleureval = aucune;
	}
    }
}
void clear_tabjeu(game_t * game)
{
    position_t position;
    int index;
    color_t couleur;
    for (position = sud; position < est + 1; position++) {
	for (couleur = club; couleur < spade + 1; couleur++) {
	    index = INDEX(position, couleur);
            if(game->tabjeuref[index])
	      free(game->tabjeuref[index]);
	    game->tabjeuref[index] = NULL;
            if(game->tabjeu[index])
	      free(game->tabjeu[index]);
	    game->tabjeu[index] = NULL;
	}
    }
    if(game->tabjeu)
      free(game->tabjeu);
    if(game->tabjeuref)
      free(game->tabjeuref);
    if(game->cardplayed)
      clear_cardplayed(game,FREE);
}
void clear_cardplayed(game_t * game,int type){
  color_t color;
  valeur_t value;
  int indcard;
  if(FREE==type) 
    free(game->cardplayed);
  else {
    for (color=club;color<spade+1;color++) {
      for(value=c2;value<cA+1;value++){
        indcard=INDCARD(color,value);
        game->cardplayed[indcard]=FALSE;
      }
    }
  }
}

void display_cardplayed(game_t *game) {
  color_t color;
  valeur_t value;
  int indcard;
  char *resco;
  printf("CARD PLAYED \n");
  for (color=club;color<spade+1;color++) {
    resco=affichage(color,COULEUR);
    printf("\n%s ",resco);
    for(value=c2;value<cA+1;value++) {
      indcard=INDCARD(color,value);
      if(game->cardplayed[indcard])
        printf("T");
      else
        printf("F");
    }
  }
  free(resco);

}

int sub_index(tablist_t ** tmpjeu, position_t position, color_t couleur,
	      valeur_t valeur)
{

    int pos_index = 0;
    for (pos_index = tmpjeu[INDEX(position, couleur)]->nbcrt - 1;
	 pos_index > -1; pos_index--) {

	if (tmpjeu[INDEX(position, couleur)]->tabcoul[pos_index] < valeur)
	    return (pos_index);
    }
    return (0);
}

int
find_index(tablist_t ** tmpjeu, position_t position, color_t couleur,
	   valeur_t valeur)
{

    int pos_index = 0;
    for (pos_index = 0;
	 pos_index <= tmpjeu[INDEX(position, couleur)]->nbcrt - 1;
	 pos_index++) {

	if (tmpjeu[INDEX(position, couleur)]->tabcoul[pos_index] == valeur)
	    return (pos_index);
    }
    printf("valeur=%d couleur=%d\n", valeur, couleur);
    return (-1);

}


int
remove_index(tablist_t ** tmpjeu, position_t position, color_t couleur,
	     int pos_index)
{
    int j = 0;

    if (pos_index == -1) {
	printf("Problem dans remove_index pos_index=%d\n", pos_index);
	exit(1);
    }
    for (j = pos_index; j < tmpjeu[INDEX(position, couleur)]->nbcrt - 1;
	 j++) {

	tmpjeu[INDEX(position, couleur)]->tabcoul[j] =
	    tmpjeu[INDEX(position, couleur)]->tabcoul[j + 1];
    }

    tmpjeu[INDEX(position, couleur)]->
	tabcoul[tmpjeu[INDEX(position, couleur)]->nbcrt - 1] = pdc;
    tmpjeu[INDEX(position, couleur)]->nbcrt =
	tmpjeu[INDEX(position, couleur)]->nbcrt - 1;

    return (pos_index);

}

int
insert_index(tablist_t ** tmpjeu, position_t position, color_t couleur,
	     valeur_t valeur, int pos_index)
{

    int j = 0;
    if ((pos_index == 0) && (tmpjeu[INDEX(position, couleur)]->nbcrt == 0)) {
	tmpjeu[INDEX(position, couleur)]->tabcoul[pos_index] = valeur;
	tmpjeu[INDEX(position, couleur)]->nbcrt =
	    tmpjeu[INDEX(position, couleur)]->nbcrt + 1;
	return (1);
    }
    if ((tmpjeu[INDEX(position, couleur)]->nbcrt == pos_index)) {
	tmpjeu[INDEX(position, couleur)]->tabcoul[pos_index] = valeur;
	tmpjeu[INDEX(position, couleur)]->nbcrt =
	    tmpjeu[INDEX(position, couleur)]->nbcrt + 1;
	return (1);
    }

    for (j = tmpjeu[INDEX(position, couleur)]->nbcrt; j > pos_index; j--) {
	if (j == 0) {
	    printf("j =0 pos_index=%d tmpjeu->nbcrt=%d\n", pos_index,
		   tmpjeu[INDEX(position, couleur)]->nbcrt);
	    exit(1);
	}

	tmpjeu[INDEX(position, couleur)]->tabcoul[j] =
	    tmpjeu[INDEX(position, couleur)]->tabcoul[j - 1];
    }
    tmpjeu[INDEX(position, couleur)]->tabcoul[pos_index] = valeur;
    tmpjeu[INDEX(position, couleur)]->nbcrt =
	tmpjeu[INDEX(position, couleur)]->nbcrt + 1;
    return (1);

}

void init_cardplayed(game_t *game) {
  game->cardplayed=malloc(NBCOULEURS*NBPCOULEURS*sizeof(gboolean ));
  clear_cardplayed(game,RESET);
}

void init_game(game_t * game)
{

#ifdef DEBUG
    game->debug = TRUE;
#else
    game->debug = FALSE;
#endif
    game->contrat = malloc(sizeof(contrat_t));
    game->bid = malloc(sizeof(bid_t));
    game->cur_bid[0] = '\0';
    init_tabjeu(game);
    game->status = 'b';
    game->buffile=NULL;
    game->nbcard=NBPCOULEURS;
    init_cardplayed(game);

}

void clear_game(game_t * game)
{
    if(game->contrat)
      free(game->contrat);
    if(game->bid)
      free(game->bid);
    game->cur_bid[0] = '\0';
    clear_tabjeu(game);

}

int char_to_int(char *c)
{
    char *atout = "TKCPS";
    int i;
    for (i = 0; i < (int) strlen(atout); i++) {
	if (atout[i] == *c) {
	    return (i);

	}
    }
    return (0);
}



void affiche_tabjeuref(game_t * game, position_t position)
{

    color_t couleur;
    char *affp, *affco, *affca;
    int i;
    printf("\n%s ", affp = affichage(position, POSITION));
    free(affp);
    for (couleur = club; couleur < aucune; couleur++) {
	printf("\n%s ", affco = affichage(couleur, COULEUR));
	free(affco);
/* Prints data in list */
	for (i = 0; i < game->tabjeuref[INDEX(position, couleur)]->nbcrt;
	     i++) {
	    printf("%s", affca =
		   affichage(game->tabjeuref[INDEX(position, couleur)]->
			     tabcoul[i], CARD));
	    free(affca);

	}

    }

}

void affiche_tabjeu_c(tablist_t ** t_jeu)
{

    color_t couleur;
    position_t position;
    char *affca, *affco, *affp;
    int i;
    for (position = sud; position < est + 1; position++) {
	printf("\n%s ", affp = affichage(position, POSITION));
	free(affp);
	for (couleur = club; couleur < aucune; couleur++) {
	    printf("\n%s ", affco = affichage(couleur, COULEUR));
	    free(affco);
/* Prints data in list */
	    for (i = 0; i < t_jeu[INDEX(position, couleur)]->nbcrt; i++) {
		printf("%s", affca =
		       affichage(t_jeu[INDEX(position, couleur)]->
				 tabcoul[i], CARD));
		free(affca);
	    }
	    printf(" Nb de cards:%d\n",
		   t_jeu[INDEX(position, couleur)]->nbcrt);

	}
    }
}


void init_cards(void)
{
    valeur_t i;
    color_t j;
    for (i = 0; i < cA + 1; i++) {
	for (j = 0; j < spade + 1; j++) {
	    tab_cards[i][j].nol = aucun;
	    tab_cards[i][j].noc = aucun;
	    tab_cards[i][j].detenteur = aucun;
	}
    }

}
void *copy_card(void *data) {
  card_t *card;
  card=malloc(sizeof(card_t));
  memcpy(card,(card_t *) data,sizeof(card_t));
  return((void*) card);
}

void *dupXXcate_trick(void *data)
{
    trick_t *copytrick;
    copytrick = malloc(sizeof(trick_t));
    memcpy(copytrick, (trick_t *) data, sizeof(trick_t));
    return ((void *) copytrick);

}



void affiche_thread_jeu(thread_jeu_t * thread_jeu)
{
    char *affca, *affco;
    affiche_tabjeu_c(thread_jeu->t_jeu);
    printf("card:%s%s\n", affca =
	   affichage(thread_jeu->card->nocard, CARD), affco =
	   affichage(thread_jeu->card->clcard, COULEUR));
    free(affca);
    free(affco);
    printf("best_cardpot:%s%s\n", affca =
	   affichage(thread_jeu->best_cardpot->nocard, CARD), affco =
	   affichage(thread_jeu->best_cardpot->clcard, COULEUR));
    free(affca);
    free(affco);
    printf("status=%d score=%d prof=%d prof_max=%d\n", thread_jeu->status,
	   thread_jeu->score, thread_jeu->prof, thread_jeu->prof_max);
}

void display_trick(trick_t * trick,gboolean display)
{
  position_t i;
  char *affca, *affco;
  ligne_t j;
  if (display) {
    /*return(0); */
    printf("DSP  nop=%d,noj %d, ", trick->nbtrick,trick->noj);
    for (i = sud; i < est + 1; i++) {
	printf("pos %d  %s%s,", i, affca =
	       affichage(trick->card[i].nocard, CARD), affco =
	       affichage(trick->card[i].clcard, COULEUR));
	free(affca);
	free(affco);
    }
    printf("higher %d,", trick->phcard.nocard);
    //printf("lastcard %d,", trick->lastcard.nocard);
    printf("lead %d,", trick->entame);
    printf("npos %d,", trick->nextpos);
    printf("leader %d,", trick->leader);
    for (j = ns; j < eo + 1; j++) {
	printf("nbtrick_line[%d] %d ", j, trick->nbtrick_line[j]);
    }
    printf("\n");
  }

}

void affiche_card(card_t * card)
{
    char *resca;
    char *rescl;
    resca = affichage(card->nocard, CARD);
    rescl = affichage(card->clcard, COULEUR);
    printf("%s", resca);
    printf("%s", rescl);
    free(resca);
    free(rescl);
}

void affiche_tab_cards(void)
{
    extern coord_t tab_cards[cA + 1][spade + 1];
    position_t i;
    valeur_t v;
    color_t c;
    for (i = sud; i < est + 1; i++) {
	for (c = club; c < spade + 1; c++) {
	    for (v = c2; v < cA + 1; v++) {
		if (tab_cards[v][c].detenteur == i)
		    printf("tab_cards[%s][%s].det=%s\n",
			   affichage(v, CARD), affichage(c, COULEUR),
			   affichage(tab_cards[v][c].detenteur,
				     POSITION));
	    }
	}
    }
}

void init_trick(trick_t * trick, int maniere)
{
    position_t i;
    ligne_t j;
    if (maniere == INIT) {
	trick->atout = aucune;
	trick->entame = aucun;
	trick->leader = aucun;
	trick->nextpos = aucun;
	trick->nbtrick = 0;
	trick->noj = 0;
	for (j = ns; j < eo + 1; j++) {
	    trick->nbtrick_line[j] = 0;
	}
	trick->lastcard.nocard = pdc;
	trick->lastcard.clcard = aucune;
    }
    trick->phcard.nocard = pdc;
    trick->phcard.clcard = aucune;
    for (i = sud; i < est + 1; i++) {
	trick->card[i].nocard = pdc;
	trick->card[i].clcard = aucune;
	trick->discard[i].nocard = pdc;
	trick->discard[i].clcard = aucune;
    }
}

void fdebug(void)
{

    int i = 3;
    i++;
}
