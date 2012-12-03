#include <stdio.h>
#include <unistd.h>
#include <string.h>
//#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include <glib.h>
#include "objets.h"
#include "traffic.h"
#include "file.h"
#define _GNU_SOURCE

#include <stdarg.h>

int i=0;
int j=0;
gboolean send_file(ihm_trick_t *ihm_pli) {
  FILE *fd;
  ssize_t ret;
  int i=0,j=0;
  int nb[NBPLAYER];
  int total=0;
  char buf[MAXFILE];
  char transbuf[MAXFILE];
  char transbufcheck[MAXFILE];
  transbuf[0]='\0';
  transbufcheck[0]='\0';
  memset (nb,0,NBPLAYER*sizeof(int));
  fd=fopen(ihm_pli->filename,"r");
  if(NULL==fd) 
    perror("fopen()");
  else {
    while(fgets(buf,MAXFILE,fd)) {
      if(0==strlen(transbuf))  
        strncpy(transbuf,buf,MAXFILE);
      else
        strcat(transbuf,buf);
      total=(strlen(buf)-1)+total; //-1 to remove \n
      i++;
      if(NBLINES==i) {
        i=0;
        nb[j]=total;
        total=0;
        j++;
      }
    }
    memcpy(transbufcheck,transbuf,MAXFILE);
    ret=check_parse(ihm_pli ,transbufcheck);
    if(FALSE==ret)
      return FALSE; 
    j=nb[0]; // just to fix the ref of the amount of cards;
    for (i=0;i<NBPLAYER;i++)
      if(j!=nb[i]) {
        fprintf(stderr,"Wrong number of cards %d != nb[%d]=%d \n",j,i,nb[i]);
        return FALSE;
      }

    if(ihm_pli->debug) 
      printf("send_file: transbuf=%s",transbuf);
    ret=write_data(ihm_pli,transbuf,'f',strlen(transbuf));
    

  }
  if(ret>=0)
   return TRUE;
  else
   return FALSE;
}


ssize_t write_header( ihm_trick_t *ihm_pli,char type,...) { 
  va_list args;
  ssize_t ret;
  net_header_t header;
  va_start (args, type);
   
  strncpy(header.head,BRIDGE,8); 
  header.status=ihm_pli->status; 
  header.level=ihm_pli->level; 
  header.debug=ihm_pli->debug; 
  header.random=ihm_pli->random; 
  if(ihm_pli->debug) {
    i++;
    fprintf(stderr,"write_header %d :%c\n",i,type);
  }
  switch(type) {
  
  case 'f':
    header.type='f';
    header.lenght=va_arg(args,int);
    ret=write (ihm_pli->socketid,  &header,sizeof(net_header_t));
    break;
  case 'g':
    header.type='g';
    header.lenght=sizeof(trick_t);
    ret=write (ihm_pli->socketid,  &header,sizeof(net_header_t));
    break;
  case 'p':
    header.type='p';
    header.lenght=sizeof(trick_t);
    ret=write (ihm_pli->socketid,  &header,sizeof(net_header_t));
    break;
  case 'c':
    header.type='c';
    header.lenght=sizeof(carte_t);
    ret=write (ihm_pli->socketid,  &header,sizeof(net_header_t));
    break;
  case 'b':
    header.type='b';
    header.lenght=sizeof(bid_t);
    ret=write (ihm_pli->socketid,  &header,sizeof(net_header_t));
    break;
  case 'u':
    header.type='u';
    header.lenght=BIDSIZE;
    ret=write (ihm_pli->socketid,  &header,sizeof(net_header_t));
    break;
  case 'n':
    header.type='n';
    header.lenght=0;
    if(ihm_pli->debug) 
      fprintf(stderr,"header.status=%c,header.random=%d,header.level=%d\n",header.status,header.random,header.level);
    ret=write (ihm_pli->socketid,  &header,sizeof(net_header_t));
    break;
  case 'e':
    header.type='e';
    header.status='e'; 
    header.lenght=0;
    if(ihm_pli->debug) 
      fprintf(stderr,"header.status=%c,header.random=%d,header.level=%d\n",header.status,header.random,header.level);
    ret=write (ihm_pli->socketid,  &header,sizeof(net_header_t));
    break;

  }
  va_end(args);
  return(ret);

}


