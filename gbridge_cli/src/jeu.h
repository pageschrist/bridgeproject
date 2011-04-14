#define NBPCOULEUR 13
#define TAILLEIMAGE 108
void position_targetzone(ihm_pli_t *ihm_pli);
void game_middle_offset_dynamic(position_t position, int *midx, int *midy, int w, int h, int cardw, int cardh);
void recuperation_jeu (ihm_pli_t *ihm_pli, position_t position );
int game_turn (ihm_pli_t *ihm_pli,imgcard_t *imgcard); 
