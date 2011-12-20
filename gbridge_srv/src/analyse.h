#define NORMAL 0
#define CONTRE 1
#define SURCONTRE 2
#define FIT 8
#define ARRET 3
typedef struct _hopestat_t
{
  couleur_t couleur;
  position_t position;
  int score;
  int nbline[2];
  carte_t *best_card;
  int profmax;
  int aff;
} hopestat_t;
carte_t  *analyse_hand (game_t *game,pli_t *plic,couleur_t couleur);
couleur_t search_best_color(position_t position,hopestat_t **hopestat);
couleur_t fit (main_t *mainjoueur, int position);
position_t eval_contrat (game_t *game);
hopestat_t ** analyse_tabjeu(game_t *game);
int small_condition(game_t *game,char *s_small);
int test_condition(game_t *game,char *s_condition); 
gboolean analyse_bid(game_t *game);
void changeeval(game_t *game,couleur_t couleureval);
