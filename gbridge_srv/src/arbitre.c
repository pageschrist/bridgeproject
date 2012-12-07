#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "distribution.h"
#include "stack.h"
#include "analyse.h"
#include "ia.h"
#include "arbitre.h"
extern coord_t tab_cartes[cA + 1][spade + 1];
/* Fonction qui liste tous les newcartes , soit on passe une couleur sinon on a le choix*/
/*trick.entame doit etre initialise */
void ftrace(void)
{
    printf("OK\n");
}
 
gboolean test_reprise(position_t position,tablist_t **t_jeu) {
  couleur_t color;
  for (color=club;color<spade+1;color++) {
    if(color==t_jeu[0]->couleureval)
      continue;
    else {
      if(t_jeu[INDEX(position,color)]->nbcrt!=0){
        if(t_jeu[INDEX(position,color)]->tabcoul[0]==cA){
          return TRUE;
        }
        else if (t_jeu[INDEX(position,color)]->tabcoul[0]==cR &&t_jeu[INDEX(position,color)]->tabcoul[1]==cD && t_jeu[INDEX(position,color)]->nbcrt>1) {
          return TRUE;
        }
        else if(t_jeu[INDEX(position,color)]->tabcoul[0]==cR && t_jeu[INDEX((position+1)%4,color)]->tabcoul[0]!=cA&&t_jeu[INDEX(position,color)]->nbcrt>1 ) {
          return TRUE;
        }
        else 
          return FALSE;

      }

    }
  }
  return FALSE;

  

}
gboolean check_invert_lead(trick_t *trick,tablist_t **t_jeu) {
  couleur_t coloreval=t_jeu[0]->couleureval; 
  if((trick->noj)%4==0) {
    if(t_jeu[INDEX(trick->nextpos,coloreval)]->nbcrt==0 && t_jeu[INDEX((trick->nextpos+2)%4,coloreval)]->nbcrt!=0) {
     
      if(test_reprise((trick->nextpos+2)%4,t_jeu)) {
        trick->nextpos=(trick->nextpos+2)%4;
        return(TRUE);
      }
    }     
  }
  return FALSE;

}

int calc_dist(gboolean *cardplayed,couleur_t color,valeur_t highest, valeur_t highest_player) {
  int dist;
  valeur_t val;

  dist=highest-highest_player;
  if(dist==0)
    return 0;
  for(val=highest;val>highest_player;val--) {
    if(cardplayed[INDCARD(color,val)]==TRUE)
      dist--;
  }
  return(dist);
}
valeur_t highest_value(gboolean *cardplayed,couleur_t color) {
  valeur_t value;
  for(value=cA;value>=c2+1;value--) {
    if( FALSE==cardplayed[INDCARD(color,value)])
      break;
  }
  return(value);
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
	    for (couleur = club; couleur < spade + 1; couleur++) {
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
	    for (couleur = club; couleur < spade + 1; couleur++) {
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
	 malloc((sizeof(tablist_t *)) * (est + 1) * (spade + 1)))) {
        perror("malloc");
	exit(EXIT_FAILURE);
    }
    if (NULL ==
	(thread_jeu->cardplayed =
	 malloc(sizeof( gboolean) * NBPCOULEURS*NBCOULEURS))) {
        perror("malloc");
	exit(EXIT_FAILURE);
    }
    memcpy(thread_jeu->cardplayed,game->cardplayed,sizeof( gboolean) * NBPCOULEURS*NBCOULEURS);
    for (position = sud; position < est + 1; position++) {
	for (couleur = club; couleur < spade + 1; couleur++) {
	    index = INDEX(position, couleur);
	    thread_jeu->t_jeu[index] = malloc(sizeof(tablist_t));
	    memcpy(thread_jeu->t_jeu[index], game->tabjeu[index],
		   sizeof(tablist_t));
	}
    }
}
void destroy_jeu(thread_jeu_t * thread_jeu)
{
    position_t position;
    couleur_t couleur;
    int index;
    for (position = sud; position < est + 1; position++) {
	for (couleur = club; couleur < spade + 1; couleur++) {
	    index = INDEX(position, couleur);
	    free(thread_jeu->t_jeu[index]);
	}
    }
    free(thread_jeu->t_jeu);
    free(thread_jeu->cardplayed);
}


