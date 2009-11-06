/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       input.c
 *
 *  purpose:    Set up filenames and read the input files, and
 *              initialize variables for this run.
 *
 *              See init.c for the initialization of variables for each
 *              experiment.
 *
 *  modified:   26 jun 86
 *              15 sep 90: read template file for floating point representation
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
/* function prototypes */
int ilog2(unsigned long n);
void setflag(char c);

/*****************************************************************/
void Input (int argc,char *argv[]) {
  int i;                /* loop control                         */
  char msg[40];         /* used when printing error message     */
  long clock;           /* current date                         */
  FILE *fp;

  /* set up file names */

  if (argc < 2) {
    strcpy(Infile,"in");   
    strcpy(Outfile,"out"); 
    strcpy(Ckptfile,"ckpt"); 
    strcpy(Minfile,"min"); 
    strcpy(Logfile, "log");
    strcpy(Initfile, "init");
    strcpy(Schemafile, "schema");
    strcpy(Templatefile, "template");
  }
  else {
    sprintf(Infile, "in.%s", argv[1]);
    sprintf(Outfile, "out.%s", argv[1]);
    sprintf(Ckptfile, "ckpt.%s", argv[1]);
    sprintf(Minfile, "min.%s", argv[1]);
    sprintf(Logfile, "log.%s", argv[1]);
    sprintf(Initfile, "init.%s", argv[1]);
    sprintf(Schemafile, "schema.%s", argv[1]);
    sprintf(Templatefile, "template.%s", argv[1]);
  }

  strcpy(Bestfile, Minfile);

  /* read in the parameters from the infile */

  if ((fp = fopen(Infile, "r")) == NULL) {
    sprintf(msg, "Input: can't open %s", Infile);
    Error(msg);
  }
  fscanf(fp,IN_FORMAT,IN_VARS);
  Seed = OrigSeed;
  fclose(fp);

  /* activate the Options */
  for (i=0; Options[i] != '\0'; i++)
    setflag(Options[i]);
  if (Displayflag)
    printf("Display inoperable in this version (CBC)\n");

  /* Bytes is the size of each packed chromosome */
  Bytes = Length / CHARSIZE;
  if (Length % CHARSIZE) Bytes++;

  /* read template file if used */
  if (Floatflag) {
    if ((fp = fopen(Templatefile, "r")) == NULL) {
      sprintf(msg, "Input: can't open %s", Templatefile);
      Error(msg);
    }
    fscanf(fp, "genes: %d ", &Genes);
    Gene = (GENESTRUCT *) calloc((unsigned) Genes,
				 sizeof(GENESTRUCT));
                        
    for (i=0; i<Genes; i++) {
      fscanf(fp, " gene %*d");
      fscanf(fp, " min: %lf", &Gene[i].min);
      fscanf(fp, " max: %lf", &Gene[i].max);
      fscanf(fp, " values: %lu", &Gene[i].values);
      fscanf(fp, " format: %s", Gene[i].format);
      Gene[i].bitlength = ilog2(Gene[i].values);
      Gene[i].incr = (Gene[i].max - Gene[i].min) / 
	(Gene[i].values - 1);
    }
    fclose(fp);
  }
  /* allocate storage for variable sized structures */

  /* used for floating representation of chromosomes */
  Vector = (double *) calloc((unsigned) Genes, sizeof(double));

  /* used for string representation of chromosomes */
  Bitstring = malloc((unsigned) (Length+1));
  Bitstring[Length] = '\0';

  if (Bitstring == NULL) {
    printf("input: Help!  Memory allocation failed for Bitstring\n");
    abort();
  }
  /* population arrays */
  Old = (STRUCTURE *) calloc((unsigned) Popsize, sizeof(STRUCTURE));
  New = (STRUCTURE *) calloc((unsigned) Popsize, sizeof(STRUCTURE));

  for (i=0; i<Popsize; i++) {
    Old[i].Gene = malloc((unsigned) Bytes);
    New[i].Gene = malloc((unsigned) Bytes);
  }

  /* used to compute moving value for Worst */
  if (Windowsize)
    Window = (double *) calloc((unsigned) Windowsize, sizeof(double));

  /* used to save best structures */
  if (Savesize)
    Bestset = (BESTSTRUCT *) calloc((unsigned) Savesize, sizeof(BESTSTRUCT));

  for (i=0; i<Savesize; i++)
    Bestset[i].Gene = malloc((unsigned) Bytes);

        
  /* echo Input params */
  if (Traceflag) printf(OUT_FORMAT, OUT_VARS);

  /* scratch the output file (unless this is a restart) */
  if (!Restartflag) {
    if ((fp = fopen(Outfile, "w")) == NULL) {
      sprintf(msg, "Input: can't open %s", Outfile);
      Error(msg);
    }
    fclose(fp);
  }

  /* log this activation */
  if (Restartflag) {
    if (Logflag) {
      if ((fp = fopen(Logfile, "a")) == NULL) {
	sprintf(msg,"Input: can't open %s", Logfile);
	Error(msg);
      }
      fprintf(fp, "%s Restarted ", argv[0]);
      time(&clock);
      fprintf(fp, "%s", ctime(&clock));
      fclose(fp);
    }
  }
  else {
    if (Logflag) {
      if ((fp = fopen(Logfile, "a")) == NULL) {
	sprintf(msg,"Input: can't open %s", Logfile);
	Error(msg);
      }
      fprintf(fp, "%s started   ", argv[0]);
      time(&clock);
      fprintf(fp, "%s", ctime(&clock));
      fclose(fp);
    }
  }
} /* end Input */


int ilog2 (unsigned long n) {
  register int i;

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


void setflag (char c) {
  switch (c) {
  case 'a' :
    Allflag = 1;
    break;
  case 'b' : 
    Bestflag = 1; 
    break;
  case 'c' : 
    Collectflag = 1; 
    Convflag = 1;
    break;
  case 'C' : 
    Collectflag = 1; 
    break;
  case 'd' : 
    Dumpflag = 1; 
    break;
  case 'D' : 
    printf("Display mode inoperable in this version (CBC)\n");
    break;
  case 'e' :
    Eliteflag = 1;
    break;
  case 'f' : 
    Floatflag = 1; 
    break;
  case 'g' :
    Grayflag = 1;
    break;
  case 'i' :
    Initflag = 1;
    break;
  case 'I' :
    printf("Interactive mode inoperable in this version (CBC)\n");
    break;
  case 'l' :
    Logflag = 1;
    break;
  case 'L' :
    Lastflag = 1;
    break;
  case 'M' :
    Maxflag = 1;
    break;
  case 'o' : 
    Onlnflag = 1; 
    break;
  case 'O' :
    Offlnflag = 1;
    break;
  case 'r' : 
    Restartflag = 1; 
    break;
  case 'R' : 
    Rankflag = 1; 
    break;
  case 's' : 
    Schemflag = 1; 
    break;
  case 't' : 
    Traceflag = 1; 
    break;
  }

  } /* end setflag */


/**** end of file ****/
