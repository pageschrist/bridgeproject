#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <glib/gtypes.h>
#include <glib/gprintf.h>
#include "objets.h"
#include "distribution.h"
#include "analyse.h"
#include "ia.h"
#include "create_config.h"

couleur_t search_best_color(position_t position, hopestat_t ** hopestat)
{
    couleur_t color, colorref;
    int nbref = 0, index;

    for (color = trefle; color < pique + 1; color++) {
	index = INDEX(position, color);
	if (hopestat[index]->nbline[position % 2] >= nbref) {
	    colorref = color;
	    nbref = hopestat[index]->nbline[position % 2];
	}


    }
    return (colorref);
}


void changeeval(game_t * game, couleur_t couleureval)
{
    couleur_t couleur;
    position_t position;
    int index;
    for (couleur = trefle; couleur < pique + 1; couleur++) {
	for (position = sud; position < est + 1; position++) {
	    index = INDEX(position, couleur);
	    game->tabjeu[index]->couleureval = couleureval;
	}
    }
}
carte_t *analyse_hand(game_t * game, pli_t * plic, couleur_t couleur)
{

    int index;
    pli_t *plin = malloc(sizeof(pli_t));
    memcpy(plin, plic, sizeof(pli_t));
    plin->atout = aucune;
    hopestat_t *hopestat = malloc(sizeof(hopestat_t));
    int sizemax;
    l_best_t *l_best = NULL;
    int nb_best = 0;
    carte_t *best_card = malloc(sizeof(carte_t));
    position_t position;
    sizemax = 0;
    for (position = sud; position < est + 1; position++) {
	index = INDEX(position, couleur);
	if (game->tabjeu[index]->nbcrt > sizemax)
	    sizemax = game->tabjeu[index]->nbcrt;
    }
    changeeval(game, couleur);
    position = plin->nextpos;
    index = INDEX(position, couleur);
    hopestat->position = position;
    hopestat->couleur = couleur;
    hopestat->best_card = NULL;;
    if (NULL == (l_best = malloc(sizeof(l_best_t)))) {
	fprintf(stderr, "Pb with malloc\n");
	exit(EXIT_FAILURE);
    } else {
	init_list_best(l_best);
    }
    nb_best = 0;
    first_explore(plin, (sizemax * 4) - plin->noj, &nb_best, l_best, game);
    if (nb_best != 0) {
	best_card = choix_best(&nb_best, l_best, game, hopestat);

    } else {
	free(best_card);
	best_card = NULL;
    }
    clear_list(l_best);
    free(l_best);
    l_best = NULL;

    changeeval(game, aucune);
    free(plin);
    return (best_card);
}


hopestat_t **analyse_tabjeu(game_t * game)
{

    int index;
    hopestat_t **hopestat = malloc(16 * sizeof(hopestat_t *));
    pli_t *pli;
    int sizemax[pique + 1];
    l_best_t *l_best = NULL;
    int nb_best = 0;
    struct timeval *timeav = malloc(sizeof(struct timeval));
    struct timeval *timeap = malloc(sizeof(struct timeval));
    pli = malloc(sizeof(pli_t));
    position_t position;
    couleur_t couleur;
    for (couleur = trefle; couleur < pique + 1; couleur++) {
	sizemax[couleur] = 0;
	for (position = sud; position < est + 1; position++) {
	    index = INDEX(position, couleur);
	    if (game->tabjeuref[index]->nbcrt > sizemax[couleur])
		sizemax[couleur] = game->tabjeuref[index]->nbcrt;
	}
    }
    for (couleur = trefle; couleur < pique + 1; couleur++) {
	changeeval(game, couleur);

	for (position = sud; position < est + 1; position++) {
	    index = INDEX(position, couleur);
	    hopestat[index] = malloc(sizeof(hopestat_t));
	    hopestat[index]->best_card = malloc(sizeof(carte_t));
	    hopestat[index]->position = position;
	    hopestat[index]->couleur = couleur;
	    init_pli(pli, INIT);
	    pli->entame = position;
	    pli->nextpos = position;
	    pli->atout = aucune;
	    if (NULL == (l_best = malloc(sizeof(l_best_t)))) {
		fprintf(stderr, "Pb with malloc\n");
		exit(EXIT_FAILURE);
	    } else {
		init_list_best(l_best);
	    }
	    nb_best = 0;
	    gettimeofday(timeav, NULL);
	    first_explore(pli, (sizemax[couleur] * 4) - pli->noj, &nb_best,
			  l_best, game);
	    gettimeofday(timeap, NULL);
	    if (nb_best != 0) {
		hopestat[index]->best_card =
		    choix_best(&nb_best, l_best, game, hopestat[index]);

	    } else {
		free(hopestat[index]->best_card);
		hopestat[index]->best_card = NULL;
	    }
	    clear_list(l_best);
	    free(l_best);
	    l_best = NULL;

	}
    }
    changeeval(game, aucune);
    free(pli);
    free(timeap);
    free(timeav);
    return (hopestat);
}

