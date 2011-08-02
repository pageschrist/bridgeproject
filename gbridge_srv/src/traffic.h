#define BRIDGE "gbridge"

typedef struct _net_header_t
{
 char head[8];  //should be gbridge\0
 char type; //should be p c g o
 int random; 
 int level; 
 char status ; //should be c e n
 int lenght; // lenght of the sata if necessary 
 gboolean debug; 
} net_header_t;
void write_data(game_t *game,void  *data,char type); 
void read_data( game_t *game,void *data, char type);
char  read_header (game_t *game,void *data,char type);
void write_header( game_t *game,char type) ;
