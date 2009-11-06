/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       setup.c
 *
 *  purpose:    create an input file for GENESIS.
 *              Default values for input parameters are provided
 *              when the user gives a null response to a prompt.
 *
 *  modified:   feb. 1986
 *              10 sep 90: handle floating point option
 */
/*****************************************************************/
#include "define.h"
#include <stdlib.h> /* for system function */

/*****************************************************************/
/* function prototypes */

int ilog2(unsigned long n);
void setpar(FILE *fp, char *prompt, char *defaultstring);
void getstring(char *s);

/*****************************************************************/
int main () {
  FILE *fp;
  int i, j;
  char s[40];
  char infile[40];
  char templatefile[40];
  char format[20];
  char cmd[80];
  int bitlength;
  int interpret;
  int genes;
  unsigned long values;
  unsigned long verify;
  double min;
  double max;
  int repetition;
  int ok;

  printf("File suffix []: ");
  getstring(s);
  if (strlen(s) == 0) {
    sprintf(infile, "in");
    sprintf(templatefile, "template");
  }
  else {
    sprintf(infile, "in.%s", s);
    sprintf(templatefile, "template.%s",s);
  }             

  printf("Floating point representation [y]: ");
  getstring(s);
  if (strlen(s) == 0 || strcmp(s, "y") == 0)
    interpret = 1;
  else
    interpret = 0;
                
  if (interpret) {
    bitlength = 0;
                
    /* get string interpretation */
    printf("number of genes: ");
    scanf("%d", &genes);
                
    fp = fopen(templatefile, "w");
    fprintf(fp, "genes: %d\n\n", genes);
    printf("\n");
                
    for (i=0; i<genes; ) {
      printf("gene %d\n", i);
      printf("min: ");
      scanf("%lf", &min);
      printf("max: ");
      scanf("%lf", &max);
      ok = 0;
      while (!ok) {
	printf("values (must be a power of 2): ");
	scanf("%lu", &values);
	verify = 1L << ilog2(values);
	ok = verify == values;
	if (!ok)
	  printf("bad choice for values\n");
      }
      printf("format string: ");
      scanf("%s", format);
      printf("repetition: ");
      scanf("%d", &repetition);
      printf("\n");

      for (j=0; j < repetition && i < genes; j++, i++) {
	fprintf(fp, "gene %d\n", i);
	fprintf(fp, "min: %g\n", min);
	fprintf(fp, "max: %g\n", max);
	fprintf(fp, "values: %lu\n", values);
	fprintf(fp, "format: %s\n", format);
	fprintf(fp, "\n");
	bitlength += ilog2(values);
      }
    }
    fclose(fp);

    /* kluge to get rid of left over LF */
    getchar();
  }

  if ((fp = fopen(infile, "w")) == NULL) {
    printf("can't open %s\n", infile);
    printf("Setup aborted.\n");
    exit(1);
  }

  setpar(fp, "Experiments", "1");
  setpar(fp, "Total Trials", "1000");
  setpar(fp, "Population Size", "50");
  if (interpret)
    fprintf(fp, "%18s = %d\n", "Structure Length", bitlength);
  else
    setpar(fp, "Structure Length", "30");
  setpar(fp, "Crossover Rate", "0.6");
  setpar(fp, "Mutation Rate", "0.001");
  setpar(fp, "Generation Gap", "1.0");
  setpar(fp, "Scaling Window", "5");
  setpar(fp, "Report Interval", "100");
  setpar(fp, "Structures Saved", "10");
  setpar(fp, "Max Gens w/o Eval", "2");
  setpar(fp, "Dump Interval", "0");
  setpar(fp, "Dumps Saved", "0");
  if (interpret)
    setpar(fp, "Options", "cefgl");
  else
    setpar(fp, "Options", "cel");
  setpar(fp, "Random Seed", "123456789");
  setpar(fp, "Rank Min", "0.75");
  fclose(fp);

  sprintf(cmd, "cat %s", infile);
  system(cmd);

  printf("Setup Done\n");

  return (0);  /* to appease compiler */

} /* end main */


int ilog2 (unsigned long n) {
  int i;

  if (n <= 0) {
    printf("Help! values is %ld, must be positive!\n", n);
    abort();
  }
  
  i = 0;
  while ((int) (n & 1) == 0) {
    n >>= 1;
    i++;
  }
  return(i);
  
} /* end ilog2 */


void setpar (FILE *fp, char *prompt, char *defaultstring) {
  char s[80];
  printf("%s [%s]: ", prompt, defaultstring);
  getstring(s);
  if (strlen(s) == 0)
    strcpy(s, defaultstring);
  fprintf(fp, "%18s = %s\n", prompt, s);

} /* end setpar */


void getstring(char *s) {
  gets(s);
} /* end getstring */

/*** end of file ***/
