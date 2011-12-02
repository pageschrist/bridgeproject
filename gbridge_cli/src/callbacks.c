#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib/ghash.h>
#include <gdk/gdkkeysyms.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include "objets.h"
#include "client.h"
#include "draw.h"
#include "ihmbridge.h"
#include "load.h"
#include "jeu.h"
#include "free.h"
#include "find.h"
#include "callbacks.h"
#include "alloc.h"
#include "traffic.h"

void fdebug (void) {
 int i;
 i=i*3;
}

void enter_callback_level( GtkWidget *entry,
                            ihm_pli_t *ihm_pli )
{
  const gchar *entry_text_level;
  entry_text_level = gtk_entry_get_text (GTK_ENTRY (entry));
  ihm_pli->level=atoi(entry_text_level);
  printf ("Level : %d\n", ihm_pli->level);
}
void enter_callback_random( GtkWidget *entry,
                            ihm_pli_t *ihm_pli )
{
  const gchar *entry_text_random;
  entry_text_random = gtk_entry_get_text (GTK_ENTRY (entry));
  ihm_pli->random=atoi(entry_text_random);
  printf ("Random : %d\n", ihm_pli->random);
}
void new_dist (GtkButton *button,ihm_pli_t *ihm_pli) {
  button=button;
  int contrat;
  couleur_t couleur;
  free_ihm_pli(ihm_pli);
  reset_ihm_pli(ihm_pli);
  write_data (ihm_pli,NULL, 'n');
  if(ihm_pli->debug) 
    fprintf(stderr,"recuperation_jeu 0, new_dist%c %d\n",ihm_pli->status,ihm_pli->read);
  recuperation_jeu(ihm_pli,0);
  draw_container_ihm(ihm_pli);
  for(contrat=0;contrat<7;contrat++){
    for(couleur=trefle;couleur<aucune+1;couleur++) 
                gtk_widget_set_sensitive(ihm_pli->Allbid[couleur*7+contrat]->bwidget, TRUE);
  }


}

 void file_ok_sel( GtkWidget        *w,
                         ihm_pli_t *ihm_pli )
{
    w=w;
    int contrat;
    couleur_t couleur;
    struct stat *buf;
    buf=malloc(sizeof(struct stat));
    snprintf (ihm_pli->filename,MAXFILENAME,"%s", gtk_file_selection_get_filename (GTK_FILE_SELECTION( (GtkFileSelection *)ihm_pli->File_S)));
    if (ihm_pli->debug)
      printf("filename=%s\n",ihm_pli->filename);
    if(strlen(ihm_pli->filename)!=0) {
      if(!stat(ihm_pli->filename,buf)) {
        
        if(!send_file(ihm_pli))
         fprintf(stderr,"Send of file failed\n");
      }
      else 
        fprintf(stderr,"File not found or unreadable\n");
      
      if(ihm_pli->debug) 
        fprintf(stderr,"recuperation_jeu 0, new_dist%c %d\n",ihm_pli->status,ihm_pli->read);
      recuperation_jeu(ihm_pli,0);
      draw_container_ihm(ihm_pli);
      for(contrat=0;contrat<7;contrat++){
        for(couleur=trefle;couleur<aucune+1;couleur++) 
                    gtk_widget_set_sensitive(ihm_pli->Allbid[couleur*7+contrat]->bwidget, TRUE);
      }

    }
}
gboolean open_file (GtkButton *button,ihm_pli_t *ihm_pli) {
  button=button;
  ihm_pli->File_S=gtk_file_selection_new ("File selection");
  g_signal_connect (G_OBJECT (ihm_pli->File_S), "destroy",
                      G_CALLBACK (gtk_widget_destroy), NULL);
    /* Connect the ok_button to file_ok_sel function */
    g_signal_connect (G_OBJECT (GTK_FILE_SELECTION (ihm_pli->File_S)->ok_button),
                      "clicked", G_CALLBACK (file_ok_sel),  ihm_pli);

    /* Connect the cancel_button to destroy the widget */
    g_signal_connect_swapped (G_OBJECT (GTK_FILE_SELECTION (ihm_pli->File_S)->cancel_button),
                              "clicked", G_CALLBACK (gtk_widget_destroy),
                              G_OBJECT (ihm_pli->File_S));

    /* Lets set the filename, as if this were a save dialog, and we are giving
     a default filename */
    gtk_file_selection_set_filename (GTK_FILE_SELECTION(ihm_pli->File_S),
                                     "penguin.png");

    gtk_widget_show (ihm_pli->File_S);

return(FALSE);

}

