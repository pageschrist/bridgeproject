#define NB_ENTREES_MENU 6
#define MAXCHAR 10 
#define LARGEUR 600
#define HAUTEUR 500

gint ihm_set_donneur (void);
void create_ihm (ihm_trick_t *ihm_pli);
void ihm_create_bridgeboard(GtkWidget *E,ihm_trick_t  *ihm_pli);
gint ihm_color_choice (void);
gint delete_bridgeboard(void);
void ihm_debug(GtkWidget *E,ihm_trick_t  *ihm_pli);
void ihm_undebug(GtkWidget *E,ihm_trick_t  *ihm_pli);
