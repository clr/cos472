/*****************************************************************/
/*
 *  GENESIS  Copyright (c) 1986, 1990 by John J. Grefenstette
 *  This program may be freely copied for educational
 *  and research purposes.  All other rights reserved.
 *
 *  file:       convert.c
 *
 *  purpose:    functions that translate between various representations
 *
 *  modified:   16 apr 86
 *              15 sep 90: translations for floating point representation
 */
/*****************************************************************/
#include "extern.h"

/*****************************************************************/
/* local prototypes */
unsigned long int char2int(char *instring, int length);
void int2cchar(unsigned long int n, char *outstring, int length);
void gray(char *instring, char *outstring, register int length);
void degray(char *instring, char *outstring, register int length);

/*****************************************************************/
/* for masking a single bit from a bit string
   stores 1000000, 01000000, 00100000, etc. 
   Note: CHARSIZE must be 8 for the following to work
*/
static char BIT[CHARSIZE] ={ '\200', '\100', '\040', '\020',
                             '\010', '\004', '\002', '\001'};

/*****************************************************************/
/* int2char and char2int translate ints to strings and vice versa */

unsigned long int char2int (char *instring, int length) {
  register int i;    /* loop control                 */
  unsigned long n;   /* accumulator for return value */

  n = (unsigned long) 0;
  for (i=0; i<length; i++) {
    n <<= 1;
    n += (*instring++ - (int) '0');
  }
  return(n);
} /* end char2int */


/*****************************************************************/
void int2char (unsigned long int n, char *outstring, int length) {
  register int i;   /* loop control                 */

  for (i=length-1; i>=0; i--) {
    outstring[i] = '0' + (n & 1);
    n >>= 1;
  }
} /* end int2char */


/*****************************************************************/
/* Pack and Unpack translate between strings and (packed) bit arrays */

void Pack (char *instring, char *outstring, int length) {
  static int firstflag = 1 ;
  static int full;  /* number of fully used bytes in outstring       */
  static int slop;  /* number of bits used in outstring's last byte  */
  register int i,j; /* loop control                                  */

  if (firstflag) {
    full = length / CHARSIZE;
    slop = length % CHARSIZE;
    firstflag = 0;
  }

  for (i=0; i<full; i++, outstring++) {
    *outstring = '\0';
    for (j=0; j < CHARSIZE; j++)
      if (*instring++ == '1')  *outstring |= BIT[j];
  }
  if (slop) {
    *outstring = '\0';
    for (j=0; j < slop; j++)
      if (*instring++ == '1')  *outstring |= BIT[j];
  }

} /* end Pack */


void Unpack (char *instring, char *outstring, int length) {
  static int firstflag = 1 ;
  static int full;  /* number of fully used bytes in instring       */
  static int slop;  /* number of bits used in instring's last byte  */
  register int i,j; /* loop control                                 */

  if (firstflag) {
    full = length / CHARSIZE;
    slop = length % CHARSIZE;
    firstflag = 0;
  }

  for (i=0; i<full; i++, instring++) {
    for (j=0; j < CHARSIZE; j++)
      if (*instring & BIT[j])
        *outstring++ = '1';
      else
        *outstring++ = '0';
  }

  if (slop) {
    for (j=0; j < slop; j++)
      if (*instring & BIT[j])
        *outstring++ = '1';
      else
        *outstring++ = '0';
  }
  *outstring = '\0';

} /* end Unpack */


/* Translations between fixed point ints and reflected Gray code */

void gray (char *instring, char *outstring, register int length) {
  register int i;      /* loop counter */
  register char last;  /* last bit looked at */

  last = '0';
  for (i=0; i<length; i++) {
    outstring[i] = '0' + (instring[i] != last);
    last = instring[i];
  }

} /* end gray */


void degray (char *instring, char *outstring, register int length) {
  register int i;     /* loop counter */
  register int last;  /* last bit looked at */

  last = 0;
  for (i=0; i<length; i++) {
    if (instring[i] == '1')
      outstring[i] = '0' + (!last);
    else
      outstring[i] = '0' + last;
    last = outstring[i] - '0';
  }

} /* end degray */


/* Translations between string representation and floating point vectors */

void FloatRep (char *instring, double * vect, int length) {
  register int i;       /* loop control                         */
  unsigned long int n;  /* decoded int value                    */
  register int pos;     /* position to start decoding           */
  char tmpstring[80];   /* used for gray code interpretation    */

  pos = 0;
  for (i=0; i < length; i++) {
    if (Grayflag) {
      degray(&instring[pos], tmpstring, Gene[i].bitlength);
      n = char2int(tmpstring, Gene[i].bitlength);
    }
    else {
      n = char2int(&instring[pos], Gene[i].bitlength);
    }
    vect[i] = Gene[i].min + n*Gene[i].incr;
    pos += Gene[i].bitlength;
  }
} /* end FloatRep */


void StringRep (double *vect, char *outstring, int length) {
  register int i;       /* loop control                         */
  unsigned long int n;  /* index of vext[i] within legal range  */
  register int pos;     /* next position for filling outstring  */
  char tmpstring[80];   /* used for gray code translation       */
        
  pos = 0;
  for (i=0; i < length; i++) {
    /* convert floating value to an index */
    n = (int) ((vect[i] - Gene[i].min) / Gene[i].incr + 0.5);
    
    /* encode n in char string */
    if (Grayflag) {
      /* convert to Gray code */
      int2char(n, tmpstring, Gene[i].bitlength);
      gray( tmpstring, &outstring[pos], Gene[i].bitlength);
    }
    else {
      int2char(n, &outstring[pos], Gene[i].bitlength);
    }
    pos += Gene[i].bitlength;
  }
  outstring[pos] = '\0';

} /* end StringRep */

/*** end of file ***/

