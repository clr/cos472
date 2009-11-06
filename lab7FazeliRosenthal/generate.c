/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       generate.c
 *
 *  purpose:    One generation consists of
 *                      (1) forming a new population of structures.
 *                      (2) evaluating the population.
 *                      (3) gathering performance statistics.
 *
 *  modified:   7 feb 86
 *
 *              2 dec 86: call Measure() before Done() so that
 *              we can quit upon convergence.  Measure() nows
 *              calls Converge().
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
void Generate () {
  static int rsflag = 1;        /* flag cleared after restart           */

  STRUCTURE *temp;              /* for swapping population pointers     */
  register int i;               /* for marking structures               */

  if (Traceflag)
    printf("                    Gen %d\n",Gen);
  Trace("Generate entered");

  /* create a new population */

  if (Restartflag && rsflag) {
    /* this is a restart so read checkpoint file */
    Restart();
    rsflag = 0;       /* disable local restart flag. */
    Converge();
  }

  else if (Gen == 0) {   /* this is a fresh experiment */
    Initialize();     /* form an initial population */
    Spin++;   
  }

  else {
    /* form a new population from */
    /* the old one via genetic operators */

    Select();
    Mutate();
    Crossover();
    if (Eliteflag)
      Elitist();
    if (Allflag)      /* mark structures for evaluation */
      for (i=0; i<Popsize; i++) New[i].Needs_evaluation = 1;
    Spin++;
  }

  /* evaluate the newly formed population */
  Evaluate();

  /* gather performance statistics */
  Measure();

  /* check termination condition for this experiment    */
  Doneflag = Done();

  /* checkpoint if appropriate */
  if (Num_dumps && Dump_freq && Gen % Dump_freq == 0) {
    if (Num_dumps > 1) {
      sprintf(Dumpfile, "dump.%d", Curr_dump);
      Curr_dump = (Curr_dump + 1) % Num_dumps;
      Checkpoint(Dumpfile);
    }
    Checkpoint(Ckptfile);
  }
  else {
    if (Doneflag) {
      if (Lastflag)
	Checkpoint(Ckptfile);
      else
	if (Savesize)
	  Printbest();
    }
  }

  /* swap pointers for next generation */
  temp = Old;
  Old = New;
  New = temp;

  /* update generation counter */
  Gen++;

  Trace("Generate completed");

} /* end Generate */

/**** end of file ****/
