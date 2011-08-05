#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "stack.h"
#include "distribution.h"
#include "analyse.h"
#include "ia.h"
#include "arbitre.h"


int prof_start = 0;
int flag_debug=0;

carte_t *choix_best(int *nb_best,l_best_t *l_best,game_t *game,hopestat_t *hopestat) {
  valeur_t tabval[pique+1];
  couleur_t coulref=aucune;
  carte_t *carte=malloc(sizeof(carte_t));
  int points[pique+1],j;
  int jline=(IALINE+1)%2;
  
  couleur_t i;
  elem_best_t *elem_best=l_best->last;
  for( i=0;i<pique+1;i++)
    tabval[i]=pdc;
  int score=l_best->last->best->score;
  if(game->debug) 
    printf("On a %d bons coups\n",*nb_best);
    
   while(elem_best) {
     if(elem_best->best->score == score) {
       if(game->debug) {
         char *affca,*affco;
         printf("Voici la carte%s%s\n", affca=affichage(elem_best->best->carte->nocarte,CARTE),affco=affichage(elem_best->best->carte->clcarte,COULEUR));
         free(affca);
         free(affco);
         printf("Voici le score=%d nbline[IALINE]=%d nbline[IALINE+1]=%d \n", elem_best->best->score,elem_best->best->nbline[IALINE],elem_best->best->nbline[jline]);
       }
       if(hopestat) {
         hopestat->score=elem_best->best->score;
         hopestat->nbline[IALINE]=elem_best->best->nbline[IALINE];
         hopestat->nbline[jline]=elem_best->best->nbline[jline];

       }
       if(tabval[elem_best->best->carte->clcarte]>elem_best->best->carte->nocarte)
         tabval[elem_best->best->carte->clcarte]=elem_best->best->carte->nocarte;
     }
     elem_best=elem_best->prev;
   }
   for (i=0;i<pique+1;i++) {
     if(tabval[i]!=pdc) {
       points[i]=0;
       for(j=0;j<game->tabjeu[INDEX(ouest,i)]->nbcrt;j++)
         points[i]=game->tabjeu[INDEX(ouest,i)]->tabcoul[j]+points[i];
       for(j=0;j<game->tabjeu[INDEX(est,i)]->nbcrt;j++)
         points[i]=game->tabjeu[INDEX(est,i)]->tabcoul[j]+points[i];
     }
   }
   for (i=0;i<pique+1;i++) {
      if(tabval[i]!=pdc) {
          coulref=i; 
      } 
   } 
   carte->nocarte=tabval[coulref];
   carte->clcarte=coulref;
  return(carte);
  
}

