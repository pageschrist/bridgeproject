#define TROUVE 0
#define OUI 1
#define NON 0
#define PASTROUVE 1
#define FIN 3
#define CARTESEULE 2
#define ENTAME 0
#define DISCARD 1
#define NONENTAME 2
int check (couleur_t couleur);
void ftrace(void);
valeur_t highest_value(gboolean *cardplayed,couleur_t color);
gboolean test_reprise(position_t position,tablist_t **t_jeu);
gboolean check_invert_lead(pli_t *pli,tablist_t **tmpjeu);
int calc_dist(gboolean *cardplayed,couleur_t color,valeur_t highest, valeur_t highest_player);
void rotation(game_t *game,hopestat_t **hopestat);
void mise_a_jour(pli_t *ppli,position_t position,couleur_t couleur,valeur_t valeur);
int list_all_coups (position_t positionc, l_item_t *l_item, pli_t * pli,tablist_t **tmpjeu,gboolean *cardplayed);
int list_all_coups_eval (position_t positionc, l_item_t *l_item, pli_t * pli,tablist_t **tmpjeu);
void define_leader(pli_t *pli);
position_t evaluation_pli (pli_t * pli);
int joue_coup (pli_t * pli,carte_t *carte,game_t *game );
rettrick_t * check_plis (pli_t * pli);
void  dup_game(thread_jeu_t *thread_jeu,game_t *game) ;
void  destroy_jeu(thread_jeu_t *thread_jeu) ;
