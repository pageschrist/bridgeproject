gboolean card_touch_dropzone(movingcard_t *movecard, rectangle_t *rectangle);
gboolean player_can_drop(ihm_trick_t *ihm_pli);
void draw_moving_card(ihm_trick_t *ihm_pli, GdkPixmap *target);
rectangle_t * draw_dropping_zone(ihm_trick_t *ihm_pli, GdkPixmap *target);
void draw_cards(ihm_trick_t *ihm_pli, GList *list, GdkPixmap *target);
void draw_container_ihm(ihm_trick_t *ihm_pli);
void draw_to(ihm_trick_t *ihm_pli, GdkPixmap *target);
void draw_copy(ihm_trick_t *ihm_pli);
