/* sudoku.c : main part of the sudoku program */
#include <stdio.h>
#include <stdlib.h>

#include "sudoku.h"
#include "mat_string.h"


//#define TRACE_OPTINSUBSET
//#define TRACE_DUPLIMAT



static char MAT[NBC*NBC];  /* our 9x9 matrix */
static char * AdM = MAT; /* current matrix adress */
static char subset_table[3][7]={"row","column","submat"};
static int sm_array[9][4]={ /* limits of submats :  */
		/* fr first row, lr last row, fc first column, lc last column  */
		{0,3,0,3},{0,3,3,6},{0,3,6,9}, /* upper band */
		{3,6,0,3},{3,6,3,6},{3,6,6,9},
		{6,9,0,3},{6,9,3,6},{6,9,6,9}  /* lower band */
};
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
   fc=sm_array[N-1][2];
   lc=sm_array[N-1][3];
   fr=sm_array[N-1][0];
   lr=sm_array[N-1][1];

   for (i=fc; i<lc; i++)
       for (j=fr; j<lr; j++)
           if (AdM[(j*NBC)+i] == '0'+V) return 1;
   return 0;
}

int is_in_subset(int S, int N, int V)
{
	if (S==1) return is_in_row(V,N);
	else if (S==2) return is_in_col(V,N);
	else if (S==3) return is_in_submat(V,N);
}

int is_empty(int S, int N, int i)
{
int r,c;
	if (S==1){
		if (AdM[((N-1)*NBC)+i] == ' ') return 1;
	}
	else if (S==2){
		if (AdM[(i*NBC)+N-1] == ' ') return 1;
	}
	else if (S==3){
		r=sm_array[N-1][0]+i/3;
		c=sm_array[N-1][2]+i%3;
		if (AdM[r*NBC+c] == ' ') return 1;
	}
	return 0;
}

/* Function evaluating the number of possible cases */
int options_in_subset(int S, int N, int * T, int V)
/*
  S subset
  N subset number
  T table of possible entries
  V value to fill

*************************************** */
{
int i,R,C, Nb=0, sm;
   /* parsing all the subset position for that subset number */
   /* i.e. "for row subset (S==1) number N==3, search through */
   /* all columns and sub-matrices" */
   for (i=0; i<NBC; i++) {
	   	if (! is_empty(S,N,i)) continue;

		if (S==1){
			if (is_in_col(V,i+1)) continue;
			sm = submat_nb(N,i+1);
			if (is_in_submat(V,sm)) continue;
		}
		else if (S==2){
			if (is_in_row(V,i+1)) continue;
			sm = submat_nb(i+1,N);
			if (is_in_submat(V,sm)) continue;
		}
		else if (S==3){
			R=sm_array[N-1][0]+i/3+1;
			C=sm_array[N-1][2]+i%3+1;
			if (is_in_row(V,R)) continue;
			if (is_in_col(V,C)) continue;
		}
      T[Nb]=i;
      Nb++;
   }
   return Nb;
}

void fill_mat(int S, int N, int T, int V) /* filling function */
{
 if (S==1) {AdM[((N-1)*9)+T-1] = '0' + V;}
 if (S==2) {AdM[((T-1)*9)+N-1] = '0' + V;}
 if (S==3) {
	 AdM[(sm_array[N-1][0]+(T-1)/3)*NBC+(sm_array[N-1][2]+(T-1)%3)] = '0' + V;
 }
}

int solve(void) /* function to solve the grid */
{
int S,N,V, Ok, n, T[NBC];
int gcnt = 0;
char * NewM;
   while(1) {
     for (V=1; V<NBC+1; V++) { /* iterating values to fill, from 1 to 9 */
       Ok=0;
       for (S=1; S<4; S++) { /* loop for different subsets : row, column, number */
		   for (N=1; N<NBC+1; N++) { /* parsing columns */
#ifdef TRACE_OPTINSUBSET
          printf("Investigating: V=%d %s N=%d\n", V, subset_table[S-1], N);
#endif
			  if (is_in_subset(S,N,V)) continue;
#ifdef TRACE_OPTINSUBSET
          printf(" Value %d missing in %s %d\n", V, subset_table[S-1], N);
#endif
			  /* checking if value V can be put in subset number N */
			  n = options_in_subset(S, N, T, V);
#ifdef TRACE_OPTINSUBSET
          printf("  There are %d options in %s %d\n", n, subset_table[S-1], N);
#endif
			  if (n==0) previous_mat(); /* exploring this matrix has failed, back */
			  if (n==1) { /* we are sure this value goes here */
				 fill_mat(S,N,*T+1,V);
#ifdef TRACE_OPTINSUBSET
          printf("   We put value V=%d in %s %d position %d\n", V, subset_table[S-1], N, *T+1);
          display_mat();
#endif
				 Ok=1; break; /* means we filled one cell ! */
			  }
		   }
		   /* carry_on_test("Moving to next value"); */
		   if (Ok) break;
       }
       if (Ok) break;
     }
     /* do we go on the guess step? */
     if (Ok==0) { /* filling cases with 100% assurance failed : starting guess loop */
        for (V=1; V<NBC+1; V++) { /* parsing values */
            for (S=1; S<4; S++) {
			  for (N=1; N<NBC+1; N++) { /* parsing columns */
				 if (is_in_subset(S,N,V)) continue;
				 /* checking if value V can be put in column N */
				 n = options_in_subset(S, N, T, V);
				 if (n==0) previous_mat(); /* exploring this matrix has failed, back */
				 if (n==2) { /* using 50% probability guess */
					NewM = new_mat(); /* creating new matrix */
					fill_mat(S,N,*T+1,V); /* Filling first matrix with first value position */
#ifdef TRACE_DUPLIMAT
                printf("Created new matrix. Displaying first matrix with first option:\n");
                display_mat();
#endif
					add_string((void*)NewM);
					AdM = NewM;
					gcnt++;
					fill_mat(S,N,T[1]+1,V); /*filling second matrix with second value position*/
#ifdef TRACE_DUPLIMAT
                printf("Displaying second matrix with second option. :\n");
                display_mat();
#endif
					Ok=1; break; /* this means we branched, returning to the beginning of the loop */
				 }
			  }
			  if (Ok) break;
           }
           if (Ok) break;
        }
     }
     if (Ok==0) {
    	 printf("This algorithm did its best ! :\n");
    	 display_mat();
    	 printf("%d 50%% proba guesses were made.\n", gcnt);
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
#ifdef TRACE_OPTINSUBSET
  printf("Verifiying block numbers in the grid :\n");
  for (i=1;i<=NBC;i++) {
      for (j=1;j<=NBC;j++) printf("%d",submat_nb(i,j));
      printf("\n");
  }
  printf("=========\n");
#endif

  solve();

  return 0;
}
