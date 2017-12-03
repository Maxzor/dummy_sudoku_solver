/* chaine.h : definition des structures et autres */

#define VersionCh 1.0

/* il s'agit ici de chainage simple (unidirectionnel ) */

struct Elt {
     void * obj; /* l'adresse non typee de l'objet */
     struct Elt * suiv; /* adresse du suivant, NULL si fin */
};

#define FIN (struct Elt*)NULL

extern void chaine_init(void);
extern struct Elt * chaine_ajout(void * objet);
extern int chaine_suppr(void * objet);
extern void chaine_liste(void);
extern void * chaine_adresseDernier(void);


