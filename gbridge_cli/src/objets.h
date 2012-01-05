#include <gtk/gtk.h>
#define INIT 0
#define RESET 1
#define INDEX ((position*(pique+1))+couleur)*sizeof(GList)
#define STARTING 1
#define GAMEOVER 0
#define PLI 2
#define CONTRAT 3
#define CARTES 4
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
#define NBCARTES 52
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
#define CARTE 100
#define COULEUR 101
#define POSITION 102
#define PASDECARTE -1
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
{ trefle, carreau, coeur, pique, aucune } couleur_t;

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

typedef struct _carte_t
{
  valeur_t nocarte;
  couleur_t clcarte;
} carte_t;

typedef struct _main_t
{
  int nbpoints;
  int nbcartes[pique+1];
  int nbpointshonneurs[pique+1];
  int nbcartesmax;
} main_t;

typedef struct _pli_t
{
  carte_t carte[est + 1];
  carte_t defausse[est + 1];
  position_t entame;
  position_t nextpos;
  carte_t phcarte;
  carte_t lastcarte;
  int nopli;
  int nbpli_ligne[eo + 1];
  int noj; //Numero du joueur qui joue
  couleur_t atout;
  position_t leader;

} pli_t;

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
  couleur_t couleur;
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
  couleur_t atout;
  valeur_t nbplis;			/* decrit le nombre de plis a realiser */
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
  couleur_t couleur;
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


typedef struct _ihm_pli_t
{
  pli_t *pli;
  int socketid;
  int level;
  int random;
  int ligneia;
  char status ; 
  gboolean debug;
  int tab_couleur[est+1][pique+1];
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
} ihm_pli_t;

typedef struct _tablist
{
  valeur_t tabcoul[13];
  gboolean debug;               //position de la derniere carte jouee
  int nbcrt;                    //nombre de carte dans la couleur
  couleur_t couleureval;
} tablist_t;

typedef struct _button_bid_t
{
  ihm_pli_t *ihm_pli;
  ihm_bid_t *ihm_bid;
} button_bid_t;

void bouge_carte(position_t position,carte_t *carte,int maxl);
void bouge_carte_jeu(position_t poscour,carte_t  *carte, int nl, int nc, int maxl) ;
void affiche_pli(pli_t *pli);
char *affichage (int valeur,int type);

char * contrat_to_char ( couleur_t atout); 
char *affichage (int valeur, int type);
void affiche_carte (carte_t *carte);
void copy_pli (pli_t *ppliori, pli_t *pplicopie);
void affiche_pli( pli_t *ppli);
void init_pli (pli_t * pli, int maniere);
void free_ihm_pli( ihm_pli_t *ihm_pli) ;
void reset_ihm_pli (ihm_pli_t *ihm_pli);
void init_ihm_pli (ihm_pli_t *ihm_pli);
void init_bid (ihm_pli_t *ihm_pli);
char *display_str(char *str ); 
