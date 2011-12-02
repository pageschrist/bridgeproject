#define MAXFILE 1024
void new_dist (GtkButton *button,ihm_pli_t *ihm_pli) ;
void file_ok_sel( GtkWidget        *w, ihm_pli_t *ihm_pli );
void open_file (GtkButton *button,ihm_pli_t *ihm_pli); 
void enter_callback_level( GtkWidget *entry,ihm_pli_t *ihm_pli);
void enter_callback_random( GtkWidget *entry,ihm_pli_t *ihm_pli);
void click_bid (GtkButton *button,button_bid_t *button_bid);
gboolean event_blink(gpointer data);
gboolean key_down(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean mise_en_place (GtkWidget *Drawing_area,ihm_pli_t *ihm_pli);
gboolean button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data);
void button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer data);
gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean rafraichissement( GtkWidget *Drawing_area, GdkEventExpose *event,ihm_pli_t *ihm_pli);
gboolean expose_comment( GtkWidget *Fenetre, GdkEventExpose *event,ihm_pli_t *ihm_pli);
gboolean quit(GtkWidget *window, ihm_pli_t *ihm_pli);
gboolean game_drop_enable(ihm_pli_t *ihm_pli);
rectangle_t * get_active_targetzone(GList *lst);
void fdebug(void);
