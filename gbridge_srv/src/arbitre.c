#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "distribution.h"
#include "stack.h"
#include "analyse.h"
#include "ia.h"
#include "arbitre.h"
extern coord_t tab_cartes[cA + 1][pique + 1];
/* Fonction qui liste tous les newcartes , soit on passe une couleur sinon on a le choix*/
/*pli.entame doit etre initialise */
void ftrace(void)
{
    printf("OK\n");
}

void rotation(game_t * game, hopestat_t ** hopestat)
{
    int i;
    position_t position;
    couleur_t couleur;
    char trump, *res;
    int rot;

    for (i = strlen(game->cur_bid) - 1; i > 0; i = i - 2) {

	if (game->cur_bid[i - 1] != 'P') {
	    trump = game->cur_bid[i];
	    break;

	}
    }

    res = strchr(game->cur_bid, trump);
    rot = ((res - game->cur_bid - 1) / 2) % 4;
    if (game->debug)
	printf("trump:%c n=%d\n", trump, rot);
    if (rot != 0) {
	tablist_t **tmpjeu = malloc(4 * sizeof(tablist_t *));
	hopestat_t **tmphope = malloc(4 * sizeof(hopestat_t *));
	for (position = sud; position < ouest + 1; position++) {
	    for (couleur = trefle; couleur < pique + 1; couleur++) {
		tmpjeu[couleur] = malloc(sizeof(tablist_t));
		memcpy(tmpjeu[couleur],
		       game->tabjeu[INDEX((position + rot) % 4, couleur)],
		       sizeof(tablist_t));
		memcpy(game->tabjeu[INDEX((position + rot) % 4, couleur)],
		       game->tabjeu[INDEX(position, couleur)],
		       sizeof(tablist_t));
		memcpy(game->
		       tabjeuref[INDEX((position + rot) % 4, couleur)],
		       game->tabjeuref[INDEX(position, couleur)],
		       sizeof(tablist_t));
		memcpy(game->tabjeu[INDEX(position, couleur)],
		       tmpjeu[couleur], sizeof(tablist_t));
		memcpy(game->tabjeuref[INDEX(position, couleur)],
		       tmpjeu[couleur], sizeof(tablist_t));
		free(tmpjeu[couleur]);
	    }
	}
	free(tmpjeu);
	for (position = sud; position < ouest + 1; position++) {
	    for (couleur = trefle; couleur < pique + 1; couleur++) {
		tmphope[couleur] = malloc(sizeof(hopestat_t));
		memcpy(tmphope[couleur],
		       hopestat[INDEX((position + rot) % 4, couleur)],
		       sizeof(hopestat_t));
		memcpy(hopestat[INDEX((position + rot) % 4, couleur)],
		       hopestat[INDEX(position, couleur)],
		       sizeof(hopestat_t));
		memcpy(hopestat[INDEX(position, couleur)],
		       tmphope[couleur], sizeof(hopestat_t));
		free(tmphope[couleur]);
	    }
	}
	free(tmphope);
    }
    if(game->debug)
    	affiche_tabjeu_c(game->tabjeu);



}


void dup_game(thread_jeu_t * thread_jeu, game_t * game)
{
    position_t position;
    couleur_t couleur;
    int index;
    if (NULL ==
	(thread_jeu->t_jeu =
	 malloc((sizeof(tablist_t *)) * (est + 1) * (pique + 1)))) {
	fprintf(stderr, "Probleme avec malloc\n");
	exit(EXIT_FAILURE);
    }
    for (position = sud; position < est + 1; position++) {
	for (couleur = trefle; couleur < pique + 1; couleur++) {
	    index = INDEX(position, couleur);
	    thread_jeu->t_jeu[index] = malloc(sizeof(tablist_t));
	    memcpy(thread_jeu->t_jeu[index], game->tabjeu[index],
		   sizeof(tablist_t));
	    //for (i = 0; i < 13; i++)
	    //  thread_jeu->t_jeu[index]->tabcoul[i]=game->tabjeu[index]->tabcoul[i] ;
	    // thread_jeu->t_jeu[index]->nbcrt =game->tabjeu[index]->nbcrt;
	    // thread_jeu->t_jeu[index]->couleureval =game->tabjeu[index]->couleureval;
	    // thread_jeu->t_jeu[index]->debug=game->tabjeu[index]->debug;
	}
    }
}
void destroy_jeu(thread_jeu_t * thread_jeu)
{
    position_t position;
    couleur_t couleur;
    int index;
    for (position = sud; position < est + 1; position++) {
	for (couleur = trefle; couleur < pique + 1; couleur++) {
	    index = INDEX(position, couleur);
	    free(thread_jeu->t_jeu[index]);
	}
    }
    free(thread_jeu->t_jeu);
}


