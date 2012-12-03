#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "stack.h"
#include "distribution.h"
#include "analyse.h"
#include "ia.h"
#include "arbitre.h"


int prof_start = 0;
int flag_debug = 0;

int  best_elem_best_l(elem_best_t *elem_best,trick_t *pli) {
  char *affca,*affcl;
  int score;
  score=-15*((pli->nextpos+1)%2) +(15*((pli->nextpos)%2));
  while (elem_best) {
    if((pli->nextpos)%2==1) {
      if(elem_best->best->score<score)
        score=elem_best->best->score;
    }
    else {
      if(elem_best->best->score>score)
        score=elem_best->best->score;
    }
    affca=affichage(elem_best->best->carte->nocarte,CARTE),
    affcl=affichage(elem_best->best->carte->clcarte,COULEUR),
    printf ("card=%s%s score=%d nbline[IALINE]=%d nbline[IALINE+1]=%d \n",
                     affca,affcl,
		     elem_best->best->score,
		     elem_best->best->nbline[IALINE],
		     elem_best->best->nbline[(IALINE+1)%2]);
   free(affca);
   free(affcl);
   elem_best = elem_best->next;
   }
   printf("best_elem_score=%d\n",score);
   return(score);
}
//void display_l_item_l(l_item_t *l_item) {
 // while(
//}
carte_t *best_choice(int *nb_best, l_best_t * l_best, game_t * game,
		    hopestat_t * hopestat,trick_t *pli)
{
    gboolean suplead[spade+1];
    valeur_t tabval[spade + 1];
    l_item_t *l_item=create_l_item(copy_card);
    carte_t *carte = malloc(sizeof(carte_t));
    unsigned int jline = (IALINE + 1) % 2;

    couleur_t i;
    int score_ref;
    elem_best_t *elem_best = l_best->first;
    elem_best_t *disp_elem_best = l_best->first;
    for (i = 0; i < spade + 1; i++)
	tabval[i] = pdc;
    for (i = 0; i < spade + 1; i++)
	suplead[i] = TRUE;
    if(pli)
      score_ref = best_elem_best_l(disp_elem_best,pli);
    else 
      score_ref = l_best->first->best->score;

    //score_ref = l_best->first->best->score;
    
    if (game->debug)
	printf("On a %d bons coups\n", *nb_best);
    if(*nb_best==1) {

      carte->clcarte=elem_best->best->carte->clcarte;
      carte->nocarte=elem_best->best->carte->nocarte;
      clean_l_item(l_item);
      return (carte);
    }


    while (elem_best) {
	if (elem_best->best->score == score_ref) {
	    if (hopestat) {
		hopestat->score = elem_best->best->score;
		hopestat->nbline[IALINE] = elem_best->best->nbline[IALINE];
		hopestat->nbline[jline] = elem_best->best->nbline[jline];
                hopestat->aff=max(game->tabjeu[INDEX(hopestat->position, hopestat->couleur)]->nbcrt,game->tabjeu[INDEX(((hopestat->position+2)%4), hopestat->couleur)]->nbcrt);
                hopestat->aff=hopestat->aff-(elem_best->best->nbline[jline]+elem_best->best->nbline[IALINE]);

	    }
            if(pli &&(pli->nextpos==(pli->entame+3)%4 &&(pli->leader)%2==jline )) {  
              if(pli->atout!=aucune) {
                if(pli->carte[pli->leader].clcarte==elem_best->best->carte->clcarte) {
                  if(pli->carte[pli->leader].nocarte<elem_best->best->carte->nocarte) {
                    tabval[elem_best->best->carte->clcarte] = elem_best->best->carte->nocarte;
                    suplead[elem_best->best->carte->clcarte]=FALSE;
                  }
                }
                else {
                  if(elem_best->best->carte->clcarte==pli->atout) {
                    tabval[elem_best->best->carte->clcarte] = elem_best->best->carte->nocarte;
                    suplead[elem_best->best->carte->clcarte]=FALSE;
                  }
                }
              }
              else {
                if(pli->carte[pli->leader].clcarte==elem_best->best->carte->clcarte) {
                  if(pli->carte[pli->leader].nocarte<elem_best->best->carte->nocarte) {
                    tabval[elem_best->best->carte->clcarte] = elem_best->best->carte->nocarte;
                    suplead[elem_best->best->carte->clcarte]=FALSE;
                  }
                }
              }
              if(suplead[elem_best->best->carte->clcarte]) {
  	        if (tabval[elem_best->best->carte->clcarte] > elem_best->best->carte->nocarte)
  	          tabval[elem_best->best->carte->clcarte] = elem_best->best->carte->nocarte;

              }


            }
            else {
  	      if (tabval[elem_best->best->carte->clcarte] > elem_best->best->carte->nocarte)
  	      tabval[elem_best->best->carte->clcarte] = elem_best->best->carte->nocarte;
            }
          carte->clcarte=elem_best->best->carte->clcarte;
          carte->nocarte=tabval[elem_best->best->carte->clcarte];
          if(carte->nocarte!=pdc)
            add_item_tail(l_item,carte);
	}
	elem_best = elem_best->next;
    }
    carte=pop_item_tail(l_item);
    clean_l_item(l_item);
    return (carte);

}

