#define INIT 0
#define RESET 1
#define INDEX(position,couleur) ((position*4)+couleur)
#define CARTES 4
#define GAMEOVER 0
#define NEWGAME 1
#define PLI 2
#define CONTRAT 3
#define BID 5
#define HAND 6
#define WAITING 7
#define NEWDIST 8
#define BIDSIZE 100
#define DEFAULTRANDOM 40
#define DEFAULTLEVEL 5

typedef  enum 
{ c2, c3, c4, c5, c6, c7, c8, c9, cX, cV, cD, cR, cA, pdc } valeur_t;

typedef  enum  
{ trefle, carreau, coeur, pique, aucune } couleur_t;

typedef  enum 
{ sud, ouest, nord, est, aucun } position_t;

typedef  enum
{ ns, eo, pdl } ligne_t;

typedef  enum
{ new, partie, end } status_t;


typedef struct _tablist
{
  valeur_t tabcoul[13];
  int poscartej;		//position de la derniere carte jouee
  int nbcrt;			//nombre de carte dans la couleur
  couleur_t couleureval;
} tablist_t;


void *duplique_pli (void *data);

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


typedef struct _best_t
{
  carte_t *carte;
  int score;
  int numero;
  int nbline[eo+1];
} best_t;

typedef struct _elem_best
{
 struct _elem_best *next; 
 struct _elem_best *prev; 
 best_t *best;
  
} elem_best_t;

typedef struct _l_best
{
  elem_best_t *first;  
  elem_best_t *last;

} l_best_t;

typedef struct _main_t
{
  int nbpoints;
  int nbcartes[pique + 1];
  int nbpointshonneurs[pique + 1];
  int nbcartesmax;
} main_t;

typedef struct _transfert_t 
{
 int status;
 int random;
 int level;
} transfert_t;

typedef struct _bid_t
{
  int nombre;
  couleur_t couleur;
  position_t position;
  gboolean passe;
  gboolean contre;
  gboolean surcontre;
} bid_t;


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
} pli_t;


typedef struct _thread_jeu
{
  tablist_t **t_jeu;
  carte_t *carte;
  carte_t *best_cartepot;
  pli_t *pli;
  short status;
  int score;
  int nbline[eo+1];
  int prof;
  int prof_max;
} thread_jeu_t;

typedef struct _contrat_t
{
  couleur_t atout;
  valeur_t nbplis;		/* decrit le nombre de plis a realiser */
  position_t declarant;		/* donne le nom du declarant */
  int nom;
  int maxc[4];
} contrat_t;

typedef struct _game_t
{
  int sockslv_id;
  char status;
  int level;
  int random;
  tablist_t **tabjeu;
  tablist_t **tabjeuref;   
  tablist_t **tabjeueval;   
  transfert_t *transfert;
  main_t mainjoueur[est + 1];
  contrat_t *contrat;
  bid_t *bid;
  char cur_bid[BIDSIZE];
  
} game_t; 

struct box
{
  void *data;
  struct box *next;
};

struct stack_box
{
  struct box *stack;
  void *(*copy_data) (void *data);
};

typedef struct stack_box *stackia_t;

int echiquier_init (void);
void init_tablist (void);
void affiche_pli (pli_t * ppli);
void *copy_carte (void *data);
void init_tabjeu(game_t *game);
int remove_index (tablist_t **t_jeu,position_t position, couleur_t couleur, int pos_index);
int find_index (tablist_t **t_jeu,position_t position, couleur_t couleur, valeur_t valeur);
void affiche_tab_cartes (void);
int insert_index (tablist_t **t_jeu,position_t position, couleur_t couleur, valeur_t valeur,int pos_index);
void affiche_tabjeuref (game_t *game,position_t position);
void affiche_carte (carte_t *carte);
int sub_index (tablist_t **t_jeu,position_t position, couleur_t couleur, valeur_t valeur);
void init_pli (pli_t * pli, int maniere);
void add_list_f(l_best_t *l_best,best_t *best) ;
void add_list_l(l_best_t *l_best,best_t *best) ;
void init_list_best(l_best_t *l_best);
void clear_list( l_best_t *l_best);
void init_game(game_t *game);
void free_game(game_t *game);
void clear_tabjeu (game_t *game);
int char_to_int(char *c);
void clear_game(game_t *game);
