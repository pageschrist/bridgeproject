#include <math.h>
#include <stdlib.h>
#include "objets.h"
#include "draw.h"


void draw_cards(ihm_pli_t *ihm_pli, GList *list, GdkPixmap *target)
{
        if(ihm_pli->debug)
          printf("Draw cards\n");
        GList *ptr;
        imgcard_t *data_ptr;
        GdkPixmap *img;
        GdkGC *gc;
        GtkStateType state = GTK_WIDGET_STATE(ihm_pli->Drawing_area);

        gc = ihm_pli->Drawing_area->style->fg_gc[state];

        for(ptr = g_list_last(list); ptr; ptr = ptr->prev)
        {
                data_ptr = ptr->data;

                if(data_ptr->draw == TRUE || data_ptr->blink == TRUE)
                {
                        if(data_ptr->draw_face == FALSE)
                        {
                                img = ihm_pli->back;
                                gdk_gc_set_clip_mask(gc, ihm_pli->backmask);
                        }
                        else if(data_ptr->draw == TRUE)
                        {
                                img = data_ptr->img;
                                gdk_gc_set_clip_mask(gc, ihm_pli->cardmask);
                        }
                        else
                                img = NULL;

                        if(img != NULL)
                        {
                                gdk_gc_set_clip_origin(gc,
                                        data_ptr->dim.x,
                                        data_ptr->dim.y);
                                gdk_draw_drawable( target,
                                        ihm_pli->Drawing_area->style->fg_gc[state],
                                        img, 0, 0,
                                        data_ptr->dim.x,
                                        data_ptr->dim.y, -1, -1);

                                gdk_gc_set_clip_mask(gc, NULL);
                        }
                }
        }
}


void draw_moving_card(ihm_pli_t *ihm_pli, GdkPixmap *target)
{   
        GdkGC *gc;
        GdkPixmap *img;
        GtkStateType state = GTK_WIDGET_STATE(ihm_pli->Drawing_area);

        gc = ihm_pli->Drawing_area->style->fg_gc[state];

        if(ihm_pli->movecard->card->draw_face == TRUE)
        {
                img = ihm_pli->movecard->card->img;
                gdk_gc_set_clip_mask(gc, ihm_pli->cardmask);
        }
        else
        {
                img = ihm_pli->back;
                gdk_gc_set_clip_mask(gc, ihm_pli->backmask);
        }

        gdk_gc_set_clip_origin(gc,
                ihm_pli->movecard->dim.x,
                ihm_pli->movecard->dim.y);

        gdk_draw_drawable( target,
                ihm_pli->Drawing_area->style->fg_gc[state],
                img, 0, 0,
                ihm_pli->movecard->dim.x,
                ihm_pli->movecard->dim.y, -1, -1);

        gdk_gc_set_clip_mask(gc, NULL);
}


gboolean player_can_drop(ihm_pli_t *ihm_pli)
{
        return
        
                (
                        (ihm_pli->pli->nextpos)%2 != ((position_t )ihm_pli->ligneia)
                );
}

gboolean card_touch_dropzone(movingcard_t *movecard, rectangle_t *rectangle)
{
        return (
                movecard->dim.x  >  rectangle->dim.x - movecard->card->dim.w

                && movecard->dim.x  <  rectangle->dim.x + rectangle->dim.w

                && movecard->dim.y  >  rectangle->dim.y - movecard->card->dim.h

                && movecard->dim.y  <  rectangle->dim.y + rectangle->dim.h
        );
}




rectangle_t * draw_dropping_zone(ihm_pli_t *ihm_pli, GdkPixmap *target)
{
        GList *lst;
        rectangle_t *rectangle = NULL;

        for(lst = ihm_pli->dropping; lst; lst = lst->next)
        {
                rectangle = (rectangle_t *)lst->data;

                rectangle->active = FALSE;

                if(ihm_pli->movecard && player_can_drop(ihm_pli) )
                {
                        if( card_touch_dropzone(ihm_pli->movecard, rectangle) )
                        {
                                gdk_draw_rectangle(target,
                                ihm_pli->Drawing_area->style->light_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                                FALSE, rectangle->dim.x, rectangle->dim.y,
                                rectangle->dim.w, rectangle->dim.h);
                                rectangle->active = TRUE;
                        }
                }
        }
        return(rectangle);
}



void draw_container_ihm(ihm_pli_t *ihm_pli)
{
        if(ihm_pli->target != NULL)
        {
                /* Paint it black */
                gdk_draw_rectangle( ihm_pli->target,
                        ihm_pli->Drawing_area->style->black_gc, TRUE, 0, 0,
                        ihm_pli->Drawing_area->allocation.width,
                        ihm_pli->Drawing_area->allocation.height );


                draw_cards(ihm_pli, ihm_pli->waiting, ihm_pli->target);

                if(ihm_pli->players != NULL)
                {
                        int i;
                        for(i=0; i<4; i++)
                        draw_cards(ihm_pli, ihm_pli->players[i],
                                        ihm_pli->target);
                }

                //if(ihm_pli->dropping) {
                //        draw_dropping_zone(ihm_pli, ihm_pli->target);
                //}
                if(ihm_pli->movecard) {
                       draw_moving_card(ihm_pli, ihm_pli->target);
                }

                /* Blit */
                gdk_draw_drawable( ihm_pli->Drawing_area->window,
                        ihm_pli->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_pli->Drawing_area)],
                        ihm_pli->target, 0, 0, 0, 0, -1, -1 );

        }
        else {

          printf("target NULL \n");
          exit(1);
        }
}

void draw_copy(ihm_pli_t *ihm_pli)
{

        if(!ihm_pli->copy)
        {
                ihm_pli->copy = gdk_pixmap_new(
                        ihm_pli->Drawing_area->window,
                        ihm_pli->Drawing_area->allocation.width,
                        ihm_pli->Drawing_area->allocation.height, -1);

        }

        if(ihm_pli->copy)
        {
                draw_to(ihm_pli, ihm_pli->copy);
        }

}

void draw_to(ihm_pli_t *ihm_pli, GdkPixmap *target)
{
        if(target != NULL)
        {
                /* Paint it black */
                gdk_draw_rectangle( target,
                        ihm_pli->Drawing_area->style->black_gc, TRUE, 0, 0,
                        ihm_pli->Drawing_area->allocation.width,
                        ihm_pli->Drawing_area->allocation.height );




                draw_cards(ihm_pli, ihm_pli->waiting, target);

                if(ihm_pli->players != NULL)
                {
                        int i;
                        for(i=0; i< 4; i++)
                                draw_cards(ihm_pli,
                                        ihm_pli->players[i], target);
                }

        }
}


