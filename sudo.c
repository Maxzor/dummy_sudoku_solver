/* sudo.c : programme principal du sudoku */
#include <stdio.h>
#include <stdlib.h>

#include "sudo.h"
#include "chaine.h"

/*
#define TRACE
*/

static char MAT[NBC*NBC];  /* la matrice 9x9 */

static char * AdM = MAT; /* adresse de la matrice en cours */

/* fonction qui cree une nouvelle matrice a partir de celle en cours */
char * new_matrice(void)
{
void * A;
char * R;
int i;
     if ((A = malloc(NBC*NBC)) == NULL) {
        perror("malloc"); exit(9);
     }
     R = (char*)A;
     for (i=0; i<(NBC*NBC);i++) R[i] = AdM[i];
     return R;
}

void finerr(char *m) /* fonction de fin d'erreur */
{
   fprintf(stderr,"ERREUR : %s !!\n",m);
   exit(1);
}

void continuOuPas(char * M)
{
char C, Ok=0;
   printf("\n%s ? :",M);
   C = (char)getchar();
   if (C == 'O') Ok=1;
   while (C != '\n') C = (char)getchar();
   if (Ok) return;
   finerr("Arret demande par l'utilisateur");  
}

/* les variables globales */
#define LBUF 512
char buf[LBUF]; /* voila un joli buffer */

void init_mat(void)
{
int i;
   for (i=0;i<(NBC*NBC);i++) AdM[i] = ' '; /* on met des espaces partout */
   return;
}

void affiche_mat(void)
{
int i,j;
   printf("## Contenu de la matrice ##\n");
   printf("/---------\\\n");
   for (i=0;i<NBC;i++) {
       printf("|");
       for (j=0;j<NBC;j++) printf("%c",AdM[(i*NBC)+j]); /* on affiche chaque elt */
       printf("|\n");
   }
   printf("/---------\\\n");
   printf("###########################\n");
   return;
}

int charge_mat(FILE * fdec)
{
int i=0,j;
   while(fgets(buf, LBUF, fdec) != NULL) {
      /* on ignore les commentaire */
      if (buf[0] == '#') continue;
      /* on remplit la ligne no i (de 0 a 8) */
      for (j=0; j<NBC; j++) {
         if(buf[j] == '\n') break; 
         if(buf[j] == '\0') break;
         if (buf[j] == '*') {
            buf[j]=' ';
         } else {
            if (buf[j] < '1') return 1; /* erreur */
            if (buf[j] > '9') return 1; /* erreur */
         }
         /* MAJ de la matrice */
         AdM[(i*NBC)+j]=buf[j];
      }
      /* on passe a la ligne suivante */
      i++;
      if (i==NBC) break;
   }
   fclose(fdec);
   return 0;
}

/* Les fonctions utiles */

/* changement de reperes */

int noSousMat(int L, int C)
{
int iC, iL, N;
   iC=(C-1)/3;
   iL=(L-1)/3;
   N = (iL*3) + iC + 1;
   return N;
}

/* les fonctions de detection de presence */

int estPresentCol(int V, int N)
/* retourne 0 si absent ou 1 si trouve */
{
int i;
  for (i=0; i<NBC; i++)
      if (AdM[(i*NBC)+N-1] == '0'+V) return 1;
  return 0;
}

int estPresentLig(int V, int N)
/* retourne 0 si absent ou 1 si trouve */
{
int i;
  for (i=0; i<NBC; i++)
      if (AdM[((N-1)*NBC)+i] == '0'+V) return 1;
  return 0;
}

int estPresentSMat(int V, int N)
/* retourne 0 si absent ou 1 si trouve */
{
int cd, cf, ld, lf, /* col debut, col fin, ligne debut, ligne fin */
    i, j;           /* les indices de balayage */
   /* mise en place des limites */
   switch(N) {
      case 1 :
         ld=0; lf=3;
         cd=0; cf=3;
         break;
      case 2 :
         ld=0; lf=3;
         cd=3; cf=6;
         break;
      case 3 :
         ld=0; lf=3;
         cd=6; cf=9;
         break;
      case 4 :
         ld=3; lf=6;
         cd=0; cf=3;
         break;
      case 5 :
         ld=3; lf=6;
         cd=3; cf=6;
         break;
      case 6 :
         ld=3; lf=6;
         cd=6; cf=9;
         break;
      case 7 :
         ld=6; lf=9;
         cd=0; cf=3;
         break;
      case 8 :
         ld=6; lf=9;
         cd=3; cf=6;
         break;
      case 9 :
         ld=6; lf=9;
         cd=6; cf=9;
         break;
   }
   for (i=cd; i<cf; i++)
       for (j=ld; j<lf; j++)
           if (AdM[(j*9)+i] == '0'+V) return 1;
   return 0;
}

