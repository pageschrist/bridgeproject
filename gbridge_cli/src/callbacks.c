#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
//#include <glib/ghash.h>
#include <glib.h>
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
#include "file.h"


void enter_callback_level( GtkWidget *entry,
                            ihm_trick_t *ihm_setup )
{
  const gchar *entry_text_level;
  entry_text_level = gtk_entry_get_text (GTK_ENTRY (entry));
  ihm_setup->level=atoi(entry_text_level);
  printf ("Level : %d\n", ihm_setup->level);
}
void enter_callback_random( GtkWidget *entry,
                            ihm_trick_t *ihm_setup )
{
  const gchar *entry_text_random;
  entry_text_random = gtk_entry_get_text (GTK_ENTRY (entry));
  ihm_setup->random=atoi(entry_text_random);
  printf ("Random : %d\n", ihm_setup->random);
}
void new_dist (GtkButton *button,ihm_trick_t *ihm_setup) {
  button=button;
  int contrat;
  couleur_t couleur;
  free_ihm_setup(ihm_setup);
  reset_ihm_setup(ihm_setup);
  write_data (ihm_setup,NULL, 'n');
  if(ihm_setup->debug) 
    fprintf(stderr,"recuperation_jeu 0, new_dist%c %d ihm_setup->nbcard=%d\n",ihm_setup->status,ihm_setup->read,ihm_setup->nbcard);
  recuperation_jeu(ihm_setup,0);
  draw_container_ihm(ihm_setup);
  for(contrat=0;contrat<7;contrat++){
    for(couleur=club;couleur<aucune+1;couleur++) 
                gtk_widget_set_sensitive(ihm_setup->Allbid[couleur*7+contrat]->bwidget, TRUE);
  }


}

 void file_ok_sel( GtkWidget        *w,
                         ihm_trick_t *ihm_setup )
{
    w=w;
    int contrat;
    couleur_t couleur;
    struct stat *buf;
    buf=malloc(sizeof(struct stat));
    snprintf (ihm_setup->filename,MAXFILENAME,"%s", gtk_file_selection_get_filename (GTK_FILE_SELECTION( (GtkFileSelection *)ihm_setup->File_S)));
    if (ihm_setup->debug)
      printf("filename=%s\n",ihm_setup->filename);
    if(strlen(ihm_setup->filename)!=0) {
      if(!stat(ihm_setup->filename,buf)) {
        
        if(!send_file(ihm_setup))
         fprintf(stderr,"Send of file failed\n");
      }
      else 
        fprintf(stderr,"File not found or unreadable%s\n",ihm_setup->filename);
      
      free_ihm_setup(ihm_setup);
      reset_ihm_setup(ihm_setup);
      recuperation_jeu(ihm_setup,0);
      draw_container_ihm(ihm_setup);
      for(contrat=0;contrat<7;contrat++){
        for(couleur=club;couleur<aucune+1;couleur++) 
                    gtk_widget_set_sensitive(ihm_setup->Allbid[couleur*7+contrat]->bwidget, TRUE);
      }

    }
    gtk_widget_destroy(ihm_setup->File_S);
}

void save_game (GtkButton *button,ihm_trick_t *ihm_setup) {
  button=button;
  ihm_setup->savegame=TRUE;

}

void open_file (GtkButton *button,ihm_trick_t *ihm_setup) {
  button=button;
  ihm_setup->File_S=gtk_file_selection_new (g_locale_to_utf8( "Select a file", -1, NULL, NULL, NULL));
  g_signal_connect (G_OBJECT (ihm_setup->File_S), "destroy",
                      G_CALLBACK (gtk_widget_destroy), NULL);
    /* Connect the ok_button to file_ok_sel function */
    g_signal_connect (G_OBJECT (GTK_FILE_SELECTION (ihm_setup->File_S)->ok_button),
                      "clicked", G_CALLBACK (file_ok_sel),  ihm_setup);

    /* Connect the cancel_button to destroy the widget */
    g_signal_connect_swapped (G_OBJECT (GTK_FILE_SELECTION (ihm_setup->File_S)->cancel_button),
                              "clicked", G_CALLBACK (gtk_widget_destroy),
                              G_OBJECT (ihm_setup->File_S));

    /* Lets set the filename, as if this were a save dialog, and we are giving
     a default filename */
    gtk_file_selection_set_filename (GTK_FILE_SELECTION(ihm_setup->File_S),
                                     DEFAULTDIRNAME);

    gtk_widget_show (ihm_setup->File_S);


}