void click_bid (GtkButton *button,button_bid_t *button_bid) {
  button=button;
  couleur_t couleur;
  int contrat;
  if(button_bid->ihm_pli->debug) {
    printf("Nombre de plis:%d\n",button_bid->ihm_bid->bid->nombre);
    printf("couleur:%d\n",button_bid->ihm_bid->bid->couleur);
  }
  for(contrat=0;contrat<button_bid->ihm_bid->bid->nombre-1;contrat++){
    for(couleur=trefle;couleur<aucune+1;couleur++) 
                gtk_widget_set_sensitive(button_bid->ihm_pli->Allbid[couleur*7+contrat]->bwidget, FALSE);
  }
  for(couleur=trefle;couleur<button_bid->ihm_bid->bid->couleur+1;couleur++) {
    gtk_widget_set_sensitive(button_bid->ihm_pli->Allbid[couleur*7+contrat]->bwidget, FALSE);

  }
  write_data(button_bid->ihm_pli,button_bid->ihm_bid->bid,'b');
  button_bid->ihm_pli->state=BID;
  button_bid->ihm_pli->status='b';
  button_bid->ihm_pli->read=TRUE;
}
gboolean key_down(GtkWidget *widget,
        GdkEventKey *event,
        gpointer user_data)
{
        widget=widget;
        ihm_pli_t *ihm_pli = user_data;

        if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Escape)
        {
                if(ihm_pli->movecard)
                {
                        if(ihm_pli->copy)
                                g_object_unref(ihm_pli->copy);

                        ihm_pli->copy = NULL;

                        ihm_pli->movecard->card->draw = TRUE;

                        g_free(ihm_pli->movecard);
                        ihm_pli->movecard = NULL;

                        draw_container_ihm(ihm_pli);
                }
        }
        return(TRUE);
}



gboolean event_blink(gpointer data)
{
        ihm_pli_t *ihm_pli = (ihm_pli_t*) data;
  if(ihm_pli->debug) 
    printf("Event blinck\n");

        imgcard_t *tmp;

        GList *lst;

        imgcard_t *card = NULL;


        for(lst = ihm_pli->waiting; lst; lst = lst->next)
        {
                tmp = lst->data;

                if(tmp->blink == TRUE)
                {
                        tmp->draw = !tmp->draw;
                        card = tmp;
                }
        }


        if(card && ihm_pli->copy)
        {
                GdkGC *gc = ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)];

                if(card->draw == TRUE)
                {
                        gdk_gc_set_clip_mask(gc,
                                ihm_pli->cardmask);

                        gdk_gc_set_clip_origin(gc,
                                card->dim.x,
                                card->dim.y);

                        gdk_draw_drawable( ihm_pli->target,
                                                                                  gc,
                                card->img, 0, 0,
                                card->dim.x, card->dim.y, card->dim.w, card->dim.h);


                        gdk_gc_set_clip_mask(gc, NULL);
                }
                else
                        gdk_draw_drawable(ihm_pli->target,
                                gc,
                                ihm_pli->copy,
                                card->dim.x, card->dim.y, card->dim.x, card->dim.y,
                                card->dim.w, card->dim.h);


                gdk_draw_drawable( ihm_pli->Drawing_area->window,
                        gc,     ihm_pli->target,
                                card->dim.x, card->dim.y, card->dim.x, card->dim.y,
                                card->dim.w, card->dim.h);

        }
        if(ihm_pli->debug)
        printf("Event blinck\n");
        return(TRUE);
}


