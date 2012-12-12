#include <gtk/gtk.h>
#define INIT 0
#define RESET 1
#define INDEX ((position*(spade+1))+couleur)*sizeof(GList)
#define STARTING 1
#define GAMEOVER 0
#define PLI 2
#define CONTRAT 3
#define CARDS 4
#define BID 5
#define HAND 6
#define PLAY 7
#define NEWDIST 8
#define NBENCHERES 35
#define AVAILABLE 1
#define NOTAVAILABLE 0
#define BIDSIZE 100
#define SUD 0
#define OUEST 1
#define NORD 2
#define EST 3
#define NBCARDS 52
#define NBJOUEURS 4
#define NBCOULEURS 4
#define COULEURS 4
#define NBPCOULEURS 13
#define TREFLE 0
#define CARREAU 1
#define COEUR 2
#define PIQUE 3
#define SANSATOUT 4
#define ENTAME 4
#define OUVREUR 0
#define INTERVENANT 1
#define REPONDANT 2
#define REPINTERVENANT 3
#define CARD 100
#define COULEUR 101
#define POSITION 102
#define PASDECARD -1
#define PASDECOULEUR -1
#define DEFAULTLEVEL 5
#define DEFAULTRANDOM 40
#define MAXFILENAME 1024





enum eposition {EP_DIAGONAL, EP_HORIZONTAL, EP_VERTICAL};

enum egstate {LOADING, DISTRIBUTING, PLAYING, WAITING,
        READY_PILE, EGS_SECOND, READY_DISTRIB1, READY_DISTRIB2, ENDGAME , BEGINING};

typedef enum
{ c2, c3, c4, c5, c6, c7, c8, c9, cX, cV, cD, cR, cA, pdc } valeur_t;

typedef enum
{ club, diamond, heart, spade, aucune } color_t;

typedef enum
{ sud, ouest, nord, est, aucun } position_t;

typedef enum
{ ns, eo, pdl } ligne_t;

typedef enum
{ new, partie, end } status_t;


typedef struct _coord_t
{
  int noc;
  int nol;
  position_t detenteur;
} coord_t;

typedef struct _card_t
{
  valeur_t nocard;
  color_t clcard;
} card_t;

typedef struct _main_t
{
  int nbpoints;
  int nbcards[spade+1];
  int nbpointshonneurs[spade+1];
  int nbcardsmax;
} main_t;

typedef struct _trick_t
{
  card_t card[est + 1];
  card_t discard[est + 1];
  position_t entame;
  position_t nextpos;
  card_t phcard;
  card_t lastcard;
  int notrick;
  int nbtricks_line[eo + 1];
  int noj; //Numero du joueur qui joue
  color_t atout;
  position_t leader;
  gboolean eval;
  color_t coloreval;

} trick_t;

typedef struct _transfert_t
{
 int status;
 int random;
 int level;
} transfert_t;

typedef struct _dim_t 
{
  gint x;
  gint y;
  gint h;
  gint w;

} dim_t;

typedef struct _imgcard_t
{
  GdkPixmap *img;
  dim_t dim;
  color_t couleur;
  valeur_t valeur;
  position_t position;
  gboolean draw;
  gboolean draw_face;
  gboolean moveable;
  gboolean blink;
  gboolean status;
} imgcard_t;

typedef struct _movingcard_t
{
        imgcard_t *card;
        dim_t dim;
} movingcard_t;

typedef struct _contrat_t
{
  color_t atout;
  valeur_t nbtricks;			/* decrit le nombre de tricks a realiser */
  position_t declarant;		/* donne le nom du declarant */
  int nom;
  int maxc[4];
} contrat_t;


typedef struct _rectangle_t
{
        dim_t dim;
        gboolean active;
} rectangle_t;

typedef struct _bid_t
{
  int nombre;
  color_t couleur;
  position_t position;
  gboolean passe;
  gboolean contre;
  gboolean surcontre;
} bid_t;

typedef struct _ihm_bid_t
{
  GdkPixbuf *wimage;
  GtkWidget *widget;
  GtkWidget *bwidget;
  bid_t *bid;

} ihm_bid_t;


typedef struct _ihm_trick_t
{
  trick_t *trick;
  int socketid;
  int level;
  int random;
  int ligneia;
  int nbcard;
  char status ; 
  gboolean debug;
  int tab_couleur[est+1][spade+1];
  contrat_t *contrat;
  char *scontrat;
  GList *allcardslist;
  GList *pile;
  GList *waiting;
  GList *dropping;
  GList *trash;
  movingcard_t *movecard;
  GList **players;
  GdkPixmap *target; 
  GdkPixmap *back;
  GdkPixmap *copy;
  GdkBitmap *backmask;
  GdkBitmap *cardmask;
  GtkWidget *Drawing_area;
  GtkWidget *Label;
  GtkWidget *Score;
  GtkWidget *Fenetre;
  GtkWidget *File_S;
  char filename[MAXFILENAME];
  char **tabjeufile;
  gboolean savegame;
  FILE *fd;
  ihm_bid_t **Allbid;
  ihm_bid_t **Othbid;
  char cur_bid[BIDSIZE];
  char *path;
  FILE *output;
  int blinkid;
  int state ; 
  transfert_t *transfert;
  gboolean read;
} ihm_trick_t;

typedef struct _tablist
{
  valeur_t tabcoul[13];
  gboolean debug;               //position de la derniere card jouee
  int nbcrt;                    //nombre de card dans la couleur
  color_t couleureval;
} tablist_t;

typedef struct _button_bid_t
{
  ihm_trick_t *ihm_setup;
  ihm_bid_t *ihm_bid;
} button_bid_t;

void bouge_card(position_t position,card_t *card,int maxl);
void bouge_card_jeu(position_t poscour,card_t  *card, int nl, int nc, int maxl) ;
void display_trick(trick_t *trick);
char *affichage (int valeur,int type);

char * contrat_to_char ( color_t atout); 
char *affichage (int valeur, int type);
void affiche_card (card_t *card);
void display_trick( trick_t *ptrick);
void init_trick (trick_t * trick, int maniere);
void free_ihm_setup( ihm_trick_t *ihm_setup) ;
void reset_ihm_setup (ihm_trick_t *ihm_setup);
void init_ihm_setup (ihm_trick_t *ihm_setup);
void init_bid (ihm_trick_t *ihm_setup);
char *display_str(char *str ); 
