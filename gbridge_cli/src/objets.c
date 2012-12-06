#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "objets.h"
#include "free.h"
#include "client.h"
char * contrat_to_char(couleur_t atout);
extern GHashTable *configHash;
carte_t **tab_coord;
void debugaff(void) {

 int i=3;
 i=i+1;
}

char * contrat_to_char ( couleur_t atout) {
  char *type;
  type=malloc (3*sizeof(char));
  if (atout==club) 
    strcpy (type,"T ");
  if (atout==diamond) 
    strcpy (type,"K ");
  if (atout==heart) 
    strcpy (type,"C ");
  if (atout==spade) 
    strcpy (type,"P ");
  if (atout==aucune) 
    strcpy (type,"SA");
  type[2]='\0';
  return type;  
}

char *affichage (int valeur, int type)
{


  char *figure="23456789XVDRAZ";
  char *couleurtype="TKCPN";
  char *positiontype="SONEZ";
  char *contrattype="01234567";
  char *pos=malloc(2*sizeof(char ));;
    
  if (type == CARTE) {
      if(valeur<(int)strlen(figure))
      pos[0]=figure[valeur];
  }
  if (type == COULEUR) {
    if(valeur<(int)strlen(couleurtype))
    pos[0]=couleurtype[valeur];
  }
  if (type == POSITION) {
    if(valeur<(int)strlen(positiontype))
    pos[0]=positiontype[valeur];
  }
  if (type == CONTRAT) {
    if(valeur<(int)strlen(contrattype))
    pos[0]=contrattype[valeur];
  }
  pos[1]='\0';
  if ((type != CARTE) && (type != COULEUR) && (type != POSITION) && (type != CONTRAT)) {
    perror ("type invalide\n");
    pos=NULL;
  }
  return  (pos);
}
 









void affiche_carte (carte_t *carte)
{
  printf("La couleur de la carte est: %d\n",carte->clcarte);
  printf("La valeur de la carte est: %d\n",carte->nocarte);
}
void display_trick( trick_t *ppli)
{
	position_t i;
        char *rescl,*resct;
	ligne_t j;
	for(i=sud;i<est+1;i++) {
        resct=affichage(ppli->carte[i].nocarte,CARTE);
        rescl=affichage(ppli->carte[i].clcarte,COULEUR);
        printf("pos %d,  %s%s\n",i,resct,rescl);
        free(resct);
        free(rescl);
	}
	printf("Vci  pli.entame %d\n",ppli->entame);
	printf("Vci  pli.nextpos %d\n",ppli->nextpos);
	printf("Vci  pli.lastcarte.no %d\n",ppli->lastcarte.nocarte);
	printf("Vci  pli.lastcarte.cl %d\n",ppli->lastcarte.clcarte);
	printf("Vci  pli.nopli %d\n",ppli->nopli);
  for (j = ns; j < eo + 1; j++)
    {
      printf("Vci pli.nbpli_ligne[%d] %d\n",j,ppli->nbpli_ligne[j]); 
    }

}
void free_ihm_setup( ihm_trick_t *ihm_setup) {
  position_t position; 
  free(ihm_setup->pli);
  ihm_setup->pli=NULL;
  free(ihm_setup->contrat);
  ihm_setup->contrat=NULL;
  ihm_setup->scontrat=NULL;
  g_list_free(ihm_setup->waiting);
  ihm_setup->waiting=NULL;
  //g_list_free(ihm_setup->dropping);
  //ihm_setup->dropping=NULL;
  ihm_setup->movecard=NULL;
  for (position=sud;position<est+1;position++ ) {
    free_list(&ihm_setup->players[position]);
    ihm_setup->players[position]=NULL;
  }
  free_list(&ihm_setup->trash);
  ihm_setup->trash=NULL;
  ihm_setup->ligneia=1;
  ihm_setup->blinkid=-1;
  ihm_setup->state=BID;
  ihm_setup->status='n';
  ihm_setup->read=FALSE;
  ihm_setup->path=g_hash_table_lookup (configHash, "dir_imgs");
}
void reset_ihm_setup( ihm_trick_t *ihm_setup) {
#ifdef DEBUG
  ihm_setup->debug=TRUE;
#else
  ihm_setup->debug=FALSE;
#endif
  couleur_t couleur; 
  position_t position; 
  ihm_setup->pli=malloc(sizeof(trick_t));
  ihm_setup->contrat=malloc(sizeof(contrat_t));
  for (position=sud;position<est+1;position++ ) {
    for (couleur=club;couleur<spade+1;couleur++) {
      ihm_setup->tab_couleur[position][couleur]=0;
    }
  }
  if(ihm_setup->filename) 
  ihm_setup->filename[0]='\0';
   
  ihm_setup->read=FALSE;
  memset(ihm_setup->pli,0,sizeof(trick_t));
  memset(ihm_setup->contrat,0,sizeof(contrat_t));
  //init_bid(ihm_setup);
}

