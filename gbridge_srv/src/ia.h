#define PROFONDEURMAX 12
#define ALPHA_START -10000
#define BETA_START 10000
#define IALINE 1
typedef struct _rettrick_t
{
  int score;
  int alpha_or_beta;
  int nbline[eo+1];
} rettrick_t;

int best_elem_best_l(elem_best_t *elem_best,trick_t *pli);
int definition_prof ( trick_t *trick_cur);
rettrick_t * explore (int prof, trick_t * trick_cur, int prof_max,int *nb_best,l_best_t *l_best);
int  first_explore ( trick_t *pli, int prof_max,int *nb_best,l_best_t *l_best,game_t *game);
rettrick_t * cur_explore (int prof , trick_t *trick_cur, int prof_max,tablist_t **t_jeu,int orialpha,int oribeta,int nbcard);
carte_t *best_choice(int *nb_best,l_best_t *l_best,game_t *game,hopestat_t *hopestat,trick_t *pli); 
void break_point(void);