int minimax(int score_courant, int best_score, position_t positionc)
{
    if (positionc % 2 == IALINE) {
	if (score_courant <= best_score)
	    return (1);
	else
	    return (0);
    } else {
	if (score_courant >= best_score)
	    return (1);
	else
	    return (0);

    }


}
rettrick_t *cur_explore(int prof, trick_t * pli_cur, int prof_max,
		      tablist_t ** t_jeu, int orialpha, int oribeta,int nbcard)
{
    //stackia_t stk;
    l_item_t *l_item;
    int alpha = orialpha;
    int beta = oribeta;
    position_t positionc;
    int pos_index;
    trick_t *pli_new;		/* nouveau pli renvoye par la liste des coups */
    rettrick_t *ret;
    rettrick_t *retup;
    int best_score,  best_nbline[eo + 1];
    couleur_t nocouleur;
    valeur_t nocarte;

    positionc = pli_cur->nextpos;	/*C'est la nouvelle position */
    /*choix du joueur qui joue en fonction de la profondeur  */
    best_score =
	100000 * ((positionc) % 2) - 100000 * ((positionc + 1) % 2);
    if ((prof == prof_max) || (pli_cur->nbtrick == nbcard)) {
	retup = check_trick(pli_cur);
	return (retup);
    }
    l_item=create_l_item(duplicate_trick);
    list_all_coups(positionc, l_item, pli_cur, t_jeu,NULL);

    // tant que la pile des coups n'est pas vide on joue le coup dépilé
    while ((pli_new = (trick_t *) pop_item_head(l_item)) != NULL) {

	nocarte = pli_new->carte[positionc].nocarte;
	nocouleur = pli_new->carte[positionc].clcarte;
	if (nocarte == pdc) {
	    printf("Erreur dans explore\n");
	    exit(2);
	}
	//On joue le coup
	pos_index = find_index(t_jeu, positionc, nocouleur, nocarte);
	if (pos_index == -1) {
	    fprintf(stderr, "Pb in cur_explore \n");
	    exit(1);
	}
	pos_index = remove_index(t_jeu, positionc, nocouleur, pos_index);
	evaluation_trick(pli_new);	/*on incremente nextpos ou on fixe la prochaine entame et nbpli++ */

	//on duplique le jeu

	ret = cur_explore(prof + 1, pli_new, prof_max, t_jeu, alpha, beta,nbcard);
	if ((prof != prof_start) && (prof != prof_max - 1)) {
	    if (positionc % 2 == 1)
		beta = ret->alpha_or_beta;
	    else
		alpha = ret->alpha_or_beta;
	}
	if (minimax(ret->score, best_score, positionc)) {
	    best_score = ret->score;
	    best_nbline[0] = ret->nbline[0];
	    best_nbline[1] = ret->nbline[1];


	}

	free(ret);


	// On annule le coup joue
	if (insert_index(t_jeu, positionc, nocouleur, nocarte, pos_index)
	    == 0) {
	    fprintf(stderr, "Pb dans insert_index\n");
	    exit(EXIT_FAILURE);
	}



	/* on efface l'element de la pile */
	free(pli_new);

	//coupe alpha/beta
	if (positionc % 2 == 1 && (best_score <= alpha)
	    && (prof != prof_start))
	    break;
	if (positionc % 2 == 0 && (best_score >= beta)
	    && (prof != prof_start))
	    break;
    }

    //vidage(stk);
    clean_l_item(l_item);

    retup = malloc(sizeof(rettrick_t));
    retup->score = best_score;
    retup->nbline[0] = best_nbline[0];
    retup->nbline[1] = best_nbline[1];
    if (positionc % 2 == 1) {
	if (alpha < best_score) {
	    retup->alpha_or_beta = alpha;
	} else {
	    retup->alpha_or_beta = beta;
	}
    } else {
	if (beta > best_score)
	    retup->alpha_or_beta = best_score;
	else
	    retup->alpha_or_beta = beta;
    }
    return (retup);


}

