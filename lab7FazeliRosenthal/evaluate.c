/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       evaluate.c
 *
 *  purpose:    evaluate the current population by
 *              calling the user-defined function "eval"
 *
 *  modified:   13 feb 86
 *              12 nov 86: pass Length to Unpack()
 *              15 sep 90: handle floating pt representation,
 *                         change args to eval
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
void Evaluate () {
  register double performance;
  register int i;

  Trace("Evaluate entered");

  for (i=0; i<Popsize; i++) {
    if ( New[i].Needs_evaluation ) {
      Unpack(New[i].Gene, Bitstring, Length);
      if (Floatflag)
	FloatRep(Bitstring, Vector, Genes);
      New[i].Perf = eval (Bitstring, Length,
			  Vector, Genes);
      performance = New[i].Perf;
      New[i].Needs_evaluation = 0;
      Trials++;
      Spin = 0;   /* we're making progress */
      if (Savesize)  Savebest(i);

      if (Trials == 1)
	Best = performance;
      if (BETTER(performance, Best)) {
	Best = performance;
      }

      Onsum += performance;
      Offsum += Best;
      if (Dumpflag) Checkpoint(Ckptfile);
    }
  }

  Trace("Evaluate completed");

} /* end Evaluate */

/** end of file **/
