#define PROFONDEURMAX 12
#define ALPHA_START -10000
#define BETA_START 10000
typedef struct _retpli_t
{
  int score;
  int alpha_or_beta;
} retpli_t;
int definition_prof ( pli_t *pplic);
retpli_t * explore (int prof, pli_t * pplic, int prof_max,int *nb_best,l_best_t *l_best);
void  first_explore ( pli_t *pli, int prof_max,int *nb_best,l_best_t *l_best,game_t *game);
retpli_t * cur_explore (int prof , pli_t *pplic, int prof_max,tablist_t **t_jeu,int orialpha,int oribeta);
carte_t *choix_best(int *nb_best,l_best_t *l_best,game_t *game); 
void break_point(void);