rettrick_t *cur_explore_eval(int prof, trick_t * pli_cur, int prof_max,
			   tablist_t ** t_jeu, int orialpha, int oribeta,int nbcard)
{
    //stackia_t stk;
    l_item_t *l_item;
    int nbcoups;
    int alpha = orialpha;
    int beta = oribeta;
    position_t positionc;
    int pos_index;
    trick_t *pli_new;		/* nouveau pli renvoye par la liste des coups */
    rettrick_t *ret;
    rettrick_t *retup;
    int best_score,  best_nbline[eo + 1];
    couleur_t nocouleur;
    valeur_t nocarte;
    positionc = pli_cur->nextpos;	/*C'est la nouvelle position */
    best_score =
	100000 * ((positionc) % 2) - 100000 * ((positionc + 1) % 2);
    //stk = create_stack(duplicate_trick);
    l_item=create_l_item(duplicate_trick);
    if (t_jeu[0]->couleureval == aucune)
	nbcoups = list_all_coups(positionc, l_item, pli_cur, t_jeu,NULL);
    else
	nbcoups = list_all_coups_eval(positionc, l_item, pli_cur, t_jeu);
    if ((nbcoups == 0) && (t_jeu[0]->couleureval != aucune))
	prof_max = prof;
    if ((prof == prof_max) || (pli_cur->nbtrick == nbcard)) {

	retup = check_trick(pli_cur);
	return (retup);
    }
    // tant que la pile des coups n'est pas vide on joue le coup dépilé
    while ((pli_new = (trick_t *) pop_item_head(l_item)) != NULL) {

	nocarte = pli_new->carte[positionc].nocarte;
	nocouleur = pli_new->carte[positionc].clcarte;
	if (nocarte == pdc) {
	    printf("Erreur dans explore\n");
	    exit(2);
	}
	//On joue le coup
	pos_index = find_index(t_jeu, positionc, nocouleur, nocarte);
	if (pos_index == -1) {
	    printf("Pb in cur_explore \n");
	    exit(1);
	}
	pos_index = remove_index(t_jeu, positionc, nocouleur, pos_index);
	evaluation_trick(pli_new);	/*on incremente nextpos ou on fixe la prochaine entame et nbpli++ */
        check_invert_lead(pli_new,t_jeu); //If we have no more cards we change the lead if we have a reprise

	ret =
	    cur_explore_eval(prof + 1, pli_new, prof_max, t_jeu, alpha,
			     beta,nbcard);
	if ((prof != prof_start) && (prof != prof_max - 1)) {
	    if (positionc % 2 == 1)
		beta = ret->alpha_or_beta;
	    else
		alpha = ret->alpha_or_beta;
	}
	if (minimax(ret->score, best_score, positionc)) {
	    best_score = ret->score;
	    best_nbline[0] = ret->nbline[0];
	    best_nbline[1] = ret->nbline[1];


	}

	free(ret);


	// On annule le coup joue
	if (insert_index(t_jeu, positionc, nocouleur, nocarte, pos_index)
	    == 0) {
	    fprintf(stderr, "Pb dans insert_index\n");
	    exit(EXIT_FAILURE);
	}



	/* on efface l'element de la pile */
	free(pli_new);

	//coupe alpha/beta
	if (positionc % 2 == 1 && (best_score <= alpha)
	    && (prof != prof_start))
	    break;
	if (positionc % 2 == 0 && (best_score >= beta)
	    && (prof != prof_start))
	    break;
    }
    clean_l_item(l_item);
    //vidage(stk);

    retup = malloc(sizeof(rettrick_t));
    retup->score = best_score;
    retup->nbline[0] = best_nbline[0];
    retup->nbline[1] = best_nbline[1];
    if (positionc % 2 == 1) {
	if (alpha < best_score) {
	    retup->alpha_or_beta = alpha;
	} else {
	    retup->alpha_or_beta = beta;
	}
    } else {
	if (beta > best_score)
	    retup->alpha_or_beta = best_score;
	else
	    retup->alpha_or_beta = beta;
    }
    return (retup);


}




