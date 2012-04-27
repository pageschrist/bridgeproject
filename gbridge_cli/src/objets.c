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
  if (atout==trefle) 
    strcpy (type,"T ");
  if (atout==carreau) 
    strcpy (type,"K ");
  if (atout==coeur) 
    strcpy (type,"C ");
  if (atout==pique) 
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
void copy_pli (pli_t *ppliori, pli_t *pplicopie)
{
  position_t i;
  ligne_t j;

  for (i = sud; i < est + 1; i++)
    {
      pplicopie->carte[i].nocarte = ppliori->carte[i].nocarte;
      pplicopie->carte[i].clcarte = ppliori->carte[i].clcarte;
      pplicopie->defausse[i].nocarte = ppliori->defausse[i].nocarte;
      pplicopie->defausse[i].clcarte = ppliori->defausse[i].clcarte;
    }
  pplicopie->entame = ppliori->entame;
  pplicopie->nextpos = ppliori->nextpos;
  pplicopie->nopli = ppliori->nopli;
  pplicopie->phcarte.nocarte = ppliori->phcarte.nocarte;
  pplicopie->phcarte.clcarte = ppliori->phcarte.clcarte;
  for (j = ns; j < eo + 1; j++)
    {
      pplicopie->nbpli_ligne[j] = ppliori->nbpli_ligne[j];
    }

}
void affiche_pli( pli_t *ppli)
{
	position_t i;
	ligne_t j;
	for(i=sud;i<est+1;i++) {
  printf("pos %d,  %s%s\n",i,affichage(ppli->carte[i].nocarte,CARTE),affichage(ppli->carte[i].clcarte,COULEUR));
//  printf("pos %d,  %c%c\n",i,affichage(ppli->defausse[i].nocarte,CARTE),affichage(ppli->defausse[i].clcarte,COULEUR));
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
void free_ihm_pli( ihm_pli_t *ihm_pli) {
  position_t position; 
  free(ihm_pli->pli);
  ihm_pli->pli=NULL;
  free(ihm_pli->contrat);
  ihm_pli->contrat=NULL;
  ihm_pli->scontrat=NULL;
  g_list_free(ihm_pli->waiting);
  ihm_pli->waiting=NULL;
  //g_list_free(ihm_pli->dropping);
  //ihm_pli->dropping=NULL;
  ihm_pli->movecard=NULL;
  for (position=sud;position<est+1;position++ ) {
    free_list(&ihm_pli->players[position]);
    ihm_pli->players[position]=NULL;
  }
  free_list(&ihm_pli->trash);
  ihm_pli->trash=NULL;
  ihm_pli->ligneia=1;
  ihm_pli->blinkid=-1;
  ihm_pli->state=BID;
  ihm_pli->status='n';
  ihm_pli->read=FALSE;
  ihm_pli->path=g_hash_table_lookup (configHash, "dir_imgs");
}
void reset_ihm_pli( ihm_pli_t *ihm_pli) {
#ifdef DEBUG
  ihm_pli->debug=TRUE;
#else
  ihm_pli->debug=FALSE;
#endif
  couleur_t couleur; 
  position_t position; 
  ihm_pli->pli=malloc(sizeof(pli_t));
  ihm_pli->contrat=malloc(sizeof(contrat_t));
  for (position=sud;position<est+1;position++ ) {
    for (couleur=trefle;couleur<pique+1;couleur++) {
      ihm_pli->tab_couleur[position][couleur]=0;
    }
  }
  if(ihm_pli->filename) 
  ihm_pli->filename[0]='\0';
   
  ihm_pli->read=FALSE;
  memset(ihm_pli->pli,0,sizeof(pli_t));
  memset(ihm_pli->contrat,0,sizeof(contrat_t));
  //init_bid(ihm_pli);
}

void init_ihm_pli( ihm_pli_t *ihm_pli) {
  couleur_t couleur; 
  position_t position; 

#ifdef DEBUG
  ihm_pli->debug=TRUE;
#else
  ihm_pli->debug=FALSE;
#endif
  ihm_pli->status='n';
  ihm_pli->level=DEFAULTLEVEL;
  ihm_pli->random=DEFAULTRANDOM;
  ihm_pli->pli=malloc(sizeof(pli_t));
  memset(ihm_pli->pli,0,sizeof(pli_t));
  ihm_pli->contrat=malloc(sizeof(contrat_t));
  memset(ihm_pli->contrat,0,sizeof(contrat_t));
  ihm_pli->transfert=malloc(sizeof(transfert_t));
  memset(ihm_pli->transfert,0,sizeof(transfert_t));
  ihm_pli->socketid=tcpclient();
  for (position=sud;position<est+1;position++ ) {
    for (couleur=trefle;couleur<pique+1;couleur++) {
      ihm_pli->tab_couleur[position][couleur]=0;
    }
  }
  ihm_pli->allcardslist=NULL;
  ihm_pli->scontrat=NULL;
  ihm_pli->waiting=NULL;
  ihm_pli->dropping=NULL;
  ihm_pli->movecard=NULL;
  ihm_pli->trash=NULL;
  ihm_pli->players = (GList **) g_malloc( sizeof(GList*) *NBJOUEURS );
  for (position=sud;position<est+1;position++ ) {
    ihm_pli->players[position]=NULL;

  }
  ihm_pli->target=NULL; 
  ihm_pli->back=NULL;
  ihm_pli->copy=NULL;
  ihm_pli->cardmask=NULL;
  ihm_pli->Drawing_area=NULL;
  ihm_pli->path=NULL;
  ihm_pli->transfert->level=DEFAULTLEVEL;
  ihm_pli->transfert->random=DEFAULTRANDOM;
  ihm_pli->output=NULL;
  ihm_pli->ligneia=1;
  ihm_pli->blinkid=0;
  ihm_pli->state=BID;
  ihm_pli->read=FALSE;
  ihm_pli->savegame=FALSE;
  ihm_pli->fd=NULL;
  ihm_pli->path=g_hash_table_lookup (configHash, "dir_imgs");
  ihm_pli->filename[0]='\0';
  init_bid(ihm_pli);
}



void init_bid(ihm_pli_t *ihm_pli) {
  int i,k;
  char *rescl,*resct;
  gchar *enchname = (gchar*)g_malloc((strlen(ihm_pli->path)+20)*sizeof(gchar));
  ihm_pli->Allbid=g_malloc(sizeof(ihm_bid_t*)*NBENCHERES);
  ihm_pli->Othbid=g_malloc(sizeof(ihm_bid_t*)*4);
  if(ihm_pli->Allbid) {
    for(i=trefle;i<aucune+1;i++) {
      for(k=0;k<7;k++) {
        ihm_pli->Allbid[i*7+k]=g_malloc(sizeof(ihm_bid_t));
        ihm_pli->Allbid[i*7+k]->bid=g_malloc(sizeof(bid_t));
        
        ihm_pli->Allbid[i*7+k]->bid->nombre=k+1;
        ihm_pli->Allbid[i*7+k]->bid->couleur=i;
        rescl=affichage((int)i,COULEUR);
        resct=affichage((int)k+1,CONTRAT);
        sprintf(enchname, "%s/ench%s%s.xpm", ihm_pli->path, resct,rescl);
        free(rescl);
        free(resct);
        ihm_pli->Allbid[i*7+k]->bid->position=aucun;
        ihm_pli->Allbid[i*7+k]->bid->passe=FALSE;
        ihm_pli->Allbid[i*7+k]->bid->contre=FALSE;
        ihm_pli->Allbid[i*7+k]->bid->surcontre=FALSE;
        ihm_pli->Allbid[i*7+k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
        ihm_pli->Allbid[i*7+k]->widget=gtk_image_new();
        ihm_pli->Allbid[i*7+k]->bwidget=gtk_button_new();
         

      }
    }
  }
  if(ihm_pli->Othbid) {
    for (k=0;k<4;k++) {
      ihm_pli->Othbid[k]=g_malloc(sizeof(ihm_bid_t));
      ihm_pli->Othbid[k]->bid=g_malloc(sizeof(bid_t));
      
      ihm_pli->Othbid[k]->bid->nombre=0;
      ihm_pli->Othbid[k]->bid->couleur=aucune;
      if(k==0) {
        sprintf(enchname, "%s/enchPP.xpm", ihm_pli->path);
        ihm_pli->Othbid[k]->bid->passe=TRUE;
        ihm_pli->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
      }
      if(k==1){
        sprintf(enchname, "%s/enchXP.xpm", ihm_pli->path);
        ihm_pli->Othbid[k]->bid->passe=FALSE;
        ihm_pli->Othbid[k]->bid->contre=TRUE;
        ihm_pli->Othbid[k]->bid->surcontre=FALSE;
        ihm_pli->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
      }
      if(k==2) {
        sprintf(enchname, "%s/enchXX.xpm", ihm_pli->path);
        ihm_pli->Othbid[k]->bid->passe=FALSE;
        ihm_pli->Othbid[k]->bid->contre=FALSE;
        ihm_pli->Othbid[k]->bid->surcontre=TRUE;
        ihm_pli->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);

      }
      if(k==3) {
        sprintf(enchname, "%s/enchND.xpm", ihm_pli->path);
        ihm_pli->Othbid[k]->bid->contre=FALSE;
        ihm_pli->Othbid[k]->bid->surcontre=FALSE;
        ihm_pli->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
      }
      ihm_pli->Othbid[k]->bid->position=aucun;
      ihm_pli->Othbid[k]->widget=gtk_image_new();
      ihm_pli->Othbid[k]->bwidget=gtk_button_new();
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
void init_pli (pli_t * pli, int maniere)
{
  position_t i;
  ligne_t j;
  for (i = sud; i < est + 1; i++)
    {
      pli->carte[i].nocarte = pdc;
      pli->carte[i].clcarte = aucune;
      pli->defausse[i].nocarte = pdc;
      pli->defausse[i].clcarte = aucune;
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




