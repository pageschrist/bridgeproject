#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <stdlib.h>
#include <glib.h>
#include "create_config.h"


GHashTable *configHash = NULL;
void create_config(void)
{

    FILE *pconf, *prconf, *bconf, *bhconf;
    char *home;
    char hombconf[BUFSIZE];
    char bufconf[BUFSIZE];
    char bidconf[BUFSIZE];
    char bidhombconf[BUFSIZE];
    char line[BUFSIZE];
    struct stat bufstat;
    home = getenv("HOME");
    snprintf(bufconf,BUFSIZE,"%s/config/%s",DATADIR,FICHIERCONF);
    snprintf(bidconf, BUFSIZE,"%s/config/%s", DATADIR, BIDCONF);
    snprintf(hombconf, BUFSIZE,"%s%s", home, FICHIERCONFHOME);
    snprintf(bidhombconf, BUFSIZE,"%s%s", home, BIDCONFHOME);
    if (-1 == stat(hombconf, &bufstat)) {
	printf("%s doesn't exist, we must create it!\n", hombconf);


	if (-1 == stat(bufconf, &bufstat)) {
	    perror("stat");
	    exit(1);
	}
	if ((pconf = fopen(hombconf, "w")) == NULL) {
	    perror("fopen");
	    exit(1);
	}
	if ((prconf = fopen(bufconf, "r")) == NULL) {
	    perror("open");
	    exit(1);
	}

	while (fgets(line, BUFSIZE, prconf)) {
	    if (fputs(line, pconf) == -1) {
		perror("fputs");
		exit(1);
	    }

	}
	fclose(prconf);
	fclose(pconf);
	printf("Validate the values in %s and restart", hombconf);
	exit(0);


    }
    if (-1 == stat(bidhombconf, &bufstat)) {
	printf("%s doesn't exist, we must create it!\n", bidhombconf);

	if (-1 == stat(bidconf, &bufstat)) {
	    perror("stat");
	    exit(1);
	}
	if ((bhconf = fopen(bidhombconf, "w")) == NULL) {
	    perror("fopen");
	    exit(1);
	}
	if ((bconf = fopen(bidconf, "r")) == NULL) {
	    perror("open");
	    exit(1);
	}

	while (fgets(line, BUFSIZE, bconf)) {
	    if (fputs(line, bhconf) == -1) {
		perror("fputs");
		exit(1);
	    }

	}
	fclose(bconf);
	fclose(bhconf);
	printf("Check the values in %s and restart\n", bidhombconf);
	exit(0);


    }

}

void skipspace(char *line)
{
    char *newline;
    int i, j = 0;
    newline = malloc((strlen(line) + 1) * (sizeof(char)));
    for (i = 0; i < (int) strlen(line); i++) {
	if ((line[i] != ' ') && (line[i] != '\t') && (line[i] != '\n')) {

	    newline[j] = line[i];
	    j++;
	}


    }
    newline[j] = '\0';
    strcpy(line, newline);
}


void skipcomment(char *line)
{
    char *p;
    p = strchr(line, '#');
    if (p != NULL) {
	*p = '\0';
    }
}

void fill_config(void)
{
    FILE *prconf;
    char buf[MAX_LINE_SIZE];
    char *delim;
    char hombconf[BUFSIZE], *home;
    struct stat bufstat;
    char line[MAX_LINE_SIZE];
    int linenum = 0;
    home = getenv("HOME");
    sprintf(hombconf, "%s%s", home, FICHIERCONFHOME);
    if (NULL == (configHash = g_hash_table_new(g_str_hash, g_str_equal))) {
	perror("g_hash_table_new");
	exit(1);

    }
    if (-1 == stat(hombconf, &bufstat)) {
	perror("stat");
	exit(1);
    }

    if ((prconf = fopen(hombconf, "r")) == NULL) {
	perror("open");
	exit(1);
    }
    linenum = 1;
    while (fgets(buf, MAX_LINE_SIZE, prconf)) {
	skipcomment(buf);
	skipspace(buf);
	if (strlen(buf) >= 1) {
	    delim = strchr(buf, '=');
	    if (!delim) {
		printf("Error, malformed line[%d]: %s\n", linenum, line);
		exit(1);
	    }
	    *delim = '\0';
/*line a une forme maintenant clée\0valeur, strdup(line) nous donne la première partie */
	    g_hash_table_insert(configHash, strdup(buf),
				strdup(delim + 1));

	    linenum++;

	}
    }
}

void verifie_config(void)
{
    printf("Check of configuration\n");
    printf("port:%d\n", atoi(g_hash_table_lookup(configHash, "port")));
    printf("dir for  images:%s\n",
	   (char *) g_hash_table_lookup(configHash, "directory_images"));
    printf("server:%s\n",
	   (char *) g_hash_table_lookup(configHash, "server"));
    printf("random:%d\n", atoi(g_hash_table_lookup(configHash, "random")));
    printf("depth:%d\n",
	   atoi(g_hash_table_lookup(configHash, "profondeur")));
    /*   g_hash_table_destroy (configHash); */
}
