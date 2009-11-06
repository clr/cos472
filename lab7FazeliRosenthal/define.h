/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:	define.h
 *
 *  purpose:	global definitions for genesis
 */
 
/************************ INCLUDE FILES ***********************/
#ifndef DEFINE_H
#define DEFINE_H

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "format.h"
#include <time.h>        /* time and ctime */

/********************** CONSTANTS ****************************/

#define CHARSIZE 8

/* used in random number generator below */
#define MASK 2147483647
#define PRIME 65539
#define SCALE 0.4656612875e-9


/************************ TYPES ******************************/

/* each member of the population has this form */
typedef struct {
	char *Gene;
	double Perf;
	int Needs_evaluation;
} STRUCTURE;

/* the best structures are saved in the following record */
typedef struct {
	char *Gene;
	double Perf;
	int Gen;
	int Trials;
} BESTSTRUCT;

/* records for interpreting bitstrings according to template file */
typedef struct {
	double min;
	double max;
	unsigned long values;
	char format[16];
	double incr;
	int bitlength;
} GENESTRUCT;

/************************ MACROS *****************************/

/* Comparison of two performance values */
#define BETTER(X,Y)	(Maxflag ? (X) > (Y) : (X) < (Y))

/* An allele has converged if all but a FEW */
/* structures have the same value at that position. */
#define	FEW		(Popsize/20)


/* print a debugging message		*/
#define Trace(s)  if (Traceflag) \
		{ printf(s); printf("\n"); fflush(stdout);}


/****************************************************************/
/*	 Rand computes a psuedo-random				*/
/*	 double value between 0 and 1, excluding 1.  Randint	*/
/*	 gives an integer value between low and high inclusive.	*/
/****************************************************************/

#define Rand()  (( Seed = ( (Seed * PRIME) & MASK) ) * SCALE )

#define Randint(low,high) ( (int) (low + (high-low+1) * Rand()))


/********************** FUNCTIONS ****************************/
/* fitness function */
double eval(); 

/* from best.c */
void Savebest(register int i);
void Printbest();
void Readbest();

/* from checkpnt.c */
void Checkpoint(char *ckptfile);

/* from convert.c */
void Pack(char *instring, char *outstring, int length);
void Unpack(char *instring, char *outstring, int length);
void FloatRep(char *instring, double *vect, int length);
void StringRep(double *vect, char *outstring, int length);

/* from cross.c */
void Crossover();

/* from display.c */
void Die();
void Dtrace(char *s);
void Interactive();

/* from done.c */
int Done();

/* from elitist.c */
void Elitist();

/* from error.c */
void Error(char *s);

/* from evaluate.c */
void Evaluate();

/* from generate.c */
void Generate();

/* from init.c */
void Initialize();

/* from input.c */
void Input(int argc, char *argv[]);

/* from measure.c */
void Measure();
void Converge();

/* from mutate.c */
void Mutate();

/* from restart.c */
void Restart();

/* from schema.c */
void Schema();

/* from select.c */
void Select();
void Gap(int *sample);


#endif  /* DEFINE_H */

/** end of file **/