int
list_all_coups_eval(position_t positionc, l_item_t *l_item, trick_t * trick,
		    tablist_t ** tmpjeu)
{
    couleur_t couleurc = tmpjeu[0]->couleureval;
    int index, situation, nbcoups = 0;
    int k = 0;
    couleur_t i;
    carte_t phcarte;
    int presence = NON, pos_index;
    if (positionc == trick->entame)
	situation = ENTAME;
    if (positionc != trick->entame) {
	situation = NONENTAME;
	if (tmpjeu[INDEX(positionc, couleurc)]->nbcrt == 0) {
	    situation = DISCARD;
	}
    }
    phcarte.nocarte = trick->phcarte.nocarte;
    phcarte.clcarte = trick->phcarte.clcarte;

    if (situation == DISCARD) {
	for (i = (couleurc + 1) % 4; (i) % 4 != couleurc; i = (i + 1) % 4) {
	    index = INDEX(positionc, i);
	    if (tmpjeu[index]->nbcrt != 0) {
		trick->carte[positionc].clcarte = i;
		trick->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
		nbcoups++;
                add_item_head(l_item, trick);
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
		    trick->carte[positionc].clcarte = i;
		    trick->carte[positionc].nocarte =
			tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		    trick->phcarte.clcarte = i;
		    trick->phcarte.nocarte =
			tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		    nbcoups++;
                    add_item_head(l_item, trick);
		    trick->phcarte.nocarte = phcarte.nocarte;
		    trick->phcarte.clcarte = phcarte.clcarte;
		}
	    } else {
		trick->carte[positionc].clcarte = i;
		trick->carte[positionc].nocarte =
		    tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		trick->phcarte.clcarte = i;
		trick->phcarte.nocarte =
		    tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		nbcoups++;
                add_item_head(l_item, trick);
		trick->phcarte.nocarte = phcarte.nocarte;
		trick->phcarte.clcarte = phcarte.clcarte;
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

	    trick->carte[positionc].clcarte = couleurc;
	    trick->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
            add_item_head(l_item, trick);
	    return (1);

	} else {
	    for (pos_index = 0; pos_index < tmpjeu[index]->nbcrt;
		 pos_index++) {

		trick->carte[positionc].clcarte = couleurc;
		trick->carte[positionc].nocarte =
		    tmpjeu[index]->tabcoul[pos_index];
		trick->phcarte.nocarte = tmpjeu[index]->tabcoul[pos_index];
		trick->phcarte.clcarte = couleurc;
		nbcoups++;
                add_item_head(l_item, trick);
	    }
	}
	return (nbcoups);
    }
    return (0);			//Normalement pas utilise
}

int points_per_color(tablist_t *tablist ) {
  int i;
  int nb=0;
  for(i=0;tablist->nbcrt&&i<NBPCOULEURS;i++) {
    switch( tablist->tabcoul[i] ) {

      case cA:
        nb=nb+4000;
        break;
      case cR:
        nb=nb+300;
        break;
      case cD:
        nb=nb+20;
        break;
      case cV:
        nb=nb+1;
        break;
      default:
        break;
    }
  
  }
  return(nb);
}

