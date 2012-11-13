/*
  Copyright 2003 Eric Lacombe
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/





#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "stack.h"



/* core of the item generic list */
/*     l_item  -->upper_item  --->  item1  next=item2
					   prev=NULL
                                           data

                                    item2  next=item3
                                           prev=item2
                                           data

              -->lower_item --->    item3  next=NULL
                                           prev=item2
                                           data
         
			

*/
l_item_t * create_l_item(void *(*gen_copy) (void *data)) {
  l_item_t *l_item=malloc(sizeof (l_item_t));
  l_item->upper_item=NULL;
  l_item->lower_item=NULL;
  l_item->dup_item=gen_copy;
  return(l_item);
}



void add_item_head(l_item_t *l_item,void *data) {
  item_t *item=malloc(sizeof(item_t));
  item->data=l_item->dup_item(data);
  if(NULL ==l_item->lower_item && NULL==l_item->upper_item) {  //First item
    l_item->lower_item=item;
    l_item->upper_item=item;
    item->next=NULL;
    item->prev=NULL;
  }
  else {
    item->next=l_item->upper_item;
    item->prev=NULL;
    item->next->prev=item;
    l_item->upper_item=item;
  }
}

void *pop_item_head(l_item_t *l_item) {
  item_t *item;
  void *data;
  if(l_item->upper_item==NULL || l_item->upper_item==NULL)
    return NULL;
  item=l_item->upper_item;
  data=item->data;
  l_item->upper_item=item->next;
  if(NULL==l_item->upper_item)
    l_item->lower_item=NULL;
  if(item->next) 
    item->next->prev=NULL;
  free(item);
  return(data);
}

void *pop_item_tail(l_item_t *l_item) {
  item_t *item;
  void *data;
  if(l_item->lower_item==NULL ||l_item->upper_item==NULL)
    return NULL;
  item=l_item->lower_item;
  data=item->data;
  l_item->lower_item=item->prev;
  if(NULL==l_item->lower_item)
    l_item->upper_item=NULL;
  if(item->prev) 
    item->prev->next=NULL;
  free(item);
  return(data);
}

void add_item_tail(l_item_t *l_item,void *data) {
  item_t *item=malloc(sizeof(item_t));
  item->data=l_item->dup_item(data);
  if(NULL ==l_item->lower_item && NULL==l_item->upper_item) {  //First item
    l_item->lower_item=item;
    l_item->upper_item=item;
    item->next=NULL;
    item->prev=NULL;
  }
  else {
    item->prev=l_item->lower_item;
    item->next=NULL;
    item->prev->next=item;
    l_item->lower_item=item;
  }
}

void clean_l_item(l_item_t *l_item) {
  item_t *item;
  
  while(l_item->upper_item) {
    item=l_item->upper_item->next;
    free(l_item->upper_item->data);
    free(l_item->upper_item);
    l_item->upper_item=item;
  }
  free(l_item);
}