void clear_blink(ihm_pli_t *ihm_pli)
{
        if(ihm_pli->debug)
          printf("On est dans blink_clear\n");
        GList *lst;
        imgcard_t *card;

        if(ihm_pli->blinkid != -1 )
        {
                g_source_remove(ihm_pli->blinkid);
                ihm_pli->blinkid = -1;
        }

        for(lst = ihm_pli->waiting; lst; lst = lst->next)
        {
                card = lst->data;
                card->blink = FALSE;
                card->draw = TRUE;
        }

        if(ihm_pli->copy)
                g_object_unref(ihm_pli->copy);

        ihm_pli->copy = NULL;
}


gboolean mise_en_place ( GtkWidget *Drawing_area,ihm_pli_t *ihm_pli) {
  Drawing_area=Drawing_area;
  int contrat;
  couleur_t couleur;
  if(ihm_pli->debug)
    printf("mise_en_place\n");
  alloc_ihm(ihm_pli);
  for(couleur=trefle;couleur<aucune+1;couleur++) {
    for(contrat=0;contrat<7;contrat++) {
      gtk_image_set_from_pixbuf( GTK_IMAGE(ihm_pli->Allbid[couleur*7+contrat]->widget),ihm_pli->Allbid[couleur*7+contrat]->wimage);
      

    }


  }
  for(contrat=0;contrat<3;contrat++) {
      gtk_image_set_from_pixbuf( GTK_IMAGE(ihm_pli->Othbid[contrat]->widget),ihm_pli->Othbid[contrat]->wimage);
      

  }
  init_game(ihm_pli);
  position_targetzone(ihm_pli);
  
  

  return TRUE;
}
void action_click(ihm_pli_t *ihm_pli)
{

        draw_container_ihm(ihm_pli);
}


gboolean button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  widget=widget;
  ihm_pli_t *ihm_pli = (ihm_pli_t *)data;
  position_t position;
  if(ihm_pli->status=='b'  ) 
    return FALSE;
  // C'est la fin on a deja 13 plis 
  if((ihm_pli->pli->nbpli_ligne[1]+ihm_pli->pli->nbpli_ligne[0] ) ==13 ) {
    //if(ihm_pli->read==FALSE  ) 
     // return TRUE;
    if(ihm_pli->debug)
      printf("The end \n");
    trash_list(ihm_pli);
    gtk_label_set_justify(GTK_LABEL(ihm_pli->Label), GTK_JUSTIFY_CENTER);
    gtk_label_set_text (GTK_LABEL (ihm_pli->Label),g_strdup_printf("Final Contrat:%s\nEnd of Game",ihm_pli->scontrat));
    for (position=sud;position<est+1;position++) {
      if(ihm_pli->debug)
        fprintf(stderr,"recuperation_jeu position=%d, button_press_event %c %d\n", position,ihm_pli->status, ihm_pli->read);
      recuperation_jeu (ihm_pli,  position);
    }
    draw_container_ihm(ihm_pli); 
    ihm_pli->state=BID; 
    ihm_pli->status='b'; 
  }
  else {
        if(ihm_pli->pli->noj==0)
          trash_list(ihm_pli);
        gboolean found = FALSE;

        if(event->button == 1)
        {
                found = find_moveable_card(ihm_pli->waiting, event, ihm_pli);

                if(found == FALSE) {

                  if((ihm_pli->pli->nextpos)%2 !=(position_t) ihm_pli->ligneia)
                        found = find_moveable_card(ihm_pli->players[ihm_pli->pli->nextpos], event, ihm_pli);
                }

                if(found == TRUE)
                        clear_blink(ihm_pli);
        }
  }
  if(ihm_pli->debug)
    printf("End button_press_event\n");
  return(TRUE);
}

