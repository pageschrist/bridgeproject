#include <stdio.h>
#include "objets.h"

carte_t best_carte;

/* Fonction qui liste tous les newcartes , soit on passe une couleur sinon on a le choix*/
/*pli.entame doit etre initialise */



int
check_plis (pli_t *ppli)
{
  extern ligne_t ligneia;
  int score;
  ligne_t lignej;
  lignej = (ligneia + 1) % 2;
  score = ppli->nbpli_ligne[lignej] - ppli->nbpli_ligne[ligneia];
  printf("Voici le score dans check_plis: %d\n",score);
  return (score);

}
