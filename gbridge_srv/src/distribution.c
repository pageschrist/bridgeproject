#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include "objets.h"
#include "traffic.h"
#include "distribution.h"
#include "analyse.h"
#include "gbridge_srv.h"
extern coord_t tab_cards[cA + 1][spade + 1];
void initialisation(void)
{
    color_t j;
    valeur_t k;

    for (j = club; j < aucune; j++) {
	for (k = c2; k < pdc; k++) {
	    tab_cards[k][j].detenteur = aucun;
	}
    }

}

/*fonction de tirage au sort */
int tirage(int diviseur)
{
    int dividende, reste, quotient;
    dividende = random();
    quotient = dividende / diviseur;
    reste = dividende - (quotient * diviseur);
    return (reste);		/*un nombre est retourne entre 0 et < diviseur */

}


char *affichage(int valeur, int type)
{
    char figure[cA + 1];
    char couleurtype[spade + 1];
    char positiontype[aucun + 1];
    char *pos;
    if (type == CARD) {
	strcpy(figure, "23456789XVDRAZ");
	pos = malloc(sizeof(char) * 2);
        if(valeur>cA+1) 
          *pos=  'W';
        else
	  memcpy(pos, figure + valeur, sizeof(char));

    }
    if (type == COULEUR) {
	strcpy(couleurtype, "TKCPS");
	pos = malloc(sizeof(char) * 2);
        if(valeur>spade+1) 
          *pos='W';
        else
	  memcpy(pos, couleurtype + valeur, sizeof(char));
    }
    if (type == POSITION) {
	strcpy(positiontype, "SONEZ");
	pos = malloc(sizeof(char) * 2);
        if(valeur>spade+1) 
          *pos='W';
        else
	  memcpy(pos, positiontype + valeur, sizeof(char));
    }
    if ((type != CARD) && (type != COULEUR) && (type != POSITION)) {
	perror("type invalide\n");
	pos = NULL;
    }
    pos[1] = '\0';
    return (pos);
}

void affiche_contrat(contrat_t * contrat)
{
    fprintf(stdout, "Voici mon contrat\n");
    fprintf(stdout, "type: %d\n", contrat->nom);
    fprintf(stdout, "atout: %d\n", contrat->atout);
    fprintf(stdout, "declarant: %d\n", contrat->declarant);
    fprintf(stdout, "nbtricks: %d\n", contrat->nbtricks);

}

void defcard(int dividende, position_t position)
{
    int res = 0;
    int div;
    valeur_t valeur;
    color_t couleur;
    while (res == 0) {
	couleur = dividende / NBPCOULEURS;
	valeur = dividende - (couleur * NBPCOULEURS);
	if (tab_cards[valeur][couleur].detenteur == aucun) {
	    tab_cards[valeur][couleur].detenteur = position;
	    res = 1;

	} else {
	    div = (dividende + 1) % NBCARDS;
	    dividende = div;
	}
    }



}

/*Fonction de tri d'un tableau en commencant a lidx et en finissant a ridx */
void qintsort(int arr[], int lidx, int ridx)
{
    int k, buffer;
    int e, mid;
    if (lidx >= ridx) {
	return;
    }
    mid = (lidx + ridx) / 2;
    buffer = arr[lidx];
    arr[lidx] = arr[mid];
    arr[mid] = buffer;
    e = lidx;
    for (k = lidx + 1; k <= ridx; k++) {
	if (arr[k] >= arr[lidx]) {
	    e++;
	    buffer = arr[e];
	    arr[e] = arr[k];
	    arr[k] = buffer;
	}
    }

    buffer = arr[lidx];
    arr[lidx] = arr[e];
    arr[e] = buffer;
    qintsort(arr, lidx, e - 1);
    qintsort(arr, e + 1, ridx);


}