void click_bid (GtkButton *button,button_bid_t *button_bid) {
  button=button;
  couleur_t couleur;
  int contrat;
  if(button_bid->ihm_setup->debug) {
    printf("Nombre de plis:%d\n",button_bid->ihm_bid->bid->nombre);
    printf("couleur:%d\n",button_bid->ihm_bid->bid->couleur);
  }
  for(contrat=0;contrat<button_bid->ihm_bid->bid->nombre-1;contrat++){
    for(couleur=club;couleur<aucune+1;couleur++) 
                gtk_widget_set_sensitive(button_bid->ihm_setup->Allbid[couleur*7+contrat]->bwidget, FALSE);
  }
  for(couleur=club;couleur<button_bid->ihm_bid->bid->couleur+1;couleur++) {
    gtk_widget_set_sensitive(button_bid->ihm_setup->Allbid[couleur*7+contrat]->bwidget, FALSE);

  }
  write_data(button_bid->ihm_setup,button_bid->ihm_bid->bid,'b');
  button_bid->ihm_setup->state=BID;
  button_bid->ihm_setup->status='b';
  button_bid->ihm_setup->read=TRUE;
}
gboolean key_down(GtkWidget *widget,
        GdkEventKey *event,
        gpointer user_data)
{
        widget=widget;
        ihm_trick_t *ihm_setup = user_data;

        if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Escape)
        {
                if(ihm_setup->movecard)
                {
                        if(ihm_setup->copy)
                                g_object_unref(ihm_setup->copy);

                        ihm_setup->copy = NULL;

                        ihm_setup->movecard->card->draw = TRUE;

                        g_free(ihm_setup->movecard);
                        ihm_setup->movecard = NULL;

                        draw_container_ihm(ihm_setup);
                }
        }
        return(TRUE);
}



gboolean event_blink(gpointer data)
{
        ihm_trick_t *ihm_setup = (ihm_trick_t*) data;
  if(ihm_setup->debug) 
    printf("Event blinck\n");

        imgcard_t *tmp;

        GList *lst;

        imgcard_t *card = NULL;


        for(lst = ihm_setup->waiting; lst; lst = lst->next)
        {
                tmp = lst->data;

                if(tmp->blink == TRUE)
                {
                        tmp->draw = !tmp->draw;
                        card = tmp;
                }
        }


        if(card && ihm_setup->copy)
        {
                GdkGC *gc = ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)];

                if(card->draw == TRUE)
                {
                        gdk_gc_set_clip_mask(gc,
                                ihm_setup->cardmask);

                        gdk_gc_set_clip_origin(gc,
                                card->dim.x,
                                card->dim.y);

                        gdk_draw_drawable( ihm_setup->target,
                                                                                  gc,
                                card->img, 0, 0,
                                card->dim.x, card->dim.y, card->dim.w, card->dim.h);


                        gdk_gc_set_clip_mask(gc, NULL);
                }
                else
                        gdk_draw_drawable(ihm_setup->target,
                                gc,
                                ihm_setup->copy,
                                card->dim.x, card->dim.y, card->dim.x, card->dim.y,
                                card->dim.w, card->dim.h);


                gdk_draw_drawable( ihm_setup->Drawing_area->window,
                        gc,     ihm_setup->target,
                                card->dim.x, card->dim.y, card->dim.x, card->dim.y,
                                card->dim.w, card->dim.h);

        }
        if(ihm_setup->debug)
        printf("Event blinck\n");
        return(TRUE);
}


