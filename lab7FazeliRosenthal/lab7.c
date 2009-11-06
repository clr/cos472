/*****************************************************************************
 File:   lab7.c
 Author: Clare Bates Congdon
  Edited by: Adnan Fazeli and Casey Rosenthal

  Date:   Tue Oct 27 00:17:47 2009

 Description:
    Fitness function for Lab 7.  Not sure if we got this exactly right.
    We had to make the population size and number of runs very large to get
    a converging answer that completely matched the faux data.

******************************************************************************/

/*****************************************************************************/
/* include files */
#include <stdio.h>   /* standard input and output */
#include <stdlib.h>  /* standard library, including memory allocation */
#include <math.h>    /* math library */
#include <string.h>  /* strings */
#include "extern.h"  /* Genesis external definitions */

/*****************************************************************************/
/* defines -- no semicolons at the ends of the lines */
#define DATA_FILE "cancer.data"  /* hardwired into this program */

/*****************************************************************************
/* data structures */
typedef struct Example* ExampleP;  /* pointer to a example in the linked list */
struct Example {
  int idNum;         /* the ID number for this line */
  int *attributes;   /* the attributes for this line of data */
  int class;         /* 2=benign; 4=malignant */
  ExampleP next;     /* next line of data */
};


/*****************************************************************************/
/* function prototypes */
double eval(char *str, int length, double *vect, int genes);  /* Genesis */
void openFile(FILE **fileptr, char *filename, char *mode);
ExampleP newExample();      /* get a new Example struct */
ExampleP fauxData ();       /* populate it with fake data */
void *emalloc(long size);   /* memory allocation, plus checking */


/*****************************************************************************/
double eval(char *str, int length, double *vect, int genes) {

  static double bestSoFar; /* best odds ratio found so far by this function */
  double fitness, dontCare;          /* the fitness of this string */
  register int i;          /* loop counter */

  ExampleP data;  /* a hardwired piece of data */


  /* Things to do for each new run of the GA */
  //if (Experiment == 0) {
    data = fauxData();  /* just one for now, and faked instead of read from
                           file */
  //}

  /* Things to do for each new Experiment */
  if (Trials == 0) {
    bestSoFar = 0;
    printf("\n\n\n");  /* blank lines between experiments */
  }
        
  fitness = 0;
  dontCare = 0;

  /* compare each of the values */
  for( i=0; i<9; i++ ){
    if( data->attributes[ i ] == (int)vect[ i ] ){
      fitness++;
    } else if( (int)vect[ i ] > 10 ){
      dontCare++;
    }
  }
  /* if all not all were either match or don't cares, then set to -1 */
  if( ( fitness + dontCare ) < 9 ){
    fitness = -1;
  }


  if (fitness > bestSoFar) {
    bestSoFar = fitness;
        
    printf("New best found in generation %d; Fitness is %.2f (%d don't cares)\n", Gen, fitness, (int)dontCare);

    printf("Bits: ");
    for (i=0; i<length; i++) {         /* print bits one by one */
      printf("%c", (int) str[i]);
    }
    printf("\n");

    printf("Genes: ");
    for (i=0; i<genes; i++) {         /* print genes one by one */
      printf("%2d ", (int) vect[i]);
    }
    printf("\n\n");
  }
  
  return fitness;

} /* end eval */

/*****************************************************************************
 Function: openFile
 Inputs:   the file to open, the file name, mode to open the file
 Returns:  nothing
 Description: opens file in specified mode or issues error message
 *****************************************************************************/
void openFile(FILE **fileptr, char *filename, char *mode) {
  *fileptr = fopen(filename,mode);       /* open the file */

  if ( !(*fileptr)) {
    printf("ERROR: Failed to open file: %s with mode %s.  Exiting.\n\n",
           filename, mode);
    exit(0);
  }

} /* end openFile */

/*****************************************************************************
 Function: fauxData
 Inputs:   none
 Returns:  a new Example
 Description: 
   allocate memory for a new example and fill it with bogus values
 *****************************************************************************/
ExampleP fauxData () {
  ExampleP faux;  /* the faux data to return */

  faux = newExample(9); /* get a new Example with 9 attributes */

  /* first line of data from cancer.data file */
  faux->idNum = 1000025;
  faux->attributes[0] = 5;
  faux->attributes[1] = 1;
  faux->attributes[2] = 1;
  faux->attributes[3] = 1;
  faux->attributes[4] = 2;
  faux->attributes[5] = 1;
  faux->attributes[6] = 3;
  faux->attributes[7] = 1;
  faux->attributes[8] = 1;
  faux->class = 2;         /* 2 = benign; 4 = malignant */

  return faux;

} /* end openFile */


/*****************************************************************************
 Function: newExample
 Inputs:   integer number of attributes
 Returns:  a pointer to a new Example
 Description: allocates and initializes memory or fails with error message
 *****************************************************************************/
        
ExampleP newExample (int numAttrs) {
  int i;   /* loop counter */
  ExampleP example = (ExampleP) emalloc(sizeof(struct Example));
    
  /* initialize all fields of the new struct */
  example->idNum = 0;
  example->attributes = (int*) emalloc (sizeof(int) * (numAttrs));
  for (i=0; i<numAttrs; i++) {
    example->attributes[i]=0;
  }
  example->class = 0;
  example->next = NULL;
        
  return example;
}

/*****************************************************************************
 Function: emalloc
 Inputs:   the size of the block of memory needed
 Returns:  a pointer to the block of memory
 Description: calls malloc, but checks for failure
 *****************************************************************************/
void *emalloc(long size) {
  void *p = (void *) malloc(size);     /* request memory */

  if (p == NULL) {            /* quit if request failed */
    printf("Unable to intitialize...out of memory.");
    exit(0);
  }

  return p;

} /* end emalloc */

/************************************************ end of file ****/
