/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       error.c
 *
 *  purpose:    print message in error log file and abort.
 *
 *  modified:   7 feb 86
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
void Error (char *s) {
  FILE *fp;
  long clock;

  fp = fopen("log.error", "a");
  fprintf(fp, "%s\n", s);
  time(&clock);
  fprintf(fp, "%s\n", ctime(&clock));
  fclose(fp);
  fprintf(stderr, "%s\n", s);
        
  exit(1);

} /* end Error */

/*** end of file ***/
