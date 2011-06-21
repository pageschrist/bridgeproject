#include <glib.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "arbitre.h"
#include "objets.h"
#include "ihmbridge.h"
#include "callbacks.h"
#include "client.h"
#include "jeu.h"

#define MAXLARGDEF 18
#define MAXHAUTDEF 18

extern GHashTable *configHash;

GtkWidget *Menu_VBox;
GtkWidget *Comment_Box;
GtkWidget *Comment_VBox;
GtkWidget *entry_level;
GtkWidget *entry_random;
GtkWidget *Board_HBox;
GtkWidget *VBox;
GtkWidget *HBox;
GtkWidget *BNew_Dist;
GtkWidget *HBox_Other;
GtkWidget **HBox_Encheres;
GtkWidget *HBox_BidO;
GtkWidget *Menu_Box;

GtkWidget *Menu;
GtkWidget *Root_menu;
GtkWidget *Menu_items;
GtkWidget *Help_menu;
GtkWidget *Label_text;
GtkWidget *Score;
GtkWidget *Bouton_board;
GtkWidget *Image;
GtkWidget *Menu_bar;
GtkWidget *Configure_menu;
GtkWidget *Configure_item;
GtkWidget *About_menu;
GtkWidget *New_game_item;
GtkWidget *Quit_item;
GtkWidget *About_item;

GtkWidget **tab_imgs;
guint id_sig_connect;
// Caracteristique du plateau
char *dir_imgs = NULL;
 
ligne_t ligneia;
int tab_couleur[est+1][pique+1];
int ref_couleur[pique+1];





void 
ihm_about(GtkWidget *Wdgt, guint value)
{
  Wdgt=Wdgt;
  value=value;
  GtkWidget *Dialogue;
  GtkWidget *Bouton;
  GtkWidget *Lab;
  GtkWidget *Box;

  Dialogue = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_policy(GTK_WINDOW(Dialogue), FALSE, FALSE, TRUE);
  gtk_window_set_title(GTK_WINDOW(Dialogue), "About ...");
  gtk_window_set_position(GTK_WINDOW(Dialogue), GTK_WIN_POS_CENTER);

  gtk_container_set_border_width(GTK_CONTAINER(Dialogue), 8);
  //gtk_window_set_transient_for(GTK_WINDOW(Dialogue), GTK_WINDOW(ihm_pli->Fenetre));

  Box = gtk_vbox_new(FALSE, 6);
  gtk_container_add(GTK_CONTAINER(Dialogue), Box);

  Lab = gtk_label_new("Author and maintainer :\n\n"
                      "Christophe PAGES      \n"
                      "christophe.brigitte@free.fr\n");
  gtk_label_set_justify(GTK_LABEL(Lab), GTK_JUSTIFY_RIGHT);
  gtk_box_pack_start(GTK_BOX(Box), Lab, TRUE, TRUE, 0);

  Bouton  = gtk_button_new_with_label("Close");
  gtk_box_pack_start(GTK_BOX(Box), Bouton, TRUE, TRUE, 0);
  gtk_signal_connect_object(GTK_OBJECT(Bouton), "clicked",
                            (GtkSignalFunc)gtk_widget_destroy,
                            GTK_OBJECT(Dialogue));

  gtk_widget_show_all(Dialogue);
}