void *new_explore(void *arg)
{
    int prof, prof_max;
    trick_t *pli_cur;
    thread_jeu_t *thread_jeu = arg;
    //stackia_t stk;
    l_item_t *l_item;

    position_t positionc;
    int curscore;
    rettrick_t *ret;
    rettrick_t *rettmp;
    int pos_index;
    trick_t *pli_new;		/* nouveau pli renvoye par la liste des coups */
    int best_score,  best_nbline[eo + 1];
    couleur_t nocouleur;
    valeur_t nocarte;
    //reconstitution des varaiables
    prof = thread_jeu->prof;
    prof_max = thread_jeu->prof_max;
    pli_cur = thread_jeu->pli;
    if (thread_jeu->t_jeu[0]->debug)
	printf("A thread is started with prof=%d, prof_max=%d\n", prof,
	       prof_max);
    positionc = pli_cur->nextpos;	/*New position */
    /*choix du joueur qui joue en fonction de la profondeur  */
    best_score =
	100000 * ((positionc) % 2) - 100000 * ((positionc + 1) % 2);
    if ((prof == prof_max) || (pli_cur->nbtrick == thread_jeu->nbcard)) {
	rettmp = check_trick(pli_cur);
        printf("new_explore: score=%d\n",rettmp->score);
	thread_jeu->score = rettmp->score;
	thread_jeu->nbline[0] = rettmp->nbline[0];
	thread_jeu->nbline[1] = rettmp->nbline[1];
	thread_jeu->status = 1;
	free(rettmp);
	if (thread_jeu->t_jeu[0]->debug) {
	    display_trick(pli_cur,TRUE);
	    fprintf(stderr, "End newplore, prof=%d pli_cur->nbtrick=%d\n",
		    prof, pli_cur->nbtrick);
	}
    }

    //stk = create_stack(duplicate_trick);
    l_item=create_l_item(duplicate_trick);
    if (thread_jeu->t_jeu[0]->couleureval == aucune)
	list_all_coups(positionc, l_item, pli_cur, thread_jeu->t_jeu,thread_jeu->cardplayed);
    else
	list_all_coups_eval(positionc, l_item, pli_cur, thread_jeu->t_jeu);

    // tant que la pile des coups n'est pas vide on joue le coup dépilé
    while ((pli_new = (trick_t *) pop_item_head(l_item)) != NULL) {

	// On sauvegarde le pli en cours (pliori,plicopie) 
	//tab_cartes[pli_new->carte[positionc].nocarte][pli_new->carte[positionc].
	//                                          clcarte].detenteur;
	nocarte = pli_new->carte[positionc].nocarte;
	nocouleur = pli_new->carte[positionc].clcarte;
	if (nocarte == pdc) {
	    printf("Erreur dans explore\n");
	    exit(2);
	}
	//On joue le coup
	pos_index =
	    find_index(thread_jeu->t_jeu, positionc, nocouleur, nocarte);
	if (pos_index == -1) {
	    printf("Pb in new_explore \n");
	    exit(1);
	}
	pos_index =
	    remove_index(thread_jeu->t_jeu, positionc, nocouleur,
			 pos_index);
	evaluation_trick(pli_new);	/*on incremente nextpos ou on fixe la prochaine entame et nbpli++ */
	if (thread_jeu->t_jeu[0]->couleureval != aucune)
	    ret =
		cur_explore_eval(prof + 1, pli_new, prof_max,
				 thread_jeu->t_jeu, ALPHA_START,
				 BETA_START,thread_jeu->nbcard);
	else
	    ret =
		cur_explore(prof + 1, pli_new, prof_max, thread_jeu->t_jeu,
			    ALPHA_START, BETA_START,thread_jeu->nbcard);
	curscore = ret->score;
	if (minimax(curscore, best_score, positionc)) {
	    best_score = curscore;
	    best_nbline[0] = ret->nbline[0];
	    best_nbline[1] = ret->nbline[1];
	}
	free(ret);


	// On annule le coup joue
	if (insert_index
	    (thread_jeu->t_jeu, positionc, nocouleur, nocarte,
	     pos_index) == 0) {
	    fprintf(stderr, "Pb dans insert_index\n");
	    exit(EXIT_FAILURE);
	}



	/* on efface l'element de la pile */
	free(pli_new);
    }

    //vidage(stk);
    clean_l_item(l_item);

    thread_jeu->score = best_score;
    thread_jeu->nbline[0] = best_nbline[0];
    thread_jeu->nbline[1] = best_nbline[1];
    thread_jeu->status = 1;
    return (NULL);


}


