#include <gtk/gtk.h>
#include "objets.h"
#include "string.h"
#include "find.h"
#include "callbacks.h"
#include "draw.h"

void set_moveable(GList *ptr, gboolean value)
{
        imgcard_t *data_ptr;

        for(; ptr; ptr = ptr->next)
        {
                data_ptr = ptr->data;

                data_ptr->moveable = value;
        }
}

gboolean test_card(ihm_pli_t *ihm_pli,imgcard_t *imgcard) {
  position_t poscour;
  couleur_t couleurentame;

  poscour=ihm_pli->pli->nextpos;
  if(ihm_pli->pli->noj==0)
    couleurentame=aucune;
  else
  couleurentame=ihm_pli->pli->carte[ihm_pli->pli->entame].clcarte;

  if(imgcard->position!=poscour)
    return(FALSE);
  else {
     if(couleurentame==aucune)
       return(TRUE);
     if(ihm_pli->tab_couleur[poscour][couleurentame]==0)
       return(TRUE);
     else {
       if(imgcard->couleur==couleurentame)
         return(TRUE);
       else
         return(FALSE);
     }
  }
  return(FALSE);


}

gboolean find_moveable_card(GList *ptr, GdkEventButton *event,
        ihm_pli_t *ihm_pli)
{
        imgcard_t *data_ptr;
        gboolean found = FALSE;
        set_moveable(ihm_pli->players[ihm_pli->pli->nextpos],TRUE);
        set_moveable(ihm_pli->players[(ihm_pli->pli->nextpos+1)%4],FALSE);
        set_moveable(ihm_pli->players[(ihm_pli->pli->nextpos+2)%4],FALSE);
        set_moveable(ihm_pli->players[(ihm_pli->pli->nextpos+3)%4],FALSE);
        while( ptr )
        {
                data_ptr = ptr->data;

                if(event->button == 1 && (gint) event->x >= data_ptr->dim.x
                        && (gint) event->y >= data_ptr->dim.y
                        && (gint) event->x < data_ptr->dim.x+data_ptr->dim.w
                        && (gint) event->y < data_ptr->dim.y+data_ptr->dim.h)
                {
                        if(     data_ptr->moveable == TRUE && ihm_pli->movecard == NULL && (test_card(ihm_pli,data_ptr)))
                        {
                                ihm_pli->movecard = (movingcard_t*)
                                        g_malloc(sizeof( movingcard_t) );
                                memset(ihm_pli->movecard,0,sizeof( movingcard_t)) ;

                                if( ihm_pli->movecard )
                                {
                                        ihm_pli->movecard->card = data_ptr;

                                        ihm_pli->movecard->dim.w = (gint)event->x - data_ptr->dim.x;
                                        ihm_pli->movecard->dim.h = (gint)event->y - data_ptr->dim.y;

                                        ihm_pli->movecard->dim.x = data_ptr->dim.x;
                                        ihm_pli->movecard->dim.y = data_ptr->dim.y;

                                        data_ptr->draw = FALSE;

                                        draw_container_ihm(ihm_pli);
                                }
                        }
                        printf("status carte:movable=%d drawable=%d\n",data_ptr->moveable,data_ptr->draw);
                        found = TRUE;
                        ptr = NULL;
                }
                else
                        ptr = ptr->next;
        }

        return(found);
}