gboolean expose_comment( GtkWidget *Fenetre, GdkEventExpose *event, ihm_pli_t *ihm_pli)
{
  event=event;
  Fenetre=Fenetre;
  int status;
  position_t position;
  if(ihm_pli->debug)
    printf("expose comment %d\n", ihm_pli->read);
      if(ihm_pli->status=='b' && ihm_pli->read==TRUE) {
        ihm_pli->read=FALSE;
        int number,contrat,i;
        couleur_t color,couleur;
        char *coulref="TKCPS";
        char *dis_bid=NULL;
        status = read_header(ihm_pli,ihm_pli->cur_bid,'u');
        printf("expose comment:read cur_bid%s\n",ihm_pli->cur_bid);
        
        if(status <=0) {
          printf("status:%d\n",status); 
          exit(1);
        }
        printf("%s\n",ihm_pli->cur_bid);
        dis_bid=display_str(ihm_pli->cur_bid);
        
        gtk_label_set_justify(GTK_LABEL(ihm_pli->Label), GTK_JUSTIFY_LEFT);
        gtk_label_set_text (GTK_LABEL (ihm_pli->Label),g_strdup_printf("Bids:\nS \tW \tN \tE \n%s\n",dis_bid));
        if((ihm_pli->cur_bid[strlen(ihm_pli->cur_bid)-1])=='P' &&(ihm_pli->cur_bid[strlen(ihm_pli->cur_bid)-2])=='P' && (ihm_pli->cur_bid[strlen(ihm_pli->cur_bid)-3])=='P'&& (ihm_pli->cur_bid[strlen(ihm_pli->cur_bid)-4])=='P') {
          printf("Game Starting\n");
          fdebug();
                   
          for(i=strlen(ihm_pli->cur_bid)-1;i>0;i=i-2) {

            if(ihm_pli->cur_bid[i-1]!='P') {
               ihm_pli->scontrat=malloc( 3*sizeof(char));
              ihm_pli->scontrat[0]=ihm_pli->cur_bid[i-1];
              ihm_pli->scontrat[1]=ihm_pli->cur_bid[i];
              ihm_pli->scontrat[2]='\0';
              break;

            }
          }
          if(ihm_pli->scontrat==NULL) {
            printf("No Bids, redistribute\n");
            return FALSE;

          }
 
        gtk_label_set_justify(GTK_LABEL(ihm_pli->Label), GTK_JUSTIFY_LEFT);
        gtk_label_set_text (GTK_LABEL (ihm_pli->Label),g_strdup_printf("Bids:\nS \tW \tN \tE \n%s\nFinal Contrat: %s\n",dis_bid,ihm_pli->scontrat));
        if(ihm_pli->debug)
	  fprintf(stderr,"recuperation_jeu +2 av, expose_comment%c %d\n",ihm_pli->status,ihm_pli->read);
        ihm_pli->state=STARTING; 
        ihm_pli->status='g'; 
          

          for(contrat=0;contrat<7;contrat++){
            for(couleur=trefle;couleur<aucune+1;couleur++) 
                gtk_widget_set_sensitive(ihm_pli->Allbid[couleur*7+contrat]->bwidget, FALSE);
          }
        if(ihm_pli->debug)
	  fprintf(stderr,"recuperation_jeu +2, expose_comment:%c %d\n",ihm_pli->status,ihm_pli->read);
          for (position=sud;position<est+1;position++ ) {
            free_list(&ihm_pli->players[position]);
            ihm_pli->players[position]=NULL;
          }
          recuperation_jeu(ihm_pli,(ihm_pli->contrat->declarant)%4);
          recuperation_jeu(ihm_pli,(ihm_pli->contrat->declarant+2)%4);
          draw_container_ihm(ihm_pli); 
          ihm_pli->pli->nextpos=(ihm_pli->contrat->declarant+1)%4;
 /* Blit */
           
        }
        else {
          number=ihm_pli->cur_bid[strlen(ihm_pli->cur_bid)-4]-48;
          for (i=0;i<(int)strlen(coulref);i++) {
            if(coulref[i]==ihm_pli->cur_bid[strlen(ihm_pli->cur_bid)-3])
              color=(couleur_t) i;
      
          }
        
          for(couleur=trefle;couleur<aucune+1;couleur++) 
            for(contrat=0;contrat<number-1;contrat++){
              if(ihm_pli->debug)
                printf("couleur=%d,number=%d,contrat=%d\n",couleur,number,contrat);
              gtk_widget_set_sensitive(ihm_pli->Allbid[couleur*7+contrat]->bwidget, FALSE);
          }
          for(couleur=trefle;couleur<color+1;couleur++) {
            gtk_widget_set_sensitive(ihm_pli->Allbid[couleur*7+contrat]->bwidget, FALSE);

          }
        }
        free(dis_bid);
      }
  return FALSE;

}
gboolean rafraichissement( GtkWidget *Drawing_area, GdkEventExpose *event, ihm_pli_t *ihm_pli)
{
      event=event;
      Drawing_area=Drawing_area;
     // int status; 

      draw_container_ihm(ihm_pli); 
      if(ihm_pli->debug)
        printf("expose Drawing_area,\n");
     // if(ihm_pli->state==BID && ihm_pli->read==TRUE) {
     //   printf("expose Drawing_area enchere,\n");
     //   bid_t *bid=malloc(sizeof(bid_t));

     //   if(ihm_pli->status=='b'  ) {
     //     status = read_header(ihm_pli,ihm_pli->cur_bid,'b');
          
          
     //     ihm_pli->read=FALSE;
      //  } 
      //  free(bid);

      //}
        return FALSE;
}

