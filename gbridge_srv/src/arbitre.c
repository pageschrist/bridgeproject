#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "distribution.h"
#include "stack.h"
#include "ia.h"
#include "arbitre.h"
extern int flag_debug;
int nblist_all_coups=0;
carte_t best_carte;
extern coord_t tab_cartes[cA + 1][pique + 1];
carte_t *find_best(best_t **tab_best);
extern tablist_t **tabjeu;	//(pique+1)*(est+1)
/* Fonction qui liste tous les newcartes , soit on passe une couleur sinon on a le choix*/
/*pli.entame doit etre initialise */
void fdefausse(void);
void fdefausse(void){ 
  printf("Defausse\n");
}
void ftrace(void) {
  printf("OK\n");
}

void  copy_jeu(thread_jeu_t *thread_jeu) {
  position_t position;
  couleur_t couleur;
  int index ,i;
  if (NULL==(thread_jeu->t_jeu = malloc ((sizeof (tablist_t *)) * (est + 1) * (pique + 1)))) {
    fprintf(stderr,"Probleme avec malloc\n");
    exit(EXIT_FAILURE);
  }
  for (position = sud; position < est + 1; position++)
    {
      for (couleur = trefle; couleur < pique + 1; couleur++)
	{
	  index = INDEX (position, couleur);
          thread_jeu->t_jeu[index]=malloc(sizeof (tablist_t));
	  //for (i = 0; i < tabjeu[index]->nbcrt; i++)
	  for (i = 0; i < 13; i++)
	    thread_jeu->t_jeu[index]->tabcoul[i]=tabjeu[index]->tabcoul[i] ;
	   thread_jeu->t_jeu[index]->nbcrt =tabjeu[index]->nbcrt;
	}
    }
}
void destroy_jeu (thread_jeu_t *thread_jeu) {
  position_t position;
  couleur_t couleur;
  int index ;
  for (position = sud; position < est + 1; position++)
    {
      for (couleur = trefle; couleur < pique + 1; couleur++)
	{
	  index = INDEX (position, couleur);
          free(thread_jeu->t_jeu[index]);
	}
    }
  free(thread_jeu->t_jeu); 
}


