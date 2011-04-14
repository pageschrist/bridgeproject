#define NORMAL 0
#define CONTRE 1
#define SURCONTRE 2
#define FIT 8
#define ARRET 3
couleur_t fit (main_t *mainjoueur, int position);
position_t eval_contrat (game_t *game);
int small_condition(game_t *game,char *s_small);
int test_condition(game_t *game,char *s_condition); 
void analyse_bid(game_t *game);