int
list_all_coups_eval(position_t positionc, stackia_t stack, pli_t * pli,
		    tablist_t ** tmpjeu)
{
    couleur_t couleurc = tmpjeu[0]->couleureval;
    int index, situation, nbcoups = 0;
    int k = 0;
    couleur_t i;
    carte_t phcarte;
    int presence = NON, pos_index;
    if (positionc == pli->entame)
	situation = ENTAME;
    if (positionc != pli->entame) {
	situation = NONENTAME;
	if (tmpjeu[INDEX(positionc, couleurc)]->nbcrt == 0) {
	    situation = DEFAUSSE;
	}
    }
    phcarte.nocarte = pli->phcarte.nocarte;
    phcarte.clcarte = pli->phcarte.clcarte;

    if (situation == DEFAUSSE) {
	for (i = (couleurc + 1) % 4; (i) % 4 != couleurc; i = (i + 1) % 4) {
	    index = INDEX(positionc, i);
	    if (tmpjeu[index]->nbcrt != 0) {
		pli->carte[positionc].clcarte = i;
		pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
		nbcoups++;
		push(stack, pli);
		break;
	    }
	}
	return (nbcoups);

    }


    if (situation == ENTAME) {
	/*On est a l'entame d'une couleur, donc on choisit n'importe quelle carte et on place la phcarte dans la structure */
	i = couleurc;
	presence = NON;
	index = INDEX(positionc, i);
	k = tmpjeu[INDEX(positionc, i)]->nbcrt;
	while (k != 0) {
	    if (presence == OUI) {
		/* On teste si la carte inférieure est équivalente */
		if ((tmpjeu[INDEX(positionc, i)]->tabcoul[k] -
		     tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1]) != 1) {
		    pli->carte[positionc].clcarte = i;
		    pli->carte[positionc].nocarte =
			tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		    pli->phcarte.clcarte = i;
		    pli->phcarte.nocarte =
			tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		    nbcoups++;
		    push(stack, pli);
		    pli->phcarte.nocarte = phcarte.nocarte;
		    pli->phcarte.clcarte = phcarte.clcarte;
		}
	    } else {
		pli->carte[positionc].clcarte = i;
		pli->carte[positionc].nocarte =
		    tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		pli->phcarte.clcarte = i;
		pli->phcarte.nocarte =
		    tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		nbcoups++;
		push(stack, pli);
		pli->phcarte.nocarte = phcarte.nocarte;
		pli->phcarte.clcarte = phcarte.clcarte;
		presence = OUI;


	    }
	    k--;
	}
	return (nbcoups);
    }




    else {
	presence = NON;


	index = INDEX(positionc, couleurc);

	if (tmpjeu[index]->nbcrt == 1) {	//Une seule carte 

	    pli->carte[positionc].clcarte = couleurc;
	    pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
	    push(stack, pli);
	    return (1);

	} else {
	    for (pos_index = 0; pos_index < tmpjeu[index]->nbcrt;
		 pos_index++) {

		pli->carte[positionc].clcarte = couleurc;
		pli->carte[positionc].nocarte =
		    tmpjeu[index]->tabcoul[pos_index];
		pli->phcarte.nocarte = tmpjeu[index]->tabcoul[pos_index];
		pli->phcarte.clcarte = couleurc;
		nbcoups++;
		push(stack, pli);

	    }

	}

	return (nbcoups);
    }
    return (0);			//Normalement pas utilise


}

