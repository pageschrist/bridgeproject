


/* méthodes de la pile */

stackia_t create_stack (void *(*gen_copy) (void *data));

int push (stackia_t stack, void *data);
void *pop (stackia_t stack);
void vidage (stackia_t stack);
