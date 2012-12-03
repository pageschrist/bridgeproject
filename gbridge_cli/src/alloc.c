#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "objets.h"
#include "alloc.h"

/*
 * alloc_pixmap : Allocate a new GdkPixmap
 * which is like an offscreen Pixmap to 
 * draw different part of the game into.
 * 
 * We need to do that when first starting 
 * the game, and after that when resizing 
 * the window a new pixmap should be 
 * allocated with the size of the resized 
 * window.
 * 
 * struct _prog *prog_data: the main 
 * struct which is what makes possible 
 * to pass pointer to every usefull 
 * part of the memory.
 * 
 * Here we need :
 * 
 * prog_data->target - the GtkPixmap
 * 
 * prog_data->area - the widget where 
 * the pixmap will be copied to be 
 * visible to the user.
 * 
 * prog_data->output - the main stream 
 * for printing message. Default is 
 * stdout.
 */

void alloc_pixmap(ihm_trick_t *ihm_pli)
{
	if(ihm_pli->target != NULL)
	{
		g_object_unref(ihm_pli->target);
		ihm_pli->target = NULL;
	}
	
	ihm_pli->target = gdk_pixmap_new( 
		ihm_pli->Drawing_area->window, 
		ihm_pli->Drawing_area->allocation.width, 
		ihm_pli->Drawing_area->allocation.height, 
		-1) ;
	
	if(ihm_pli->target == NULL)
	{
		if(ihm_pli->output != NULL)
			fprintf(ihm_pli->output, 
				_("Unable to allocated target pixmap\n") );
	}
}

/* alloc_targetzone : a target zone is a 
 * rectangle in the game area where you 
 * can drop a card during the game.
 * 
 * Example: the top left area represent 
 * the pile. The bottom of the screen is 
 * the user hand. And the middle is the 
 * table where you drop one of your card 
 * to play it.
 * 
 * This function allocate a new target zone 
 * and prepend it to a GList.
 * 
 * struct _prog *prog_data: see file def.h
 * 
 * Here we need : 
 * 
 * prog_data->dropping : a GList in which we put 
 * every dropping area in a data struct _target 
 * which contains a dim (x,y, w,h) and a flag 
 * (active or not).
 * 
 * return : struct _target *
 * pointer to the target zone allocated. 
 * NULL in case of an error.
 */

rectangle_t * alloc_targetzone(ihm_trick_t *ihm_pli)
{
	rectangle_t *zone = (rectangle_t *) g_malloc(sizeof(rectangle_t) );
	
	if(zone)
	{
		zone->active = FALSE;
		zone->dim.x = 0;
		zone->dim.y = 0;
		zone->dim.w = 0;
		zone->dim.h = 0;
		
		ihm_pli->dropping = g_list_prepend(
			ihm_pli->dropping, (gpointer)zone);
	}
	
	return( zone );
}

/* alloc_prog: takes care of allocation memory 
 * for every important part of the program. 
 * 
 * Each element is considered as allocated during 
 * the game. Which is why this alloc_prog function 
 * is needed.
 * 
 * struct _prog *prog_data: see file def.h
 * 
 * Here we need: 
 * 
 * prog_data->icons : an array composed by four 
 * GdkPixbuf containing icons of each family 
 * of the card game (diamond, spade, heart, club)
 * This is used to reflect the main family chosen 
 * by the master player in the beginning of each 
 * round.
 * 
 * prog_data->players : array composed by four 
 * GList, each corresponding to a player hand.
 * Each element in all lists contains a struct 
 * _card which is composed of informations 
 * to know which card it is, like the family, 
 * the points, the position on the game area, 
 * etc. See file def.h for more details about 
 * the _card structure.
 * 
 * prog_data->teams : array composed by two
 * GList, each containing cards winned during 
 * all turns played by teams. This is where 
 * cards disappears. 
 * 
 * All GList (players and teams) are freed 
 * when a round is finished. It's more like 
 * a real list of cards you put next to you 
 * when playing a real card game. 
 * 
 * 
 */
void alloc_ihm(ihm_trick_t *ihm_pli)
{
	
	alloc_pixmap(ihm_pli);
	alloc_targetzone(ihm_pli);

}

