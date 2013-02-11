#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <errno.h> 
#include <string.h> 
#include <glib.h> 
#define MAX_LINE_SIZE 4096 
#define MAX_STRING_SIZE 256 
#define BUFSIZE 1024 
#define PORT 9999; 
#define FICHIERCONF "gbridge_srv.conf"
#ifndef DATADIR
#define DATADIR "/usr/share/gbridge_srv"
#else

#endif

#define BIDCONF "bids_srv.conf"
#define FICHIERCONFHOME "/.gbridge_srv.conf"
#define BIDCONFHOME "/.bids_srv.conf"

void create_config (void);
void skipspace (char *line);
void skipcomment (char *line);
void fill_config(void);
void verifie_config (void);