void init_ihm_setup( ihm_trick_t *ihm_setup) {
  couleur_t couleur; 
  position_t position; 

#ifdef DEBUG
  ihm_setup->debug=TRUE;
#else
  ihm_setup->debug=FALSE;
#endif
  ihm_setup->status='n';
  ihm_setup->level=DEFAULTLEVEL;
  ihm_setup->random=DEFAULTRANDOM;
  ihm_setup->pli=malloc(sizeof(trick_t));
  memset(ihm_setup->pli,0,sizeof(trick_t));
  ihm_setup->contrat=malloc(sizeof(contrat_t));
  memset(ihm_setup->contrat,0,sizeof(contrat_t));
  ihm_setup->transfert=malloc(sizeof(transfert_t));
  memset(ihm_setup->transfert,0,sizeof(transfert_t));
  ihm_setup->socketid=tcpclient();
  for (position=sud;position<est+1;position++ ) {
    for (couleur=club;couleur<spade+1;couleur++) {
      ihm_setup->tab_couleur[position][couleur]=0;
    }
  }
  ihm_setup->allcardslist=NULL;
  ihm_setup->scontrat=NULL;
  ihm_setup->waiting=NULL;
  ihm_setup->dropping=NULL;
  ihm_setup->movecard=NULL;
  ihm_setup->trash=NULL;
  ihm_setup->players = (GList **) g_malloc( sizeof(GList*) *NBJOUEURS );
  for (position=sud;position<est+1;position++ ) {
    ihm_setup->players[position]=NULL;

  }
  ihm_setup->target=NULL; 
  ihm_setup->back=NULL;
  ihm_setup->copy=NULL;
  ihm_setup->cardmask=NULL;
  ihm_setup->Drawing_area=NULL;
  ihm_setup->path=NULL;
  ihm_setup->transfert->level=DEFAULTLEVEL;
  ihm_setup->transfert->random=DEFAULTRANDOM;
  ihm_setup->output=NULL;
  ihm_setup->ligneia=1;
  ihm_setup->blinkid=0;
  ihm_setup->state=BID;
  ihm_setup->read=FALSE;
  ihm_setup->savegame=FALSE;
  ihm_setup->fd=NULL;
  ihm_setup->path=g_hash_table_lookup (configHash, "dir_imgs");
  ihm_setup->filename[0]='\0';
  init_bid(ihm_setup);
}