void clear_blink(ihm_trick_t *ihm_setup)
{
        if(ihm_setup->debug)
          printf("On est dans blink_clear\n");
        GList *lst;
        imgcard_t *card;

        if(ihm_setup->blinkid != -1 )
        {
                g_source_remove(ihm_setup->blinkid);
                ihm_setup->blinkid = -1;
        }

        for(lst = ihm_setup->waiting; lst; lst = lst->next)
        {
                card = lst->data;
                card->blink = FALSE;
                card->draw = TRUE;
        }

        if(ihm_setup->copy)
                g_object_unref(ihm_setup->copy);

        ihm_setup->copy = NULL;
}


gboolean mise_en_place ( GtkWidget *Drawing_area,ihm_trick_t *ihm_setup) {
  Drawing_area=Drawing_area;
  int contrat;
  couleur_t couleur;
  if(ihm_setup->debug)
    printf("mise_en_place\n");
  alloc_ihm(ihm_setup);
  for(couleur=club;couleur<aucune+1;couleur++) {
    for(contrat=0;contrat<7;contrat++) {
      gtk_image_set_from_pixbuf( GTK_IMAGE(ihm_setup->Allbid[couleur*7+contrat]->widget),ihm_setup->Allbid[couleur*7+contrat]->wimage);
      

    }


  }
  for(contrat=0;contrat<3;contrat++) {
      gtk_image_set_from_pixbuf( GTK_IMAGE(ihm_setup->Othbid[contrat]->widget),ihm_setup->Othbid[contrat]->wimage);
      

  }
  init_game(ihm_setup);
  position_targetzone(ihm_setup);
  
  

  return TRUE;
}
void action_click(ihm_trick_t *ihm_setup)
{

        draw_container_ihm(ihm_setup);
}


gboolean button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  widget=widget;
  ihm_trick_t *ihm_setup = (ihm_trick_t *)data;
  position_t position;
  if(ihm_setup->status=='b'  ) 
    return FALSE;
  if(NULL==ihm_setup->pli  ) 
    return FALSE;
  // C'est la fin on a deja 13 plis 
  if((ihm_setup->pli->nbpli_ligne[1]+ihm_setup->pli->nbpli_ligne[0] ) ==ihm_setup->nbcard ) {
    //if(ihm_setup->read==FALSE  ) 
     // return TRUE;
      printf("The end \n");
    trash_list(ihm_setup);
    gtk_label_set_justify(GTK_LABEL(ihm_setup->Label), GTK_JUSTIFY_CENTER);
    gtk_label_set_text (GTK_LABEL (ihm_setup->Label),g_strdup_printf("Final Contrat:%s\nEnd of Game",ihm_setup->scontrat));
    for (position=sud;position<est+1;position++) {
      if(ihm_setup->debug)
        fprintf(stderr,"recuperation_jeu position=%d, button_press_event %c %d\n", position,ihm_setup->status, ihm_setup->read);
      recuperation_jeu (ihm_setup,  position);
    }
    draw_container_ihm(ihm_setup); 
    ihm_setup->state=BID; 
    ihm_setup->status='b'; 
  }
  else {
        if(ihm_setup->pli->noj==0)
          trash_list(ihm_setup);
        gboolean found = FALSE;

        if(event->button == 1)
        {
                found = find_moveable_card(ihm_setup->waiting, event, ihm_setup);

                if(found == FALSE) {

                  if((ihm_setup->pli->nextpos)%2 !=(position_t) ihm_setup->ligneia)
                        found = find_moveable_card(ihm_setup->players[ihm_setup->pli->nextpos], event, ihm_setup);
                }

                if(found == TRUE)
                        clear_blink(ihm_setup);
        }
  }
  if(ihm_setup->debug)
    printf("End button_press_event\n");
  return(TRUE);
}