void evalmain(game_t * game)
{
    position_t position;
    valeur_t valeur;
    color_t couleur;
    for (position = sud; position < aucun; position++) {
	game->mainjoueur[position].nbpoints = 0;
	for (couleur = club; couleur < aucune; couleur++) {
	    game->mainjoueur[position].nbcards[couleur] = 0;
	    game->mainjoueur[position].nbpointshonneurs[couleur] = 0;
	    game->mainjoueur[position].nbcardsmax = 0;

	}
    }
    for (position = sud; position < aucun; position++) {
	for (valeur = c2; valeur < pdc; valeur++) {
	    for (couleur = club; couleur < aucune; couleur++) {
		if (valeur >= cV) {
		    game->mainjoueur[tab_cards[valeur][couleur].
				     detenteur].nbpoints =
			game->mainjoueur[tab_cards[valeur][couleur].
					 detenteur].nbpoints + (valeur -
								8);
		    game->mainjoueur[tab_cards[valeur][couleur].
				     detenteur].nbpointshonneurs[couleur] =
			game->mainjoueur[tab_cards[valeur][couleur].
					 detenteur].
			nbpointshonneurs[couleur] + (valeur - 8);
		}

		game->mainjoueur[tab_cards[valeur][couleur].detenteur].
		    nbcards[couleur] =
		    game->mainjoueur[tab_cards[valeur][couleur].
				     detenteur].nbcards[couleur] + 1;
	    }
	}
	for (position = sud; position < aucun; position++) {
	    for (couleur = club; couleur < aucune; couleur++) {
		if (game->mainjoueur[position].nbcards[couleur] >
		    game->mainjoueur[position].nbcardsmax)
		    game->mainjoueur[position].nbcardsmax =
			game->mainjoueur[position].nbcards[couleur];
		if (game->mainjoueur[position].nbcards[couleur] >=
		    BONUSLONG)
		    game->mainjoueur[position].nbpoints =
			game->mainjoueur[position].nbpoints +
			game->mainjoueur[position].nbcards[couleur] -
			BONUSLONG + 1;

	    }

	}

    }
}
void envoi_jeu(position_t position, game_t * game)
{

    color_t couleur;
    card_t *card;
    char *affca;
    card = malloc(sizeof(card_t));
    int valeur;
    int index;
    for (couleur = club; couleur < aucune; couleur++) {
	index = INDEX(position, couleur);
	card->clcard = couleur;
	if (game->tabjeuref[index]->nbcrt != 0) {
	    for (valeur = 0; valeur < game->tabjeuref[index]->nbcrt;
		 valeur++) {
                if(game->debug) {
		  printf("%s", affca =
		       affichage(game->tabjeuref[index]->tabcoul[valeur],
				 CARD));
		  free(affca);
                }
		card->nocard = game->tabjeuref[index]->tabcoul[valeur];
		if('e'==write_data(game, card, 'c')) 
                  end_session(game);

	    }
	}
    }
    free(card);

}

void fill_list_game(game_t * game)
{
    position_t position;
    valeur_t valeur;
    color_t couleur;
    int index, nbcrt, nbcrtref;
    init_tabjeu(game);
    for (couleur = club; couleur < spade + 1; couleur++) {
	for (valeur = c2; valeur < pdc; valeur++) {
	    position = tab_cards[valeur][couleur].detenteur;
	    position = (position) % 4;
	    index = INDEX(position, couleur);
	    nbcrt = game->tabjeu[index]->nbcrt;
	    nbcrtref = game->tabjeuref[index]->nbcrt;
	    game->tabjeu[index]->tabcoul[nbcrt] = valeur;
	    game->tabjeuref[index]->tabcoul[nbcrtref] = valeur;
	    game->tabjeu[index]->nbcrt++;
	    game->tabjeuref[index]->nbcrt++;
	}
    }
    game->nbcard=NBPCOULEURS;

}
void distribution(void)
{
    int i = NBCARDS;
    int nouvellecard;
    position_t position;
    position = sud;
    initialisation();
    for (i = NBCARDS; i > 0; i--) {
	nouvellecard = tirage(NBCARDS);
	defcard(nouvellecard, position);

	position = (position + 1) % NBJOUEURS;
    }
}


/* Fonction de definition de l'ouverture */
/*conditionouverture(int i) {*/
/* Fonction de debut des encheres */
void init_distrib(game_t * game)
{
    int random = game->random;
    char phrase[255];
    bzero((char *) phrase, sizeof(char));
    if (game->debug)
	printf("randomdist =%d\n", random);
    //init_trick(trick,INIT);
    initialisation();
    //initstate (time (NULL), phrase, sizeof (phrase)); 
    if (random <= 0)
	initstate(time(NULL), phrase, sizeof(phrase));
    else
	initstate(random, phrase, sizeof(phrase));
    //initstate (0, phrase, sizeof (phrase));
    distribution();
    evalmain(game);
    //rotation=eval_contrat (game);
    fill_list_game(game);
}