int
list_all_coups(position_t positionc, stackia_t stack, pli_t * pli,
	       tablist_t ** tmpjeu)
{
    int index, situation, nbcoups = 0;
    int k = 0, sub;
    couleur_t couleurc;
    couleur_t i;
    carte_t phcarte;
    int presence = NON, pos_index;
    if (positionc == pli->entame)
	situation = ENTAME;
    if (positionc != pli->entame) {
	situation = NONENTAME;
	couleurc = pli->carte[pli->entame].clcarte;
	if (tmpjeu[INDEX(positionc, couleurc)]->nbcrt == 0) {
	    situation = DEFAUSSE;
	}
    }
    phcarte.nocarte = pli->phcarte.nocarte;
    phcarte.clcarte = pli->phcarte.clcarte;

    if (situation == DEFAUSSE) {
    
	for (i = trefle; i < pique + 1; i++) {
	    index = INDEX(positionc, i);
            if(tmpjeu[index]->debug)
              ftrace();
	    if (tmpjeu[index]->nbcrt != 0) {
		if (i != pli->atout) {
		    pli->carte[positionc].clcarte = i;
		    pli->carte[positionc].nocarte =
			tmpjeu[index]->tabcoul[tmpjeu[index]->nbcrt-1];
		    nbcoups++;
		    push(stack, pli);
		} else {
		    for (k = 0; k < tmpjeu[index]->nbcrt; k++) {
			pli->carte[positionc].clcarte = i;
			pli->carte[positionc].nocarte =
			    tmpjeu[index]->tabcoul[k];
			nbcoups++;
			push(stack, pli);
		    }
		}

	    }






	}
	return (nbcoups);

    }


    if (situation == ENTAME) {
	/*On est a l'entame d'une couleur, donc on choisit n'importe quelle carte et on place la phcarte dans la structure */

	for (i = trefle; i < pique + 1; i++) {
	    presence = NON;
	    index = INDEX(positionc, i);
	    k = tmpjeu[INDEX(positionc, i)]->nbcrt;
	    while (k != 0) {
		if (presence == OUI) {
		    /* On teste si la carte inférieure est équivalente */
		    if ((tmpjeu[INDEX(positionc, i)]->tabcoul[k] -
			 tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1]) != 1)
		    {
			pli->carte[positionc].clcarte = i;
			pli->carte[positionc].nocarte =
			    tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
			pli->phcarte.clcarte = i;
			pli->phcarte.nocarte =
			    tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
			if (pli->carte[positionc].nocarte == pdc) {
			    int h;
			    affiche_pli(pli);
			    for (h = 0; h < NBJOUEURS; h++)
				//affiche_tabjeu(h);
				printf("Pb dans list_all_coups 1 \n");
			    printf("k=%d,presence=%d,positionc=%d,i=%d", k,
				   presence, positionc, i);
			    exit(3);
			}
			nbcoups++;
			push(stack, pli);
			pli->phcarte.nocarte = phcarte.nocarte;
			pli->phcarte.clcarte = phcarte.clcarte;
		    }
		} else {
		    pli->carte[positionc].clcarte = i;
		    pli->carte[positionc].nocarte =
			tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		    pli->phcarte.clcarte = i;
		    pli->phcarte.nocarte =
			tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		    if (pli->carte[positionc].nocarte == pdc) {
			int h;
			affiche_pli(pli);
			for (h = 0; h < NBJOUEURS; h++)
			    //affiche_tmpjeu(h);
			    printf("Pb dans list_all_coups 2\n");
			exit(3);
		    }
		    nbcoups++;
		    push(stack, pli);
		    pli->phcarte.nocarte = phcarte.nocarte;
		    pli->phcarte.clcarte = phcarte.clcarte;
		    presence = OUI;


		}
		k--;
	    }
	}
	return (nbcoups);
    }




    else {
	couleurc = pli->carte[pli->entame].clcarte;	/*On recupere la couleur entame */
	presence = NON;

	index = INDEX(positionc, couleurc);

	if (tmpjeu[index]->nbcrt == 1) {	//Une seule carte 

	    pli->carte[positionc].clcarte = couleurc;
	    pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
	    push(stack, pli);
	    return (1);

	} else {

	    sub =
		sub_index(tmpjeu, positionc, couleurc,
			  pli->phcarte.nocarte);
	    if (sub == 0 && tmpjeu[index]->tabcoul[tmpjeu[index]->nbcrt - 1] < pli->phcarte.nocarte) {	//Toutes les cartes 
		pli->carte[positionc].clcarte = couleurc;
		pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
		push(stack, pli);
		return (1);
	    } else {
		pli->carte[positionc].clcarte = couleurc;
		pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
		push(stack, pli);
		nbcoups++;
		for (pos_index = sub + 1; pos_index < tmpjeu[index]->nbcrt;
		     pos_index++) {

		    pli->carte[positionc].clcarte = couleurc;
		    pli->carte[positionc].nocarte =
			tmpjeu[index]->tabcoul[pos_index];
		    pli->phcarte.nocarte =
			tmpjeu[index]->tabcoul[pos_index];
		    pli->phcarte.clcarte = couleurc;
		    nbcoups++;
		    push(stack, pli);
		    //while(pos_index<tmpjeu[index]->nbcrt-1) {
		    //  if(tmpjeu[index]->tabcoul[pos_index+1]-tmpjeu[index]->tabcoul[pos_index] == 1) {
		    //    pos_index++;
		    // }
		    //}

		}
	    }

	}



	return (nbcoups);
    }
    return (0);			//Normalement pas utilise


}

void *copy_carte(void *data)
{
    carte_t *carte;

    carte = malloc(sizeof(carte_t));

    (*carte).nocarte = ((carte_t *) data)->nocarte;
    (*carte).clcarte = ((carte_t *) data)->clcarte;

    return ((void *) carte);
}

