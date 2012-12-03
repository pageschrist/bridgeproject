#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
//#include <glib/ghash.h>
#include <glib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "objets.h"
#include "free.h"
#include "draw.h"
#include "callbacks.h"

void trash_list ( ihm_trick_t *ihm_pli) {
  GList *ptrwait;
  imgcard_t *data_ptr;
  for(ptrwait=ihm_pli->waiting;ptrwait;ptrwait=ptrwait->next) {
    

    data_ptr=ptrwait->data;

    ihm_pli->trash=g_list_prepend(ihm_pli->trash, (gpointer)data_ptr);
    ihm_pli->waiting=g_list_remove(ihm_pli->waiting, data_ptr);
    
  }
  ihm_pli->waiting=NULL;


}
void free_list(GList **ptrlist)
{
        GList *ptr;
        imgcard_t *data_ptr;

        for(ptr = *ptrlist; ptr; ptr = ptr->next)
        {
                data_ptr = ptr->data;

                g_object_unref(data_ptr->img);
                g_free(data_ptr);
                ptr->data = NULL;
        }

        if(*ptrlist)
        g_list_free(*ptrlist);
        *ptrlist = NULL;
}