gboolean quit(GtkWidget *window, ihm_pli_t *ihm_pli)
{
        window=window;
        ihm_pli->status='e';
        //if(ihm_pli->debug) 
          fprintf(stdout,"End of Session\n");
        write_data(ihm_pli,NULL,'e');
        shutdown(ihm_pli->socketid,SHUT_RDWR);
        close(ihm_pli->socketid);
        gtk_main_quit();

        return TRUE;
}

gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
        widget=widget;
        ihm_pli_t *ihm_pli = (ihm_pli_t*)data;
        if(ihm_pli->debug)
          printf("On est dans motion_notify_event\n");

        rectangle_t *zone = NULL;

        GdkPixmap *source;

        int x, y, w, h, toph, leftw,
            ax, ay, aw, ah,
            bx, by, bw, bh;

        if(ihm_pli->movecard)
        {
                if(!ihm_pli->copy)
                {
                        ihm_pli->movecard->card->draw = FALSE;

                        draw_copy(ihm_pli);

                        ihm_pli->movecard->card->draw = TRUE;
                }

                if(ihm_pli->copy)
                {
                        x = ihm_pli->movecard->dim.x;
                        y = ihm_pli->movecard->dim.y;

                        w = ihm_pli->movecard->card->dim.w;
                        h = ihm_pli->movecard->card->dim.h;

                        gdk_draw_drawable(ihm_pli->target,
                                ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                ihm_pli->copy,
                                x-1, y-1, x-1, y-1, w+2, h+2);

                        if(ihm_pli->dropping)
                        {
                                zone = draw_dropping_zone(ihm_pli,
                                        ihm_pli->target);

                        }

                        ihm_pli->movecard->dim.x =
                                (gint)event->x -ihm_pli->movecard->dim.w;

                        ihm_pli->movecard->dim.y =
                                (gint)event->y -ihm_pli->movecard->dim.h;
                        if(zone)
                        {
                                if(zone->active == FALSE)
                                        source = ihm_pli->copy;
                                else
                                        source = ihm_pli->target;


                                gdk_draw_drawable(ihm_pli->Drawing_area->window,
                                ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                source,
                                zone->dim.x, zone->dim.y,
                                zone->dim.x, zone->dim.y,
                                zone->dim.w, 1);


                                gdk_draw_drawable(ihm_pli->Drawing_area->window,
                                ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                source,
                                zone->dim.x+zone->dim.w,
                                zone->dim.y,
                                zone->dim.x+zone->dim.w,
                                zone->dim.y,
                                1, zone->dim.h);

                                gdk_draw_drawable(ihm_pli->Drawing_area->window,
                                ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                source,
                                zone->dim.x,
                                zone->dim.y+zone->dim.h,
                                zone->dim.x,
                                zone->dim.y+zone->dim.h,
                                zone->dim.w+1, 1);

                                gdk_draw_drawable(ihm_pli->Drawing_area->window,
                                ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                source,
                                zone->dim.x,
                                zone->dim.y,
                                zone->dim.x,
                                zone->dim.y,
                                1, zone->dim.h);

                        }

                        draw_moving_card(ihm_pli, ihm_pli->target);

                        gdk_draw_drawable(ihm_pli->Drawing_area->window,
                                ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                ihm_pli->target,
                                ihm_pli->movecard->dim.x,
                                ihm_pli->movecard->dim.y,
                                ihm_pli->movecard->dim.x,
                                                                             ihm_pli->movecard->dim.y,
                                w, h);


                        /*----Refreshing area optimization----
                         * try this on an old XXX Mhz, and 
                         * see how fast it is when you drag 
                         * a card around. */
                        if(x <= ihm_pli->movecard->dim.x)
                        {
                                leftw = ihm_pli->movecard->dim.x -x;
                                bx = x-1;

                                bw = leftw+2;
                        }
                        else
                        {
                                leftw = w-x+ihm_pli->movecard->dim.x;
                                bx = x+leftw-1;

                                bw = w-leftw+2;
                        }

                        if(y <= ihm_pli->movecard->dim.y)
                        {
                                ax = x-1;
                                ay = y-1;
                                toph = ihm_pli->movecard->dim.y -y;
                                ah = toph+1;
                                aw = w+2;

                                by = y+toph;
                                bh = h - toph +1;
                        }
                        else
                        {
                                toph = h-(y-ihm_pli->movecard->dim.y);
                                ax = x -1;
                                ay = y+toph;
                                              ah = h - toph +1;
                                aw = w +2;

                                by = y-1;
                                bh = toph+1;
                        }


                        gdk_draw_drawable(
                                ihm_pli->Drawing_area->window,
                                ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                ihm_pli->target,
                                ax, ay, ax, ay,
                                aw, ah);

                        gdk_draw_drawable(
                                ihm_pli->Drawing_area->window,
                                ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                ihm_pli->target,
                                bx, by, bx, by,
                                bw, bh);

                }

        }

        return TRUE;
}