int small_condition(game_t * game, char *s_smallref)
{

    int i, index, meml = 0, memr = 0, testcond = 0, sum = 0;
    couleur_t couleur;
    char *ptr;
    char *s_sec;
    char *s_small = malloc(sizeof(char) * (strlen(s_smallref) + 1));
    memset(s_small, 0, sizeof(char) * (strlen(s_smallref) + 1));
    strncpy(s_small, s_smallref, strlen(s_smallref));
    if(game->debug)
      printf("voici s_small:%s\n", s_small);
    switch (s_smallref[0]) {
    case 'p':
	index = INDEX(game->bid->position, pique);
	meml = game->tabjeuref[index]->nbcrt;
	break;
    case 'c':
	index = INDEX(game->bid->position, coeur);
	meml = game->tabjeuref[index]->nbcrt;
	break;
    case 'k':
	index = INDEX(game->bid->position, carreau);
	meml = game->tabjeuref[index]->nbcrt;
	break;
    case 't':
	index = INDEX(game->bid->position, trefle);
	meml = game->tabjeuref[index]->nbcrt;
	break;
    case 'h':
	for (couleur = trefle; couleur < pique + 1; couleur++) {
	    index = INDEX(game->bid->position, couleur);
	    for (i = 0; i < game->tabjeuref[index]->nbcrt; i++) {
		if (game->tabjeuref[index]->tabcoul[i] == cA)
		    meml = meml + 4;
		if (game->tabjeuref[index]->tabcoul[i] == cR)
		    meml = meml + 3;
		if (game->tabjeuref[index]->tabcoul[i] == cD)
		    meml = meml + 2;
		if (game->tabjeuref[index]->tabcoul[i] == cV)
		    meml = meml + 1;
	    }
	}
	break;
    case 'd':
	for (couleur = trefle; couleur < pique + 1; couleur++) {
	    index = INDEX(game->bid->position, couleur);
	    for (i = 0; i < game->tabjeuref[index]->nbcrt; i++) {
		if (game->tabjeuref[index]->tabcoul[i] == cA)
		    meml = meml + 4;
		if (game->tabjeuref[index]->tabcoul[i] == cR)
		    meml = meml + 3;
		if (game->tabjeuref[index]->tabcoul[i] == cD)
		    meml = meml + 2;
		if (game->tabjeuref[index]->tabcoul[i] == cV)
		    meml = meml + 1;
	    }
	}
	break;
    }
    if (s_smallref[2] < 'A') {

	s_sec = strtok_r(s_small, &s_small[1], &ptr);
	s_sec = strtok_r(NULL, &s_small[1], &ptr);
	memr = atoi(s_sec);
    } else {
	switch (s_smallref[2]) {
	case 'p':
	    index = INDEX(game->bid->position, pique);
	    memr = game->tabjeuref[index]->nbcrt;
	    break;
	case 'c':
	    index = INDEX(game->bid->position, coeur);
	    memr = game->tabjeuref[index]->nbcrt;
	    break;
	case 'k':
	    index = INDEX(game->bid->position, carreau);
	    memr = game->tabjeuref[index]->nbcrt;
	    break;
	case 't':
	    index = INDEX(game->bid->position, trefle);
	    memr = game->tabjeuref[index]->nbcrt;
	    break;
	case 'h':
	    for (couleur = trefle; couleur < pique + 1; couleur++) {
		index = INDEX(game->bid->position, couleur);
		for (i = 0; i < game->tabjeuref[index]->nbcrt; i++) {
		    if (game->tabjeuref[index]->tabcoul[i] == cA)
			sum = sum + 4;
		    if (game->tabjeuref[index]->tabcoul[i] == cR)
			sum = sum + 3;
		    if (game->tabjeuref[index]->tabcoul[i] == cD)
			sum = sum + 2;
		    if (game->tabjeuref[index]->tabcoul[i] == cV)
			sum = sum + 1;

		}
	    }
	    memr = sum;
	    break;
	case 'd':
	    for (couleur = trefle; couleur < pique + 1; couleur++) {
		index = INDEX(game->bid->position, couleur);
		for (i = 0; i < game->tabjeuref[index]->nbcrt; i++) {
		    if (game->tabjeuref[index]->tabcoul[i] == cA)
			sum = sum + 4;
		    if (game->tabjeuref[index]->tabcoul[i] == cR)
			sum = sum + 3;
		    if (game->tabjeuref[index]->tabcoul[i] == cD)
			sum = sum + 2;
		    if (game->tabjeuref[index]->tabcoul[i] == cV)
			sum = sum + 1;


		}
	    }
	    memr = sum;
	    break;

	}

    }

    if (s_smallref[1] == '>')
	testcond = meml - memr;
    else
	testcond = memr - meml;
    free(s_small);
    if(game->debug)
      printf("testcondition:%d\n", testcond);
    return (testcond);

}

