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
  card_t *best_card;
  int profmax;
  int aff;
  int goodiness[2];
} hopestat_t;

typedef struct _choice_color_t
{
  couleur_t interessant;
  couleur_t toavoid;
} choice_color_t;

card_t  *analyse_hand (game_t *game,trick_t *trick_cur,couleur_t couleur);
void search_best_color(position_t position,hopestat_t **hopestat,choice_color_t *choice_color);
couleur_t fit (main_t *mainjoueur, int position);
position_t eval_contrat (game_t *game);
hopestat_t ** analyse_tabjeu(game_t *game,trick_t *trick);
int small_condition(game_t *game,char *s_small);
int test_condition(game_t *game,char *s_condition); 
gboolean analyse_bid(game_t *game);
void changeeval(game_t *game,couleur_t couleureval);
