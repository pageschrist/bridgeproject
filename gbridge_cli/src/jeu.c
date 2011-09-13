#include "objets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include "client.h"
#include "jeu.h"
#include "traffic.h"
#include "load.h"
#include "draw.h"
#include "free.h"
#include "callbacks.h"
extern int level;
extern int randomval;
extern coord_t case_a_depot[est + 1];
extern GtkWidget **tab_imgs;
extern GtkWidget *Table;
extern GHashTable *configHash;
void game_middle_offset_dynamic(position_t position, int *midx, int *midy,
        int w, int h, int cardw, int cardh)
{
        switch(position)
        {
                case NORD:
                *midy = cardh + 20;
                break;

                case EST:
                *midx = w - 2*cardw - 20;
                break;

                case SUD:
                *midy = h - 2*cardh - 20;
                break;

                case OUEST:
                *midx = cardw + 20;
                break;

                case aucun:
                printf("Pb in game_middle_offset_dynamic\n");
                exit(1);
                break;
        }

}

void position_middle( ihm_pli_t *ihm_pli,imgcard_t *card,position_t position) {
  int midx,midy;
  if (card==NULL) {
   printf ("Erreur in position_middle\n");
   exit(1);
  }
  midx = (ihm_pli->Drawing_area->allocation.width - card->dim.w)/2;
  midy = (ihm_pli->Drawing_area->allocation.height - card->dim.h)/2;
  
  game_middle_offset_dynamic(position, &midx, &midy,
                ihm_pli->Drawing_area->allocation.width,
                ihm_pli->Drawing_area->allocation.height,
                card->dim.w, card->dim.h);

        if(card->dim.x != midx || card->dim.y != midy)
        {
                //if(ihm_pli->animation)
                 //       position_move_card(card, midx, midy, prog_data);


                card->dim.x = midx;
                card->dim.y = midy;
                draw_container_ihm(ihm_pli);
        }
 

}
void position_targetzone(ihm_pli_t *ihm_pli)
{
        
        if(ihm_pli->dropping != NULL)
        {
                int w, h;
                GList *lst = ihm_pli->dropping;

                rectangle_t *zone;

                if(ihm_pli->back)
                {

                        gdk_drawable_get_size(ihm_pli->back,
                                &w, &h);

                        if(lst)
                        {
                                zone = (rectangle_t *)lst->data;

                                zone->dim.x = ihm_pli->Drawing_area->allocation.width/2;
                                zone->dim.y = ihm_pli->Drawing_area->allocation.height/2;

                                zone->dim.x -= w;
                                zone->dim.y -= (2*h)/3;
                                zone->dim.w = 2*w;
                                zone->dim.h = (4*h)/3;

                                zone->active = TRUE;
                        }
                }
        }
}




void destroy_ihm_pli (ihm_pli_t *ihm_pli) {
 position_t position;
 free( ihm_pli->contrat);
 for (position=sud;position>est+1;position++ ) 
   g_free (ihm_pli->players[position]);
 g_free(ihm_pli->players);
 free(ihm_pli->pli);
}

void init_game(ihm_pli_t *ihm_pli) {
   
  int contrat; 
  couleur_t couleur;
  free_ihm_pli(ihm_pli);
  reset_ihm_pli(ihm_pli);
  write_data (ihm_pli,NULL, 'n');

  recuperation_jeu(ihm_pli,0);
  draw_container_ihm(ihm_pli);
  for(contrat=0;contrat<7;contrat++){
    for(couleur=trefle;couleur<aucune+1;couleur++)
                gtk_widget_set_sensitive(ihm_pli->Allbid[couleur*7+contrat]->bwidget, TRUE);
  }



  
  //on initialise le jeu
  //printf("recup jeu:%d\n",(ihm_pli->contrat->declarant+2)%4);
  //recuperation_jeu(ihm_pli,0);
  //recuperation_jeu(ihm_pli,(ihm_pli->contrat->declarant+2)%4);
  ihm_pli->pli->nextpos=ouest;

}


