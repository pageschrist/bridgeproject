#include <stdio.h>
#include <mcheck.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "objets.h"
#include "ihmbridge.h"
#include "create_config.h"
#include "gbridge_cli.h"
#ifdef DEBUG
gboolean debug=TRUE;
#else
gboolean debug=FALSE;
#endif

#ifdef ANIMATION
gboolean animation=TRUE;
#else
gboolean animation=FALSE;
#endif
/* Il s'agit du programme principal */
static GOptionEntry entries[] =
{
  { "debug", 0, 0, G_OPTION_ARG_NONE, &debug, N_("Start in debug mode, for developers only"), NULL },
  { "disable-animation", 0, 0, G_OPTION_ARG_NONE, &animation, N_("Do not display cards' move. Useful on old computers."), NULL },
};


extern GtkWidget *Fenetre;
int main (int argc, char *argv[])
{

  GOptionContext* context;
  GError *error = NULL;
  ihm_trick_t *ihm_pli;
  mtrace();
  ihm_pli=malloc(sizeof(ihm_trick_t));
  memset(ihm_pli,0,sizeof(ihm_trick_t));
  context = g_option_context_new ( _("Gbridge GTK game") );
  if(context) {
    //g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
    g_option_context_add_main_entries (context, entries, "gbridge");
    g_option_context_add_group (context, gtk_get_option_group (TRUE));
    g_option_context_parse (context, &argc, &argv, &error);

    g_free(context);
    create_config();
    rempli_config();
    verifie_config();
    create_ihm(ihm_pli);        
    
  //gtk_init (&argc, &argv);
    if (ihm_pli->Fenetre != NULL ) { 
      gtk_widget_show_all(ihm_pli->Fenetre);
      gtk_main ();
    }
  }
  return (0);

}
