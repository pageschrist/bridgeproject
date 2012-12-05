#define BRIDGE "gbridge"
#define MAXFILE 1024
#define NBPLAYER 4
#define NBLINES 5

typedef struct _net_header_t
{
 char head[8];  //should be gbridge\0
 char type; //should be p c g o
 int random;
 int level;
 char status ; //should be c e n
 int lenght; // lenght of the sata if necessary 
 gboolean debug; // lenght of the sata if necessary 
 int nbcard;
} net_header_t;
ssize_t write_data(ihm_trick_t *ihm_pli,void  *data,char type,...); 
void read_data( ihm_trick_t *ihm_pli,void *data, char type);
int  read_header (ihm_trick_t *ihm_pli,void *data,char type);
ssize_t write_header( ihm_trick_t *ihm_pli,char type,...) ;
gboolean send_file(ihm_trick_t *ihm_pli);
