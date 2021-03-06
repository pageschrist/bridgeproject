

typedef struct item {
void *data;
struct item *next;
struct item *prev;
} item_t;

typedef struct _l_item_t {
item_t *upper_item ;
item_t *lower_item ;
void *(*dup_item)(void *data);
}l_item_t; 


l_item_t * create_l_item(void *(*gen_copy) (void *data));
void add_item_head(l_item_t *l_item,void *data);
void *pop_item_head(l_item_t *l_item);
void *pop_item_tail(l_item_t *l_item);
void add_item_tail(l_item_t *l_item,void *data);
void clean_l_item(l_item_t *l_item);
