#define TAILLEIMAGE 108
#ifndef SAVEFILEDIR
#define SAVEFILEDIR "/home/cpages/dev/bridgeproject-0.9/"
#endif
void position_targetzone(ihm_trick_t *ihm_pli);
void game_middle_offset_dynamic(position_t position, int *midx, int *midy, int w, int h, int cardw, int cardh);
void recuperation_jeu (ihm_trick_t *ihm_pli, position_t position );
int game_turn (ihm_trick_t *ihm_pli,imgcard_t *imgcard); 
void position_list(GList *ptr, int x, int y, enum eposition ep, float coef);
void init_game(ihm_trick_t *ihm_pli);