void init_bid(ihm_trick_t *ihm_setup) {
  int i,k;
  char *rescl,*resct;
  gchar *enchname = (gchar*)g_malloc((strlen(ihm_setup->path)+20)*sizeof(gchar));
  ihm_setup->Allbid=g_malloc(sizeof(ihm_bid_t*)*NBENCHERES);
  ihm_setup->Othbid=g_malloc(sizeof(ihm_bid_t*)*4);
  if(ihm_setup->Allbid) {
    for(i=club;i<aucune+1;i++) {
      for(k=0;k<7;k++) {
        ihm_setup->Allbid[i*7+k]=g_malloc(sizeof(ihm_bid_t));
        ihm_setup->Allbid[i*7+k]->bid=g_malloc(sizeof(bid_t));
        
        ihm_setup->Allbid[i*7+k]->bid->nombre=k+1;
        ihm_setup->Allbid[i*7+k]->bid->couleur=i;
        rescl=affichage((int)i,COULEUR);
        resct=affichage((int)k+1,CONTRAT);
        sprintf(enchname, "%s/ench%s%s.xpm", ihm_setup->path, resct,rescl);
        free(rescl);
        free(resct);
        ihm_setup->Allbid[i*7+k]->bid->position=aucun;
        ihm_setup->Allbid[i*7+k]->bid->passe=FALSE;
        ihm_setup->Allbid[i*7+k]->bid->contre=FALSE;
        ihm_setup->Allbid[i*7+k]->bid->surcontre=FALSE;
        ihm_setup->Allbid[i*7+k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
        ihm_setup->Allbid[i*7+k]->widget=gtk_image_new();
        ihm_setup->Allbid[i*7+k]->bwidget=gtk_button_new();
         

      }
    }
  }
  if(ihm_setup->Othbid) {
    for (k=0;k<4;k++) {
      ihm_setup->Othbid[k]=g_malloc(sizeof(ihm_bid_t));
      ihm_setup->Othbid[k]->bid=g_malloc(sizeof(bid_t));
      
      ihm_setup->Othbid[k]->bid->nombre=0;
      ihm_setup->Othbid[k]->bid->couleur=aucune;
      if(k==0) {
        sprintf(enchname, "%s/enchPP.xpm", ihm_setup->path);
        ihm_setup->Othbid[k]->bid->passe=TRUE;
        ihm_setup->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
      }
      if(k==1){
        sprintf(enchname, "%s/enchXP.xpm", ihm_setup->path);
        ihm_setup->Othbid[k]->bid->passe=FALSE;
        ihm_setup->Othbid[k]->bid->contre=TRUE;
        ihm_setup->Othbid[k]->bid->surcontre=FALSE;
        ihm_setup->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
      }
      if(k==2) {
        sprintf(enchname, "%s/enchXX.xpm", ihm_setup->path);
        ihm_setup->Othbid[k]->bid->passe=FALSE;
        ihm_setup->Othbid[k]->bid->contre=FALSE;
        ihm_setup->Othbid[k]->bid->surcontre=TRUE;
        ihm_setup->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);

      }
      if(k==3) {
        sprintf(enchname, "%s/enchND.xpm", ihm_setup->path);
        ihm_setup->Othbid[k]->bid->contre=FALSE;
        ihm_setup->Othbid[k]->bid->surcontre=FALSE;
        ihm_setup->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
      }
      ihm_setup->Othbid[k]->bid->position=aucun;
      ihm_setup->Othbid[k]->widget=gtk_image_new();
      ihm_setup->Othbid[k]->bwidget=gtk_button_new();
    } 
  
  }
  g_free(enchname);

}

char *display_str(char *str ) {
  char *ret_str=NULL;
  int i;
  if((strlen(str))%2!=0) 
   return (ret_str);
  ret_str=malloc((3*((strlen(str))/2)+1)*sizeof(char));
  for(i=0;i<(int)(strlen(str))/2;i++) {
    ret_str[3*i]=str[2*i];
    ret_str[3*i+1]=str[2*i+1];
    if(ret_str[3*i]=='P'&&ret_str[3*i+1]=='P') {
      ret_str[3*i]='-';
      ret_str[3*i+1]='-';
    }

    if((i+1)%4==0 &&i !=0)
      ret_str[3*i+2]='\n';
    else
      ret_str[3*i+2]='\t';

  }

  ret_str[3*((strlen(str))/2)]='\0';
  return(ret_str);  
}
void init_trick (trick_t * pli, int maniere)
{
  position_t i;
  ligne_t j;
  for (i = sud; i < est + 1; i++)
    {
      pli->carte[i].nocarte = pdc;
      pli->carte[i].clcarte = aucune;
      pli->discard[i].nocarte = pdc;
      pli->discard[i].clcarte = aucune;
    }
  pli->phcarte.nocarte = pdc;
  pli->phcarte.clcarte = aucune;
  if (maniere == INIT)
    {
      pli->entame = aucun;
      pli->nextpos = aucun;
      pli->nopli = 0;
      pli->noj = 0;
      for (j = ns; j < eo + 1; j++)
	{
	  pli->nbpli_ligne[j] = 0;
	}
    }
}




