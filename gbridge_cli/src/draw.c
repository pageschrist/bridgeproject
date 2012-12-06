#include <math.h>
#include <stdlib.h>
#include "objets.h"
#include "draw.h"


void draw_cards(ihm_trick_t *ihm_setup, GList *list, GdkPixmap *target)
{
        if(ihm_setup->debug)
          printf("Draw cards\n");
        GList *ptr;
        imgcard_t *data_ptr;
        GdkPixmap *img;
        GdkGC *gc;
        GtkStateType state = GTK_WIDGET_STATE(ihm_setup->Drawing_area);

        gc = ihm_setup->Drawing_area->style->fg_gc[state];

        for(ptr = g_list_last(list); ptr; ptr = ptr->prev)
        {
                data_ptr = ptr->data;

                if(data_ptr->draw == TRUE || data_ptr->blink == TRUE)
                {
                        if(data_ptr->draw_face == FALSE)
                        {
                                img = ihm_setup->back;
                                gdk_gc_set_clip_mask(gc, ihm_setup->backmask);
                        }
                        else if(data_ptr->draw == TRUE)
                        {
                                img = data_ptr->img;
                                gdk_gc_set_clip_mask(gc, ihm_setup->cardmask);
                        }
                        else
                                img = NULL;

                        if(img != NULL)
                        {
                                gdk_gc_set_clip_origin(gc,
                                        data_ptr->dim.x,
                                        data_ptr->dim.y);
                                gdk_draw_drawable( target,
                                        ihm_setup->Drawing_area->style->fg_gc[state],
                                        img, 0, 0,
                                        data_ptr->dim.x,
                                        data_ptr->dim.y, -1, -1);

                                gdk_gc_set_clip_mask(gc, NULL);
                        }
                }
        }
}


void draw_moving_card(ihm_trick_t *ihm_setup, GdkPixmap *target)
{   
        GdkGC *gc;
        GdkPixmap *img;
        GtkStateType state = GTK_WIDGET_STATE(ihm_setup->Drawing_area);

        gc = ihm_setup->Drawing_area->style->fg_gc[state];

        if(ihm_setup->movecard->card->draw_face == TRUE)
        {
                img = ihm_setup->movecard->card->img;
                gdk_gc_set_clip_mask(gc, ihm_setup->cardmask);
        }
        else
        {
                img = ihm_setup->back;
                gdk_gc_set_clip_mask(gc, ihm_setup->backmask);
        }

        gdk_gc_set_clip_origin(gc,
                ihm_setup->movecard->dim.x,
                ihm_setup->movecard->dim.y);

        gdk_draw_drawable( target,
                ihm_setup->Drawing_area->style->fg_gc[state],
                img, 0, 0,
                ihm_setup->movecard->dim.x,
                ihm_setup->movecard->dim.y, -1, -1);

        gdk_gc_set_clip_mask(gc, NULL);
}


gboolean player_can_drop(ihm_trick_t *ihm_setup)
{
        return
        
                (
                        (ihm_setup->pli->nextpos)%2 != ((position_t )ihm_setup->ligneia)
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




rectangle_t * draw_dropping_zone(ihm_trick_t *ihm_setup, GdkPixmap *target)
{
        GList *lst;
        rectangle_t *rectangle = NULL;

        for(lst = ihm_setup->dropping; lst; lst = lst->next)
        {
                rectangle = (rectangle_t *)lst->data;

                rectangle->active = FALSE;

                if(ihm_setup->movecard && player_can_drop(ihm_setup) )
                {
                        if( card_touch_dropzone(ihm_setup->movecard, rectangle) )
                        {
                                gdk_draw_rectangle(target,
                                ihm_setup->Drawing_area->style->light_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                                FALSE, rectangle->dim.x, rectangle->dim.y,
                                rectangle->dim.w, rectangle->dim.h);
                                rectangle->active = TRUE;
                        }
                }
        }
        return(rectangle);
}



void draw_container_ihm(ihm_trick_t *ihm_setup)
{
        printf("Draw !!\n");
        if(ihm_setup->target != NULL)
        {
                /* Paint it black */
                gdk_draw_rectangle( ihm_setup->target,
                        ihm_setup->Drawing_area->style->black_gc, TRUE, 0, 0,
                        ihm_setup->Drawing_area->allocation.width,
                        ihm_setup->Drawing_area->allocation.height );


                draw_cards(ihm_setup, ihm_setup->waiting, ihm_setup->target);

                if(ihm_setup->players != NULL)
                {
                        int i;
                        for(i=0; i<4; i++)
                        draw_cards(ihm_setup, ihm_setup->players[i],
                                        ihm_setup->target);
                }

                //if(ihm_setup->dropping) {
                //        draw_dropping_zone(ihm_setup, ihm_setup->target);
                //}
                if(ihm_setup->movecard) {
                       draw_moving_card(ihm_setup, ihm_setup->target);
                }

                /* Blit */
                gdk_draw_drawable( ihm_setup->Drawing_area->window,
                        ihm_setup->Drawing_area->style->fg_gc[GTK_WIDGET_STATE(ihm_setup->Drawing_area)],
                        ihm_setup->target, 0, 0, 0, 0, -1, -1 );

        }
        else {

          printf("target NULL \n");
          exit(1);
        }
}

void draw_copy(ihm_trick_t *ihm_setup)
{

        if(!ihm_setup->copy)
        {
                ihm_setup->copy = gdk_pixmap_new(
                        ihm_setup->Drawing_area->window,
                        ihm_setup->Drawing_area->allocation.width,
                        ihm_setup->Drawing_area->allocation.height, -1);

        }

        if(ihm_setup->copy)
        {
                draw_to(ihm_setup, ihm_setup->copy);
        }

}

void draw_to(ihm_trick_t *ihm_setup, GdkPixmap *target)
{
        if(target != NULL)
        {
                /* Paint it black */
                gdk_draw_rectangle( target,
                        ihm_setup->Drawing_area->style->black_gc, TRUE, 0, 0,
                        ihm_setup->Drawing_area->allocation.width,
                        ihm_setup->Drawing_area->allocation.height );




                draw_cards(ihm_setup, ihm_setup->waiting, target);

                if(ihm_setup->players != NULL)
                {
                        int i;
                        for(i=0; i< 4; i++)
                                draw_cards(ihm_setup,
                                        ihm_setup->players[i], target);
                }

        }
}