int
minimax (int score_courant, int best_score, position_t positionc)
{
  if (positionc % 2 == IALINE)
    {
      if (score_courant <= best_score)
	return (1);
      else
	return (0);
    }
  else
    {
      if (score_courant >= best_score)
	return (1);
      else
	return (0);

    }


}
retpli_t *
cur_explore (int prof , pli_t *pplic, int prof_max,tablist_t **t_jeu,int orialpha,int oribeta)
{
  carte_t best_cartepot;
  stackia_t stk;
  int nbcoups ;
  int alpha=orialpha;
  int beta=oribeta;
  position_t positionc;
  int pos_index;
  pli_t *pplin;			/* nouveau pli renvoye par la liste des coups */
  retpli_t *ret;
  retpli_t *retup;
  int best_score, index,best_nbline[eo+1];
  couleur_t nocouleur;
  valeur_t nocarte;
  ligne_t ligne;
  
  positionc = pplic->nextpos;	/*C'est la nouvelle position */
  /*choix du joueur qui joue en fonction de la profondeur  */
  best_score = 100000 * ((positionc) % 2) - 100000 * ((positionc + 1) % 2);
  if ((prof == prof_max) || (pplic->nopli == 13))
    {
      retup = check_plis (pplic);
      return (retup);
    }
  stk = create_stack (duplique_pli);
  nbcoups=list_all_coups (positionc, stk, pplic,t_jeu);

  // tant que la pile des coups n'est pas vide on joue le coup dépilé
  while ((pplin = (pli_t *) pop (stk)) != NULL)
    {
      
        nocarte = pplin->carte[positionc].nocarte;
        nocouleur = pplin->carte[positionc].clcarte;
        if( nocarte== pdc) { 
          printf("Erreur dans explore\n");
          exit(2);
        }
        //On joue le coup
        index = INDEX (positionc, nocouleur);
        pos_index = find_index (t_jeu,positionc,nocouleur,nocarte);
        if(pos_index==-1) {
          fprintf(stderr,"Pb in cur_explore \n");
          exit(1);
        }
        pos_index=remove_index(t_jeu,positionc,nocouleur,pos_index);
        best_cartepot.nocarte = pplin->carte[positionc].nocarte;
        best_cartepot.clcarte = pplin->carte[positionc].clcarte;
        ligne = evaluation_pli (pplin );	/*on incremente nextpos ou on fixe la prochaine entame et nbpli++ */

        //on duplique le jeu

        ret = cur_explore (prof + 1, pplin, prof_max,t_jeu,alpha,beta);
        if((prof!=prof_start) && (prof != prof_max -1)) {
          if(positionc%2==1) 
            beta=ret->alpha_or_beta;
          else
            alpha=ret->alpha_or_beta;
        }
        if (minimax (ret->score, best_score, positionc))
	  {
	   best_score = ret->score;
           best_nbline[0]=ret->nbline[0];
           best_nbline[1]=ret->nbline[1];
           

	  }

        free(ret);


        // On annule le coup joue
        index = INDEX (positionc, nocouleur);
        if(insert_index(t_jeu,positionc,nocouleur,nocarte,pos_index)==0) {
          fprintf(stderr,"Pb dans insert_index\n");
          exit (EXIT_FAILURE);
        }
          


        /* on efface l'element de la pile */
        free (pplin);

        //coupe alpha/beta
        if(positionc%2 == 1 && (best_score<=alpha)&&(prof != prof_start))
          break;
        if(positionc%2 == 0 && (best_score>=beta)&&(prof != prof_start))
          break;
      }

    vidage (stk);

    retup=malloc(sizeof(retpli_t));
    retup->score=best_score;
    retup->nbline[0]=best_nbline[0];
    retup->nbline[1]=best_nbline[1];
    if(positionc%2==1) {
      if(alpha<best_score) {
        retup->alpha_or_beta = alpha;
      }
      else {
        retup->alpha_or_beta = beta;
      }
    } 
    else {
      if(beta>best_score)
        retup->alpha_or_beta =best_score;
      else
        retup->alpha_or_beta=beta;
    } 
    return (retup);


}
retpli_t *
cur_explore_eval (int prof , pli_t *pplic, int prof_max,tablist_t **t_jeu,int orialpha,int oribeta)
{
  carte_t best_cartepot;
  stackia_t stk;
  int nbcoups ;
  int alpha=orialpha;
  int beta=oribeta;
  position_t positionc;
  int pos_index;
  pli_t *pplin;			/* nouveau pli renvoye par la liste des coups */
  retpli_t *ret;
  retpli_t *retup;
  int best_score, index,best_nbline[eo+1];
  couleur_t nocouleur;
  valeur_t nocarte;
  ligne_t ligne;
  
  positionc = pplic->nextpos;	/*C'est la nouvelle position */
  /*choix du joueur qui joue en fonction de la profondeur  */
  best_score = 100000 * ((positionc) % 2) - 100000 * ((positionc + 1) % 2);
  stk = create_stack (duplique_pli);
  if(t_jeu[0]->couleureval==aucune)
    nbcoups=list_all_coups (positionc, stk, pplic,t_jeu);
  else
    nbcoups=list_all_coups_eval (positionc, stk, pplic,t_jeu);
  if((nbcoups==0) && (t_jeu[0]->couleureval!=aucune))
    prof_max=prof;
  if ((prof == prof_max) || (pplic->nopli == 13))
    {
      retup = check_plis (pplic);
      return (retup);
    }


  // tant que la pile des coups n'est pas vide on joue le coup dépilé
  while ((pplin = (pli_t *) pop (stk)) != NULL)
    {
      
        nocarte = pplin->carte[positionc].nocarte;
        nocouleur = pplin->carte[positionc].clcarte;
        if( nocarte== pdc) { 
          printf("Erreur dans explore\n");
          exit(2);
        }
        //On joue le coup
        index = INDEX (positionc, nocouleur);
        pos_index = find_index (t_jeu,positionc,nocouleur,nocarte);
        if(pos_index==-1) {
          printf("Pb in cur_explore \n");
          exit(1);
        }
        pos_index=remove_index(t_jeu,positionc,nocouleur,pos_index);
        best_cartepot.nocarte = pplin->carte[positionc].nocarte;
        best_cartepot.clcarte = pplin->carte[positionc].clcarte;
        ligne = evaluation_pli (pplin );	/*on incremente nextpos ou on fixe la prochaine entame et nbpli++ */

        //on duplique le jeu

        ret = cur_explore_eval (prof + 1, pplin, prof_max,t_jeu,alpha,beta);
        if((prof!=prof_start) && (prof != prof_max -1)) {
          if(positionc%2==1) 
            beta=ret->alpha_or_beta;
          else
            alpha=ret->alpha_or_beta;
        }
        if (minimax (ret->score, best_score, positionc))
	  {
	   best_score = ret->score;
           best_nbline[0]=ret->nbline[0];
           best_nbline[1]=ret->nbline[1];
           

	  }

        free(ret);


        // On annule le coup joue
        index = INDEX (positionc, nocouleur);
        if(insert_index(t_jeu,positionc,nocouleur,nocarte,pos_index)==0) {
          fprintf(stderr,"Pb dans insert_index\n");
          exit (EXIT_FAILURE);
        }
          


        /* on efface l'element de la pile */
        free (pplin);

        //coupe alpha/beta
        if(positionc%2 == 1 && (best_score<=alpha)&&(prof != prof_start))
          break;
        if(positionc%2 == 0 && (best_score>=beta)&&(prof != prof_start))
          break;
      }

    vidage (stk);

    retup=malloc(sizeof(retpli_t));
    retup->score=best_score;
    retup->nbline[0]=best_nbline[0];
    retup->nbline[1]=best_nbline[1];
    if(positionc%2==1) {
      if(alpha<best_score) {
        retup->alpha_or_beta = alpha;
      }
      else {
        retup->alpha_or_beta = beta;
      }
    } 
    else {
      if(beta>best_score)
        retup->alpha_or_beta =best_score;
      else
        retup->alpha_or_beta=beta;
    } 
    return (retup);


}




