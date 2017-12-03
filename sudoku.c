/* sudo.c : main part of the sudoku program */
#include <stdio.h>
#include <stdlib.h>

#include "sudoku.h"
#include "mat_string.h"


//#define TRACE_NBPOSSCOL
//#define TRACE_DUPLIMAT



static char MAT[NBC*NBC];  /* our 9x9 matrix */

static char * AdM = MAT; /* current matrix adress */

/* function creating a new matrix from the current one */
char * new_mat(void)
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

void finerr(char *m) /* error function */
{
   fprintf(stderr,"ERROR : %s !!\n",m);
   exit(1);
}

void previous_mat(void)
{
   /* we delete the last 9*9 matrix of the string */
   if (del_string((void*)AdM) == 0) finerr("Missing matrix !?");
   free((void*)AdM);  /* physically deleting matrix */
   AdM = (char*)last_addr_string();
   return;
}

void carry_on_test(char * M)
{
char C, Ok=0;
   printf("\n%s ? :",M);
   C = (char)getchar();
   if (C == 'O') Ok=1;
   while (C != '\n') C = (char)getchar();
   if (Ok) return;
   finerr("Stopping on user request.");
}

/* global variables */
#define LBUF 512
char buf[LBUF]; /* here is a nice buffer */

void init_mat(void)
{
int i;
   for (i=0;i<(NBC*NBC);i++) AdM[i] = ' '; /* filling all with spaces */
   return;
}

void display_mat(void)
{
int i,j;
   printf("## Content of the grid ##\n");
   printf("/---------\\\n");
   for (i=0;i<NBC;i++) {
       printf("|");
       for (j=0;j<NBC;j++) printf("%c",AdM[(i*NBC)+j]); /* displaying each cell */
       printf("|\n");
   }
   printf("/---------\\\n");
   printf("###########################\n");
   return;
}

int load_mat(FILE * fdec)
{
int i=0,j;
   while(fgets(buf, LBUF, fdec) != NULL) {
      /* ignoring comments */
      if (buf[0] == '#') continue;
      /* we fill row nb i (from 0 to 8) */
      for (j=0; j<NBC; j++) {
         if(buf[j] == '\n') break;
         if(buf[j] == '\0') break;
         if (buf[j] == '*') {
            buf[j]=' ';
         } else {
            if (buf[j] < '1') return 1; /* error */
            if (buf[j] > '9') return 1; /* error */
         }
         /* Update matrix */
         AdM[(i*NBC)+j]=buf[j];
      }
      /* next row */
      i++;
      if (i==NBC) break;
   }
   fclose(fdec);
   return 0;
}

/* Useful functions */

int submat_nb(int L, int C)
{
int iC, iL, N;
   iC=(C-1)/3;
   iL=(L-1)/3;
   N = (iL*3) + iC + 1;
   return N;
}

/* Functions detecting presence */

int is_in_col(int V, int N)
/* return 0 if absent or 1 if found */
{
int i;
  for (i=0; i<NBC; i++)
      if (AdM[(i*NBC)+N-1] == '0'+V) return 1;
  return 0;
}

int is_in_row(int V, int N)
/* return 0 if absent or 1 if found */
{
int i;
  for (i=0; i<NBC; i++)
      if (AdM[((N-1)*NBC)+i] == '0'+V) return 1;
  return 0;
}

int is_in_submat(int V, int N)
/* return 0 if absent or 1 if found */
{
int fc, lc, fr, lr, /* first col, last col, first row, last row */
    i, j;           /* parsing indexes */
   /* setting up limits */
   switch(N) {
      case 1 :
         fr=0; lr=3;
         fc=0; lc=3;
         break;
      case 2 :
         fr=0; lr=3;
         fc=3; lc=6;
         break;
      case 3 :
         fr=0; lr=3;
         fc=6; lc=9;
         break;
      case 4 :
         fr=3; lr=6;
         fc=0; lc=3;
         break;
      case 5 :
         fr=3; lr=6;
         fc=3; lc=6;
         break;
      case 6 :
         fr=3; lr=6;
         fc=6; lc=9;
         break;
      case 7 :
         fr=6; lr=9;
         fc=0; lc=3;
         break;
      case 8 :
         fr=6; lr=9;
         fc=3; lc=6;
         break;
      case 9 :
         fr=6; lr=9;
         fc=6; lc=9;
         break;
   }
   for (i=fc; i<lc; i++)
       for (j=fr; j<lr; j++)
           if (AdM[(j*9)+i] == '0'+V) return 1;
   return 0;
}

int is_empty(int L, int C)
{
  if (AdM[(L*9)+C] == ' ') return 1;
  return 0;
}