int
list_all_coups (position_t positionc, stackia_t stack, pli_t * pli,tablist_t **tmpjeu)
{
  int  index,situation,nbcoups=0;
  int k=0,sub,sup;
  couleur_t couleurc;
  couleur_t i;
  carte_t phcarte,carte_choisie;
  int presence=NON,pos_index;
  nblist_all_coups++;
  if(positionc == pli->entame)
     situation=ENTAME;
  if(positionc != pli->entame) {
     situation=NONENTAME;
     couleurc=pli->carte[pli->entame].clcarte;
     if(tmpjeu[INDEX (positionc,couleurc)]->nbcrt==0) {
       situation=DEFAUSSE;
     }
  }
  phcarte.nocarte=pli->phcarte.nocarte;
  phcarte.clcarte=pli->phcarte.clcarte;

  if(situation == DEFAUSSE) {

    for (i = trefle; i < pique + 1; i++) {
      index=INDEX (positionc, i);
      if(tmpjeu[index]->nbcrt != 0) {
        if(i!=pli->atout ) {
          pli->carte[positionc].clcarte = i;
	  pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
          nbcoups++;
          push (stack, pli);
        }
        else {
          for (k=0;k<tmpjeu[index]->nbcrt;k++) {
            pli->carte[positionc].clcarte = i;
	    pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[k];
            nbcoups++;
            push (stack, pli);
          }
        }

      }


      
        
    
      
    }
    return(nbcoups);

  }
 

  if (situation==ENTAME) 
    {
      /*On est a l'entame d'une couleur, donc on choisit n'importe quelle carte et on place la phcarte dans la structure */

      for (i = trefle; i < pique + 1; i++)
	{
	  presence = NON;
	  index = INDEX (positionc, i);
	  k = tmpjeu[INDEX (positionc, i)]->nbcrt;
	  while (k != 0)
	    {
	      if (presence == OUI)
		{
		  /* On teste si la carte inférieure est équivalente */
		  if ((tmpjeu[INDEX (positionc, i)]->tabcoul[k] -
		       tmpjeu[INDEX (positionc, i)]->tabcoul[k - 1]) != 1)
		    {
		      pli->carte[positionc].clcarte = i;
		      pli->carte[positionc].nocarte =
			tmpjeu[INDEX (positionc, i)]->tabcoul[k - 1];
		      pli->phcarte.clcarte = i;
		      pli->phcarte.nocarte =
			tmpjeu[INDEX (positionc, i)]->tabcoul[k - 1];
                      if(pli->carte[positionc].nocarte == pdc) {
                        int h;
                        fdefausse();
                        affiche_pli(pli);
                        for(h=0;h<NBJOUEURS;h++)
                        //affiche_tabjeu(h);
                        printf("Pb dans list_all_coups 1 \n");
                        printf("k=%d,presence=%d,positionc=%d,i=%d",k,presence,positionc,i);
                        exit(3);
                      }
                      nbcoups++;
		      push (stack, pli);
                      pli->phcarte.nocarte=phcarte.nocarte;
                      pli->phcarte.clcarte=phcarte.clcarte;
		    }
		}
	      else
		{
		  pli->carte[positionc].clcarte = i;
		  pli->carte[positionc].nocarte =
		    tmpjeu[INDEX (positionc, i)]->tabcoul[k - 1];
		  pli->phcarte.clcarte = i;
		  pli->phcarte.nocarte =
		    tmpjeu[INDEX (positionc, i)]->tabcoul[k - 1];
                      if(pli->carte[positionc].nocarte == pdc) {
                        int h;
                        fdefausse();
                        affiche_pli(pli);
                        for(h=0;h<NBJOUEURS;h++)
                        //affiche_tmpjeu(h);
                        printf("Pb dans list_all_coups 2\n");
                        exit(3);
                      }
                  nbcoups++;
		  push (stack, pli);
                  pli->phcarte.nocarte=phcarte.nocarte;
                  pli->phcarte.clcarte=phcarte.clcarte;
		  presence = OUI;


		}
	      k--;
	    }
	}
      return(nbcoups);
    }




  else
    {
      couleurc = pli->carte[pli->entame].clcarte;	/*On recupere la couleur entame */
      presence = NON;


      index = INDEX (positionc, couleurc);
        
      sub=sub_index (tmpjeu,positionc, couleurc, pli->phcarte.nocarte);
      sup=sub_index (tmpjeu,positionc, couleurc, pli->phcarte.nocarte);

      if(sup==tmpjeu[index]->nbcrt ){

        pli->carte[positionc].clcarte = couleurc;
	pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
        push (stack, pli);
        return(1);

      }
      else {
        if(sub!=sup) {
          pli->carte[positionc].clcarte = couleurc;
	  pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[0];
          nbcoups++;
          push (stack, pli);
        }

        carte_choisie.nocarte=pdc; 
        for ( pos_index=sup;pos_index<tmpjeu[index]->nbcrt;pos_index++) {
          if(carte_choisie.nocarte==pdc) {
            pli->carte[positionc].clcarte = couleurc;
	    pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[pos_index];
	    pli->phcarte.nocarte = tmpjeu[index]->tabcoul[pos_index];
	    pli->phcarte.clcarte = couleurc;
            nbcoups++;
            push (stack, pli);

          }
          else{
            if(pli->carte[positionc].nocarte-carte_choisie.nocarte==1) {
              carte_choisie.nocarte++;
            }
            else {
              pli->carte[positionc].clcarte = couleurc;
              pli->carte[positionc].nocarte = tmpjeu[index]->tabcoul[pos_index];
	      pli->phcarte.nocarte = tmpjeu[index]->tabcoul[pos_index];
	      pli->phcarte.clcarte = couleurc;
              nbcoups++;
              push (stack, pli);
            }



          }
        }
        return(nbcoups);
      }

  }
  return(0); //Normalement pas utilise


}

void *
copy_carte (void *data)
{
  carte_t *carte;

  carte = malloc (sizeof (carte_t));

  (*carte).nocarte = ((carte_t *) data)->nocarte;
  (*carte).clcarte = ((carte_t *) data)->clcarte;

  return ((void *) carte);
}