void position_list(GList *ptr, int x, int y, enum eposition ep, float coef)
{
        int incx, incy;
        int xref=x;
        int step,i;
        int num;
        couleur_t couleurref=aucune;
        int tab_coul[pique+1];
        imgcard_t *data_ptr;
        GList *ptrref;
        ptrref=ptr;
        for(i=0;i<4;i++)
          tab_coul[i]=0;
        for(num=0;ptrref;ptrref=ptrref->next) {
          data_ptr = (imgcard_t *) ptrref->data;
          tab_coul[data_ptr->couleur]=tab_coul[data_ptr->couleur]+1;
        }

        
        



        switch(ep)
        {
                case EP_DIAGONAL:
                incx = (int) floorf(coef);
                incy = (int) floorf(coef);
                step = 3;
                break;

                case EP_HORIZONTAL:
                //incx = (int) floorf(hspace*coef);
                incx = -15;
                incy = 0;
                step = 1;
                break;

                case EP_VERTICAL:
                incx = 15;
                //incy = (int) floorf(vspace*coef);
                incy = 30;
                step = 1;
                break;
        }

        for(num = 0;ptr; ptr = ptr->next) {
          data_ptr = (imgcard_t *) ptr->data;
          if(data_ptr->couleur!=couleurref && data_ptr->position==EST){
            x=xref;
            y = y -incy;
            couleurref=data_ptr->couleur;
          }
          if(data_ptr->couleur!=couleurref &&data_ptr->position==OUEST){
            x=xref+incx*tab_coul[data_ptr->couleur];
            y = y -incy;
            couleurref=data_ptr->couleur;
          }
          if(data_ptr->couleur!=couleurref &&data_ptr->position==NORD){
            x=x-10;
            couleurref=data_ptr->couleur;
          }
          if(data_ptr->couleur!=couleurref &&data_ptr->position==SUD){
            x=x-10;
            couleurref=data_ptr->couleur;
          }
          if(couleurref==aucune)
            couleurref=data_ptr->couleur;

            data_ptr->dim.x = x;
            data_ptr->dim.y = y;
                  


            num++;

            if(num == step)
                  {
                    if(data_ptr->position==OUEST) {
                      x = x-incx;
                    }

                    if(data_ptr->position==EST) {
                        x -= incx;
                    }

                    if(data_ptr->position!=EST && data_ptr->position!=OUEST) {
                        x += incx;
                        y += incy;


                    
                    }                    
                    num = 0;
                  }
        }


}


