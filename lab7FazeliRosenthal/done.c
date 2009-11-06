/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       done.c
 *
 *  purpose:    test experiment-termination conditions.
 *
 *  modified:   7 feb 86
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************
 * Function: Done
 * Description:  
 *   check for a variety of termination conditions
 *****************************************************************/
int Done()
{
  if (Interflag)  /* user will indicate when to quit */
    return(0);

  return ((Trials >= Totaltrials)  || ( Lost >= Length)
          || (Spin >= Maxspin));
}

/**** end of file ****/
