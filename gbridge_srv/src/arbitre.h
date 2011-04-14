
void ia_play (int prof);

int check (couleur_t couleur);
void mise_a_jour(pli_t *ppli,position_t position,couleur_t couleur,valeur_t valeur);
int list_all_coups (position_t positionc, stackia_t stack, pli_t * pli,tablist_t **tmpjeu);
position_t evaluation_pli (pli_t * pli);
int joue_coup (pli_t * pli,carte_t *carte );
int check_plis (pli_t * pli);
void  copy_jeu(thread_jeu_t *thread_jeu) ;
void  destroy_jeu(thread_jeu_t *thread_jeu) ;
#define TROUVE 0
#define OUI 1
#define NON 0
#define PASTROUVE 1
#define FIN 3
#define CARTESEULE 2
#define ENTAME 0
#define DEFAUSSE 1
#define NONENTAME 2
