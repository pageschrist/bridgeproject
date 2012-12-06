#define MAXFILE 1024
#define DEFAULTDIRNAME  "/home/cpages/dev/bridgeproject-0.9/donne.t04"
void new_dist (GtkButton *button,ihm_trick_t *ihm_setup) ;
void file_ok_sel( GtkWidget        *w, ihm_trick_t *ihm_setup );
void open_file (GtkButton *button,ihm_trick_t *ihm_setup); 
void save_game (GtkButton *button,ihm_trick_t *ihm_setup);
void enter_callback_level( GtkWidget *entry,ihm_trick_t *ihm_setup);
void enter_callback_random( GtkWidget *entry,ihm_trick_t *ihm_setup);
void click_bid (GtkButton *button,button_bid_t *button_bid);
gboolean event_blink(gpointer data);
gboolean key_down(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean mise_en_place (GtkWidget *Drawing_area,ihm_trick_t *ihm_setup);
gboolean button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer data);
gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean rafraichissement( GtkWidget *Drawing_area, GdkEventExpose *event,ihm_trick_t *ihm_setup);
gboolean expose_comment( GtkWidget *Fenetre, GdkEventExpose *event,ihm_trick_t *ihm_setup);
gboolean quit(GtkWidget *window, ihm_trick_t *ihm_setup);
gboolean game_drop_enable(ihm_trick_t *ihm_setup);
rectangle_t * get_active_targetzone(GList *lst);
void fdebug(void);