void * 
new_explore (void *arg)
{
  int prof,prof_max;
  pli_t * pplic;
  thread_jeu_t *thread_jeu=arg;
  carte_t best_cartepot;
  stackia_t stk;
  int nbcoups ;
  
  position_t positionc;
  int curscore;
  retpli_t *ret;
  retpli_t *rettmp;
  int pos_index;
  pli_t *pplin;			/* nouveau pli renvoye par la liste des coups */
  int best_score, index,best_nbline[eo+1];
  couleur_t nocouleur;
  valeur_t nocarte;
  ligne_t ligne;
  //reconstitution des varaiables
  prof=thread_jeu->prof;
  prof_max=thread_jeu->prof_max;
  //affiche_thread_jeu(thread_jeu);
  pplic=thread_jeu->pli;
  printf("Un thread est lance prof=%d, prof_pax=%d\n",prof,prof_max);
  positionc = pplic->nextpos;	/*C'est la nouvelle position */
  /*choix du joueur qui joue en fonction de la profondeur  */
  best_score = 100000 * ((positionc) % 2) - 100000 * ((positionc + 1) % 2);
  if ((prof == prof_max) || (pplic->nopli == 13))
    {
      rettmp=check_plis (pplic);
      thread_jeu->score = rettmp->score;
      thread_jeu->nbline[0] = rettmp->nbline[0];
      thread_jeu->nbline[1] = rettmp->nbline[1];
      thread_jeu->status=1;
      free(rettmp);
    }

  stk = create_stack (duplique_pli);
  if(thread_jeu->t_jeu[0]->couleureval==aucune)
    nbcoups=list_all_coups (positionc, stk, pplic,thread_jeu->t_jeu);
  else
    nbcoups=list_all_coups_eval (positionc, stk, pplic,thread_jeu->t_jeu);

  // tant que la pile des coups n'est pas vide on joue le coup dépilé
  while ((pplin = (pli_t *) pop (stk)) != NULL)
    {
      
      // On sauvegarde le pli en cours (pliori,plicopie) 
	//tab_cartes[pplin->carte[positionc].nocarte][pplin->carte[positionc].
	//					    clcarte].detenteur;
        nocarte = pplin->carte[positionc].nocarte;
        nocouleur = pplin->carte[positionc].clcarte;
        if( nocarte== pdc) { 
          printf("Erreur dans explore\n");
          exit(2);
        }
        //On joue le coup
        index = INDEX (positionc, nocouleur);
        pos_index = find_index (thread_jeu->t_jeu,positionc,nocouleur,nocarte);
        if(pos_index==-1) {
          printf("Pb in new_explore \n");
          exit(1);
        }
        pos_index=remove_index(thread_jeu->t_jeu,positionc,nocouleur,pos_index);
        best_cartepot.nocarte = pplin->carte[positionc].nocarte;
        best_cartepot.clcarte = pplin->carte[positionc].clcarte;
        ligne = evaluation_pli (pplin );	/*on incremente nextpos ou on fixe la prochaine entame et nbpli++ */
        if(thread_jeu->t_jeu[0]->couleureval!=aucune)
          ret = cur_explore_eval (prof + 1, pplin, prof_max,thread_jeu->t_jeu,ALPHA_START,BETA_START);
        else
          ret = cur_explore (prof + 1, pplin, prof_max,thread_jeu->t_jeu,ALPHA_START,BETA_START);
        curscore=ret->score;
        if (minimax (curscore, best_score, positionc))
	  {
	  best_score = curscore;
          best_nbline[0]=ret->nbline[0];
          best_nbline[1]=ret->nbline[1];
	}
        free(ret);


        // On annule le coup joue
        index = INDEX (positionc, nocouleur);
        if(insert_index(thread_jeu->t_jeu,positionc,nocouleur,nocarte,pos_index)==0) {
          fprintf(stderr,"Pb dans insert_index\n");
          exit (EXIT_FAILURE);
        }
          


        /* on efface l'element de la pile */
        free (pplin);
      }

    vidage (stk);

    thread_jeu->score=best_score;
    thread_jeu->nbline[0]=best_nbline[0];
    thread_jeu->nbline[1]=best_nbline[1];
    thread_jeu->status=1;
    return (NULL);


}


