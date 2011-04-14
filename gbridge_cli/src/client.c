#include <stdio.h>		/* perror() */
#include <stdlib.h>		/* atoi() */
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>		/* read() */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <regex.h>
#include "client.h"
#include <glib.h>		/* For Glib functions */
#include <gtk/gtk.h>		/* For Glib functions */
#include "create_config.h"
#include "objets.h"
couleur_t atout;
extern GHashTable *configHash;
void
init_event (event_t suivi)
{
  suivi.mort = NONEXIST;
  suivi.declarant = NONEXIST;
  suivi.entame = NONEXIST;
  suivi.nbtour = NONEXIST;

}

int
tcpclient (void)
{
  int clientsocket;
  event_t suivi;
  int status = 0, port;
  struct hostent *phost = NULL;
  struct sockaddr_in ianame ;
  char buffer[256] = "";
  char *iaserver = NULL;
  port = atoi (g_hash_table_lookup (configHash, "port"));
  init_event (suivi);
  iaserver = g_hash_table_lookup (configHash, "server");
  port = atoi (g_hash_table_lookup (configHash, "port"));
  printf ("Voici mon server ia: %s", iaserver);
  printf ("Voici mon port : %d", port);

  clientsocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == clientsocket)
    {

      perror ("socket()");
      exit (1);
    }

  /*
   * need to resolve the remote server name or IP address
   */
  phost = gethostbyname (iaserver);
  if (NULL == phost)
    {
      phost = gethostbyaddr (iaserver, strlen (iaserver), AF_INET);
      if (NULL == phost)
	{
	  perror ("Error resolving server address ");
	  exit (1);
	}
    }

  ianame.sin_family = AF_INET;
  ianame.sin_port = htons (port);
  (void) memcpy (&ianame.sin_addr, phost->h_addr, phost->h_length);

  status = connect (clientsocket, (struct sockaddr *) &ianame,
		    sizeof (ianame));
  if (-1 == status)
    {
      perror ("connect()");
      exit (1);
    }
  return (clientsocket);
  /*
   * Client application specific code goes here
   *
   * e.g. receive messages from server, respond, etc.
   */
  while (0 < (status = read (clientsocket, buffer, sizeof (buffer))))
    {
      printf ("Message iaserver:%s", buffer);
      /*for (i = 0; i <= 255; i++)
         buffer[i] = 0;
         fgets (buffer, 256, stdin); */
      if (strncmp (buffer, ":G:", strlen (":G:")))
	{
	  printf ("New Game\n");
	  sprintf (buffer, ":K:G:");
	  write (clientsocket, buffer, strlen (buffer));
	  printf ("New Game OK \n");


	}
      }
    }