//return -1 Pb of type  0 End 1 OK 
int  read_header (ihm_trick_t *ihm_pli,void *data,char type) {
  net_header_t header;
  int ret;
  if(ihm_pli->debug) { 
    j++;
    fprintf(stderr,"read_header %d :%c\n",j,type);
  }
  ret=read (ihm_pli->socketid,  &header,sizeof(net_header_t));
  if(ret != sizeof(net_header_t)) {
    fprintf(stderr,"Error in the read of the header ret=%d %lu\n",ret,sizeof(net_header_t));
    return(-1);
  }
  ihm_pli->status=header.status;
  ihm_pli->level=header.level;
  ihm_pli->random=header.random;
  ihm_pli->nbcard=header.nbcard;
  if(header.status=='e') {
    if(ihm_pli->debug)  
      printf("End of ihm_pli\n");
    return(0);
  }
  
  if(header.type!=type && type!='g') {
    fprintf(stderr,"wrongtype, header.type=%c,type=%c\n",header.type,type);
    return(-1); 
  }
  else{
    read_data(ihm_pli,data,type);
  }
  return(1);
}
  
void read_data( ihm_trick_t *ihm_pli,void *data, char type) {
  trick_t *pli;
  carte_t *carte;
  bid_t *bid;
  char *cur_bid;
  switch(type) {
    case 'p':
      pli= data;
      read (ihm_pli->socketid,  pli, sizeof (trick_t));
      break;
    case 'c':
      carte= data;
      read (ihm_pli->socketid,  carte, sizeof (carte_t));
      break;
    case 'b':
      bid= data;
      read (ihm_pli->socketid,  bid, sizeof (bid_t));
      break;
    case 'u':
      cur_bid= data;
      read (ihm_pli->socketid,  cur_bid, BIDSIZE*sizeof (char));
      break;
    case 'g':
      break;
 

  }

}
ssize_t write_data(ihm_trick_t *ihm_pli,void  *data,char type,...) {

  va_list args;
  ssize_t ret=0;
  va_start (args, type);
  trick_t *pli;
  int size;
  carte_t *carte;
  bid_t *bid;
  int sizetmp;
  char *cur_bid;
  char *buf,*bufref;
  
  
  switch(type) {
    case 'p':
      pli=(trick_t *) data;
      write_header(ihm_pli,type);
      ret=write (ihm_pli->socketid,  pli, sizeof (trick_t));
      break;
    case 'f':
      size=va_arg(args,int);      
      if(ihm_pli->debug)
        fprintf(stdout,"size=%d",size);
      write_header(ihm_pli,type,size);
      ret=write (ihm_pli->socketid,(char *) data, size );
      bufref=data;
      sizetmp=strlen(bufref);
      if(ihm_pli->debug) {
        while(NULL!=(buf=strchr(data,'\n'))) {
          buf[0]='\0';
          printf("data=%s\n",(char *)data);
          if(buf>(bufref+sizetmp-2))
            break;
          data=buf+1;
        
      
        }
      }
      
      break;
    case 'c':
      carte= data;
      write_header(ihm_pli,type);
      ret=write (ihm_pli->socketid,  carte, sizeof (carte_t));
      break;
    case 'b':
      bid= data;
      write_header(ihm_pli,type);
      ret=write (ihm_pli->socketid,  bid, sizeof (bid_t));
      break;
    case 'u':
      cur_bid= data;
      write_header(ihm_pli,type);
      ret=write (ihm_pli->socketid,  cur_bid, BIDSIZE);
      break;
    case 'n':
      write_header(ihm_pli,type);
      break;
    case 'e':
      write_header(ihm_pli,type);
      break;
 

  }
  va_end(args);
  return(ret);

}
