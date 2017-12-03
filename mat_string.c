/* mat_string.c : general matrix <> chaining string library */
#include <stdio.h>
#include <stdlib.h>

#include "mat_string.h"

static struct Elt *Anchor;

void init_string(void)
{
   Anchor = END;
}

void list_string(void)
{
struct Elt * T;
int i=1;
   T = Anchor;
   printf("List of the elements :\n");
   while (T != END) { /* there is a following element */
     /* object address is in T->obj */
     printf("Address of object no %d : 0x%lx\n", i++, (long)T->obj);
     T = T->fol; /* next */
   }
   printf("****** END of list ********\n");
   return;
}

struct Elt * add_string(void * objet)
{
void * A;
struct Elt *E, **W; /* W is a pointer of pointer of element */
    if ((A = malloc(sizeof(struct Elt))) == NULL) {
       fprintf(stderr,"Malloc impossible ! Stopping !\n");
       exit(1);
    }
    /* ok, initialize new element */
    E = (struct Elt *)A;
    E->obj = objet;
    E->fol=END;
    W = &Anchor;
    while (*W != END) W = &((*W)->fol);
    *W = E;
    return E;
}

int del_string(void * objet)
{
struct Elt *E, **W; /* W is a pointer of pointer of element */
    W = &Anchor;
    while (*W != END) {
        if ((*W)->obj == objet) { /* found it ! */
           E = (*W); /* address of the element to delete */
           /* we skip the element we are about to delete */
           *W = E->fol;
           /* freeing memory taken by element */
           free((void*)E);
           return 1;
        }
        W = &((*W)->fol);
    }
    return 0; /* object not found */
}

void * last_addr_string(void)
{
struct Elt * T;
void * A;
int i=1;
   T = Anchor;
   if (T == END) return NULL;
   while (T != END) { /* there is a following element */
     A = T->obj;
     T = T->fol; /* next */
   }
   return A;
}


