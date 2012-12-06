#include "objets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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

void position_middle( ihm_trick_t *ihm_setup,imgcard_t *card,position_t position) {
  int midx,midy;
  if (card==NULL) {
   printf ("Erreur in position_middle\n");
   exit(1);
  }
  midx = (ihm_setup->Drawing_area->allocation.width - card->dim.w)/2;
  midy = (ihm_setup->Drawing_area->allocation.height - card->dim.h)/2;
  
  game_middle_offset_dynamic(position, &midx, &midy,
                ihm_setup->Drawing_area->allocation.width,
                ihm_setup->Drawing_area->allocation.height,
                card->dim.w, card->dim.h);

        if(card->dim.x != midx || card->dim.y != midy)
        {
                //if(ihm_setup->animation)
                 //       position_move_card(card, midx, midy, prog_data);


                card->dim.x = midx;
                card->dim.y = midy;
                draw_container_ihm(ihm_setup);
        }
 

}
void position_targetzone(ihm_trick_t *ihm_setup)
{
        
        if(ihm_setup->dropping != NULL)
        {
                int w, h;
                GList *lst = ihm_setup->dropping;

                rectangle_t *zone;

                if(ihm_setup->back)
                {

                        gdk_drawable_get_size(ihm_setup->back,
                                &w, &h);

                        if(lst)
                        {
                                zone = (rectangle_t *)lst->data;

                                zone->dim.x = ihm_setup->Drawing_area->allocation.width/2;
                                zone->dim.y = ihm_setup->Drawing_area->allocation.height/2;

                                zone->dim.x -= w;
                                zone->dim.y -= (2*h)/3;
                                zone->dim.w = 2*w;
                                zone->dim.h = (4*h)/3;

                                zone->active = TRUE;
                        }
                }
        }
}




void destroy_ihm_setup (ihm_trick_t *ihm_setup) {
 position_t position;
 free( ihm_setup->contrat);
 for (position=sud;position>est+1;position++ ) 
   g_free (ihm_setup->players[position]);
 g_free(ihm_setup->players);
 free(ihm_setup->pli);
}

void init_game(ihm_trick_t *ihm_setup) {
   
  int contrat; 
  couleur_t couleur;
  free_ihm_setup(ihm_setup);
  reset_ihm_setup(ihm_setup);
  write_data (ihm_setup,NULL, 'n');

  recuperation_jeu(ihm_setup,0);
  draw_container_ihm(ihm_setup);
  for(contrat=0;contrat<7;contrat++){
    for(couleur=club;couleur<aucune+1;couleur++)
                gtk_widget_set_sensitive(ihm_setup->Allbid[couleur*7+contrat]->bwidget, TRUE);
  }



  
  ihm_setup->pli->nextpos=ouest;

}


