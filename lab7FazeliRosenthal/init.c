/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       init.c
 *
 *  purpose:    create an initial population of structures,
 *              and initalize some performance variables.
 *              This is called at the start of each experiment.
 *
 *  modified:   7 feb 86
 *              12 nov 66:  pass Length to Pack()
 *              23 sep 90:  handle floating point representation in initfile
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
void Initialize () {
  FILE *fp;
  register int i, j;    /* loop control                         */
  int status;           /* indicates end of file in initfile    */

  Trace("Initialize entered");

  if (Totalexperiments > 1)
    sprintf(Bestfile, "%s.%d", Minfile, Experiment+1);

  /* prepare for new experiment */
  Doneflag = 0;
  Curr_dump = 0;
  Bestsize = 0;
  /* set next mutation location */
  if (M_rate < 1.0)
    Mu_next = ceil (log(Rand()) / log(1.0 - M_rate));
  else
    Mu_next = 1;

  Trials = Gen = 0;
  Lost = Conv = 0;
  Plateau = 0;
  Spin = 0;
  Onsum = Offsum = 0.0;
  for (i=0; i<Windowsize; i++) Window[i] = 0.0;

  /* set up initial population */

  i = 0;                        /* current structure */

  if (Initflag) {        /* get some structures from Initfile */
    if ((fp = fopen(Initfile, "r")) == NULL) {
      char msg[40];
      sprintf(msg, "Initialize: can't open %s", Initfile);
      Error(msg);
    }

    status = 1;
    if (Floatflag) {
      for (j = 0; j < Genes && status != EOF; j++) {
	status = fscanf(fp, "%lf", &Vector[j]);           
      }
    }
    else
      status = fscanf(fp, "%s", Bitstring);
    
    while (status != EOF && i < Popsize) {
      if (Floatflag)
	StringRep(Vector, Bitstring, Genes);
      
      Pack(Bitstring, New[i].Gene, Length);
      New[i].Needs_evaluation = 1;
      i++;
      
      /* get the next structure */
      if (Floatflag)
	for (j = 0; j < Genes && status != EOF; j++)
	  status = fscanf(fp, "%lf", &Vector[j]);           
      else
	status = fscanf(fp, "%s", Bitstring);
    }
    fclose(fp);
  }

  /********************************************************/
  /* The seed for the random number generator is saved  */
  /* after the initialization of the first population   */
  /* in each experiment.  The saved value is used as the        */
  /* Seed in subsequent experiments.  The reason is:    */
  /* often we will run several experiments with one set */
  /* of parameters, and compare the results with several        */
  /* experiments run with a different set of parameters.        */
  /* This means that for all runs which use the         */
  /* same population size, the initial populations for  */
  /* corresponding experiments will be the same.                */
  /********************************************************/
  if ( Experiment > 0 ) Seed = Initseed;

  for (; i < Popsize; i++) {  /* initialize remainder randomly */ 
    for (j = 0; j < Length; j++) {
      if (Randint(0,1))
	Bitstring[j] = '1';
      else
	Bitstring[j] = '0';
    }
    Pack(Bitstring , New[i].Gene , Length);
    New[i].Needs_evaluation = 1;
  }

  Initseed = Seed;
  
  Trace("Initialize completed");

} /* end Initialize */

/**** end of file ****/
