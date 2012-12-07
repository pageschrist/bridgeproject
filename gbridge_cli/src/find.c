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

gboolean test_card(ihm_trick_t *ihm_setup,imgcard_t *imgcard) {
  position_t poscour;
  couleur_t couleurentame;

  poscour=ihm_setup->trick->nextpos;
  if(ihm_setup->trick->noj==0)
    couleurentame=aucune;
  else
  couleurentame=ihm_setup->trick->carte[ihm_setup->trick->entame].clcarte;

  if(imgcard->position!=poscour)
    return(FALSE);
  else {
     if(couleurentame==aucune)
       return(TRUE);
     if(ihm_setup->tab_couleur[poscour][couleurentame]==0)
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
        ihm_trick_t *ihm_setup)
{
        imgcard_t *data_ptr;
        gboolean found = FALSE;
        set_moveable(ihm_setup->players[ihm_setup->trick->nextpos],TRUE);
        set_moveable(ihm_setup->players[(ihm_setup->trick->nextpos+1)%4],FALSE);
        set_moveable(ihm_setup->players[(ihm_setup->trick->nextpos+2)%4],FALSE);
        set_moveable(ihm_setup->players[(ihm_setup->trick->nextpos+3)%4],FALSE);
        while( ptr )
        {
                data_ptr = ptr->data;

                if(event->button == 1 && (gint) event->x >= data_ptr->dim.x
                        && (gint) event->y >= data_ptr->dim.y
                        && (gint) event->x < data_ptr->dim.x+data_ptr->dim.w
                        && (gint) event->y < data_ptr->dim.y+data_ptr->dim.h)
                {
                        if(     data_ptr->moveable == TRUE && ihm_setup->movecard == NULL && (test_card(ihm_setup,data_ptr)))
                        {
                                ihm_setup->movecard = (movingcard_t*)
                                        g_malloc(sizeof( movingcard_t) );
                                memset(ihm_setup->movecard,0,sizeof( movingcard_t)) ;

                                if( ihm_setup->movecard )
                                {
                                        ihm_setup->movecard->card = data_ptr;

                                        ihm_setup->movecard->dim.w = (gint)event->x - data_ptr->dim.x;
                                        ihm_setup->movecard->dim.h = (gint)event->y - data_ptr->dim.y;

                                        ihm_setup->movecard->dim.x = data_ptr->dim.x;
                                        ihm_setup->movecard->dim.y = data_ptr->dim.y;

                                        data_ptr->draw = FALSE;

                                        draw_container_ihm(ihm_setup);
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

