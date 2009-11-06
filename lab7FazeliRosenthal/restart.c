/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       restart.c
 *
 *  purpose:    restart an interupted GA run.
 *
 *  modified:   13 feb 86
 *
 *              12 nov 86:  pass Length to Pack()
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
void Restart () {
  FILE *fp;
  int i;
  char msg[40];

  Trace("Restart entered");

  fp = fopen(Ckptfile, "r");
  if (fp == NULL) {
    sprintf(msg,"Restart: Ckptfile %s not found",
	    Ckptfile);
    Error(msg);
  }

  fscanf(fp, "Experiment %d ", &Experiment);
  fscanf(fp, "Totonline %lf ", &Totonline);
  fscanf(fp, "Totoffline %lf ", &Totoffline);
  fscanf(fp, "Gen %d ",  &Gen);
  fscanf(fp, "Onsum  %lf ", &Onsum);
  fscanf(fp, "Offsum %lf ", &Offsum);
  fscanf(fp, "Trials %d ", &Trials);
  fscanf(fp, "Plateau %d ", &Plateau);
  fscanf(fp, "Best  %lf ", &Best);
  fscanf(fp, "Worst  %lf ", &Worst);
  fscanf(fp, "Spin %d ", &Spin);
  fscanf(fp, "Curr_dump %d ", &Curr_dump);
  fscanf(fp, "Mu_next %d ", &Mu_next);
  fscanf(fp," Random Seed %lu ", &Seed);
  fscanf(fp, " Initialization Seed %ui ", &Initseed);

  fscanf(fp, " Window ");
  for (i=0; i<Windowsize; i++) fscanf(fp, "%lf", &Window[i]);

  for (i=0; i<Popsize; i++) {
    fscanf(fp, "%s", Bitstring);
    fscanf(fp, " %lf ", &New[i].Perf);
    Pack(Bitstring, New[i].Gene, Length);
    fscanf(fp, " %d ", &New[i].Needs_evaluation);
  }
  fclose(fp);

  if (Totalexperiments > 1)
    sprintf(Bestfile, "%s.%d", Minfile, Experiment+1);

  Readbest();

  Trace("Restart completed");

} /* end Restart */

/**** end of file ****/

