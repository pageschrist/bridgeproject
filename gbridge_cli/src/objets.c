#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "objets.h"
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
  if (atout==aucun) 
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
      pos[0]=figure[valeur];
  }
  if (type == COULEUR) {
    pos[0]=couleurtype[valeur];
  }
  if (type == POSITION) {
    pos[0]=positiontype[valeur];
  }
  if (type == CONTRAT) {
    pos[0]=contrattype[valeur];
  }
  pos[1]='\0';
  if ((type != CARTE) && (type != COULEUR) && (type != POSITION) && (type != CONTRAT)) {
    perror ("type invalide\n");
    pos=NULL;
  }
  return  (pos);
}
 
void init_tab_coord (int maxhaut,int maxlarg) {
  int nc,nl;
  tab_coord=malloc(maxhaut*maxlarg*sizeof(carte_t *));
  for(nl=0;nl<maxhaut;nl++) {
    for(nc=0;nc<maxlarg;nc++) {
      tab_coord[(nc+maxlarg*nl)]=malloc(sizeof(carte_t));
      tab_coord[(nc+maxlarg*nl)]->nocarte=pdc;
      tab_coord[(nc+maxlarg*nl)]->clcarte=aucune;
    }
  }
}


char *loc_carte(carte_t  *carte) {

  char *chaine=NULL,coul[3],val[3];
  char *dir_imgs=NULL;
  dir_imgs= g_hash_table_lookup (configHash, "dir_imgs");
  if( NULL==(chaine=malloc((strlen(dir_imgs)+8)*sizeof(char)))) {
    fprintf(stderr, "Probleme avec malloc"); 
    exit(EXIT_FAILURE);
  } 
  strcpy(val,affichage(carte->nocarte,CARTE));
  strcpy(coul,affichage(carte->clcarte,COULEUR));

  
  sprintf(chaine,"%s/%c%c.jpg\\0",dir_imgs,val[0],coul[0]);
  return(chaine);

}


carte_t *loc_case(int nl,int nc,  int maxlarg) {
  carte_t *carte;	
  if (NULL == (carte=malloc(sizeof(carte_t))))
  { fprintf(stderr, "Probleme avec malloc"); 
    exit(EXIT_FAILURE);
  }
  carte->nocarte=tab_coord[nc+maxlarg*nl]->nocarte;
  carte->clcarte=tab_coord[nc+maxlarg*nl]->clcarte;
  if ((carte->clcarte != trefle)&&(carte->clcarte != carreau) && (carte->clcarte != coeur) && (carte->clcarte != pique)) {
    printf("Pb: nc=%d nl=%d carte->nocarte=%s carte->clcarte=%s\n",nc,nl,affichage(carte->nocarte,CARTE),affichage(carte->clcarte,COULEUR));
    printf("Pb: nc=%d nl=%d\n",nc,nl);
    return(NULL); 
  }
  return (carte);
  
}

 
void affichage_tab_coord(int maxlarg,int maxhaut) {
  int nc,nl;
  for (nl=0;nl<maxhaut;nl++) {
    for(nc=0;nc<maxlarg;nc++) {
      printf("%d ",tab_coord[nc+nl*maxlarg]->clcarte);
    }
    printf("\n");
  }

}
void affichage_tab_couleur(int tab_couleur[est+1][pique+1]) {
    couleur_t couleur;
    position_t position;
    for(position=sud;position<est+1;position++) {
      for(couleur=trefle;couleur<pique+1;couleur++) 
        printf("tab_couleur[%d][%d]:%d\n",position,couleur,tab_couleur[position][couleur]);
    }
}




void set_case(int nl,int nc,  int maxlarg,carte_t *carte) {
  tab_coord[nc+maxlarg*nl]->nocarte=carte->nocarte;
  tab_coord[nc+maxlarg*nl]->clcarte=carte->clcarte;
  
}
void remove_case(int nl,int nc,  int maxlarg) {
  tab_coord[nc+maxlarg*nl]->nocarte=pdc;
  tab_coord[nc+maxlarg*nl]->clcarte=aucune;
  
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
void * duplique_pli (void *data)
{
  pli_t *plicopie;
  position_t i;
  ligne_t j;
  plicopie=malloc(sizeof(pli_t));
  for (i = sud; i < est + 1; i++)
    {
      (*plicopie).carte[i].nocarte = ((pli_t *) data)->carte[i].nocarte;
      (*plicopie).carte[i].clcarte = ((pli_t *) data)->carte[i].clcarte;
      (*plicopie).defausse[i].nocarte = ((pli_t *) data)->defausse[i].nocarte;
      (*plicopie).defausse[i].clcarte = ((pli_t *) data)->defausse[i].clcarte;
    }
  (*plicopie).entame = ((pli_t *) data)->entame;
  (*plicopie).nextpos = ((pli_t *) data)->nextpos;
  (*plicopie).nopli = ((pli_t *) data)->nopli;
  (*plicopie).phcarte.nocarte = ((pli_t *) data)->phcarte.nocarte;
  (*plicopie).phcarte.clcarte = ((pli_t *) data)->phcarte.clcarte;
  for (j = ns; j < eo + 1; j++)
    {
      (*plicopie).nbpli_ligne[j] = ((pli_t *) data)->nbpli_ligne[j];
    }
  return ((void *) plicopie);

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
  couleur_t couleur; 
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
  ihm_pli->blinkid=0;
  ihm_pli->state=BID;
  ihm_pli->read=FALSE;
  ihm_pli->path=g_hash_table_lookup (configHash, "dir_imgs");
}
void reset_ihm_pli( ihm_pli_t *ihm_pli) {
  couleur_t couleur; 
  position_t position; 
  ihm_pli->pli=malloc(sizeof(pli_t));
  ihm_pli->contrat=malloc(sizeof(contrat_t));
  ihm_pli->maxlarg=400;
  ihm_pli->maxhaut=450;
  for (position=sud;position<est+1;position++ ) {
    for (couleur=trefle;couleur<pique+1;couleur++) {
      ihm_pli->tab_couleur[position][couleur]=0;
    }
  }
  //init_bid(ihm_pli);
}

void init_ihm_pli( ihm_pli_t *ihm_pli) {
  couleur_t couleur; 
  position_t position; 
  ihm_pli->pli=malloc(sizeof(pli_t));
  ihm_pli->contrat=malloc(sizeof(contrat_t));
  ihm_pli->transfert=malloc(sizeof(transfert_t));
  ihm_pli->maxlarg=400;
  ihm_pli->maxhaut=450;
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
  ihm_pli->path=g_hash_table_lookup (configHash, "dir_imgs");
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
        ihm_pli->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
      }
      if(k==1){
        sprintf(enchname, "%s/enchXP.xpm", ihm_pli->path);
        ihm_pli->Othbid[k]->bid->contre=TRUE;
        ihm_pli->Othbid[k]->bid->surcontre=FALSE;
        ihm_pli->Othbid[k]->wimage=gdk_pixbuf_new_from_file(enchname, NULL);
      }
      if(k==2) {
        sprintf(enchname, "%s/enchXX.xpm", ihm_pli->path);
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

char *display_str(char *str ,position_t position) {
  char *ret_str=NULL;
  position_t pos;
  int i;
  if((strlen(str))%2!=0) 
   return (ret_str);
  ret_str=malloc((3*((strlen(str))/2)+1)*sizeof(char));
  for(i=0;i<(strlen(str))/2;i++) {
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