gboolean expose_comment( GtkWidget *Fenetre, GdkEventExpose *event, ihm_trick_t *ihm_setup)
{
  event=event;
  Fenetre=Fenetre;
  int status;
  position_t position;
  if(ihm_setup->debug)
    printf("expose comment %d\n", ihm_setup->read);
      if(ihm_setup->status=='b' && ihm_setup->read==TRUE) {
        ihm_setup->read=FALSE;
        int number,contrat,i;
        couleur_t color,couleur;
        char *coulref="TKCPS";
        char *dis_bid=NULL;
        status = read_header(ihm_setup,ihm_setup->cur_bid,'u');
        printf("expose comment:read cur_bid%s\n",ihm_setup->cur_bid);
        
        if(status <=0) {
          printf("status:%d\n",status); 
          exit(1);
        }
        printf("%s\n",ihm_setup->cur_bid);
        dis_bid=display_str(ihm_setup->cur_bid);
        
        gtk_label_set_justify(GTK_LABEL(ihm_setup->Label), GTK_JUSTIFY_LEFT);
        gtk_label_set_text (GTK_LABEL (ihm_setup->Label),g_strdup_printf("Bids:\nS \tW \tN \tE \n%s\n",dis_bid));
        if((ihm_setup->cur_bid[strlen(ihm_setup->cur_bid)-1])=='P' &&(ihm_setup->cur_bid[strlen(ihm_setup->cur_bid)-2])=='P' && (ihm_setup->cur_bid[strlen(ihm_setup->cur_bid)-3])=='P'&& (ihm_setup->cur_bid[strlen(ihm_setup->cur_bid)-4])=='P') {
          printf("Game Starting\n");
                   
          for(i=strlen(ihm_setup->cur_bid)-1;i>0;i=i-2) {

            if(ihm_setup->cur_bid[i-1]!='P') {
               ihm_setup->scontrat=malloc( 3*sizeof(char));
              ihm_setup->scontrat[0]=ihm_setup->cur_bid[i-1];
              ihm_setup->scontrat[1]=ihm_setup->cur_bid[i];
              ihm_setup->scontrat[2]='\0';
              break;

            }
          }
          if(ihm_setup->scontrat==NULL) {
            printf("No Bids, redistribute\n");
            return FALSE;

          }
 
        gtk_label_set_justify(GTK_LABEL(ihm_setup->Label), GTK_JUSTIFY_LEFT);
        gtk_label_set_text (GTK_LABEL (ihm_setup->Label),g_strdup_printf("Bids:\nS \tW \tN \tE \n%s\nFinal Contrat: %s\n",dis_bid,ihm_setup->scontrat));
        ihm_setup->state=STARTING; 
        ihm_setup->status='g'; 
          

          for(contrat=0;contrat<7;contrat++){
            for(couleur=club;couleur<aucune+1;couleur++) 
                gtk_widget_set_sensitive(ihm_setup->Allbid[couleur*7+contrat]->bwidget, FALSE);
          }
          for (position=sud;position<est+1;position++ ) {
            free_list(&ihm_setup->players[position]);
            ihm_setup->players[position]=NULL;
          }
          recuperation_jeu(ihm_setup,(ihm_setup->contrat->declarant)%4);
          recuperation_jeu(ihm_setup,(ihm_setup->contrat->declarant+2)%4);
          draw_container_ihm(ihm_setup); 
          ihm_setup->pli->nextpos=(ihm_setup->contrat->declarant+1)%4;
 /* Blit */
           
        }
        else {
          number=ihm_setup->cur_bid[strlen(ihm_setup->cur_bid)-4]-48;
          for (i=0;i<(int)strlen(coulref);i++) {
            if(coulref[i]==ihm_setup->cur_bid[strlen(ihm_setup->cur_bid)-3])
              color=(couleur_t) i;
      
          }
        
          for(couleur=club;couleur<aucune+1;couleur++) 
            for(contrat=0;contrat<number-1;contrat++){
              if(ihm_setup->debug)
                printf("couleur=%d,number=%d,contrat=%d\n",couleur,number,contrat);
              gtk_widget_set_sensitive(ihm_setup->Allbid[couleur*7+contrat]->bwidget, FALSE);
          }
          for(couleur=club;couleur<color+1;couleur++) {
            gtk_widget_set_sensitive(ihm_setup->Allbid[couleur*7+contrat]->bwidget, FALSE);

          }
        }
        free(dis_bid);
      }
  return FALSE;

}
gboolean rafraichissement( GtkWidget *Drawing_area, GdkEventExpose *event, ihm_trick_t *ihm_setup)
{
      event=event;
      Drawing_area=Drawing_area;
     // int status; 

      draw_container_ihm(ihm_setup); 
      if(ihm_setup->debug)
        printf("expose Drawing_area,\n");
     // if(ihm_setup->state==BID && ihm_setup->read==TRUE) {
     //   printf("expose Drawing_area enchere,\n");
     //   bid_t *bid=malloc(sizeof(bid_t));

     //   if(ihm_setup->status=='b'  ) {
     //     status = read_header(ihm_setup,ihm_setup->cur_bid,'b');
          
          
     //     ihm_setup->read=FALSE;
      //  } 
      //  free(bid);

      //}
        return FALSE;
}