int
first_explore ( pli_t * pplic, int prof_max,int *nb_best,l_best_t *l_best,game_t *game)
{
  thread_jeu_t **thread_jeu=NULL;
  pthread_t pid[13];
  int nothr=0;
  stackia_t stk;
  int nbcoups,i,threads_remaining=1 ;
  position_t positionc;
  int pos_index;
  pli_t *pplin;			/* nouveau pli renvoye par la liste des coups */
  int best_score, index;
  couleur_t nocouleur;
  valeur_t nocarte;
  ligne_t ligne;
  best_t *best;
  positionc = pplic->nextpos;	/*C'est la nouvelle position */
  /*choix du joueur qui joue en fonction de la profondeur  */
  best_score = 100000 * ((positionc) % 2) - 100000 * ((positionc + 1) % 2);
  printf("first_explore: debug=%d\n",game->tabjeu[0]->debug);
  stk = create_stack (duplique_pli);
  if(game->tabjeu[0]->couleureval==aucune) { //Tous les jeux contiennes la couleur d'evaluation si necessaire
    nbcoups=list_all_coups (positionc, stk, pplic,game->tabjeu);
    if(nbcoups==1) {
      pplin = (pli_t *) pop (stk);
      best=malloc(sizeof(best_t));
      best->score=1;
      best->numero=1;
      best->carte=malloc(sizeof(carte_t));
      best->carte->nocarte=pplin->carte[positionc].nocarte;
      best->carte->clcarte=pplin->carte[positionc].clcarte;
      add_list_l(l_best,best);
      vidage(stk);
      return(*nb_best);

    }
  }
  else
    nbcoups=list_all_coups_eval (positionc, stk, pplic,game->tabjeu);
  printf("Voici le nombre de coups à examiner: %d\n",nbcoups);

  // tant que la pile des coups n'est pas vide on joue le coup dépilé
    while ((pplin = (pli_t *) pop (stk)) != NULL) {  

    thread_jeu=realloc(thread_jeu,(sizeof(thread_jeu_t *))*(nothr+1));
    thread_jeu[nothr]=malloc(sizeof(thread_jeu_t));
    copy_jeu (thread_jeu[nothr],game);
    thread_jeu[nothr]->carte=malloc(sizeof(carte_t));
    thread_jeu[nothr]->best_cartepot=malloc(sizeof(carte_t));
    nocarte = pplin->carte[positionc].nocarte;
    nocouleur = pplin->carte[positionc].clcarte;
    thread_jeu[nothr]->carte->nocarte = pplin->carte[positionc].nocarte;
    thread_jeu[nothr]->carte->clcarte = pplin->carte[positionc].clcarte;
    thread_jeu[nothr]->pli=pplin;
    thread_jeu[nothr]->status=0;
    thread_jeu[nothr]->prof_max=prof_max;
    thread_jeu[nothr]->prof=1;
    //On joue le coup
    index = INDEX (positionc, nocouleur);
    pos_index = find_index (thread_jeu[nothr]->t_jeu,positionc,nocouleur,nocarte);
    pos_index=remove_index(thread_jeu[nothr]->t_jeu,positionc,nocouleur,pos_index);
    thread_jeu[nothr]->best_cartepot->nocarte = pplin->carte[positionc].nocarte;
    thread_jeu[nothr]->best_cartepot->clcarte= pplin->carte[positionc].clcarte;
    ligne = evaluation_pli (thread_jeu[nothr]->pli );	/*on incremente nextpos ou on fixe la prochaine entame et nbpli++ */


          
    printf("thread %d created\n",nothr);
    pthread_create(&pid[nothr],NULL,new_explore,thread_jeu[nothr]);
    nothr++;
  }        
  while (threads_remaining) {
    
    //printf("thread remaings\n",nothr);
    threads_remaining = 0;
    for (i = 0; i < nothr; i++) {
      if (1 == thread_jeu[i]->status)
          {
            printf("thread %d termine\n",i);
            pthread_join (pid[i], NULL);
            thread_jeu[i]->status = 2;
          }
      if (0 == thread_jeu[i]->status)
        threads_remaining = 1;
    }
  }
  
  printf("Les Threads sont termines\n");
  for (i=0 ;i<nothr;i++) {
    printf("score=%d\n",thread_jeu[i]->score);
    if (minimax (thread_jeu[i]->score, best_score, positionc))
	  {
	  best_score = thread_jeu[i]->score;
          *nb_best=*nb_best+1;
  
              //Ajout liste
              printf("Voici nb_best:%d\n",*nb_best);
              if(NULL==(best=malloc(sizeof(best_t)))) {
                fprintf(stderr,"Pb with malloc \n");
                exit(EXIT_FAILURE);
              }
              best->score=best_score;
              best->numero=*nb_best-1;
              best->carte=malloc(sizeof(carte_t));
              best->carte->nocarte=thread_jeu[i]->best_cartepot->nocarte;
              best->carte->clcarte=thread_jeu[i]->best_cartepot->clcarte;
              best->nbline[0]=thread_jeu[i]->nbline[0];
              best->nbline[1]=thread_jeu[i]->nbline[1];
              add_list_l(l_best,best);

    }
  }
  for (i = 0; i < nothr; i++) {
  
    free(thread_jeu[i]->best_cartepot);
    free(thread_jeu[i]->carte);
    destroy_jeu (thread_jeu[i]);
    free(thread_jeu[i]);

  }
    free(thread_jeu);


  vidage (stk);
  return(*nb_best);
}
