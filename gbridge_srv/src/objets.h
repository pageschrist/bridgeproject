#define INIT 0
#define RESET 1
#define FREE 2
#define INDEX(position,couleur) ((position*4)+couleur)
#define INDCARD(color,value) ((color*NBPCOULEURS)+value)
#define CARDS 4
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
{ club, diamond, heart, spade, aucune } color_t;

typedef  enum 
{ sud, ouest, nord, est, aucun } position_t;

typedef  enum
{ ns, eo, pdl } ligne_t;

typedef  enum
{ new, partie, end } status_t;


typedef struct _tablist
{
  valeur_t tabcoul[13];
  gboolean debug;		//position de la derniere card jouee
  int nbcrt;			//nombre de card dans la couleur
  color_t couleureval;
} tablist_t;


  

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


typedef struct _best_t
{
  card_t *card;
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
  int nbcards[spade + 1];
  int nbpointshonneurs[spade + 1];
  int nbcardsmax;
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
  color_t couleur;
  position_t position;
  gboolean passe;
  gboolean contre;
  gboolean surcontre;
} bid_t;


typedef struct _trick_t
{
  card_t card[est + 1];
  card_t discard[est + 1];
  position_t entame;
  position_t nextpos;
  card_t phcard;
  card_t lastcard;
  int nbtrick;
  int nbtrick_line[eo + 1];
  int noj; //Numero du joueur qui joue
  color_t atout;
  position_t leader;
  gboolean eval;
  color_t coloreval;
} trick_t;


typedef struct _thread_jeu
{
  tablist_t **t_jeu;
  card_t *card;
  card_t *best_cardpot;
  trick_t *trick;
  short status;
  int score;
  int nbline[eo+1];
  int prof;
  int prof_max;
  gboolean *cardplayed;
  int nbcard;
} thread_jeu_t;

typedef struct _contrat_t
{
  color_t atout;
  valeur_t nbtricks;		/* decrit le nombre de tricks a realiser */
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
  char *buffile;
  gboolean debug;
  tablist_t **tabjeu;
  tablist_t **tabjeuref;   
  tablist_t **tabjeueval;   
  transfert_t *transfert;
  main_t mainjoueur[est + 1];
  contrat_t *contrat;
  bid_t *bid;
  char cur_bid[BIDSIZE];
  gboolean *cardplayed;
  int nbcard;
  
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

int max(int a,int b);
void *dupXXcate_trick (void *data);
void init_tablist (void);
void display_trick (trick_t * trick,gboolean display);
void *copy_card (void *data);
void init_tabjeu(game_t *game);
int remove_index (tablist_t **t_jeu,position_t position, color_t couleur, int pos_index);
int find_index (tablist_t **t_jeu,position_t position, color_t couleur, valeur_t valeur);
void affiche_tab_cards (void);
int insert_index (tablist_t **t_jeu,position_t position, color_t couleur, valeur_t valeur,int pos_index);
void affiche_tabjeuref (game_t *game,position_t position);
void affiche_tabjeu_c (tablist_t **t_jeu);
void affiche_card (card_t *card);
int sub_index (tablist_t **t_jeu,position_t position, color_t couleur, valeur_t valeur);
void init_trick (trick_t * trick, int maniere);
void add_list_f(l_best_t *l_best,best_t *best) ;
void add_list_l(l_best_t *l_best,best_t *best) ;
void init_list_best(l_best_t *l_best);
void clear_list( l_best_t *l_best);
void display_cardplayed(game_t * game);
void clear_cardplayed(game_t *game,int type);
void init_cardplayed(game_t *game);
void init_game(game_t *game);
void free_game(game_t *game);
void clear_tabjeu (game_t *game);
int char_to_int(char *c);
void clear_game(game_t *game);
void fdebug(void);