/*Cette fonction determine qui sera le suivant a jouer et met a jour les scores*/
position_t
evaluation_pli (pli_t * pli)
{

  carte_t carte_atout; 
  carte_atout.clcarte=pli->atout;
  carte_atout.nocarte=pdc;
  position_t postmp;
  position_t posgagnante;
  carte_t carteref;
/*  printf("Passage evolution\n"); */
  /* On teste si on est le dernier joueur du pli */
  if (pli->noj == 3)
    {
      postmp = pli->entame;
      carteref.nocarte = pli->carte[pli->entame].nocarte;
      carteref.clcarte = pli->carte[pli->entame].clcarte;
      posgagnante = pli->entame;
      postmp = (postmp + 1) % 4;
      while (postmp != pli->entame)
	{
         if(carte_atout.nocarte==pdc ) { //On n'a jamais joue atout
	    if (pli->carte[postmp].clcarte == carteref.clcarte )
	      {
	        if (pli->carte[postmp].nocarte > carteref.nocarte)
	  	  {
		    posgagnante = postmp;
		    carteref.nocarte = pli->carte[postmp].nocarte;
	  	}
	    }
            else {
              if ((pli->atout==pli->carte[postmp].clcarte) && (pli->atout!=aucune)) {
                posgagnante = postmp;
                carte_atout.nocarte=pli->carte[postmp].nocarte;
              }
                
            }
 

          }
          else { //on a joue atout
            if ((pli->atout==pli->carte[postmp].clcarte) &&   (pli->carte[postmp].nocarte >carte_atout.nocarte   )) {
              posgagnante = postmp;
              carte_atout.nocarte=pli->carte[postmp].nocarte;
            }
          }
	postmp = (postmp + 1) % 4;
      }
      pli->entame = posgagnante;
      pli->nextpos = posgagnante;
      pli->nopli++;
      pli->nbpli_ligne[(posgagnante%2)]++;

      init_pli (pli, RESET);	/*on reinitialise le pli */
      //affiche_pli (pli);
    }
  else
    {
      if(pli->phcarte.nocarte==pdc){
        pli->phcarte.nocarte=pli->carte[pli->nextpos].nocarte;
        pli->phcarte.clcarte=pli->carte[pli->nextpos].clcarte;
      }
      else {
        if((pli->phcarte.clcarte==pli->carte[pli->nextpos].clcarte)&&(pli->carte[pli->nextpos].nocarte >pli->phcarte.nocarte  )) {
          
          pli->phcarte.nocarte=pli->carte[pli->nextpos].nocarte;
          pli->phcarte.clcarte=pli->carte[pli->nextpos].clcarte;

        }
         

      }
        
      pli->nextpos = (pli->nextpos + 1) % 4;
    }
  pli->noj=(pli->noj+1)%4;
  return (pli->nextpos);
}

int
joue_coup (pli_t * pli,carte_t *carte )
{
  int posindex;
  position_t position;
  // NULL correspond au joue ( IHM) tout est dans pli
  if (carte != NULL ) {
    position=pli->nextpos;
    pli->carte[position].nocarte = carte->nocarte;
    pli->carte[position].clcarte = carte->clcarte;
    posindex=find_index(NULL,position,carte->clcarte,carte->nocarte);
    if(posindex==-1) {
      printf("Pb in joue_coup1\n");
      affiche_pli(pli);
      affiche_carte(carte);
      return(0);
    }
    posindex=remove_index(NULL,position,carte->clcarte,posindex);
  }
  else {
    position=pli->nextpos;
    posindex=find_index(NULL,position,pli->carte[position].clcarte,pli->carte[position].nocarte);
    if(posindex==-1) {
      printf("Pb in joue_coup2\n");
       return(0);
    }
    posindex=remove_index(NULL,position,pli->carte[position].clcarte,posindex);
    if(pli->carte[position].clcarte==pli->carte[pli->entame].clcarte) {
      if(pli->phcarte.nocarte<pli->carte[position].nocarte)
        pli->phcarte.nocarte=pli->carte[position].nocarte;
    }
  }

 return(1);
}


int
check_plis (pli_t * pli)
{
  extern ligne_t ligneia;
  int score;
  ligne_t lignej;
  lignej = (ligneia + 1) % 2;
  score = pli->nbpli_ligne[lignej] - pli->nbpli_ligne[ligneia];
  /* printf ("Voici le score dans check_plis: %d\n", score); */
  return (score);

}