int game_turn (ihm_pli_t *ihm_pli,imgcard_t *imgcard) {

  position_t poscour;
  char *cardname,*rescl,*resvl,*resps;
  GtkStateType state = GTK_WIDGET_STATE(ihm_pli->Drawing_area);
  int status;
  char *reschaine;
  GdkGC *gc;
  
  printf("Debut game_turn\n");
    //Nettoyage,  à changer 
  if(ihm_pli->pli->noj==0) {
    printf("Nettoyage\n");
    trash_list(ihm_pli);
  }
  // C'est la fin au a deja 13 plis 
  if((ihm_pli->pli->nbpli_ligne[1]+ihm_pli->pli->nbpli_ligne[0] ) ==13 ) {
    printf("The end recuperation_jeu\n");
      
   return 0;
  }
  poscour = ihm_pli->pli->nextpos;
  if(ihm_pli->debug)
    fprintf(stdout,"ihm_pli->pli->nbpli_ligne[1]=%d\nihm_pli->pli->nbpli_ligne[0]=%d\nposcour=%d\n",ihm_pli->pli->nbpli_ligne[1],ihm_pli->pli->nbpli_ligne[0],poscour);


  if((ihm_pli->pli->nextpos%2)==(position_t)ihm_pli->ligneia) {
   
    printf("IA joue\n");
    status = read_header(ihm_pli,ihm_pli->pli, 'p');
    cardname = (gchar*)g_malloc((strlen(ihm_pli->path)+20)*(sizeof(gchar)));
    gc = ihm_pli->Drawing_area->style->fg_gc[state];
    gdk_gc_set_clip_mask(gc,ihm_pli->backmask);
    if( ihm_pli->pli->noj==0) {
      rescl=affichage((int)ihm_pli->pli->lastcarte.clcarte,COULEUR);
      resvl=affichage((int)ihm_pli->pli->lastcarte.nocarte,CARTE);
      sprintf(cardname, "%s/%s%s.xpm", ihm_pli->path, resvl,rescl);
      if ( g_file_test(cardname, G_FILE_TEST_EXISTS) == TRUE ) {
        imgcard=load_imgcard(cardname,&ihm_pli->pli->lastcarte,ihm_pli,poscour); 
      }
        
           
      position_middle(ihm_pli,imgcard,poscour);
      ihm_pli->waiting=g_list_prepend(ihm_pli->waiting, (gpointer)imgcard);
    }

    else {
      resvl=affichage(ihm_pli->pli->carte[poscour].nocarte,CARTE);
      rescl=affichage(ihm_pli->pli->carte[poscour].clcarte,COULEUR);
      sprintf(cardname, "%s/%s%s.xpm", ihm_pli->path, resvl,rescl);
      if ( g_file_test(cardname, G_FILE_TEST_EXISTS) == TRUE ) {
        imgcard=load_imgcard(cardname,&ihm_pli->pli->carte[poscour],ihm_pli,poscour); 
      }
        position_middle(ihm_pli,imgcard,poscour);
        ihm_pli->waiting=g_list_prepend(ihm_pli->waiting, (gpointer)imgcard);

    }
    //Ajout clignotement
    if(imgcard->position==ihm_pli->pli->nextpos) {
      imgcard->blink=TRUE;
      imgcard->draw=FALSE;
      draw_copy(ihm_pli);
      ihm_pli->blinkid=g_timeout_add(800,event_blink, (gpointer)ihm_pli);
    }

    draw_container_ihm(ihm_pli);
    resps=affichage(ihm_pli->pli->nextpos,POSITION); 
      reschaine=malloc(30*sizeof(char));
      sprintf(reschaine,"NS:%d  EO:%d  \n Doit Jouer:%c",ihm_pli->pli->nbpli_ligne[0],ihm_pli->pli->nbpli_ligne[1],resps[0]);
    gtk_label_set_text (GTK_LABEL (ihm_pli->Score),reschaine );
    free(reschaine);
    free(resps);
    gtk_label_set_text (GTK_LABEL (ihm_pli->Label),g_strdup_printf("Contrat Final:%s",ihm_pli->scontrat));

    
    return((ihm_pli->pli->nextpos)%2);
  }
  else {
    printf("Player\n");

  
    position_middle(ihm_pli,imgcard,poscour);
    ihm_pli->waiting=g_list_prepend(ihm_pli->waiting, (gpointer)imgcard);
    ihm_pli->players[poscour]=g_list_remove(ihm_pli->players[poscour],(gconstpointer) imgcard);
    
    ihm_pli->pli->carte[poscour].clcarte=imgcard->couleur;
    ihm_pli->pli->carte[poscour].nocarte=imgcard->valeur;
    ihm_pli->tab_couleur[poscour][imgcard->couleur]--;
    draw_container_ihm(ihm_pli);

    
    write_data(ihm_pli,ihm_pli->pli,'p');
    status = read_header (ihm_pli,ihm_pli->pli, 'p');
    resps=affichage(ihm_pli->pli->nextpos,POSITION); 
    gtk_label_set_text (GTK_LABEL (ihm_pli->Score),g_strdup_printf("NS:%d  EO:%d  \n Doit Jouer:%c",ihm_pli->pli->nbpli_ligne[0],ihm_pli->pli->nbpli_ligne[1],resps[0] ));
    gtk_label_set_text (GTK_LABEL (ihm_pli->Label),g_strdup_printf("Contrat Final:%s",ihm_pli->scontrat));
    return(ihm_pli->pli->nextpos%2);

  }

  free(rescl);
  free(resvl);
  free(resps);
}

