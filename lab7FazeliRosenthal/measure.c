/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       measure.c
 *
 *  purpose:    calculate performance measures and append them
 *              to the output file.
 *
 *  modified:   26 mar 86
 *
 *              2 dec 86: call Converge() right before output,
 *              and fake remainder of output if Bias > 0.99
 *
 *              10 sep 90: print statistics in display mode,
 *              handle max or min problems.
 *     CBC: removed interactive routines
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
/* function prototypes */
double newWorst();

/*****************************************************************/
/* constants  */
#define DISPMEAS 4

/*****************************************************************/
/* for masking a single bit from a bit string
   stores 1000000, 01000000, 00100000, etc. 
   Note: CHARSIZE must be 8 for the following to work
*/
static char BIT[CHARSIZE] ={ '\200', '\100', '\040', '\020',
                             '\010', '\004', '\002', '\001'};

/*****************************************************************/
void Measure () {
  FILE *fp;
  register int i;
  register int w;
  register double performance;

  Trace("Measure entered");

  for (i=0; i<Popsize; i++) {
    /* update current statistics */
    performance = New[i].Perf;
    if (i>0) {
      Ave_current_perf += performance;
      if (BETTER(performance, Best_current_perf)) {
	Best_current_perf = performance;
	Best_guy = i;
      }
      if (BETTER(Worst_current_perf, performance))
	Worst_current_perf = performance;
    }
    else {
      Best_current_perf = performance;
      Worst_current_perf = performance;
      Ave_current_perf = performance;
      Best_guy = 0;
    }

  }

  Ave_current_perf /= Popsize;

  /* update Worst */
  if (Windowsize) {
    /* Worst = worst in last (Windowsize) generations */
    w = Gen % Windowsize;
    Window[w] = newWorst();
    Worst = Window[0];
    for (i=1; i < Windowsize; i++)
      if (BETTER(Worst, Window[i])) Worst = Window[i];
  }
  else
    if (BETTER(Worst, Worst_current_perf))
      Worst = newWorst();

  /* update overall performance measures */
  Online = Onsum / Trials;
  Offline = Offsum / Trials;

  if (Traceflag) {
    printf("     Gen %d     Trials %d\n",Gen,Trials);
    if (Onlnflag) printf("     Online %e\n", Online);
    if (Offlnflag) printf("     Offline %e\n", Offline);
  }

  if ( Interval && 
       Collectflag && 
       ((Trials >= Plateau) || Doneflag)) {
      /* add measures to the output file */
      Converge();
      fp = fopen(Outfile, "a");
      fprintf(fp,OUT_F2, OUT_V2);

      if (Bias > 0.99) {
	int temp;
	temp = Trials;
	while (temp < Totaltrials)
	  {
	    temp += Interval;
	    Gen += (Interval/Popsize)?(Interval/Popsize):1;
	    fprintf(fp,OUT_F2,OUT_V2);
	  }
	Spin = Maxspin;
      }

      fclose(fp);
      Plateau = (Trials/Interval)*Interval + Interval;
  }

  if (Logflag  && (Spin >= Maxspin)) {
    fp = fopen(Logfile, "a");
    fprintf(fp, "Experiment %1d  ", Experiment);
    fprintf(fp, "SPINNING at Gen %1d, ",Gen);
    fprintf(fp, "after %1d Trials\n", Trials);
    fclose(fp);
  }

  if (Schemflag)
    Schema();

  Trace("Measure completed");

} /* end Measure */


double newWorst() {
  double delta;
        
  /* return a value a little worse than Worst_current_perf */
        
  if (Maxflag)
    delta = 1.0e-4;
  else
    delta = -1.0e-4;

  if (Worst_current_perf == 0.0) return (-delta);

  if (Worst_current_perf > 0.0)
    return (Worst_current_perf*(1.0 - delta));

  return (Worst_current_perf*(1.0 + delta));

} /* end newWorst */


/* measure population convergence       */
void Converge () {
  register int i,j;     /* loop control                         */
  register int ones;    /* number of ones in a given position   */
  int focus;            /* index of current byte                */
  int bit;              /* index of current bit                 */
  FILE *fp;

  Trace("Converge entered");

  Bias = 0.0;
  Lost = Conv = 0;
  if (!Convflag) return;

  for (j = 0; j < Length; j++) {
    focus = j / CHARSIZE;
    bit = j % CHARSIZE;
    ones = 0;
    for (i=0; i < Popsize; i++)
      ones += ((New[i].Gene[focus] & BIT[bit]) != 0);
    Lost += (ones == 0) || (ones == Popsize);
    Conv += (ones <= FEW) || (ones >= Popsize - FEW);
    Bias += (ones > Popsize/2) ? ones : (Popsize - ones);
  }

  Bias /= (Popsize*Length);

  if (Logflag && (Lost==Length)) {
    fp = fopen(Logfile, "a");
    fprintf(fp, "CONVERGED at Gen %1d, ",Gen);
    fprintf(fp, "after %1d Trials\n", Trials);
    fclose(fp);
  }

  Trace("Converge completed");

} /* end Converge */


/**** end of file ****/

