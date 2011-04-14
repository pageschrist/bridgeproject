#define NVP :G
#define CPIQUE :P
#define CCOEUR :C
#define CCARREAU :K
#define CTREFLE :T
#define MORT :M
#define FIN :
#define ATOUT :A
#define TOUR :R
#define DECLARANT :D
#define OK :K
#define EXIST 1
#define NONEXIST 0


typedef struct _event_t {

int mort;
int declarant;
int entame;
int nbtour;
 } event_t;

int tcpclient(void);
