/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       checkpoint.c
 *
 *  purpose:    save global variables in a file for later restart
 *
 *  modified:   18 apr 86
 *              12 nov 86: pass Length to Unpack()
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
void Checkpoint (char *ckptfile) {
  FILE *fp; /* checkpoint file */
  int i,j;  /* loop counters */

  Trace("Checkpoint entered");

  fp = fopen(ckptfile, "w");
  fprintf(fp, "Experiment %d\n", Experiment);
  fprintf(fp, "Totonline %12.6e\n", Totonline);
  fprintf(fp, "Totoffline %12.6e\n", Totoffline);
  fprintf(fp, "Gen %d\n", Gen);
  fprintf(fp, "Onsum  %12.6e\n", Onsum);
  fprintf(fp, "Offsum %12.6e\n", Offsum);
  fprintf(fp, "Trials %d\n", Trials);
  fprintf(fp, "Plateau %d\n", Plateau);
  fprintf(fp, "Best %12.6e\n", Best);
  fprintf(fp, "Worst  %12.6e\n", Worst);
  fprintf(fp, "Spin %d\n", Spin);
  fprintf(fp, "Curr_dump %d\n", Curr_dump);
  fprintf(fp, "Mu_next %d\n", Mu_next);
  fprintf(fp, "Random Seed %lu\n", Seed);
  fprintf(fp, "Initialization Seed %i\n", Initseed);

  fprintf(fp,"\n");
  fprintf(fp, "Window\n");
  for (i=0; i<Windowsize; i++) fprintf(fp, "%12.6e\n", Window[i]);
  fprintf(fp,"\n");

  for (i=0; i<Popsize; i++) {
    Unpack(New[i].Gene, Bitstring, Length);
    fprintf(fp, "%s", Bitstring);
    fprintf(fp, " %12.8e ", New[i].Perf);
    fprintf(fp, "%1d", New[i].Needs_evaluation);
    fprintf(fp, "\n");
  }

  if (Floatflag) {      /* print floating point representation */
    fprintf(fp, "\n");
    for (i=0; i<Popsize; i++) {
      Unpack(New[i].Gene, Bitstring, Length);
      FloatRep(Bitstring, Vector, Genes);
      for (j=0; j < Genes; j++) {
	fprintf(fp, Gene[j].format, Vector[j]);
	fprintf(fp, " ");
      }
      fprintf(fp, " %10.4f\n", New[i].Perf);
    }
  }

  fclose(fp);

  /*  save the best structures */
  if (Savesize)
    Printbest();

  Trace("Checkpoint completed");

} /* end Checkpoint */

/** end of file **/
