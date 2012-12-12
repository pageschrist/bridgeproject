#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "objets.h"
#include "free.h"
#include "client.h"
char * contrat_to_char(color_t atout);
extern GHashTable *configHash;
card_t **tab_coord;
void debugaff(void) {

 int i=3;
 i=i+1;
}

char * contrat_to_char ( color_t atout) {
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
    
  if (type == CARD) {
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
  if ((type != CARD) && (type != COULEUR) && (type != POSITION) && (type != CONTRAT)) {
    perror ("type invalide\n");
    pos=NULL;
  }
  return  (pos);
}
 









void affiche_card (card_t *card)
{
  printf("La couleur de la card est: %d\n",card->clcard);
  printf("La valeur de la card est: %d\n",card->nocard);
}
void display_trick( trick_t *trick)
{
	position_t i;
        char *rescl,*resct;
	ligne_t j;
	for(i=sud;i<est+1;i++) {
        resct=affichage(trick->card[i].nocard,CARD);
        rescl=affichage(trick->card[i].clcard,COULEUR);
        printf("pos %d,  %s%s\n",i,resct,rescl);
        free(resct);
        free(rescl);
	}
	printf("Vci  trick.entame %d\n",trick->entame);
	printf("Vci  trick.nextpos %d\n",trick->nextpos);
	printf("Vci  trick.lastcard.no %d\n",trick->lastcard.nocard);
	printf("Vci  trick.lastcard.cl %d\n",trick->lastcard.clcard);
	printf("Vci  trick.notrick %d\n",trick->notrick);
  for (j = ns; j < eo + 1; j++)
    {
      printf("Vci trick.nbtricks_line[%d] %d\n",j,trick->nbtricks_line[j]); 
    }

}
void free_ihm_setup( ihm_trick_t *ihm_setup) {
  position_t position; 
  free(ihm_setup->trick);
  ihm_setup->trick=NULL;
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
  color_t couleur; 
  position_t position; 
  ihm_setup->trick=malloc(sizeof(trick_t));
  ihm_setup->contrat=malloc(sizeof(contrat_t));
  for (position=sud;position<est+1;position++ ) {
    for (couleur=club;couleur<spade+1;couleur++) {
      ihm_setup->tab_couleur[position][couleur]=0;
    }
  }
  if(ihm_setup->filename) 
  ihm_setup->filename[0]='\0';
   
  ihm_setup->read=FALSE;
  memset(ihm_setup->trick,0,sizeof(trick_t));
  memset(ihm_setup->contrat,0,sizeof(contrat_t));
  //init_bid(ihm_setup);
}

void init_ihm_setup( ihm_trick_t *ihm_setup) {
  color_t couleur; 
  position_t position; 

#ifdef DEBUG
  ihm_setup->debug=TRUE;
#else
  ihm_setup->debug=FALSE;
#endif
  ihm_setup->status='n';
  ihm_setup->level=DEFAULTLEVEL;
  ihm_setup->random=DEFAULTRANDOM;
  ihm_setup->trick=malloc(sizeof(trick_t));
  memset(ihm_setup->trick,0,sizeof(trick_t));
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
void init_trick (trick_t * trick, int maniere)
{
  position_t i;
  ligne_t j;
  for (i = sud; i < est + 1; i++)
    {
      trick->card[i].nocard = pdc;
      trick->card[i].clcard = aucune;
      trick->discard[i].nocard = pdc;
      trick->discard[i].clcard = aucune;
    }
  trick->phcard.nocard = pdc;
  trick->phcard.clcard = aucune;
  if (maniere == INIT)
    {
      trick->entame = aucun;
      trick->nextpos = aucun;
      trick->notrick = 0;
      trick->noj = 0;
      for (j = ns; j < eo + 1; j++)
	{
	  trick->nbtricks_line[j] = 0;
	}
    }
}




