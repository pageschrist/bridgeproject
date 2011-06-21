#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "traffic.h"
void write_header( game_t *game,char type) { 
  net_header_t header;
  strncpy(header.head,BRIDGE,8); 
  header.status=game->status;
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

  }

}


//return -1 Pb of type  0 End 1 OK 
int  read_header (game_t *game,void *data,char type) {
  net_header_t header;
  int ret;
  ret=read (game->sockslv_id,  &header,sizeof(net_header_t));
  if(ret != sizeof(net_header_t)) {
    printf("Error in the read of the header ret=%d\n",ret);
    return(-1);
  }
  game->status=header.status;
  if(header.status=='e') {
    printf("End of game\n");
    return(0);
  }
  
  if(header.type!=type) {
    printf("wrongtype, header.type=%c,type=%c\n",header.type,type);
    return(-1); 
  }
  else{
    read_data(game,data,type);
  }
  return(1);
}
  
void read_data( game_t *game,void *data, char type) {
  pli_t *pli;
  carte_t *carte;
  bid_t *bid;
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
 

  }

}
void write_data(game_t *game,void  *data,char type) {

  pli_t *pli;
  carte_t *carte;
  bid_t *bid;
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
 

  }


}
