#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "objets.h"
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


void clear_buffile(game_t *game) {
  if(NULL!=game->buffile)
    free(game->buffile);
  game->buffile=NULL;
}

gboolean check_tabjeu(game_t *game) {
  position_t position;
  couleur_t color;
  int index;
  int nbref[est+1];
  for (position=sud;position<est+1;position++) {
    nbref[position]=0;
    for (color=trefle;color<pique+1;color++){
      index=INDEX(position,color);
      nbref[position]=nbref[position]+game->tabjeu[index]->nbcrt;
    }
  }
  for (position=sud;position<est+1;position++) {
    if (nbref[position]!=nbref[sud]) {
      fprintf(stderr,"Number of cards must be the same on all sides: nbref[%d]!=nbref[sud]\n",position);
      return FALSE;
    }
      
  }
  game->nbcard=nbref[sud];
  return TRUE;
}

gboolean file_parse (game_t *game) {
  char *bufref;
  position_t position;
  couleur_t couleur=trefle;
  size_t j;
  int tabref[est+1][cA+1];
  int index,k;
  char *buf;
  char *pos="SONE";
  char *coul="TKCP";
  memset(tabref,0,(cA+1)*(est+1)*sizeof(int));
  bufref=game->buffile;

#ifdef DEBUG
    game->debug = TRUE;
#else
    game->debug = FALSE;
#endif

  init_tabjeu(game);
  for (position = sud; position < est + 1; position++) {
    if(NULL!=(buf=strchr(game->buffile,'\n'))) {
      buf[0]='\0';
      if((strlen(game->buffile)!=1) || game->buffile[0]!=pos[position]) {
        fprintf(stderr,"Error in file parsing, strlen(buffer)=%d, %c!=%c\n",(int)strlen(game->buffile),game->buffile[0],(char)pos[position]);
        clear_buffile(game);
        clear_tabjeu(game);
        return FALSE;
      }
      game->buffile=buf+1;
    }
    else {
      fprintf(stderr,"Error in file parsing, No \\n at the end of a line probably\n");
      clear_tabjeu(game);
      clear_buffile(game);
      return FALSE;

    }
    if(game->debug)
      fprintf(stdout,"Load of pos=%c\n",pos[position]);

    for (couleur = trefle; couleur < pique + 1; couleur++) {
      if(NULL!=(buf=strchr(game->buffile,'\n'))) {
        buf[0]='\0';
        if(game->buffile[0]!=coul[couleur]) {
          fprintf(stderr,"Error in file parsing,  %c!=%c\n",game->buffile[0],coul[couleur]);
          clear_tabjeu(game);
          clear_buffile(game);
          return FALSE;
        }
        index = INDEX(position, couleur);
        
        if(!check_value(game->buffile,couleur,tabref)){
          clear_tabjeu(game);
          clear_buffile(game);
          return FALSE;
        }
        k=strlen(game->buffile);
        for(j=1;j<strlen(game->buffile);j++) {
          //game->tabjeu[index]->tabcoul[k-j-1]=convert_char_card(game->buffile[j]);
          //game->tabjeuref[index]->tabcoul[k-j-1]=convert_char_card(game->buffile[j]);
          game->tabjeu[index]->tabcoul[j-1]=convert_char_card(game->buffile[j]);
          game->tabjeuref[index]->tabcoul[j-1]=convert_char_card(game->buffile[j]);
          game->tabjeu[index]->nbcrt++;
          game->tabjeuref[index]->nbcrt++;
          if(game->debug)
            game->tabjeu[index]->debug=TRUE;
        }
        
        if(game->debug)
          fprintf(stdout,"Load of col=%c\n",coul[couleur]);
        game->buffile=buf+1;
      }
      else {
        fprintf(stderr,"Error in file parsing, buf=NULL\n");
        clear_tabjeu(game);
        clear_buffile(game);
        return FALSE;
  
      }
      
     
    }
  }
  if(bufref)
    free(bufref);
  if(game->debug)
    affiche_tabjeu_c(game->tabjeu);
  game->buffile=NULL;
  game->contrat = malloc(sizeof(contrat_t));
  game->bid = malloc(sizeof(bid_t));
  game->cur_bid[0] = '\0';
  game->status = 'b';
  return TRUE;

}


