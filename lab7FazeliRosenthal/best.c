/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       best.c
 *
 *  purpose:    input, maintenance and output of best structures
 *
 *  modified:   15 apr 86
 *              12 nov 86: pass Length to Pack() and Unpack()
 *              16 dec 86: don't print Bestsize to minfile in Printbest()
 *              15 sep 90: handle float representation in i/o
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
/* static variables */
static double worst_value;      /* worst value in the Bestset           */
static int worst;               /* pointer to element with worst_value  */

/*****************************************************************
 * Function: Savebest
 * Description:  
 *   Save the ith structure in current population
 *   if it is one of the Savesize best seen so far 
 *****************************************************************/
void Savebest (register int i) {

  register int j, k; /* loop counters */
  int found;         /* true if string is already found in best set*/

  if (Bestsize < Savesize) {
      /* Bestsize is the number saved so far, so          */
      /* there are still empty slots in the Bestset.      */
      /* Check if an identical structure is already there */
      for (j=0, found=0; j<Bestsize && (!found); j++)
        for (k=0, found=1; (k<Bytes) && (found); k++)
          found = (New[i].Gene[k] 
                   == Bestset[j].Gene[k]);
      if (found) return;

      /* insert ith structure */
      for (k=0; k<Bytes; k++) {
	Bestset[Bestsize].Gene[k] = New[i].Gene[k];
      }
      Bestset[Bestsize].Perf = New[i].Perf;
      Bestset[Bestsize].Gen = Gen;
      Bestset[Bestsize].Trials = Trials;
      Bestsize++;
      if (Bestsize == Savesize) {
	/* find worst element in Bestset */
	worst_value = Bestset[0].Perf;
	worst = 0;
	for (j=1; j<Savesize; j++) {
	  if (BETTER(worst_value,Bestset[j].Perf)) {
	    worst_value = Bestset[j].Perf;
	    worst = j;
	  }
	}
      }
  } /* end if */

  else {
    if (BETTER(New[i].Perf, worst_value)) {   /* then save New[i] */
      /* unless its already there */
      for (j=0, found=0; j<Bestsize && (!found); j++)
	for (k=0, found=1; (k<Bytes) && (found); k++)
	  found = (New[i].Gene[k] 
		   == Bestset[j].Gene[k]);
      if (found) return;

      /* overwrite the worst one */
      for (k=0; k<Bytes; k++) {
	Bestset[worst].Gene[k] = New[i].Gene[k];
      }
      Bestset[worst].Perf = New[i].Perf;
      Bestset[worst].Gen = Gen;
      Bestset[worst].Trials = Trials;
      /* find worst element in Bestset */
      worst_value = Bestset[0].Perf;
      worst = 0;
      for (j=1; j<Savesize; j++) {
	if (BETTER(worst_value, Bestset[j].Perf)) {
	  worst_value = Bestset[j].Perf;
	  worst = j;
	}
      }
    }
  } /* end else */

} /* end Savebest */


/*****************************************************************
 * Function: Printbest
 * Description:
 *   Write the Best structures out to the Bestfile.
 *****************************************************************/
void Printbest () {

  register int i, j;   /* loop counters */
  FILE *fp;            /* file with best structures */

  Trace("Printbest entered");

  fp = fopen(Bestfile, "w");
  /* fprintf(fp, "%d\n", Bestsize); */
  for (i=0; i<Bestsize; i++) {
    Unpack(Bestset[i].Gene, Bitstring, Length);
    if (Floatflag) {
      FloatRep(Bitstring, Vector, Genes);
      for (j=0; j<Genes; j++) {
	fprintf(fp, Gene[j].format, Vector[j]);
      }
    }
    else {
      for (j=0; j<Length; j++) {
	fprintf(fp, "%c", Bitstring[j]);
      }
    }

    fprintf(fp, "  %11.4e ", Bestset[i].Perf);
    fprintf(fp, " %4d  %4d\n", Bestset[i].Gen, Bestset[i].Trials);
  }

  fclose(fp);
  Trace("Printbest completed");

} /* end Printbest */


/*****************************************************************
 * Function: Readbest
 * Description:  
 *   Read the Best structures in from the Bestfile
 *   (during a Restart)
 *****************************************************************/

void Readbest () {
  int i;      /* loop counter */
  FILE *fp;   /* best file */
  int status; /* to check for EOF */

  if (Savesize) {
    Trace("Readbest entered");
    
    fp = fopen(Bestfile, "r");
    if (fp == NULL) {
      Bestsize = 0;
      return;
    }
                
    Bestsize = 0;
    status = 1;
    if (Floatflag) {
      for (i = 0; i < Genes && status != EOF; i++) {
	status = fscanf(fp, "%lf", &Vector[i]);           
      }
    }
    else
      status = fscanf(fp, "%s", Bitstring);

    while (status != EOF) {
      if (Floatflag)
	StringRep(Vector, Bitstring, Genes);
      
      Pack(Bitstring, Bestset[Bestsize].Gene, Length);

      fscanf(fp, "%lf ", &Bestset[Bestsize].Perf);
      fscanf(fp, "%d  %d", &Bestset[Bestsize].Gen,
	     &Bestset[Bestsize].Trials);
      Bestsize++;
        
      /* get the next structure */
      if (Floatflag)
	for (i = 0; i < Genes && status != EOF; i++)
	  status = fscanf(fp, "%lf", &Vector[i]);           
      else
	status = fscanf(fp, "%s", Bitstring);
    }
    fclose(fp);
    
    /* find worst element in Bestset */
    worst_value = Bestset[0].Perf;
    worst = 0;
    for (i=1; i<Bestsize; i++) {
      if (BETTER(worst_value, Bestset[i].Perf))	{
	worst_value = Bestset[i].Perf;
	worst = i;
      }
    }

    Trace("Readbest completed");
  }
} /* end Readbest */

/**** end of file ****/