/*Cette fonction determine qui sera le suivant a jouer et met a jour les scores*/
position_t evaluation_pli(pli_t * pli)
{

    carte_t carte_atout;
    carte_atout.clcarte = pli->atout;
    carte_atout.nocarte = pdc;
    position_t postmp;
    position_t posgagnante;
    carte_t carteref;
    /* On teste si on est le dernier joueur du pli */
    if (pli->noj == 3) {
	postmp = pli->entame;
	carteref.nocarte = pli->carte[pli->entame].nocarte;
	carteref.clcarte = pli->carte[pli->entame].clcarte;
	posgagnante = pli->entame;
	postmp = (postmp + 1) % 4;
	while (postmp != pli->entame) {
	    if (carte_atout.nocarte == pdc) {	//On n'a jamais joue atout
		if (pli->carte[postmp].clcarte == carteref.clcarte) {
		    if (pli->carte[postmp].nocarte > carteref.nocarte) {
			posgagnante = postmp;
			carteref.nocarte = pli->carte[postmp].nocarte;
		    }
		} else {
		    if ((pli->atout == pli->carte[postmp].clcarte)
			&& (pli->atout != aucune)) {
			posgagnante = postmp;
			carte_atout.nocarte = pli->carte[postmp].nocarte;
		    }

		}


	    } else {		//on a joue atout
		if ((pli->atout == pli->carte[postmp].clcarte)
		    && (pli->carte[postmp].nocarte >
			carte_atout.nocarte)) {
		    posgagnante = postmp;
		    carte_atout.nocarte = pli->carte[postmp].nocarte;
		}
	    }
	    postmp = (postmp + 1) % 4;
	}
	pli->entame = posgagnante;
	pli->nextpos = posgagnante;
	pli->nopli++;
	pli->nbpli_ligne[(posgagnante % 2)]++;

	init_pli(pli, RESET);	/*on reinitialise le pli */
	//affiche_pli (pli);
    } else {
	if (pli->phcarte.nocarte == pdc) {
	    pli->phcarte.nocarte = pli->carte[pli->nextpos].nocarte;
	    pli->phcarte.clcarte = pli->carte[pli->nextpos].clcarte;
	} else {
	    if ((pli->phcarte.clcarte == pli->carte[pli->nextpos].clcarte)
		&& (pli->carte[pli->nextpos].nocarte >
		    pli->phcarte.nocarte)) {

		pli->phcarte.nocarte = pli->carte[pli->nextpos].nocarte;
		pli->phcarte.clcarte = pli->carte[pli->nextpos].clcarte;

	    }


	}

	pli->nextpos = (pli->nextpos + 1) % 4;
    }
    pli->noj = (pli->noj + 1) % 4;
    return (pli->nextpos);
}

int joue_coup(pli_t * pli, carte_t * carte, game_t * game)
{
    int posindex;
    position_t position;
    // NULL correspond au joue ( IHM) tout est dans pli
    if (carte != NULL) {
	position = pli->nextpos;
	pli->carte[position].nocarte = carte->nocarte;
	pli->carte[position].clcarte = carte->clcarte;
	posindex =
	    find_index(game->tabjeu, position, carte->clcarte,
		       carte->nocarte);
	if (posindex == -1) {
	    printf("Pb in joue_coup1\n");
	    affiche_pli(pli);
	    affiche_carte(carte);
	    return (0);
	}
	posindex =
	    remove_index(game->tabjeu, position, carte->clcarte, posindex);
    } else {
	position = pli->nextpos;
	posindex =
	    find_index(game->tabjeu, position,
		       pli->carte[position].clcarte,
		       pli->carte[position].nocarte);
	if (posindex == -1) {
	    printf("Pb in joue_coup2\n");
	    return (0);
	}
	posindex =
	    remove_index(game->tabjeu, position,
			 pli->carte[position].clcarte, posindex);
	if (pli->carte[position].clcarte ==
	    pli->carte[pli->entame].clcarte) {
	    if (pli->phcarte.nocarte < pli->carte[position].nocarte)
		pli->phcarte.nocarte = pli->carte[position].nocarte;
	}
    }

    return (1);
}


retpli_t *check_plis(pli_t * pli)
{
    retpli_t *retpli = malloc(sizeof(retpli_t));
    ligne_t lignej;
    lignej = (IALINE + 1) % 2;
    retpli->score = pli->nbpli_ligne[lignej] - pli->nbpli_ligne[IALINE];
    retpli->nbline[lignej] = pli->nbpli_ligne[lignej];
    retpli->nbline[IALINE] = pli->nbpli_ligne[IALINE];
    retpli->alpha_or_beta = 0;
    /* printf ("Voici le score dans check_plis: %d\n", score); */
    return (retpli);

}