int estVide(int L, int C)
{
  if (AdM[(L*9)+C] == ' ') return 1;
  return 0;
}

/* les fonctions d'evaluation du nb de cas possibles */
int nbCasPossCol(int V, int N, int * T)
/*
  V la valeur a remplir
  N le no de la colonne
  T est le tableau des indices possibles
*************************************** */
{
int i, Nb=0, sm;
   /* on se balaye toutes les lignes de la colonne */
   for (i=0; i<NBC; i++) {
      if (! estVide(i,N-1)) continue;
#ifdef TRACE
      printf("    nbCasPossCol : la case de L=%d est vide !\n",i+1);
#endif
      if (estPresentLig(V,i+1)) continue;
#ifdef TRACE
      printf("    nbCasPossCol : %d n'est pas present dans la ligne %d !\n",V,i+1);
#endif
      sm = noSousMat(i+1,N);
      if (estPresentSMat(V,sm)) continue;
#ifdef TRACE
      printf("    nbCasPossCol : %d est absent de la SMat %d !\n",V,sm);
#endif
      T[Nb]=i;
      Nb++;
   }
   return Nb;
}

/* les fonctions de remplissage */
void rempliMat(int L, int C, int V)
{
 AdM[((L-1)*9)+C-1] = '0' + V;
}

int resolution(void) /* fct qui resout la grille */
{
int V,N, Ok, n, T[NBC];
   while(1) {
     for (V=1; V<NBC+1; V++) {/*on prend les valeurs les unes apres les autres*/
       Ok=0;
       for (N=1; N<NBC+1; N++) { /* on essaie de traiter les colonnes */
#ifdef TRACE
          printf("Resolution: V=%d N=%d\n", V, N);
#endif
          if (estPresentCol(V,N)) continue;
#ifdef TRACE
          printf("   %d absent de la colonne %d\n", V, N);
#endif
          /* on cherche si il est possible de mettre V dans la colonne N */
          n = nbCasPossCol(V, N, T);
#ifdef TRACE
          printf("   Pour V=%d il y a %d cas dans la colonne %d\n", V, n, N);
#endif
          if (n==0) finerr("Pas de cas possible alors que absent");
          if (n==1) { /* on est certain que la valeur est bonne */
             rempliMat(*T+1,N,V);
#ifdef TRACE
          printf("   On met V=%d dans L=%d et C=%d\n", V, *T+1, N);
#endif

             Ok=1; break; /* signifie que l'on a trouve ! */ 
          }
       }
       /* continuOuPas("Passage a la valeur suivante"); */
       if (Ok) break;
     }
     /* est ce que l'on repart ou pas ? */
     if (Ok==0) break; /* on ne peut plus rien remplir avec cet algo ! */
   }

}

int main(int N, char *P[])
{
FILE * fd;
int i, j;
  printf("Sudo version %s\n",Version);
  /* verification que le nom du fichier a bien été passé */
  if (N != 2) {
     fprintf(stderr,"Utilisation : %s nom_du_fchier_matrice !\n",P[0]);
     return 1;
  }
  if ((fd = fopen(P[1],"r")) == NULL) {
     perror(P[1]); return 2;
  }

  init_mat();
  if (charge_mat(fd)) {
     fprintf(stderr,"Erreur lors de la lecture du fichier %s !\n",P[1]);
     return 3;
  }
  printf("La matrice est initialisee !\n");
  chaine_init();             /* initialisation du chainage */
  chaine_ajout((void*)MAT);  /* ajout dans le chainage de la matrice de depart */
  affiche_mat();
#ifdef TRACE
  printf("Verification des no de sous-matrice :\n");
  for (i=1;i<=NBC;i++) {
      for (j=1;j<=NBC;j++) printf("%d",noSousMat(i,j));
      printf("\n");
  }
  printf("=========\n");
#endif

  resolution();
  affiche_mat();
  /* pour tester la duplication */
  AdM = new_matrice();
  printf("Affichage de la copie de la matrice\n");
  affiche_mat();

  return 0;
}