int
list_all_coups(position_t positionc, l_item_t *l_item, trick_t * trick,
	       tablist_t ** tmpjeu,gboolean *cardplayed)
{
    int index, situation, nbcoups = 0;
    int k = 0, sub,dist,kmem=0;
    valeur_t higher_value;
    couleur_t couleurc;
    couleur_t i;
    carte_t phcarte;
    int presence = NON, pos_index;
    if (positionc == trick->entame)
	situation = ENTAME;
    if (positionc != trick->entame) {
	situation = NONENTAME;
	couleurc = trick->carte[trick->entame].clcarte;
	if (tmpjeu[INDEX(positionc, couleurc)]->nbcrt == 0 ) {
	    situation = DISCARD; //DISCARD or TRUMP
	}
    }
    phcarte.nocarte = trick->phcarte.nocarte;
    phcarte.clcarte = trick->phcarte.clcarte;

    if (situation == DISCARD) {
	for (i = club; i < spade + 1; i++) {
	    index = INDEX(positionc, i);
	    if (tmpjeu[index]->nbcrt != 0) {
		if (i != trick->atout) {
                    points_per_color(tmpjeu[index]);
		    trick->carte[positionc].clcarte = i;
		    trick->carte[positionc].nocarte =
			tmpjeu[index]->tabcoul[0];
                  if(NULL==cardplayed || tmpjeu[index]->nbcrt ==1) {  //if tmpjeu[index]->nbcrt ==1 no choice
                    add_item_tail(l_item, trick);

                  }
                  else {
                    //correspond to the highest value not played
                    higher_value=highest_value(cardplayed,i);
                    dist=calc_dist(cardplayed,i,higher_value,tmpjeu[index]->tabcoul[0]);

                    if(dist+1<tmpjeu[index]->nbcrt) {
                      add_item_tail(l_item, trick);
                    }
                    else 
                      add_item_head(l_item, trick);    
                  }
		  nbcoups++;
		} else {
		    for (k = 0;  k<tmpjeu[index]->nbcrt; k++) {
                        if(k!=0) {
                          if(trick->carte[positionc].nocarte-(1+kmem)==tmpjeu[index]->tabcoul[k]) {
                            kmem++;
                            continue;
                          }
                        }
			trick->carte[positionc].clcarte = i;
			trick->carte[positionc].nocarte =
			    tmpjeu[index]->tabcoul[k];
			nbcoups++;
			add_item_head(l_item, trick);
		    }
		}
	    }
	}
	return (nbcoups);

    }


    if (situation == ENTAME) {

	for (i = club; i < spade + 1; i++) {
	    presence = NON;
	    index = INDEX(positionc, i);
	    k = tmpjeu[INDEX(positionc, i)]->nbcrt;
	    while (k != 0) {
		if (presence == OUI) {
		    /* On teste si la carte inférieure est équivalente */
		    if ((tmpjeu[INDEX(positionc, i)]->tabcoul[k] -
			 tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1]) != 1)
		    {
			trick->carte[positionc].clcarte = i;
			trick->carte[positionc].nocarte =
			    tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
			trick->phcarte.clcarte = i;
			trick->phcarte.nocarte =
			    tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
			if (trick->carte[positionc].nocarte == pdc) {
			    int h;
			    display_trick(trick,TRUE);
			    for (h = 0; h < NBJOUEURS; h++)
				//affiche_tabjeu(h);
				printf("Pb dans list_all_coups 1 \n");
			    printf("k=%d,presence=%d,positionc=%d,i=%d", k,
				   presence, positionc, i);
			    exit(3);
			}
			nbcoups++;
			//push(stack, trick);
                        add_item_head(l_item, trick);
			trick->phcarte.nocarte = phcarte.nocarte;
			trick->phcarte.clcarte = phcarte.clcarte;
		    }
		} else {
		    trick->carte[positionc].clcarte = i;
		    trick->carte[positionc].nocarte =
			tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		    trick->phcarte.clcarte = i;
		    trick->phcarte.nocarte =
			tmpjeu[INDEX(positionc, i)]->tabcoul[k - 1];
		    if (trick->carte[positionc].nocarte == pdc) {
			int h;
			display_trick(trick,TRUE);
			for (h = 0; h < NBJOUEURS; h++)
			    //affiche_tmpjeu(h);
			    printf("Pb dans list_all_coups 2\n");
			exit(3);
		    }
		    nbcoups++;
		    //push(stack, trick);
                    add_item_head(l_item, trick);
		    trick->phcarte.nocarte = phcarte.nocarte;
		    trick->phcarte.clcarte = phcarte.clcarte;
		    presence = OUI;


		}
		k--;
	    }
	}
	return (nbcoups);
    }




    else {
	couleurc = trick->carte[trick->entame].clcarte;	/*On recupere la couleur entame */
	presence = NON;

	index = INDEX(positionc, couleurc);

	if (tmpjeu[index]->nbcrt == 1) {	//Une seule carte 

	    trick->carte[positionc].clcarte = couleurc;
	    trick->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
	    //push(stack, trick);
            add_item_head(l_item, trick);
	    return (1);

	} else {

	    sub =
		sub_index(tmpjeu, positionc, couleurc,
			  trick->phcarte.nocarte);
	    if (sub == 0 && tmpjeu[index]->tabcoul[tmpjeu[index]->nbcrt - 1] < trick->phcarte.nocarte) {	//Toutes les cartes 
		trick->carte[positionc].clcarte = couleurc;
		trick->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
		//push(stack, trick);
                add_item_head(l_item, trick);
		return (1);
	    } else {
		trick->carte[positionc].clcarte = couleurc;
		trick->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
		//push(stack, trick);
                add_item_head(l_item, trick);
		nbcoups++;
		for (pos_index = sub + 1; pos_index < tmpjeu[index]->nbcrt;
		     pos_index++) {

		    trick->carte[positionc].clcarte = couleurc;
		    trick->carte[positionc].nocarte =
			tmpjeu[index]->tabcoul[pos_index];
		    trick->phcarte.nocarte =
			tmpjeu[index]->tabcoul[pos_index];
		    trick->phcarte.clcarte = couleurc;
		    nbcoups++;
		    //push(stack, trick);
                    add_item_head(l_item, trick);

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
position_t evaluation_trick(trick_t * trick)
{

    carte_t carte_atout;
    carte_atout.clcarte = trick->atout;
    carte_atout.nocarte = pdc;
    position_t postmp;
    position_t posgagnante;
    carte_t carteref;

    define_leader(trick); // We setup who  is winning
    /* On teste si on est le dernier joueur du trick */
    if (trick->noj == 3) {
	postmp = trick->entame;
	carteref.nocarte = trick->carte[trick->entame].nocarte;
	carteref.clcarte = trick->carte[trick->entame].clcarte;
	posgagnante = trick->entame;
	postmp = (postmp + 1) % 4;
	while (postmp != trick->entame) {
	    if (carte_atout.nocarte == pdc) {	//On n'a jamais joue atout
		if (trick->carte[postmp].clcarte == carteref.clcarte) {
		    if (trick->carte[postmp].nocarte > carteref.nocarte) {
			posgagnante = postmp;
			carteref.nocarte = trick->carte[postmp].nocarte;
		    }
		} else {
		    if ((trick->atout == trick->carte[postmp].clcarte)
			&& (trick->atout != aucune)) {
			posgagnante = postmp;
			carte_atout.nocarte = trick->carte[postmp].nocarte;
		    }

		}


	    } else {		//on a joue atout
		if ((trick->atout == trick->carte[postmp].clcarte)
		    && (trick->carte[postmp].nocarte >
			carte_atout.nocarte)) {
		    posgagnante = postmp;
		    carte_atout.nocarte = trick->carte[postmp].nocarte;
		}
	    }
	    postmp = (postmp + 1) % 4;
	}
	trick->entame = posgagnante;
	trick->nextpos = posgagnante;
	trick->nbtrick++;
	trick->nbtrick_line[(posgagnante % 2)]++;

	init_trick(trick, RESET);	/*on reinitialise le trick */
	//display_trick (trick);
    } else {
	if (trick->phcarte.nocarte == pdc) {
	    trick->phcarte.nocarte = trick->carte[trick->nextpos].nocarte;
	    trick->phcarte.clcarte = trick->carte[trick->nextpos].clcarte;
	} else {
	    if ((trick->phcarte.clcarte == trick->carte[trick->nextpos].clcarte)
		&& (trick->carte[trick->nextpos].nocarte >
		    trick->phcarte.nocarte)) {

		trick->phcarte.nocarte = trick->carte[trick->nextpos].nocarte;
		trick->phcarte.clcarte = trick->carte[trick->nextpos].clcarte;

	    }


	}

	trick->nextpos = (trick->nextpos + 1) % 4;
    }
    trick->noj = (trick->noj + 1) % 4;
    return (trick->nextpos);
}

int joue_coup(trick_t * trick, carte_t * carte, game_t * game)
{
    int posindex;
    position_t position;
    // NULL is used when  (IHM) plays , all the information is in trick
    if (carte != NULL) {
	position = trick->nextpos;
	trick->carte[position].nocarte = carte->nocarte;
	trick->carte[position].clcarte = carte->clcarte;
        game->cardplayed[INDCARD(carte->clcarte,carte->nocarte)]=TRUE;
	posindex =
	    find_index(game->tabjeu, position, carte->clcarte,
		       carte->nocarte);
	if (posindex == -1) {
	    printf("Pb in joue_coup1\n");
	    display_trick(trick,TRUE);
	    affiche_carte(carte);
	    return (0);
	}
	posindex =
	    remove_index(game->tabjeu, position, carte->clcarte, posindex);
    } else {
	position = trick->nextpos;
	posindex =
	    find_index(game->tabjeu, position,
		       trick->carte[position].clcarte,
		       trick->carte[position].nocarte);
	if (posindex == -1) {
	    printf("Pb in joue_coup2\n");
	    return (0);
	}
	posindex =
	    remove_index(game->tabjeu, position,
			 trick->carte[position].clcarte, posindex);
	if (trick->carte[position].clcarte ==
	    trick->carte[trick->entame].clcarte) {
	    if (trick->phcarte.nocarte < trick->carte[position].nocarte)
		trick->phcarte.nocarte = trick->carte[position].nocarte;
	}
        game->cardplayed[INDCARD(trick->carte[position].clcarte,trick->carte[position].nocarte)]=TRUE;
    }

    return (1);
}


rettrick_t *check_trick(trick_t * trick)
{
    rettrick_t *rettrick = malloc(sizeof(rettrick_t));
    ligne_t lignej;
    lignej = (IALINE + 1) % 2;
    rettrick->score = trick->nbtrick_line[lignej] - trick->nbtrick_line[IALINE];
    rettrick->nbline[lignej] = trick->nbtrick_line[lignej];
    rettrick->nbline[IALINE] = trick->nbtrick_line[IALINE];
    rettrick->alpha_or_beta = 0;
    /* printf ("Voici le score dans check_trick: %d\n", score); */
    return (rettrick);

}
void define_leader(trick_t *trick) {
  if(trick->entame==trick->nextpos) {
   trick->leader=trick->entame;
  }
  else {
    if(trick->atout!=aucune) {
      if(trick->carte[trick->leader].clcarte==trick->carte[trick->nextpos].clcarte) {
        if(trick->carte[trick->leader].nocarte<trick->carte[trick->nextpos].nocarte)
          trick->leader=trick->nextpos;
      }
      else {
        if(trick->carte[trick->nextpos].clcarte==trick->atout)
          trick->leader=trick->nextpos;
      }
    }
    else {
      if(trick->carte[trick->leader].clcarte==trick->carte[trick->nextpos].clcarte) {
        if(trick->carte[trick->leader].nocarte<trick->carte[trick->nextpos].nocarte)
          trick->leader=trick->nextpos;
      }

    }
  }

}