int test_condition(game_t * game, char *s_condition)
{
    char *s_small;
    char *saveptr1;
    s_small = strtok_r(s_condition, "&,", &saveptr1);
    if (small_condition(game, s_small) <= 0) {
	return 0;
    }
    s_small = strtok_r(NULL, "&,", &saveptr1);
    if(game->debug)
      printf("s_small:%s\n", s_small);
    while (s_small != NULL) {
	//while((s_small=strtok(NULL,"&,"))!=NULL){
	if (small_condition(game, s_small) <= 0) {
	    return 0;
	}
	s_small = strtok_r(NULL, "&,", &saveptr1);

    }
    return 1;
}


gboolean analyse_bid(game_t * game)
{
    FILE *obidhomeconf;
    char bidhomeconf[1024];
    gboolean found = FALSE;
    char *home, tmpstring[2];
    char *ptr, *affc;
    char *s_bidref, *s_newbid, *s_condition;
    char buf[16384];
    home = getenv("HOME");
    sprintf(bidhomeconf, "%s%s", home, BIDCONFHOME);
    obidhomeconf = fopen(bidhomeconf, "r");
    if (game->bid->passe == TRUE) {
	strcat(game->cur_bid, "PP");
    } else {
	tmpstring[0] = (char) (game->bid->nombre + 48);
	tmpstring[1] = '\0';
	game->bid->position = (game->bid->position + 2) % 4;
	strcat(game->cur_bid, tmpstring);
	affc = affichage(game->bid->couleur, COULEUR);
	strcat(game->cur_bid, affc);
	free(affc);
	strcat(game->cur_bid, "PP");
	while (fgets(buf, 16380, obidhomeconf)) {
	    if (!strncmp(buf, game->cur_bid, strlen(game->cur_bid))) {
		buf[strlen(buf) - 1] = '\0';
		s_bidref = strtok_r(buf, ":", &ptr);
		s_newbid = strtok_r(NULL, ":", &ptr);
		s_condition = strtok_r(NULL, ":", &ptr);
		if (test_condition(game, s_condition)) {
		    found = TRUE;
		    break;
		}
	    }



	}
	if (found == FALSE) {
	    strcat(game->cur_bid, "PP");
	} else
	    strcat(game->cur_bid, s_newbid);
    }
    strcat(game->cur_bid, "PP");
    fclose(obidhomeconf);
    return (found);
}