int
first_explore(trick_t * pli_cur, int prof_max, int *nb_best, l_best_t * l_best,
	      game_t * game)
{
    thread_jeu_t **thread_jeu = NULL;
    pthread_t pid[NBPCOULEURS];
    int nothr = 0;
    //stackia_t stk;
    l_item_t *l_item;
    int nbcoups, i, threads_remaining = 1;
    position_t positionc;
    int pos_index;
    trick_t *pli_new;		/* nouveau pli renvoye par la liste des coups */
    int best_score;
    couleur_t nocouleur;
    valeur_t nocarte;
    best_t *best;
    positionc = pli_cur->nextpos;	/*C'est la nouvelle position */
    /*choix du joueur qui joue en fonction de la profondeur  */
    best_score =
	100000 * ((positionc) % 2) - 100000 * ((positionc + 1) % 2);
    //stk = create_stack(duplicate_trick);
    l_item=create_l_item(duplicate_trick); 
    if (game->tabjeu[0]->couleureval == aucune) {	//Tous les jeux contiennes la couleur d'evaluation si necessaire
	nbcoups = list_all_coups(positionc, l_item, pli_cur, game->tabjeu,game->cardplayed);
        /////////////////////////////////
        printf("first_explore:   nbcoups=%d\n",nbcoups);
        ////////////////////////////////
	if (nbcoups == 1) {
	    //pli_new = (trick_t *) pop(stk);
	    pli_new = (trick_t *) pop_item_head(l_item);
	    best = malloc(sizeof(best_t));
	    best->score = 1;
	    best->numero = 1;
	    best->carte = malloc(sizeof(carte_t));
	    best->carte->nocarte = pli_new->carte[positionc].nocarte;
	    best->carte->clcarte = pli_new->carte[positionc].clcarte;
	    add_list_l(l_best, best);
	    //vidage(stk);
            clean_l_item(l_item);
	    return (*nb_best);

	}
    } else
	nbcoups = list_all_coups_eval(positionc, l_item, pli_cur, game->tabjeu);
    if (game->debug)
	fprintf(stderr, "Voici le nombre de coups à examiner: %d\n",
		nbcoups);

    // tant que la pile des coups n'est pas vide on joue le coup dépilé
    while ((pli_new = (trick_t *) pop_item_head(l_item)) != NULL) {

	thread_jeu =
	    realloc(thread_jeu, (sizeof(thread_jeu_t *)) * (nothr + 1));
	thread_jeu[nothr] = malloc(sizeof(thread_jeu_t));
	dup_game(thread_jeu[nothr], game);
	thread_jeu[nothr]->carte = malloc(sizeof(carte_t));
	thread_jeu[nothr]->best_cartepot = malloc(sizeof(carte_t));
	nocarte = pli_new->carte[positionc].nocarte;
	nocouleur = pli_new->carte[positionc].clcarte;
	thread_jeu[nothr]->carte->nocarte =
	    pli_new->carte[positionc].nocarte;
	thread_jeu[nothr]->carte->clcarte =
	    pli_new->carte[positionc].clcarte;
	thread_jeu[nothr]->pli = pli_new;
	thread_jeu[nothr]->status = 0;
	thread_jeu[nothr]->prof_max = prof_max;
	thread_jeu[nothr]->prof = 1;
	thread_jeu[nothr]->nbcard = game->nbcard;
	//On joue le coup
	pos_index =
	    find_index(thread_jeu[nothr]->t_jeu, positionc, nocouleur,
		       nocarte);
	pos_index =
	    remove_index(thread_jeu[nothr]->t_jeu, positionc, nocouleur,
			 pos_index);
	thread_jeu[nothr]->best_cartepot->nocarte =
	    pli_new->carte[positionc].nocarte;
	thread_jeu[nothr]->best_cartepot->clcarte =
	    pli_new->carte[positionc].clcarte;
	evaluation_trick(thread_jeu[nothr]->pli);	/*on incremente nextpos ou on fixe la prochaine entame et nbpli++ */



	if (game->debug)
	    printf("thread %d created\n", nothr);
	pthread_create(&pid[nothr], NULL, new_explore, thread_jeu[nothr]);
	nothr++;
    }
    while (threads_remaining) {

	//printf("thread remaings\n",nothr);
	threads_remaining = 0;
	for (i = 0; i < nothr; i++) {
	    if (1 == thread_jeu[i]->status) {
		if (game->debug)
		    printf("thread %d termine\n", i);
		pthread_join(pid[i], NULL);
		thread_jeu[i]->status = 2;
	    }
	    if (0 == thread_jeu[i]->status)
		threads_remaining = 1;
	}
    }
    if (game->debug)
	printf("Threads are terminated\n");
    for (i = 0; i < nothr; i++) {
	if (game->debug)
	    printf("score=%d\n", thread_jeu[i]->score);
	if (minimax(thread_jeu[i]->score, best_score, positionc)) {
	    best_score = thread_jeu[i]->score;
	    *nb_best = *nb_best + 1;

	    //Ajout liste
	    if (game->debug)
		printf("nb_best=%d\n", *nb_best);
	    if (NULL == (best = malloc(sizeof(best_t)))) {
		fprintf(stderr, "Pb with malloc __LINE__\n");
		exit(EXIT_FAILURE);
	    }
	    best->score = best_score;
	    best->numero = *nb_best - 1;
	    if (NULL == (best->carte = malloc(sizeof(carte_t)))) {
		fprintf(stderr, "Pb with malloc __LINE__\n");
		exit(EXIT_FAILURE);
	    }
	    best->carte->nocarte = thread_jeu[i]->best_cartepot->nocarte;
	    best->carte->clcarte = thread_jeu[i]->best_cartepot->clcarte;
	    best->nbline[0] = thread_jeu[i]->nbline[0];
	    best->nbline[1] = thread_jeu[i]->nbline[1];
	    add_list_l(l_best, best);

	}
    }
    for (i = 0; i < nothr; i++) {

	free(thread_jeu[i]->best_cartepot);
	free(thread_jeu[i]->carte);
	destroy_jeu(thread_jeu[i]);
	free(thread_jeu[i]);

    }
    free(thread_jeu);


    //vidage(stk);
    clean_l_item(l_item);
    return (*nb_best);
}
