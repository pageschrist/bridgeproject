#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <errno.h> 
#include <string.h> 
#include <glib.h> 
#define MAX_LINE_SIZE 4096 
#define MAX_STRING_SIZE 256 
#define PORT 9999; 
#define BUFSIZE 1024
#define FICHIERCONF "gbridge_cli.conf"
#ifndef DATADIR
#define DATADIR "/usr/share/gbridge_cli"

#endif


#define FICHIERCONFHOME "/.gbridge_cli.conf"

void create_config(void);
void fill_config(void);
void verifie_config(void);
void skipcomment(char *line);
void skipspace (char *line);
