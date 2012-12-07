#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
//#include <glib/gtypes.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "traffic.h"
#include "file.h"
int i = 0;
int j = 0;
void write_header(game_t * game, char type)
{
    net_header_t header;
    memset(&header, 0, sizeof(net_header_t));
    if (game->debug) {
	i++;
	fprintf(stderr, "write_header %d :%c\n", i, type);
    }
    strncpy(header.head, BRIDGE, 8);
    header.status = game->status;
    header.level = game->level;
    header.random = game->random;
    header.nbcard = game->nbcard;
    switch (type) {
    case 'p':
	header.type = 'p';
	header.lenght = sizeof(trick_t);
	write(game->sockslv_id, &header, sizeof(net_header_t));
	break;
    case 'c':
	header.type = 'c';
	header.lenght = sizeof(card_t);
	write(game->sockslv_id, &header, sizeof(net_header_t));
	break;
    case 'b':
	header.type = 'b';
	header.lenght = sizeof(bid_t);
	write(game->sockslv_id, &header, sizeof(net_header_t));
	break;
    case 'u':
	header.type = 'u';
	header.lenght = sizeof(BIDSIZE);
	write(game->sockslv_id, &header, sizeof(net_header_t));
	break;

    }

}


//return -1 Pb of type  0 End 1 OK 
char read_header(game_t * game, void *data, char type)
{
    net_header_t header;
    position_t position;
    couleur_t couleur;
    int index,errnoori;
    int ret;
    ret = read(game->sockslv_id, &header, sizeof(net_header_t));
    errnoori=errno; 
    if (ret != sizeof(net_header_t)) {
       perror("read()");
	fprintf(stderr,
		"Error in the read of the header ret=%d\n header.type=%c, errno=%d\n",
		ret, header.type,errnoori);
	return ('e');
    }
    game->status = header.status;
    game->level = header.level;
    game->random = header.random;
    game->debug = header.debug;
    if (game->debug) {
	j++;
	fprintf(stderr, "read_header %d :%c\n", j, type);
    }
    for (position = sud; position < est + 1; position++) {
	for (couleur = club; couleur < spade + 1; couleur++) {
	    index = INDEX(position, couleur);
	    game->tabjeu[index]->debug = header.debug;
	}
    }
    if (header.status == 'e') {
      if(game->debug)
	printf("End of game\n");
	return ('e');
    }

    if (header.type != type && header.type != 'n' && header.type != 'e' && header.type != 'f') {
	fprintf(stderr, "wrongtype, header.type=%c,type=%c\n", header.type,
		type);
	return ('z');
    } else {
	switch (header.type) {
	case 'b':
	    game->status = 'b';
	    read_data(game, data, type);
	    break;
	case 'u':
	    game->status = 'u';
	    read_data(game, data, type);
	    break;
	case 'g':
	    game->status = 'g';
	    break;
	case 'n':
	    game->status = 'n';
	    game->random = header.random;
	    game->level = header.level;
	    if (game->debug)
		fprintf(stderr,
			"header.status=%c,header.random=%d,header.level=%d,header.nbcard=%d\n",
			header.status, header.random, header.level,header.nbcard);
	    break;
	case 'f':
	    game->status = 'f';
	    game->random = header.random;
	    game->level = header.level;
	    if (game->debug)
		fprintf(stderr,
			"header.status=%c,header.random=%d,header.level=%d,header.lenght=%d,header.nbcard=%d\n",
			header.status, header.random, header.level,header.lenght,header.nbcard);
	    read_data(game, data, 'f',header.lenght);
	    break;

	default:
	    read_data(game, data, type);
	    break;

	}
    }
    return (game->status);
}

void read_data(game_t * game, void *data, char type,...)
{
    va_list args;
    trick_t *trick;
    ssize_t size;
    va_start (args, type);
    card_t *card;
    bid_t *bid;
    char *cur_bid;
    switch (type) {
    case 'p':
	trick = (trick_t *) data;
	read(game->sockslv_id, trick, sizeof(trick_t));
	break;
    case 'c':
	card = (card_t *) data;
	read(game->sockslv_id, card, sizeof(card_t));
	break;
    case 'b':
	bid = (bid_t *) data;
	read(game->sockslv_id, bid, sizeof(bid_t));
	break;
    case 'u':
	cur_bid = data;
	read(game->sockslv_id, cur_bid, BIDSIZE);
	break;
    case 'f':
        size=va_arg(args,int);
        if(!game->buffile)
          game->buffile=malloc((size+1)*sizeof(char));
	read(game->sockslv_id, game->buffile, size);
        game->buffile[size]='\0';
	break;

    }

}
char  write_data(game_t * game, void *data, char type)
{

    trick_t *trick;
    card_t *card;
    bid_t *bid;
    char *cur_bid;
    if(game->status=='e') 
      return('e');
    switch (type) {
    case 'p':
	trick = data;
        display_trick(trick,TRUE);
	write_header(game, type);
	write(game->sockslv_id, trick, sizeof(trick_t));
	break;
    case 'c':
	card = data;
	write_header(game, type);
	write(game->sockslv_id, card, sizeof(card_t));
	break;
    case 'b':
	bid = data;
	write_header(game, type);
	write(game->sockslv_id, bid, sizeof(bid_t));
	break;
    case 'u':
	cur_bid = data;
	write_header(game, type);
	if (game->debug)
	    fprintf(stderr, "cur_bid=%s\n", cur_bid);
	write(game->sockslv_id, cur_bid, BIDSIZE);
	break;

    }
    return(type);

}