void button_release_event (GtkWidget *widget,
        GdkEventButton *event, gpointer data)
{
        widget=widget;
        ihm_pli_t *ihm_pli = (ihm_pli_t *)data;
        int ligne;
        imgcard_t *drop = NULL;
        if(event->button == 1)
        {

          if( ihm_pli->pli->nextpos%2!=(position_t) ihm_pli->ligneia && ihm_pli->movecard) {
                        if(ihm_pli->copy)
                                g_object_unref(ihm_pli->copy);

                        ihm_pli->copy = NULL;

                        ihm_pli->movecard->card->draw = TRUE;
                        if(ihm_pli->movecard->card && game_drop_enable(ihm_pli))
                          drop = ihm_pli->movecard->card;
                        else {
                          g_free(ihm_pli->movecard);
                          ihm_pli->movecard=NULL; 
                        }


                        if(drop != NULL) {
                           drop->draw_face= TRUE;
                           drop->moveable= FALSE;
                           g_free(ihm_pli->movecard);
                           ihm_pli->movecard = NULL;
                          ligne=game_turn(ihm_pli, drop);
                        }

                        draw_container_ihm(ihm_pli);

                }
                if(ihm_pli->pli->nextpos%2==(position_t)ihm_pli->ligneia) {
                  do {
                
                    ligne=game_turn(ihm_pli, NULL);
                    printf("noj=%d\n",ihm_pli->pli->noj);
                  }
                  while(ligne==ihm_pli->ligneia && (ihm_pli->pli->noj != 0));
                 }


          }
}

gboolean game_drop_enable(ihm_pli_t *ihm_pli)
{
        rectangle_t *zone = get_active_targetzone(ihm_pli->dropping);

        return (
                 ( zone
                        && zone->active == TRUE)
        );
}

rectangle_t * get_active_targetzone(GList *lst)
{
        rectangle_t *zone = NULL;

        while(lst)
        {
                zone = lst->data;

                if(zone->active == TRUE)
                        lst = NULL;
                else
                        lst = lst->next;
        }

        return(zone);
}