/* Function evaluating the number of possible cases */
int nb_poss_col(int V, int N, int * T)
/*
  V value to fill
  N column number
  T table of possible entries
*************************************** */
{
int i, Nb=0, sm;
   /* parsing all the rows for that column */
   for (i=0; i<NBC; i++) {
      if (! is_empty(i,N-1)) continue;
#ifdef TRACE_NBPOSSCOL
      printf("    NBPOSSCOL : Cell R=%d is empty !\n",i+1);
#endif
      if (is_in_row(V,i+1)) continue;
#ifdef TRACE_NBPOSSCOL
      printf("    NBPOSSCOL : %d is not in row %d !\n",V,i+1);
#endif
      sm = submat_nb(i+1,N);
      if (is_in_submat(V,sm)) continue;
#ifdef TRACE_NBPOSSCOL
      printf("    NBPOSSCOL : %d is not in submatrix %d !\n",V,sm);
#endif
      T[Nb]=i;
      Nb++;
   }
   return Nb;
}

void fill_mat(int L, int C, int V) /* filling function */
{
 AdM[((L-1)*9)+C-1] = '0' + V;
}

int solve(void) /* function to solve the grid */
{
int V,N, Ok, n, T[NBC];
char * NewM;
   while(1) {
     for (V=1; V<NBC+1; V++) {/* iterating values to fill, from 1 to 9 */
       Ok=0;
       for (N=1; N<NBC+1; N++) { /* parsing columns */
#ifdef TRACE_NBPOSSCOL
          printf("Investigating: V=%d N=%d\n", V, N);
#endif
          if (is_in_col(V,N)) continue;
#ifdef TRACE_NBPOSSCOL
          printf("   %d missing in column %d\n", V, N);
#endif
          /* checking if value V can be put in column N */
          n = nb_poss_col(V, N, T);
#ifdef TRACE_NBPOSSCOL
          printf("   For value V=%d, there are %d options in column %d\n", V, n, N);
#endif
          if (n==0) previous_mat(); /* exploring this matrix has failed, back */
          if (n==1) { /* we are sure this value goes here */
             fill_mat(*T+1,N,V);
#ifdef TRACE_NBPOSSCOL
          printf("   We put value V=%d in row R=%d and column C=%d\n", V, *T+1, N);
          display_mat();
#endif
             Ok=1; break; /* means we filled one cell ! */
          }
       }
       /* carry_on_test("Moving to next value"); */
       if (Ok) break;
     }
     /* do we go on the guess step? */
     if (Ok==0) { /* filling cases with 100% assurance failed : starting guess loop */
        for (V=1; V<NBC+1; V++) { /* parsing values */
          for (N=1; N<NBC+1; N++) { /* parsing columns */
             if (is_in_col(V,N)) continue;
             /* checking if value V can be put in column N */
             n = nb_poss_col(V, N, T);
             if (n==0) previous_mat(); /* exploring this matrix has failed, back */
             if (n==2) { /* using 50% probability guess */
                NewM = new_mat(); /* creating new matrix */
                fill_mat(*T+1,N,V); /* Filling second matrix with first value position */
#ifdef TRACE_DUPLIMAT
                printf("Created new matrix. Displaying first matrix with first option:\n");
                display_mat();
#endif
                add_string((void*)NewM);
                AdM = NewM;
                fill_mat(T[1]+1,N,V); /*filling second matrix with second value position*/
#ifdef TRACE_DUPLIMAT
                printf("Displaying second matrix with second option. :\n");
                display_mat();
#endif
                Ok=1; break; /* this means we branched, returning to the beginning of the loop */
             }
          }
        }
     }
     if (Ok==0) {
    	 printf("This algorithm did its best ! :\n");
    	 break;
     }
   }
}

int main(int N, char *P[])
{
FILE * fd;
int i, j;
char * s = malloc(snprintf(NULL, 0, "%s%s", "./dirmat/", P[1]));
  sprintf(s, "%s%s", "./dirmat/", P[1]);
  printf("Sudoku version %s\n",Version);
  /* verifying that the name of the file has been passed correctly */
  if (N != 2) {
     fprintf(stderr,"Use : %s matrix_file_name.txt !\n",P[0]);
     return 1;
  }
  if ((fd = fopen(s,"r")) == NULL) {
     perror(P[1]); return 2;
  }

  init_mat();
  if (load_mat(fd)) {
     fprintf(stderr,"Error reading file %s !\n",P[1]);
     return 3;
  }
  printf("Grid initialized !\n");
  init_string();
  add_string((void*)MAT);  /* add in string the matrix */
  display_mat();
#ifdef TRACE_NBPOSSCOL
  printf("Verifiying block numbers in the grid :\n");
  for (i=1;i<=NBC;i++) {
      for (j=1;j<=NBC;j++) printf("%d",submat_nb(i,j));
      printf("\n");
  }
  printf("=========\n");
#endif

  solve();
  display_mat();

  return 0;
}