void position_list(GList *ptr, int x, int y, enum eposition ep, float coef)
{
        int incx, incy;
        int xref=x;
        int step,i;
        int num;
        couleur_t couleurref=aucune;
        int tab_coul[spade+1];
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


int game_turn (ihm_trick_t *ihm_setup,imgcard_t *imgcard) {

  position_t poscour;
  char *cardname,*rescl,*resvl,*resps;
  GtkStateType state = GTK_WIDGET_STATE(ihm_setup->Drawing_area);
  char *reschaine;
  GdkGC *gc;
  
  printf("Debut game_turn\n");
    //Nettoyage,  à changer 
  if(ihm_setup->pli->noj==0) {
    printf("Nettoyage\n");
    trash_list(ihm_setup);
  }
  // C'est la fin au a deja ihm_setup->nbcard plis 
  if((ihm_setup->pli->nbtricks_line[1]+ihm_setup->pli->nbtricks_line[0] ) ==ihm_setup->nbcard ) {
    printf("The end recuperation_jeu\n");
      
   return 0;
  }
  poscour = ihm_setup->pli->nextpos;
  if(ihm_setup->debug)
    fprintf(stdout,"ihm_setup->pli->nbtricks_line[1]=%d\nihm_setup->pli->nbtricks_line[0]=%d\nposcour=%d\n",ihm_setup->pli->nbtricks_line[1],ihm_setup->pli->nbtricks_line[0],poscour);


  if((ihm_setup->pli->nextpos%2)==(position_t)ihm_setup->ligneia) {
   
    printf("IA joue\n");
    read_header(ihm_setup,ihm_setup->pli, 'p');
    display_trick(ihm_setup->pli);
    cardname = (gchar*)g_malloc((strlen(ihm_setup->path)+20)*(sizeof(gchar)));
    gc = ihm_setup->Drawing_area->style->fg_gc[state];
    gdk_gc_set_clip_mask(gc,ihm_setup->backmask);
    if( ihm_setup->pli->noj==0) {
      rescl=affichage((int)ihm_setup->pli->lastcarte.clcarte,COULEUR);
      resvl=affichage((int)ihm_setup->pli->lastcarte.nocarte,CARTE);
      sprintf(cardname, "%s/%s%s.xpm", ihm_setup->path, resvl,rescl);
      if ( g_file_test(cardname, G_FILE_TEST_EXISTS) == TRUE ) {
        imgcard=load_imgcard(cardname,&ihm_setup->pli->lastcarte,ihm_setup,poscour); 
      }
        
           
      position_middle(ihm_setup,imgcard,poscour);
      ihm_setup->waiting=g_list_prepend(ihm_setup->waiting, (gpointer)imgcard);
    }

    else {
      resvl=affichage(ihm_setup->pli->carte[poscour].nocarte,CARTE);
      rescl=affichage(ihm_setup->pli->carte[poscour].clcarte,COULEUR);
      sprintf(cardname, "%s/%s%s.xpm", ihm_setup->path, resvl,rescl);
      if ( g_file_test(cardname, G_FILE_TEST_EXISTS) == TRUE ) {
        imgcard=load_imgcard(cardname,&ihm_setup->pli->carte[poscour],ihm_setup,poscour); 
      }
        position_middle(ihm_setup,imgcard,poscour);
        ihm_setup->waiting=g_list_prepend(ihm_setup->waiting, (gpointer)imgcard);

    }
    //Ajout clignotement
    if(imgcard->position==ihm_setup->pli->nextpos) {
      imgcard->blink=TRUE;
      imgcard->draw=FALSE;
      draw_copy(ihm_setup);
      ihm_setup->blinkid=g_timeout_add(800,event_blink, (gpointer)ihm_setup);
    }

    draw_container_ihm(ihm_setup);
    resps=affichage(ihm_setup->pli->nextpos,POSITION); 
      reschaine=malloc(30*sizeof(char));
      sprintf(reschaine,"NS:%d  EO:%d  \n Doit Jouer:%c",ihm_setup->pli->nbtricks_line[0],ihm_setup->pli->nbtricks_line[1],resps[0]);
    gtk_label_set_text (GTK_LABEL (ihm_setup->Score),reschaine );
    free(reschaine);
    free(resps);
    gtk_label_set_text (GTK_LABEL (ihm_setup->Label),g_strdup_printf("Contrat Final:%s",ihm_setup->scontrat));

    
    return((ihm_setup->pli->nextpos)%2);
  }
  else {
    printf("Player\n");

  
    position_middle(ihm_setup,imgcard,poscour);
    ihm_setup->waiting=g_list_prepend(ihm_setup->waiting, (gpointer)imgcard);
    ihm_setup->players[poscour]=g_list_remove(ihm_setup->players[poscour],(gconstpointer) imgcard);
    
    ihm_setup->pli->carte[poscour].clcarte=imgcard->couleur;
    ihm_setup->pli->carte[poscour].nocarte=imgcard->valeur;
    ihm_setup->tab_couleur[poscour][imgcard->couleur]--;
    draw_container_ihm(ihm_setup);

    
    write_data(ihm_setup,ihm_setup->pli,'p');
    read_header (ihm_setup,ihm_setup->pli, 'p');
    display_trick(ihm_setup->pli);
    resps=affichage(ihm_setup->pli->nextpos,POSITION); 
    gtk_label_set_text (GTK_LABEL (ihm_setup->Score),g_strdup_printf("NS:%d  EO:%d  \n Doit Jouer:%c",ihm_setup->pli->nbtricks_line[0],ihm_setup->pli->nbtricks_line[1],resps[0] ));
    gtk_label_set_text (GTK_LABEL (ihm_setup->Label),g_strdup_printf("Contrat Final:%s",ihm_setup->scontrat));
    return(ihm_setup->pli->nextpos%2);

  }

  free(rescl);
  free(resvl);
  free(resps);
}

void locate_cards(GList *listplayer,position_t position,ihm_trick_t *ihm_setup) {

  int x, y, w, h;
  int coef;
  enum eposition ep;
  if(ihm_setup->back) {
    gdk_drawable_get_size(ihm_setup->back, &w, &h);
    switch(position) {
      case NORD:
        y = 10;
        coef = 1;
        x = (ihm_setup->Drawing_area->allocation.width - w)*2 / 3;
        ep = EP_HORIZONTAL;
        break;
        case EST:
          x = ihm_setup->Drawing_area->allocation.width - w -10;
          coef = 1;
          //y = (ihm_setup->Drawing_area->allocation.height - h)*2 / 2;
          y = (ihm_setup->Drawing_area->allocation.height - h -10)/2+60;
          ep = EP_VERTICAL;
          break;

        case SUD:
          y = ihm_setup->Drawing_area->allocation.height - h -10;
          coef = -1;
          x = (ihm_setup->Drawing_area->allocation.width - w) *2/ 3;
          ep = EP_HORIZONTAL;
          break;

        case OUEST:
          x = 0;
          coef = 1;
          //y = (ihm_setup->Drawing_area->allocation.height - h)*2 / 2 ;
          y = (ihm_setup->Drawing_area->allocation.height - h -10)/2+60;
          ep = EP_VERTICAL;
          break;

        default:
          break;

    }
    position_list(listplayer, x, y, ep, coef);
  }
}

void
recuperation_jeu (ihm_trick_t *ihm_setup, position_t position)
{
  imgcard_t *imgcard;
  char *rescl,*resvl,refcoul='O',*resp;
  char savefile[MAXFILENAME];
  carte_t *carte;
  char writebuf[NBPCOULEURS+2];
  char *colorref="TKCP";
  int j=0,k;
  carte=malloc(sizeof(carte_t));
  int i;
  gchar *cardname=NULL;
  GdkGC *gc;
  GtkStateType state = GTK_WIDGET_STATE(ihm_setup->Drawing_area);
  couleur_t couleur;
  char model[] = "paris";
  resp=affichage(position,POSITION);
  cardname = (gchar*)g_malloc((strlen(ihm_setup->path)+20)*(sizeof(gchar)));
  gc = ihm_setup->Drawing_area->style->fg_gc[state];
  gdk_gc_set_clip_mask(gc,ihm_setup->backmask);
  if(ihm_setup->back==NULL) {
    sprintf(cardname, "%s/%s/back.xpm", ihm_setup->path,model);
    if(g_file_test(cardname, G_FILE_TEST_EXISTS) == TRUE) {
      ihm_setup->back = gdk_pixmap_create_from_xpm( ihm_setup->Drawing_area->window,
                                &(ihm_setup->backmask),
                                &(ihm_setup->Drawing_area->style->black),
                                cardname);
    }
  }
  for( couleur=club;couleur<spade+1;couleur++)
    ihm_setup->tab_couleur[position][couleur]=0;
  if(((ihm_setup->pli->nbtricks_line[1]+ihm_setup->pli->nbtricks_line[0] ) ==ihm_setup->nbcard)&&TRUE==ihm_setup->savegame) {
    if(NULL==ihm_setup->fd) {
      snprintf(savefile,MAXFILENAME,SAVEFILEDIR"donne.%d",ihm_setup->random);
      if(position==sud) 
        ihm_setup->fd=fopen(savefile,"w");
      else
        ihm_setup->fd=fopen(savefile,"a");
    }
    if(position==sud){
      snprintf(writebuf,NBPCOULEURS+2,"%c",resp[0]);
      fwrite(writebuf,strlen(writebuf),1,ihm_setup->fd);
    }
    else{
      snprintf(writebuf,NBPCOULEURS+2,"\n%c",resp[0]);
      fwrite(writebuf,strlen(writebuf),1,ihm_setup->fd);
    }
  }
  i=0;
  do {
      read_header (ihm_setup, carte, 'c');
      resvl=affichage(carte->nocarte,CARTE);
      rescl=affichage(carte->clcarte,COULEUR);
      sprintf(cardname, "%s/%s%s.xpm", ihm_setup->path,resvl ,rescl);

      if ( g_file_test(cardname, G_FILE_TEST_EXISTS) == TRUE ) {
        imgcard=load_imgcard(cardname,carte,ihm_setup,position); 
         ihm_setup->players[position]=g_list_prepend(ihm_setup->players[position], (gpointer)imgcard);
        if(((ihm_setup->pli->nbtricks_line[1]+ihm_setup->pli->nbtricks_line[0] ) ==ihm_setup->nbcard)&&TRUE==ihm_setup->savegame) {
          if(ihm_setup->fd) {
            if(refcoul!=rescl[0]){ 
              if(rescl[0] != colorref[j]) {
                snprintf(writebuf,NBPCOULEURS+2,"\n%c",colorref[j]);
                fwrite(writebuf,strlen(writebuf),1,ihm_setup->fd);
                j++;
              }
              snprintf(writebuf,NBPCOULEURS+2,"\n%c%c",rescl[0],resvl[0]);
              fwrite(writebuf,strlen(writebuf),1,ihm_setup->fd);
              refcoul=rescl[0];
              j++;
            }
            else{
              snprintf(writebuf,NBPCOULEURS+2,"%c",resvl[0]);
              fwrite(writebuf,strlen(writebuf),1,ihm_setup->fd);
            }
          }
          
        }
      }

      ihm_setup->tab_couleur[position][carte->clcarte]=ihm_setup->tab_couleur[position][carte->clcarte]+1;
	
      free(rescl);
      free(resvl);
      i++;
      
  } while (i<ihm_setup->nbcard);
  if((ihm_setup->fd)&&(j<NBCOULEURS)) {
    for(k=j;k<NBCOULEURS;k++) {
       printf("k=%d\n",k);
       snprintf(writebuf,NBPCOULEURS+2,"\n%c",colorref[k]);
       fwrite(writebuf,strlen(writebuf),1,ihm_setup->fd);
    }
    
  }
  if(((ihm_setup->pli->nbtricks_line[1]+ihm_setup->pli->nbtricks_line[0] ) ==ihm_setup->nbcard)&&(TRUE==ihm_setup->savegame )&& (position==est) &&ihm_setup->fd) {
    snprintf(writebuf,NBPCOULEURS+2,"\n");
    fwrite(writebuf,1,1,ihm_setup->fd);
  }
  locate_cards(ihm_setup->players[position],position,ihm_setup);
  free(carte);
  free(resp);
  
  if(ihm_setup->fd) {
    fclose(ihm_setup->fd);
    ihm_setup->fd=NULL;
  }
  
  g_free(cardname);
}