void locate_cards(GList *listplayer,position_t position,ihm_pli_t *ihm_pli) {

  int x, y, w, h;
  int coef;
  enum eposition ep;
  if(ihm_pli->back) {
    gdk_drawable_get_size(ihm_pli->back, &w, &h);
    switch(position) {
      case NORD:
        y = 10;
        coef = 1;
        x = (ihm_pli->Drawing_area->allocation.width - w)*2 / 3;
        ep = EP_HORIZONTAL;
        break;
        case EST:
          x = ihm_pli->Drawing_area->allocation.width - w -10;
          coef = 1;
          //y = (ihm_pli->Drawing_area->allocation.height - h)*2 / 2;
          y = (ihm_pli->Drawing_area->allocation.height - h -10)/2+60;
          ep = EP_VERTICAL;
          break;

        case SUD:
          y = ihm_pli->Drawing_area->allocation.height - h -10;
          coef = -1;
          x = (ihm_pli->Drawing_area->allocation.width - w) *2/ 3;
          ep = EP_HORIZONTAL;
          break;

        case OUEST:
          x = 0;
          coef = 1;
          //y = (ihm_pli->Drawing_area->allocation.height - h)*2 / 2 ;
          y = (ihm_pli->Drawing_area->allocation.height - h -10)/2+60;
          ep = EP_VERTICAL;
          break;

        default:
          break;

    }
    position_list(listplayer, x, y, ep, coef);
  }
}

void
recuperation_jeu (ihm_pli_t *ihm_pli, position_t position)
{
  imgcard_t *imgcard;
  char *rescl,*resvl;
  carte_t *carte;
  carte=malloc(sizeof(carte_t));
  int i, status;
  gchar *cardname=NULL;
  GdkGC *gc;
  GtkStateType state = GTK_WIDGET_STATE(ihm_pli->Drawing_area);
  couleur_t couleur;
  char model[] = "paris";

  cardname = (gchar*)g_malloc((strlen(ihm_pli->path)+20)*(sizeof(gchar)));
  gc = ihm_pli->Drawing_area->style->fg_gc[state];
  gdk_gc_set_clip_mask(gc,ihm_pli->backmask);
  if(ihm_pli->back==NULL) {
    sprintf(cardname, "%s/%s/back.xpm", ihm_pli->path,model);
    if(g_file_test(cardname, G_FILE_TEST_EXISTS) == TRUE) {
      ihm_pli->back = gdk_pixmap_create_from_xpm( ihm_pli->Drawing_area->window,
                                &(ihm_pli->backmask),
                                &(ihm_pli->Drawing_area->style->black),
                                cardname);
    }
  }
  for( couleur=trefle;couleur<pique+1;couleur++)
    ihm_pli->tab_couleur[position][couleur]=0;

  for (i = 0; i < NBPCOULEUR; i++) {
      status=read_header (ihm_pli, carte, 'c');
      resvl=affichage(carte->nocarte,CARTE);
      rescl=affichage(carte->clcarte,COULEUR);
      sprintf(cardname, "%s/%s%s.xpm", ihm_pli->path,resvl ,rescl);

      if ( g_file_test(cardname, G_FILE_TEST_EXISTS) == TRUE ) {
        imgcard=load_imgcard(cardname,carte,ihm_pli,position); 
         ihm_pli->players[position]=g_list_prepend(ihm_pli->players[position], (gpointer)imgcard);
      }

      ihm_pli->tab_couleur[position][carte->clcarte]=ihm_pli->tab_couleur[position][carte->clcarte]+1;
	
      
    }
    locate_cards(ihm_pli->players[position],position,ihm_pli);
  free(carte);
  free(rescl);
  free(resvl);
  g_free(cardname);
}
