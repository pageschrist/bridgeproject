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




/* stack.c 
   corps de la pile générique
*/

#include <stdlib.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "stack.h"

stackia_t create_stack(void *(*gen_copy) (void *data))
{
    stackia_t stack;

    stack = malloc(sizeof(struct stack_box));
    stack->stack = NULL;
    stack->copy_data = gen_copy;

    return (stack);
}

int push(stackia_t stack, void *data)
{
    struct box *box;
    void *data_dup = data;

    box = malloc(sizeof(struct box));
    data_dup = stack->copy_data(data);

    box->data = data_dup;
    box->next = stack->stack;

    stack->stack = box;

    return (0);
}

void *pop(stackia_t stack)
{
    void *data;
    struct box *tmp;

    if (stack->stack == NULL)
	return (NULL);

    data = stack->copy_data(stack->stack->data);
    tmp = stack->stack->next;

    free(stack->stack->data);
    free(stack->stack);
    stack->stack = tmp;

    return (data);
}

void vidage(stackia_t stack)
{
    struct box *tmp;

    while (stack->stack != NULL) {
	tmp = stack->stack->next;
	free(stack->stack->data);
	free(stack->stack);
	stack->stack = tmp;
    }
    free(stack);

}
