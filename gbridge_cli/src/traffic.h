#define BRIDGE "gbridge"

typedef struct _net_header_t
{
 char head[8];  //should be gbridge\0
 char type; //should be p c g o
 int random;
 int level;
 char status ; //should be c e n
 int lenght; // lenght of the sata if necessary 
} net_header_t;
void write_data(ihm_pli_t *ihm_pli,void  *data,char type); 
void read_data( ihm_pli_t *ihm_pli,void *data, char type);
int  read_header (ihm_pli_t *ihm_pli,void *data,char type);
void write_header( ihm_pli_t *ihm_pli,char type) ;