void create_ihm (ihm_pli_t *ihm_pli) {

  char chaine[MAXCHAR];
  couleur_t couleur;
  int res,contrat;
  button_bid_t **button_bid;
  button_bid_t **button_obid;
  init_ihm_pli(ihm_pli);
  ihm_pli->Fenetre = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (ihm_pli->Fenetre), "gbridge_cli");
  gtk_window_set_position (GTK_WINDOW (ihm_pli->Fenetre), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (ihm_pli->Fenetre), FALSE);
  HBox_Encheres=g_malloc(sizeof(GtkWidget *)*5);
  button_obid=g_malloc(sizeof(button_bid_t *)*3);
  button_bid=g_malloc(sizeof(button_bid_t *)*NBENCHERES);

  Menu_VBox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (Menu_VBox), 1);
  gtk_container_add (GTK_CONTAINER (ihm_pli->Fenetre), Menu_VBox);

  Menu=gtk_menu_new();
  Root_menu = gtk_menu_item_new_with_label("Configuration");

  Menu_items = gtk_menu_item_new_with_label("New Dist");
  gtk_menu_append(GTK_MENU (Menu), Menu_items);
  g_signal_connect (G_OBJECT (Menu_items), "activate",
                              G_CALLBACK (new_dist),
                               ihm_pli);
  Menu_items = gtk_menu_item_new_with_label("Debug");
  gtk_menu_append(GTK_MENU (Menu), Menu_items);
  g_signal_connect (G_OBJECT (Menu_items), "activate",
                              G_CALLBACK (ihm_debug),
                               ihm_pli);
  Menu_items = gtk_menu_item_new_with_label("Random");
  gtk_menu_append(GTK_MENU (Menu), Menu_items);
  Menu_items = gtk_menu_item_new_with_label("Quit");
  gtk_menu_append(GTK_MENU (Menu), Menu_items);
  g_signal_connect_swapped (G_OBJECT (Menu_items), "activate",
                              G_CALLBACK (gtk_main_quit),
                              NULL);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (Root_menu), Menu);

  
  Menu=gtk_menu_new();
  Help_menu= gtk_menu_item_new_with_label("Help");
  Menu_items = gtk_menu_item_new_with_label("About");
  gtk_menu_append(GTK_MENU (Menu), Menu_items);
  g_signal_connect_swapped (G_OBJECT (Menu_items), "activate",
                              G_CALLBACK (ihm_about),
                              NULL);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (Help_menu), Menu);
  Menu_bar=gtk_menu_bar_new();

  gtk_box_pack_start(GTK_BOX(Menu_VBox), Menu_bar, FALSE, FALSE, 2);

  gtk_menu_bar_append(GTK_MENU_BAR (Menu_bar), Root_menu);
  gtk_menu_bar_append(GTK_MENU_BAR (Menu_bar), Help_menu);

  Board_HBox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (Menu_VBox), Board_HBox, FALSE, FALSE, 0);
  Bouton_board= gtk_button_new ();
  gtk_button_set_relief(GTK_BUTTON(Bouton_board), GTK_RELIEF_NONE);
  gtk_box_pack_start(GTK_BOX(Board_HBox), Bouton_board, FALSE, FALSE, 0);
  ihm_pli->Drawing_area=gtk_drawing_area_new();
  //gtk_box_pack_start (GTK_BOX (Menu_VBox),  ihm_pli->Drawing_area, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX (Board_HBox),  ihm_pli->Drawing_area, TRUE, TRUE, 2);
  gtk_widget_set_size_request( ihm_pli->Drawing_area, LARGEUR, HAUTEUR);


  Comment_VBox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start(GTK_BOX(Board_HBox), Comment_VBox, TRUE, FALSE, 0);
  ihm_pli->Label = gtk_label_new("");
  gtk_label_set_justify(GTK_LABEL(ihm_pli->Label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_end(GTK_BOX(Comment_VBox), ihm_pli->Label, FALSE, FALSE, 0);

  ihm_pli->Score = gtk_label_new("");
  gtk_label_set_justify(GTK_LABEL(ihm_pli->Score), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start(GTK_BOX(Comment_VBox), ihm_pli->Score, FALSE, FALSE, 0);
  
  entry_level = gtk_entry_new ();
  g_signal_connect (G_OBJECT (entry_level), "activate",
                      G_CALLBACK (enter_callback_level),
                      (gpointer) ihm_pli);
  gtk_entry_set_max_length (GTK_ENTRY (entry_level), 5);
  res=snprintf(chaine,MAXCHAR,"%d",DEFAULTLEVEL);
  gtk_entry_set_text (GTK_ENTRY (entry_level), chaine);
  gtk_entry_set_max_length (GTK_ENTRY (entry_level), 2);
  Label_text = gtk_label_new("level (1-10):");
  gtk_label_set_justify(GTK_LABEL(Label_text), GTK_JUSTIFY_CENTER);
  HBox_Other = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start(GTK_BOX(HBox_Other), Label_text, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (HBox_Other), entry_level, FALSE, FALSE, 0);
  entry_random = gtk_entry_new ();
  Label_text = gtk_label_new("Rnd (0-25000):");
  gtk_label_set_justify(GTK_LABEL(Label_text), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start(GTK_BOX(HBox_Other), Label_text, FALSE, FALSE, 0);
  res=snprintf(chaine,MAXCHAR,"%d",DEFAULTRANDOM);
  gtk_entry_set_text (GTK_ENTRY (entry_random), chaine);
  gtk_entry_set_max_length (GTK_ENTRY (entry_random), 5);
  gtk_box_pack_start (GTK_BOX (HBox_Other), entry_random, FALSE, FALSE, 0);
  Label_text = gtk_label_new("New Dist");
  BNew_Dist=gtk_button_new();
  gtk_box_pack_start (GTK_BOX (HBox_Other), BNew_Dist, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER(BNew_Dist),Label_text);
  g_signal_connect( G_OBJECT(BNew_Dist), "clicked", G_CALLBACK(new_dist), ihm_pli);
  gtk_box_pack_start (GTK_BOX (Comment_VBox), HBox_Other, FALSE, FALSE, 0);
  for(couleur=trefle;couleur<aucune+1;couleur++) {
    HBox_Encheres[couleur] = gtk_hbox_new (TRUE, 1);
    gtk_box_pack_start(GTK_BOX(Comment_VBox),HBox_Encheres[couleur],FALSE, TRUE, 2);
    for(contrat=0;contrat<7;contrat++){
      button_bid[couleur*7+contrat]=g_malloc(sizeof(button_bid_t));
      button_bid[couleur*7+contrat]->ihm_pli=ihm_pli;
      button_bid[couleur*7+contrat]->ihm_bid=ihm_pli->Allbid[couleur*7+contrat];
      gtk_box_pack_start(GTK_BOX(HBox_Encheres[couleur]),ihm_pli->Allbid[couleur*7+contrat]->bwidget,FALSE,TRUE,2);
      gtk_container_add( GTK_CONTAINER(ihm_pli->Allbid[couleur*7+contrat]->bwidget), ihm_pli->Allbid[couleur*7+contrat]->widget);
      g_signal_connect( G_OBJECT(ihm_pli->Allbid[couleur*7+contrat]->bwidget), "clicked", G_CALLBACK(click_bid), button_bid[couleur*7+contrat]);
    }
 } 
 HBox_BidO=gtk_hbox_new(TRUE,1);
 gtk_box_pack_start(GTK_BOX(Comment_VBox),HBox_BidO,FALSE, TRUE, 2);
 for (contrat=0;contrat<3;contrat++) {
   button_obid[contrat]=g_malloc(sizeof(button_bid_t));
   button_obid[contrat]->ihm_pli=ihm_pli;
   button_obid[contrat]->ihm_bid=ihm_pli->Othbid[contrat];
   gtk_box_pack_start(GTK_BOX(HBox_BidO),ihm_pli->Othbid[contrat]->bwidget,FALSE,TRUE,2);
   gtk_container_add( GTK_CONTAINER(ihm_pli->Othbid[contrat]->bwidget), ihm_pli->Othbid[contrat]->widget);
   g_signal_connect( G_OBJECT(ihm_pli->Othbid[contrat]->bwidget), "clicked", G_CALLBACK(click_bid), button_obid[contrat]);

 }
  id_sig_connect = g_signal_connect (G_OBJECT(ihm_pli->Drawing_area),"realize",G_CALLBACK(mise_en_place),ihm_pli);
  id_sig_connect = g_signal_connect (G_OBJECT(ihm_pli->Drawing_area),"expose_event",G_CALLBACK(rafraichissement),ihm_pli);
  id_sig_connect = g_signal_connect (G_OBJECT(Comment_VBox),"expose_event",G_CALLBACK(expose_comment),ihm_pli);
  gtk_widget_add_events(ihm_pli->Drawing_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_MOTION_MASK );
  id_sig_connect = g_signal_connect (G_OBJECT(ihm_pli->Drawing_area),"button_press_event",G_CALLBACK(button_press_event),ihm_pli);
  id_sig_connect = g_signal_connect (G_OBJECT(ihm_pli->Drawing_area),"button_release_event",G_CALLBACK(button_release_event),ihm_pli);
  id_sig_connect = g_signal_connect (G_OBJECT(ihm_pli->Drawing_area),"motion_notify_event",G_CALLBACK(motion_notify_event),ihm_pli);
  id_sig_connect = g_signal_connect (G_OBJECT(ihm_pli->Drawing_area),"key_press_event",G_CALLBACK(key_down),ihm_pli);
  id_sig_connect = g_signal_connect (G_OBJECT (entry_random), "activate",
                      G_CALLBACK (enter_callback_random),
                      ihm_pli);
  //gtk_widget_show (entry_random);
  // Ajout pour boite latérale

}

void ihm_debug(GtkWidget *E,ihm_pli_t  *ihm_pli) {
          E=E;
          printf("****random=%d***************************************\n",ihm_pli->transfert->random);
          printf("*******************************************\n");


}



