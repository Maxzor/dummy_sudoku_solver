/* chaine.c : code de la librairie chainage general */
#include <stdio.h>
#include <stdlib.h>

#include "chaine.h"

static struct Elt *Ancre;

void chaine_init(void)
{
   Ancre = FIN;
}

void chaine_liste(void)
{
struct Elt * T;
int i=1;
   T = Ancre;
   printf("Liste des elements :\n");
   while (T != FIN) { /* il y a un element suivant */
     /* l'adresse de l'objet est dans T->obj */
     printf("Adresse objet no %d : 0x%lx\n", i++, (long)T->obj);
     T = T->suiv; /* je passe au suivant */
   }
   printf("****** fin de la liste ********\n");
   return;
}

struct Elt * chaine_ajout(void * objet)
{
void * A;
struct Elt *E, **W; /* W est un pointeur de pointeur d'element */
    if ((A = malloc(sizeof(struct Elt))) == NULL) {
       fprintf(stderr,"Malloc impossible ! Arret !\n");
       exit(1);
    }
    /* c'est bon donc j'initialise le nouvel element */
    E = (struct Elt *)A;
    E->obj = objet;
    E->suiv=FIN;
    W = &Ancre;
    while (*W != FIN) W = &((*W)->suiv);
    *W = E;
    return E;
}

int chaine_suppr(void * objet)
{
struct Elt *E, **W; /* W est un pointeur de pointeur d'element */
    W = &Ancre;
    while (*W != FIN) {
        if ((*W)->obj == objet) { /* on a trouve ! */
           E = (*W); /* adresse de l'elt a supprimer */
           /* on schunte l'element que l'on va supprimer */
           *W = E->suiv;
           /* on supprime l'espace memoire de l'element */
           free((void*)E);
           return 1;
        }
        W = &((*W)->suiv);
    }
    return 0; /* objet pas trouve */
}

void * chaine_adresseDernier(void)
{
struct Elt * T;
void * A;
int i=1;
   T = Ancre;
   if (T == FIN) return NULL;
   while (T != FIN) { /* il y a un element suivant */
     A = T->obj;
     T = T->suiv; /* je passe au suivant */
   }
   return A;
}


