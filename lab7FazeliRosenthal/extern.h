
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:	extern.h
 *
 *  purpose:	external declarations for genesis.
 */

#include "define.h"

#include <stdlib.h>    /* CBC: memory management, exiting, etc. */

/* CBC char *calloc(); */
/* CBC char *malloc(); */


/*************************************** from parameters.h **/

/* The Input file specifies these parameters */

extern int    Totalexperiments;	/* number of experiments		*/
extern int    Totaltrials;	/* trials per experiment		*/
extern int    Popsize;		/* population size			*/
extern int    Length;		/* bit length of a structure		*/
extern double  C_rate;		/* crossover rate			*/
extern double  M_rate;		/* mutation rate			*/
extern double  Gapsize;		/* fraction of pop replaced per gen	*/
extern int    Windowsize;	/* used to update worst performance	*/
extern int    Interval;		/* trials between printing statistics	*/
extern int    Savesize;		/* number of structures in minfile	*/
extern int    Maxspin;		/* max gens without evals		*/
extern int    Dump_freq;	/* gens between checkpointing		*/
extern int    Num_dumps;	/* number of checkpoint files kept	*/
extern char   Options[];	/* option flags				*/
extern unsigned long Seed;	/* seed for random number generator	*/
extern unsigned long OrigSeed;	/* original value for random seed	*/



/********************************************  from global.h   ******/

/*	Global variables.	*/


/* File names */
extern char   Bestfile[];	/* file of best structures		*/
extern char   Ckptfile[];	/* check point file			*/
extern int    Curr_dump;	/* suffix of most recent dumpfile	*/
extern char   Dumpfile[];	/* current dumpfile (if more than one)	*/
extern char   Initfile[];	/* file of initial structures		*/
extern char   Infile[];		/* input file				*/
extern char   Logfile[];	/* logs starts and restarts		*/
extern char   Minfile[];	/* file prefix of bestfile		*/
extern char   Outfile[];	/* output file				*/
extern char   Schemafile[];	/* file for record a schema's history	*/
extern char   Templatefile[];	/* file describing gene interpretation	*/

extern char   *Bitstring;	/* string representation of chromosome	*/
extern double *Vector;		/* floating point representation	*/
extern int Genes;		/* number of interpreted genes		*/
extern GENESTRUCT *Gene;	/* pointer to interpretation records	*/
extern STRUCTURE  *Old;		/* pointer to population		*/
extern STRUCTURE  *New;		/* pointer to population		*/
extern BESTSTRUCT *Bestset;	/* set of best structures		*/

/* data collection and loop control variables */
extern double  Ave_current_perf;/* ave perf in current generation	*/
extern double  Best;		/* best performance seen so far		*/
extern double  Best_current_perf;/* best perf in current generation	*/
extern int    Best_guy;		/* index of best_current_perf		*/
extern int    Bestsize;		/* number of currently saved structures */
extern double  Bias;		/* ave. domination of alleles		*/
extern int    Bytes;		/* byte-length of packed structures	*/
extern int    Conv;		/* number of partially coverged genes	*/
extern char   Doneflag;		/* set when termination conditions hold	*/
extern int    Experiment;	/* experiment counter			*/
extern int    Gen;		/* generation counter			*/
extern unsigned int    Initseed; /* seed used to initialize population	*/
extern int    Lost;		/* number of totally coverged positions */
extern int    Mu_next;		/* next mutated position		*/
extern double  Offline;		/* offline performance			*/
extern double  Offsum;		/* accumulator for offline performance	*/
extern double  Online;		/* online performance			*/
extern double  Onsum;		/* accumulator for online performance	*/
extern int    Plateau;		/* trial counter for next output	*/
extern double Rank_min;		/* minimum sampling rate under ranking	*/
extern double  Totbest;		/* total for best			*/
extern double  Totoffline;	/* total for offline			*/
extern double  Totonline;	/* total for online			*/
extern int    Trials;		/* trial counter			*/
extern double  *Window;		/* circular queue of recent worsts	*/
extern double  Worst;		/* worst performance seen so far	*/
extern double  Worst_current_perf;/* worst perf in current generation	*/
extern int    	Spin;		/* number of gens since eval occurred	*/

/* flags set according to the Options string */
extern char   Allflag;		/*  evaluate all structures		*/
extern char   Bestflag;		/*  print final best value		*/
extern char   Collectflag; 	/*  collect performance data in outfile */
extern char   Convflag;		/*  collect convergence data in outfile */
extern char   Displayflag;	/*  display statistics each generation	*/
extern char   Dumpflag;		/*  dump after each evaluation		*/
extern char   Eliteflag;	/*  use elitist selection strategy	*/
extern char   Floatflag;	/*  convert strings to floating point	*/
extern char   Grayflag;		/*  use gray code 			*/
extern char   Initflag;		/*  read initial structures		*/
extern char   Interflag;	/*  interactive mode			*/
extern char   Lastflag;		/*  dump last generation		*/
extern char   Logflag;		/*  log starts and restarts		*/
extern char   Maxflag;		/*  maximize instead of minimize	*/
extern char   Offlnflag;	/*  print final offline measure		*/
extern char   Onlnflag;		/*  print final online measure		*/
extern char   Rankflag;		/*  used rank-based selection		*/
extern char   Restartflag;	/*  restart a run			*/
extern char   Schemflag;	/*  trace history of a schema		*/
extern char   Traceflag;	/*  trace execution			*/

/** end of file **/
