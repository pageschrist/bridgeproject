#define NB_ENTREES_MENU 6
#define DEFAULTRANDOM 40
#define DEFAULTLEVEL 5
#define MAXCHAR 10 
#define LARGEUR 600
#define HAUTEUR 500

gint ihm_set_donneur (void);
void create_ihm (gboolean debug,gboolean animation,ihm_pli_t *ihm_pli);
void ihm_create_bridgeboard(GtkWidget *E,ihm_pli_t  *ihm_pli);
gint ihm_color_choice (void);
gint delete_bridgeboard(void);