/*Cette fonction essaie de trouver un fit en majeur, sinon elle teste l'arret a sans atout , sinon elle est essaie un fit en mineur, enfin elle retourne la couleur la plus longue */
couleur_t fit(main_t * mainjoueur, int position)
{
    int nbmaxcarte = 0, i;
    couleur_t refcouleur;
    if (mainjoueur[position].nbcartes[pique] +
	mainjoueur[position + 2].nbcartes[pique] >= FIT)
	return (pique);

    if (mainjoueur[position].nbcartes[coeur] +
	mainjoueur[position + 2].nbcartes[coeur] >= FIT)
	return (coeur);

    if ((mainjoueur[position].nbpointshonneurs[pique] +
	 mainjoueur[position + 2].nbpointshonneurs[pique] >= ARRET)
	&& (mainjoueur[position].nbpointshonneurs[coeur] +
	    mainjoueur[position + 2].nbpointshonneurs[coeur] >= ARRET)
	&& (mainjoueur[position].nbpointshonneurs[carreau] +
	    mainjoueur[position + 2].nbpointshonneurs[carreau] >= ARRET)
	&& (mainjoueur[position].nbpointshonneurs[trefle] +
	    mainjoueur[position + 2].nbpointshonneurs[trefle] >= ARRET))
	return (aucune);

    if (mainjoueur[position].nbcartes[carreau] +
	mainjoueur[position + 2].nbcartes[carreau] >= FIT)
	return (carreau);
    if (mainjoueur[position].nbcartes[trefle] +
	mainjoueur[position + 2].nbcartes[trefle] >= FIT)
	return (trefle);
    for (i = pique; i >= trefle; i--) {
	if (mainjoueur[position].nbcartes[i] +
	    mainjoueur[position + 2].nbcartes[i] >= nbmaxcarte) {
	    nbmaxcarte =
		mainjoueur[position].nbcartes[i] + mainjoueur[position +
							      2].
		nbcartes[i];
	    refcouleur = i;
	}

    }
    return (refcouleur);
}

/* Nouvelle Fonction de calcul du contrat a jouer en attendant les fonctions d'bid */
position_t eval_contrat(game_t * game)
{
    int nbpointsns, nbpointseo, pointsadditionnels, ref, refpts, jouepar;
    couleur_t color, i;
    nbpointsns =
	game->mainjoueur[nord].nbpoints + game->mainjoueur[sud].nbpoints;
    nbpointseo =
	game->mainjoueur[est].nbpoints + game->mainjoueur[ouest].nbpoints;
    /* on cherche l'atout dans le camp majoritaire en point */
    if (nbpointsns >= nbpointseo) {
	color = fit(game->mainjoueur, SUD);
	printf("Couleur de reference:%d\n", color);
	ref = SUD;
	refpts = nbpointsns;
    } else {
	color = fit(game->mainjoueur, OUEST);
	ref = OUEST;
	refpts = nbpointseo;
    }
    printf("Points: %d", refpts);
    pointsadditionnels = 0;
    if (color != aucune) {
	for (i = 0; i < NBCOULEURS; i++) {
	    if (color != i) {
		if (game->mainjoueur[ref].nbcartes[i] <= 2)
		    pointsadditionnels =
			pointsadditionnels + 3 -
			game->mainjoueur[ref].nbcartes[i];
		if (game->mainjoueur[ref + 2].nbcartes[i] <= 2)
		    pointsadditionnels =
			pointsadditionnels + 3 -
			game->mainjoueur[ref].nbcartes[i];
	    }

	}
	if (game->mainjoueur[ref].nbcartes[color] >=
	    game->mainjoueur[ref + 2].nbcartes[color]) {
	    jouepar = ref;
	} else {
	    jouepar = ref + 2;
	}
	refpts = refpts + pointsadditionnels;
    } else {
	for (i = 0; i < NBCOULEURS; i++) {
	    if ((game->mainjoueur[ref].nbpointshonneurs[i] +
		 game->mainjoueur[ref + 2].nbpointshonneurs[i]) == ARRET) {

		if (game->mainjoueur[ref].nbpointshonneurs[i] >=
		    game->mainjoueur[ref + 2].nbpointshonneurs[i]) {
		    jouepar = ref;
		} else {
		    jouepar = ref + 2;
		}
	    }
	}
    }
    if ((refpts >= 20) && (refpts < 23))
	game->contrat->nom = 1;
    if ((refpts >= 23) && (refpts < 25))
	game->contrat->nom = 2;
    if ((refpts >= 25) && (refpts < 27))
	game->contrat->nom = 3;
    if ((refpts >= 27) && (refpts < 30))
	game->contrat->nom = 4;
    if ((refpts >= 30) && (refpts < 33))
	game->contrat->nom = 5;
    if ((refpts >= 33) && (refpts < 37))
	game->contrat->nom = 6;
    if (refpts >= 37)
	game->contrat->nom = 7;
    game->contrat->atout = color;
    game->contrat->nbplis = 6 + game->contrat->nom;
    game->contrat->declarant = 0;	//modification pour faire pivoter le jeu   =jouepar
    printf("Dans eval_contrat\n");
    return (4 - jouepar);
}
