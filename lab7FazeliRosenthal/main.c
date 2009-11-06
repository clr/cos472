/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       main.c
 *
 *  purpose:    main program for genesis.
 *
 *  modified:   28 mar 86
 *              15 sep 90 - to use display routines
 */
/*****************************************************************/
#include "global.h"

/*****************************************************************/
int main(int argc, char *argv[]) {
  FILE *fp;
  long clock;
        
  /* see input.c for the use of command line args */
  Input(argc,argv);

  do {           /* one experiment */
    if (Traceflag) 
      printf("Experiment %d\n", Experiment); 

    do {         /* see generate.c for main GA loop */
      Generate();
    } while (!Doneflag);

    if (Traceflag)
      printf("Online %e   Offline %e    Best %e\n",
	     Online, Offline, Best);

    /* accumulate performance measurements */
    Totonline += Online;
    Totoffline += Offline;
    Totbest += Best;

    /* get ready for next experiment */
    Experiment++;
    Gen = 0;

  } while (Experiment < Totalexperiments);
        
  /* compute and print final performance measures */

  Totonline /= Totalexperiments;
  Totoffline /= Totalexperiments;
  Totbest /= Totalexperiments;
  if (Onlnflag)
    printf("%e\n", Totonline);
  if (Offlnflag)
    printf("%e\n", Totoffline);
  if (Bestflag)
    printf("%e\n", Totbest);
  if (Logflag) {
    fp = fopen(Logfile, "a");
    fprintf(fp, "Online %e    ", Totonline);
    fprintf(fp, "Offline %e   ", Totoffline);
    fprintf(fp, "Best %e\n", Totbest);
    time(&clock);
    fprintf(fp, "%s\n", ctime(&clock));
    fclose(fp);
  }

  return (0);  /* to appease compiler */

} /* end main */

/**** end of file ****/
