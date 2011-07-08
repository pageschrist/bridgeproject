#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "traffic.h"
int i=0;
int j=0;
void write_header( game_t *game,char type) { 
  net_header_t header;
  i++;
  fprintf(stderr,"write_header %d :%c\n",i,type);
  strncpy(header.head,BRIDGE,8); 
  header.status=game->status;
  header.level=game->level;
  header.random=game->random;
  switch(type) {
  case 'p':
    header.type='p';
    header.lenght=sizeof(pli_t);
    write (game->sockslv_id,  &header,sizeof(net_header_t));
    break;
  case 'c':
    header.type='c';
    header.lenght=sizeof(carte_t);
    write (game->sockslv_id,  &header,sizeof(net_header_t));
    break;
  case 'b':
    header.type='b';
    header.lenght=sizeof(bid_t);
    write (game->sockslv_id,  &header,sizeof(net_header_t));
    break;
  case 'u':
    header.type='u';
    header.lenght=sizeof(BIDSIZE);
    write (game->sockslv_id,  &header,sizeof(net_header_t));
    break;

  }

}


//return -1 Pb of type  0 End 1 OK 
char  read_header (game_t *game,void *data,char type) {
  net_header_t header;
  j++;
  fprintf(stderr,"read_header %d :%c\n",j,type);
  int ret;
  ret=read (game->sockslv_id,  &header,sizeof(net_header_t));
  if(ret != sizeof(net_header_t)) {
    fprintf(stderr,"Error in the read of the header ret=%d\n",ret);
    return('z');
  }
  game->status=header.status;
  game->level=header.level;
  game->random=header.random;
  if(header.status=='e') {
    printf("End of game\n");
    return('e');
  }
  
  if(header.type!=type) {
    fprintf(stderr,"wrongtype, header.type=%c,type=%c\n",header.type,type);
    return('z'); 
  }
  else{
    switch(header.type) {
      case 'b':
      game->status='b';
      read_data(game,data,type);
      break;
      case 'u':
      game->status='u';
      read_data(game,data,type);
      break;
      case 'g':
      game->status='g';
      break;
      case 'n':
      game->status='n';
      game->random=header.random;
      game->level=header.level;
      break;

      default:
      read_data(game,data,type);
      break;

    }
  }
  return(game->status);
}
  
void read_data( game_t *game,void *data, char type) {
  pli_t *pli;
  carte_t *carte;
  bid_t *bid;
  char *cur_bid;
  switch(type) {
    case 'p':
      pli=(pli_t *) data;
      read (game->sockslv_id,  pli, sizeof (pli_t));
      break;
    case 'c':
      carte=(carte_t *) data;
      read (game->sockslv_id,  carte, sizeof (carte_t));
      break;
    case 'b':
      bid=(bid_t *) data;
      read (game->sockslv_id,  bid, sizeof (bid_t));
      break;
    case 'u':
      cur_bid=data;
      read (game->sockslv_id,  cur_bid, BIDSIZE);
      break;
    default:
      break;
 

  }

}
void write_data(game_t *game,void  *data,char type) {

  pli_t *pli;
  carte_t *carte;
  bid_t *bid;
  char *cur_bid;
  switch(type) {
    case 'p':
      pli= data;
      write_header(game,type);
      write (game->sockslv_id,  pli, sizeof (pli_t));
      break;
    case 'c':
      carte= data;
      write_header(game,type);
      write (game->sockslv_id,  carte, sizeof (carte_t));
      break;
    case 'b':
      bid= data;
      write_header(game,type);
      write (game->sockslv_id,  bid, sizeof (bid_t));
      break;
    case 'u':
      cur_bid= data;
      write_header(game,type);
      write (game->sockslv_id,  cur_bid, BIDSIZE);
      break;
 

  }


}
