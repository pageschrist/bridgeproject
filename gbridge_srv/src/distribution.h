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
#define OUVREUR 0
#define INTERVENANT 1
#define REPONDANT 2
#define REPINTERVENANT 3
#define CARTE 100
#define COULEUR 101
#define POSITION 102
#define PASDECARTE -1
#define PASDECOULEUR -1
#define BONUSLONG 5

char desccouleur[NBPCOULEURS];
typedef struct jeu
{
  int nombre[NBCOULEURS];	/*indique le nombre de carte dans chaque couleur */
  int couleur[NBCOULEURS][NBPCOULEURS];	/*indique la liste des cartes dans chaque couleur */
  int nbpoints[NBCOULEURS];
  int nbpointstotal;
} jeu;

typedef struct _joueur
{
  int position;			/*ouvreur, repondant, intervenant ou reponse intervenant */
  int numero;			/*corrspond au nord sud est ouest */
  struct jeu main;
  int nbplitotal;
  int minpoints;
  int maxpoints;
} joueur_t;

char * affichage (int valeur, int type);
void initialisation (void);
int tirage (int diviseur);
char * affichage (int valeur, int type);
void affiche_contrat (contrat_t * contrat);
void defcarte (int dividende, position_t position);
void qintsort (int arr[], int lidx, int ridx);
void evalmain (game_t *game);
void presentation (main_t * mainjoueur);
void envoi_jeu (position_t position, game_t *game);
void remplit_liste_jeu (game_t *game);
void distribution (void);
void init_distrib (game_t *game);
