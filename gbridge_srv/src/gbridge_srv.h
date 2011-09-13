#define BACK_LOG 5
void signal_handler(int sig);
gboolean  newgame(game_t *game,hopestat_t **hopestat);
void  main_game(game_t *game);
void end_session(game_t * game);