gboolean quit(GtkWidget *window, ihm_trick_t *ihm_setup)
{
        window=window;
        ihm_setup->status='e';
        //if(ihm_setup->debug) 
          fprintf(stdout,"End of Session\n");
        write_data(ihm_setup,NULL,'e');
        shutdown(ihm_setup->socketid,SHUT_RDWR);
        close(ihm_setup->socketid);
        gtk_main_quit();

        return TRUE;
}

gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
        widget=widget;
        ihm_trick_t *ihm_setup = (ihm_trick_t*)data;
        if(ihm_setup->debug)
          printf("On est dans motion_notify_event\n");

        rectangle_t *zone = NULL;

        GdkPixmap *source;

        int x, y, w, h, toph, leftw,
            ax, ay, aw, ah,
            bx, by, bw, bh;

        if(ihm_setup->movecard)
        {
                if(!ihm_setup->copy)
                {
                        ihm_setup->movecard->card->draw = FALSE;

                        draw_copy(ihm_setup);

                        ihm_setup->movecard->card->draw = TRUE;
                }

                if(ihm_setup->copy)
                {
                        x = ihm_setup->movecard->dim.x;
                        y = ihm_setup->movecard->dim.y;

                        w = ihm_setup->movecard->card->dim.w;
                        h = ihm_setup->movecard->card->dim.h;

                        gdk_draw_drawable(ihm_setup->target,
                                ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                ihm_setup->copy,
                                x-1, y-1, x-1, y-1, w+2, h+2);

                        if(ihm_setup->dropping)
                        {
                                zone = draw_dropping_zone(ihm_setup,
                                        ihm_setup->target);

                        }

                        ihm_setup->movecard->dim.x =
                                (gint)event->x -ihm_setup->movecard->dim.w;

                        ihm_setup->movecard->dim.y =
                                (gint)event->y -ihm_setup->movecard->dim.h;
                        if(zone)
                        {
                                if(zone->active == FALSE)
                                        source = ihm_setup->copy;
                                else
                                        source = ihm_setup->target;


                                gdk_draw_drawable(ihm_setup->Drawing_area->window,
                                ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                source,
                                zone->dim.x, zone->dim.y,
                                zone->dim.x, zone->dim.y,
                                zone->dim.w, 1);


                                gdk_draw_drawable(ihm_setup->Drawing_area->window,
                                ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                source,
                                zone->dim.x+zone->dim.w,
                                zone->dim.y,
                                zone->dim.x+zone->dim.w,
                                zone->dim.y,
                                1, zone->dim.h);

                                gdk_draw_drawable(ihm_setup->Drawing_area->window,
                                ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                source,
                                zone->dim.x,
                                zone->dim.y+zone->dim.h,
                                zone->dim.x,
                                zone->dim.y+zone->dim.h,
                                zone->dim.w+1, 1);

                                gdk_draw_drawable(ihm_setup->Drawing_area->window,
                                ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                source,
                                zone->dim.x,
                                zone->dim.y,
                                zone->dim.x,
                                zone->dim.y,
                                1, zone->dim.h);

                        }

                        draw_moving_card(ihm_setup, ihm_setup->target);

                        gdk_draw_drawable(ihm_setup->Drawing_area->window,
                                ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                ihm_setup->target,
                                ihm_setup->movecard->dim.x,
                                ihm_setup->movecard->dim.y,
                                ihm_setup->movecard->dim.x,
                                                                             ihm_setup->movecard->dim.y,
                                w, h);


                        /*----Refreshing area optimization----
                         * try this on an old XXX Mhz, and 
                         * see how fast it is when you drag 
                         * a card around. */
                        if(x <= ihm_setup->movecard->dim.x)
                        {
                                leftw = ihm_setup->movecard->dim.x -x;
                                bx = x-1;

                                bw = leftw+2;
                        }
                        else
                        {
                                leftw = w-x+ihm_setup->movecard->dim.x;
                                bx = x+leftw-1;

                                bw = w-leftw+2;
                        }

                        if(y <= ihm_setup->movecard->dim.y)
                        {
                                ax = x-1;
                                ay = y-1;
                                toph = ihm_setup->movecard->dim.y -y;
                                ah = toph+1;
                                aw = w+2;

                                by = y+toph;
                                bh = h - toph +1;
                        }
                        else
                        {
                                toph = h-(y-ihm_setup->movecard->dim.y);
                                ax = x -1;
                                ay = y+toph;
                                              ah = h - toph +1;
                                aw = w +2;

                                by = y-1;
                                bh = toph+1;
                        }


                        gdk_draw_drawable(
                                ihm_setup->Drawing_area->window,
                                ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                ihm_setup->target,
                                ax, ay, ax, ay,
                                aw, ah);

                        gdk_draw_drawable(
                                ihm_setup->Drawing_area->window,
                                ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                ihm_setup->target,
                                bx, by, bx, by,
                                bw, bh);

                }

        }

        return TRUE;
}


