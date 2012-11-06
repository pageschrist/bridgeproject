#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
//#include <glib/ghash.h>
#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include "objets.h"
#include "client.h"
#include "draw.h"
#include "ihmbridge.h"
#include "load.h"
#include "jeu.h"
#include "free.h"
#include "find.h"
#include "callbacks.h"
#include "alloc.h"
#include "traffic.h"
#include "file.h"

valeur_t convert_char_card(char c ) {
    valeur_t n;
    if(((c>'1') && (c<='9')) || (c=='A') || (c=='R') || (c=='D') || (c=='V')||(c=='X')) {

      if((c>'1') && (c<='9'))
        n=(valeur_t) (c-50);
      if(c=='A')
        n=cA;
      if(c=='R')
        n=cR;
      if(c=='V')
        n=cV;
      if(c=='D')
        n=cD;
      if(c=='X')
        n=cX;

      return(n);
    }
    else {
      fprintf(stderr,"%c is not correct\n",c);
      return(pdc);

    }
}

gboolean check_value(char *buffer,couleur_t couleur,int tabref[est+1][cA+1]) {
  size_t i;
  int n;
  if(strlen(buffer) <=0 )
    return (FALSE);
  for (i=1;i<strlen(buffer);i++) {
    if(pdc==(n=convert_char_card(buffer[i])))
      return(FALSE);
    tabref[couleur][n]++;
    if(tabref[couleur][n]>1 ) {
      fprintf(stderr,"some cards are in double\n");
      return(FALSE);
    }

  }

  return(TRUE);
}

gboolean check_parse (ihm_pli_t *ihm_pli ,char *transbuf) {
  position_t position;
  couleur_t couleur=trefle;
  int tabref[est+1][cA+1];
  char *buf;
  char *pos="SONE";
  char *coul="TKCP";
  memset(tabref,0,(cA+1)*(est+1)*sizeof(int));


  for (position = sud; position < est + 1; position++) {
    if(NULL!=(buf=strchr(transbuf,'\n'))) {
      buf[0]='\0';
      if((strlen(transbuf)!=1) || transbuf[0]!=pos[position]) {
        fprintf(stderr,"Error in file parsing, strlen(buffer)=%d, %c!=%c\n",(int)strlen(transbuf),transbuf[0],(char)pos[position]);
        free(transbuf);
        return FALSE;
      }
      transbuf=buf+1;
    }

    else {
      fprintf(stderr,"Error in file parsing, No \\n at the end of a line probably\n");
      free(transbuf);
      return FALSE;
    }
        if(ihm_pli->debug|TRUE)
      fprintf(stdout,"Load of pos=%c\n",pos[position]);

    for (couleur = trefle; couleur < pique + 1; couleur++) {
      if(NULL!=(buf=strchr(transbuf,'\n'))) {
        buf[0]='\0';
        if(transbuf[0]!=coul[couleur]) {
          fprintf(stderr,"Error in file parsing,  %c!=%c\n",transbuf[0],coul[couleur]);
          free(transbuf);
          return FALSE;
        }

        if(!check_value(transbuf,couleur,tabref)){
          free(transbuf);
          transbuf=NULL;
          return FALSE;
        }

        if(ihm_pli->debug)
          fprintf(stdout,"Load of col=%c\n",coul[couleur]);
        transbuf=buf+1;
      }
      else {
        fprintf(stderr,"Error in file parsing, buf=NULL\n");
        free(transbuf);
        transbuf=NULL;
        return FALSE;

      }


    }
  }
  //transbuf=NULL;
  return TRUE;

}

