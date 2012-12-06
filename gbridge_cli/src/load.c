#include <string.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <stdlib.h>
//#include <glib/gstring.h>
#include <glib.h>
#include <glib/gprintf.h>
//#include "mask.xbm"
#include "objets.h"
#include "load.h"
extern GHashTable *configHash;
imgcard_t  *load_imgcard(gchar *file, carte_t *carte,ihm_trick_t *ihm_setup,position_t position) {

  imgcard_t *imgcard = (imgcard_t *) g_malloc(sizeof(imgcard_t) );

  if(imgcard != NULL) {
    imgcard->couleur = carte->clcarte;
    imgcard->valeur = carte->nocarte;
    imgcard->position = position;

    imgcard->dim.x = 0;
    imgcard->dim.y = 0;

    imgcard->draw = TRUE;
    imgcard->draw_face = TRUE;
    imgcard->moveable = FALSE;

    imgcard->blink = FALSE;
    imgcard->status = AVAILABLE;
    imgcard->img = gdk_pixmap_create_from_xpm( ihm_setup->Drawing_area->window, NULL, NULL, file);

    gdk_drawable_get_size(imgcard->img, &(imgcard->dim.w), &(imgcard->dim.h));

  }
  return(imgcard);
}

void unload_imgcard(imgcard_t *imgcard) {
 g_object_unref(imgcard->img);
 g_free(imgcard);
}