gboolean  button_release_event (GtkWidget *widget,
        GdkEventButton *event, gpointer data)
{
        widget=widget;
        ihm_trick_t *ihm_setup = (ihm_trick_t *)data;
        int ligne;
        if(NULL==ihm_setup->pli)
          return FALSE;
        imgcard_t *drop = NULL;
        if(event->button == 1)
        {

          if( ihm_setup->pli->nextpos%2!=(position_t) ihm_setup->ligneia && ihm_setup->movecard) {
                        if(ihm_setup->copy)
                                g_object_unref(ihm_setup->copy);

                        ihm_setup->copy = NULL;

                        ihm_setup->movecard->card->draw = TRUE;
                        if(ihm_setup->movecard->card && game_drop_enable(ihm_setup))
                          drop = ihm_setup->movecard->card;
                        else {
                          g_free(ihm_setup->movecard);
                          ihm_setup->movecard=NULL; 
                        }


                        if(drop != NULL) {
                           drop->draw_face= TRUE;
                           drop->moveable= FALSE;
                           g_free(ihm_setup->movecard);
                           ihm_setup->movecard = NULL;
                          ligne=game_turn(ihm_setup, drop);
                        }

                        draw_container_ihm(ihm_setup);

                }
                if(ihm_setup->pli->nextpos%2==(position_t)ihm_setup->ligneia) {
                  do {
                
                    ligne=game_turn(ihm_setup, NULL);
                    printf("noj=%d\n",ihm_setup->pli->noj);
                  }
                  while(ligne==ihm_setup->ligneia && (ihm_setup->pli->noj != 0));
                 }


          }
          return TRUE;
}

gboolean game_drop_enable(ihm_trick_t *ihm_setup)
{
        rectangle_t *zone = get_active_targetzone(ihm_setup->dropping);

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
