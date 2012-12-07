#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <stdlib.h>
#include <glib.h>
#include "create_config.h"
GHashTable *configHash = NULL;

void
create_config (void )
{

  FILE *pconf, *prconf;
  char *home;
  char homeconf[1024];
  char line[1024];
  struct stat bufstat;
  home = getenv ("HOME");
  sprintf (homeconf, "%s%s", home, FICHIERCONFHOME);
  if (-1 == stat (homeconf, &bufstat))
    {
      printf ("%s n'existe pas on va le créer!\n", homeconf);


      if (-1 == stat (FICHIERCONF, &bufstat))
	{
	  perror ("stat");
	  exit (1);
	}
      if ((pconf = fopen (homeconf, "w")) == NULL)
	{
	  perror ("fopen");
	  exit (1);
	}
      if ((prconf = fopen (FICHIERCONF, "r")) == NULL)
	{
	  perror ("open");
	  exit (1);
	}

      while (fgets (line, 1024, prconf))
	{
	  if (fputs (line, pconf) == -1)
	    {
	      perror ("fputs");
	      exit (1);
	    }

	}
      fclose (prconf);
      fclose (pconf);
      printf ("Validez les valeurs dans %s et relancer", homeconf);
      exit (0);


    }

}

void
skipspace (char *line)
{
  char *newline;
  int  j = 0;
  unsigned int i;
  newline = malloc ((strlen (line) + 1) * (sizeof (char)));
  for (i = 0; i < strlen (line); i++)
    {
      if ((line[i] != ' ') && (line[i] != '\t') &&(line[i] != '\n'))
	{

	  newline[j] = line[i];
	  j++;
	}


    }
  newline[j]='\0';
  strcpy (line, newline);
}


void
skipcomment (char *line)
{
  char *p;
  p = strchr (line, '#');
  if (p != NULL)
    {
      *p = '\0';
    }
}

void fill_config (void )
{
  FILE  *prconf;
  char buf[MAX_LINE_SIZE];
char *delim;
  struct stat bufstat;
  char line[MAX_LINE_SIZE];
  int linenum = 0; 
  if (NULL == (configHash = g_hash_table_new (g_str_hash, g_str_equal)))
    {
      perror ("g_hash_table_new");
      exit (1);

    }
  if (-1 == stat (FICHIERCONF, &bufstat))
    {
      perror ("stat");
      exit (1);
    }

  if ((prconf = fopen (FICHIERCONF, "r")) == NULL)
    {
      perror ("open");
      exit (1);
    }
  linenum = 1;
  while (fgets (buf, MAX_LINE_SIZE, prconf))
    {
      skipcomment (buf);
      skipspace (buf);
      if (strlen (buf) >= 1)
	{
	  delim = strchr (buf, '=');
	  if (!delim)
	    {
	      printf ("Error, malformed line[%d]: %s\n", linenum, line);
	      exit (1);
	    }
	  *delim = '\0';
/*line a une forme maintenant clée\0valeur, strdup(line) nous donne la première partie */
	  g_hash_table_insert (configHash, strdup (buf), strdup (delim + 1));

	  linenum++;

	}
    }
}

void verifie_config(void)
{
  printf ("Verification de la configuration\n");
  printf("Voici le port:%d\n",atoi (g_hash_table_lookup (configHash, "port")));
  printf("Voici le répertoire images:%s\n",(char *) g_hash_table_lookup (configHash,  "dir_imgs"));
  printf("Voici le server:%s\n",(char *) g_hash_table_lookup (configHash, "server"));
  //g_hash_table_destroy (configHash);
}


