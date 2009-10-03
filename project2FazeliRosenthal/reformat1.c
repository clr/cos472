/*****************************************************************************
 Adnan Fazeli
 Casey Rosenthal

 File:   reformat.c

 Description:
 Read mushroom data from a file, reformat the data into a decision tree,
 and print this back out to a new file.

 Notes:
 run this program with the file mushrooms.data. You can pick the output file
 name. Some functions are from program P4.c.

 Data Structures:
 This program stores information about mushrooms in a linked list of
 structures. Each structure contains information about the Mushroom's number, 
 attribures, and a a pointer to the next structure.
 ******************************************************************************/

/*****************************************************************************/
/* include files */
#include <stdio.h>   /* standard input and output */
#include <stdlib.h>  /* standard library, including memory allocation */
#include <math.h>    /* math library */
#include <string.h>  /* string library */

/*****************************************************************************/
/* constants -- no semicolons at the ends of the lines */
#define STRLEN 100   /* length (in chars) for strings */

/*****************************************************************************/
/* global variables */
char InTrainfilename[STRLEN]; /* input file name for training data */
char InTestfilename[STRLEN]; /* input file name for testing data */
char Outfilename[STRLEN]; /* output file name */
/*****************************************************************************/

/*****************************************************************************
 data structures:
 *****************************************************************************/
typedef struct Mushroom * MushroomP; /* pointer to a Mushroom structure */
struct Mushroom {
	int lineNumber; /* line number of this mushroom */
	char* attributes; /* attributes of the mushroom */
	MushroomP next; /* pointer to the next mushroom */
};
typedef struct Value * ValueP;
struct Value {
  int attribute;    /* column number from file */
  char letter;    /* letter value from file */
  int count;     /* number of values for an attribute */
  int classCount;  /* number of shrooms with this value that are also edible */
  float entropy; /* calculated entropy when the list is found */
  ValueP next;   /* pointer to next node in the linked list */
};
typedef struct Decision * DecisionP; /* pointer to a Decision structure */
struct Decision {
  int isLeaf; /* boolean 1 for leaf node, 0 for not */
  int isClass; /* -1 for no, 0 for maybe, 1 for yes */
  int mushroomCount; /* keep track of how many pass through here */
  ValueP splitValue; /* pointer to the Value on which the decision splits */
  DecisionP left; /* pointer to positive matches */
  DecisionP right; /* pointer to negative matches */
};
/*****************************************************************************/
/* prototypes */
MushroomP readMushrooms( char infilename[STRLEN] );
MushroomP appendMushroom( MushroomP list, MushroomP newshroom );
MushroomP newMushroom( int attributeCount );

DecisionP createDecisionTree( char letter, int attribute, MushroomP shroomList, ValueP availableValues );
DecisionP createLeafNode( int isClass, int mushroomCount );
DecisionP createSplitNode( ValueP splitValue, char letter, int attribute, MushroomP positiveShrooms, MushroomP negativeShrooms, ValueP availableValues, int mushroomCount );
DecisionP newDecision( );
void printDecisionTree( DecisionP decisionTree );
void printDecision( FILE * outfile, DecisionP decisionTree, int tab );

ValueP findAvailableValues( char letter, int attribute, MushroomP shroomList );
ValueP newValue( char letter, int attribute );
ValueP appendValue( ValueP list, ValueP newvalue );
ValueP updateValue( ValueP list, char letter, int attribute, int isClass );
ValueP cloneValues( ValueP list );

float calculateEntropy( int classCount, int shroomCount );
ValueP findHighestGain( char letter, int attribute, ValueP valueList, MushroomP shroomList );
float logTwo( float value );

void *emalloc( long size );
void openFile( FILE **fileptr, char *filename, char *mode );

/*****************************************************************************
  We run the program by calling something along the lines of:
  guessMushroom -train mushrooms.data -test moreMushrooms.data -out mushrooms.out 
 ******************************************************************************/
int main(int argc, char* argv[]) {
  MushroomP shroomList;
  ValueP availableValues;
  DecisionP decisionTree;
  shroomList = NULL;
  availableValues = NULL;
  decisionTree = NULL;
  char infilenameTraining[STRLEN]; /* input file name for training data */

  printf( "This program processes data about the attributes mushrooms.\n" );
  
  /* initialize filenames to dummy strings */
  strcpy( infilenameTraining, "none" );
  strcpy( Outfilename, "none" );
  /* TAKEN FROM p5.c EXAMLPE
     process command-line arguments */
  while( *++argv ) {             /* while there are still command-line args */
    if ( !strcmp( *argv, "-train" ) )          /* set input file name */
      strcpy( infilenameTraining, *++argv );
    else if( !strcmp( *argv,"-test" ) )          /* set input file name */
      strcpy( InTestfilename,*++argv );
    else if( !strcmp( *argv,"-out" ) )     /* set output file name */
      strcpy( Outfilename,*++argv );
    else {  /*  error checking */
      printf( "\nERROR: option %s not recognized\n", *argv );
      printf( "...exiting program\n\n" );
      exit( 0 );
    }
  } /* end, while there are still command-line args */
 
  shroomList = readMushrooms( infilenameTraining );      /* read shroom information from file into linked list */

  availableValues = findAvailableValues( 'e', 0, shroomList );

  decisionTree = createDecisionTree( 'e', 0, shroomList, availableValues );

  printDecisionTree( decisionTree );

	return (0); /* return a value */

} /* end main */

/*****************************************************************************
  Read the file of 'shroom data into a linked list.

  @return {MushroomP} list
 *****************************************************************************/
MushroomP readMushrooms( char infilename[STRLEN] ) {
  FILE* infile;              /* input file (for data) */

  int NumMushrooms;    /* number of Mushrooms read from the file */
  int NumAttributes; /* number of attributes per Mushroom */
  int i, j;                     /* loop counter */
  MushroomP currentMushroom;    /* pointer to a Mushroom for the loop */
  MushroomP list;          /* pointer to the head of the list */
  list = NULL;
  currentMushroom = NULL;
  ValueP currentValue;    /* pointer to a value for the loop */
  currentValue = NULL;

  /* prompt user and read filename */
  if (strcmp(infilename, "none") == 0) {  /* file name has not been set */
    printf("Please type the name of the input file:\n");
    printf("  (Hint: it is mushrooms.data)\n");
    printf("Name of the input file: ");
    scanf("%s", infilename);
  }

  /* open the file for reading */
  openFile(&infile, infilename, "r");
  if( !infile ){
    printf("ERROR: could not open file %s\n", infilename );
    exit(0);
  }

  /* read the number of lines to follow in the file */
  fscanf( infile, "attributes: %d\n", &NumAttributes);
  fscanf( infile, "lines: %d\n", &NumMushrooms);

  char attributeArray[( NumAttributes * 2 )];
  /* for each line in the file, read from the file, create a Mushroom, store the attributes */
  for (i=0; i<NumMushrooms; i++) {
    currentMushroom = newMushroom( NumAttributes );     /* allocate memory for this shroom */
    currentMushroom->lineNumber = i;
    fscanf( infile, "%s\n", &attributeArray );
    for( j=0; j<NumAttributes; j++ ){
      currentMushroom->attributes[j] = attributeArray[( j * 2 )];
    }
    /* close the string */
    currentMushroom->attributes[j] = 0;
    list = appendMushroom( list, currentMushroom );
  } /* end i loop */

  /* close the input file */
  fclose(infile);
  
  return( list );
} /* end readMushrooms */

/*****************************************************************************
 Allocates and initializes memory for new Mushroom or fails with error message
 
 @return {MushroomP} shroom
 *****************************************************************************/
MushroomP newMushroom( int attributeCount ) {
  MushroomP shroom;  /* the new object to return */
  
  shroom = (MushroomP) emalloc( sizeof( struct Mushroom ) );
        
  if( shroom == NULL ) {
    printf("ERROR: emalloc failed for new Mushroom\n");
    exit(0);
  }

  /* initialize fields */
  shroom->lineNumber = 0;
  shroom->attributes = (char *) emalloc( ( sizeof( char ) + 1 ) * attributeCount );
  shroom->next = NULL;

  return( shroom );

} /* end newMushroom */

/*****************************************************************************
 Add a Mushroom to the end of the list.  If the list is empty, start it with this
 node.

 @return { MushroomP } a pointer to the first Mushroom in the list
 *****************************************************************************/
MushroomP appendMushroom( MushroomP list, MushroomP newshroom ){
  MushroomP lastshroom;  /* pointer to the last Mushroom of the list */
  lastshroom = list;

  /* check for an empty list */
  if( !list ){
    return( newshroom );
  }
  /* loop through the list until we find the end */
  while( lastshroom->next ){
    lastshroom = lastshroom->next;
  }
  /* now we are at the end, so point to the new node */
  lastshroom->next = newshroom;  

  return( list );
} /* end appendMushroom */

/*****************************************************************************
  This is our main function in a 2-part recursion.  We decide here whether and
  where to split, and prepare the decision nodes accordingly.

  @param {char, int, MushroomP, ValueP} letter, attribute, shroomList, availableValues
  @return {DecisionP}
 *****************************************************************************/
DecisionP createDecisionTree( char letter, int attribute, MushroomP shroomList, ValueP availableValues ){
  MushroomP shroomIterate;
  shroomIterate = NULL;
  int edibleCount, shroomCount;
  edibleCount = 0;
  shroomCount = 0;
  ValueP splitValue;
  splitValue = NULL;
  MushroomP positiveShroomList, negativeShroomList;
  positiveShroomList = NULL;
  negativeShroomList = NULL;

  /* find the number of class in this list (edible count) */
  shroomIterate = shroomList;
  while( shroomIterate ){
    if( shroomIterate->attributes[ attribute ] == letter ){
      edibleCount++;
    }
    shroomCount++;
    shroomIterate = shroomIterate->next;
  }
  /* we now have edibleCount for this set */

  /* if we have nothing left to split on, then return a 'maybe' but we should 
   * probably never arrive here, correct? */
  if( !availableValues ){
    return createLeafNode( 0, shroomCount );
  }
  /* return a positive leaf node if all of the children have this class */
  if( edibleCount == shroomCount ){
    return createLeafNode( 1, shroomCount );
  }
  /* return a negative leaf node if none of the children have this class */
  if( edibleCount == 0 ){
    return createLeafNode( -1, shroomCount );
  }

  /* choose a value on which to split; for now, we just pick the next in the
   * list and unshift it off the stack */
  splitValue = findHighestGain( letter, attribute, availableValues, shroomList );

  /* go through the mushroom list, and sort them into one list or the other */
  while( shroomList ){
    shroomIterate = shroomList->next;
    shroomList->next = NULL;
    if( shroomList->attributes[ splitValue->attribute ] == splitValue->letter ){
      positiveShroomList = appendMushroom( positiveShroomList, shroomList );
    } else {
      negativeShroomList = appendMushroom( negativeShroomList, shroomList );
    }
    shroomList = shroomIterate;
  }

  return createSplitNode( splitValue, letter, attribute, positiveShroomList, negativeShroomList, availableValues, shroomCount );
} /* end createDecisionTree */


/*****************************************************************************
  Create a Decision node that is a leaf.

  @param {int} isClass whether or not this leaf indicates it is in the class
  @return {DecisionP} pointer to the leaf for attaching
 *****************************************************************************/
DecisionP createLeafNode( int isClass, int mushroomCount ){
  DecisionP decision;
  decision = newDecision();

  decision->isLeaf = 1;
  decision->isClass = isClass;
  decision->mushroomCount = mushroomCount;

  return decision;
}

/*****************************************************************************
  Create a Decision node that is a split in the tree.

  @param {ValueP, char, int, MushroomP, MushroomP, ValueP} splitValue, letter, attribute, positiveShrooms, negativeShrooms, availableValues
  @return {DecisionP} pointer to the split for attaching to the parent node
 *****************************************************************************/
DecisionP createSplitNode( ValueP splitValue, char letter, int attribute, MushroomP positiveShrooms, MushroomP negativeShrooms, ValueP availableValues, int mushroomCount ){
  DecisionP decision;
  decision = newDecision();

  decision->isLeaf = 0;
  decision->splitValue = splitValue;
  decision->mushroomCount = mushroomCount;
  decision->left = createDecisionTree( letter, attribute, positiveShrooms, cloneValues( availableValues ) );
  decision->right = createDecisionTree( letter, attribute, negativeShrooms, cloneValues( availableValues ) );
  return decision;
}

/*****************************************************************************
 Allocates and initializes memory for new Decision or fails with error message
 
 @return {DecisionP} shroom
 *****************************************************************************/
DecisionP newDecision() {
  DecisionP decision;  /* the new object to return */
  
  decision = (DecisionP) emalloc( sizeof( struct Decision ) );
        
  if( decision == NULL ) {
    printf("ERROR: emalloc failed for new Decision\n");
    exit(0);
  }

  return( decision );

} /* end newDecision */


/*****************************************************************************
 Iterate through a list of mushrooms and collect the values into a list. Then we also calculate the entropy for each node, to make it easier to find the information gain later.
 
 @return {ValueP} list of values
 *****************************************************************************/
ValueP findAvailableValues( char letter, int attribute, MushroomP shroomList ){
  MushroomP shroomIterate;
  shroomIterate = shroomList;
  ValueP list, listIterate;
  list = NULL;
  listIterate = NULL;
  int i;

  /* for each mushroom, iterate through its attributes, and update the list of values for that attribute */
  shroomIterate = shroomList;
  while( shroomIterate != NULL ){
    i = 1; /* we don't look at attribute 0, because that is the class we are trying to identify */
    while( shroomIterate->attributes[ i ] != 0 ){
      /* precount whether these values are edible or not */
      if( shroomIterate->attributes[ attribute ] == letter ){
        list = updateValue( list, shroomIterate->attributes[ i ], i, 1 );
      } else {
        list = updateValue( list, shroomIterate->attributes[ i ], i, 0 );
      }
      i++;
    }
    shroomIterate = shroomIterate->next;
  }

  return list;
}
/*****************************************************************************
 Allocates and initializes memory for new Value or fails with error message
 
 @return {ValueP} shroom
 *****************************************************************************/
ValueP newValue( char letter, int attribute ) {
  ValueP value;  /* the new object to return */

  value = (ValueP) emalloc (sizeof (struct Value));
        
  if( value == NULL ) {
    printf("ERROR: emalloc failed for new Value\n");
    exit(0);
  }

  /* initialize fields */
  value->letter = letter;
  value->attribute = attribute;
  value->count = 0;
  value->classCount = 0;
  value->next = NULL;

  return( value );

} /* end newValue */

/*****************************************************************************
 Add a Value to the end of the list.  If the list is empty, start it with this
 node.

 @return { ValueP } a pointer to the first Value in the list
 *****************************************************************************/
ValueP appendValue( ValueP list, ValueP newvalue ){
  ValueP lastvalue;  /* pointer to the last Value of the list */
  lastvalue = list;

  /* check for an empty list */
  if( !list ){
    return( newvalue );
  }
  /* loop through the list until we find the end */
  while( lastvalue->next ){
    lastvalue = lastvalue->next;
  }
  /* now we are at the end, so point to the new node */
  lastvalue->next = newvalue;  

  return( list );
} /* end appendValue */

/*****************************************************************************
  Find an Value with the given attribute [column] and letter [class value]
  in the list or create a new Value and append it to the list.

  @param { ValueP, char, int, int } list, letter, attribute pointer to the first Value in the list, boolean for whether it is in the target class
  @return { ValueP } found the matching Value
 *****************************************************************************/
ValueP updateValue( ValueP list, char letter, int attribute, int isClass ){
  ValueP current;  /* pointer to the currect value */
  current = list;
  ValueP found;  /* pointer to the result, found or created */
  found = NULL;

  /* search the list first */
  while( current && !found ){
    if( ( current->letter == letter ) && ( current->attribute == attribute ) ){
      found = current;
    }
    current = current->next;
  }
  if( !found ){
    found = newValue( letter, attribute );
    list = appendValue( list, found );
  }
  found->count = found->count++;
  found->classCount += isClass;
  return( list );
} /* end findValue */


/*****************************************************************************
  Clone this linked list of values and return the new one.

  @param { ValueP } list to the first Value in the list
  @return { ValueP } new list of clones
 *****************************************************************************/
ValueP cloneValues( ValueP list ){
  ValueP newList;  /* pointer to the currect value */
  newList = NULL;
  ValueP newNode;  /* pointer to the result, found or created */
  newNode = NULL;

  /* search the list first */
  while( list ){
    newNode = newValue( list->letter, list->attribute );
    newNode->count = list->count;
    newList = appendValue( newList, newNode );
    list = list->next;
  }
  return newList;
} /* end cloneValues */


/*****************************************************************************
  Look at each available value, pretend to split on it, calculate the entropy 
  of what that would look like, pick the best reduction in entropy, and
  return that value to split on, reducing the available values by one.

  @param {char, int, ValueP, MushroomP} letter, attribute, valueList, shroomList
  @return {ValueP} highestGainValue the popped value that we want to split on.
******************************************************************************/
ValueP findHighestGain( char letter, int attribute, ValueP valueList, MushroomP shroomList ){
  ValueP valueIterate;
  valueIterate = valueList;
  ValueP highestGainValue;
  highestGainValue = NULL;
  MushroomP shroomIterate;
  shroomIterate = NULL;
  float gain, maxGain, currentEntropy, leftEntropy, rightEntropy;
  int leftCount, leftEdible, rightCount, rightEdible, shroomCount;
  gain = -1.0;
  maxGain = -1.0;

  /* go through each available attribute, and calculate the gain of each, but
   * only retain the highest value */
  while( valueIterate != NULL ){
    currentEntropy = 0.0;
    leftEntropy = 0.0;
    leftCount = 0;
    leftEdible = 0;
    rightEntropy = 0.0;
    rightCount = 0;
    rightEdible = 0;
    shroomIterate = shroomList;
    while( shroomIterate != NULL ){
      /* check to see if this mushroom would go to the right or to the left */
      if( shroomIterate->attributes[ valueIterate->attribute ] == valueIterate->letter ){
        /* it would go to the left, so now check and see if it's edible or not */
        leftCount++;
        if( shroomIterate->attributes[ attribute ] == letter ){
          leftEdible++;
        }
      } else {
        /* it would go to the right, so now check and see if it's edible or not */
        rightCount++;
        if( shroomIterate->attributes[ attribute ] == letter ){
          rightEdible++;
        }
      }
      shroomIterate = shroomIterate->next;
    }
    /* we now have the count values for everything that we need to perform our
     * gain calculation */
    shroomCount = leftCount + rightCount;
    currentEntropy = calculateEntropy( leftEdible + rightEdible, shroomCount );
    leftEntropy = calculateEntropy( leftEdible, leftCount );
    rightEntropy = calculateEntropy( rightEdible, rightCount ); 
      
    gain = currentEntropy - (float)( leftCount / shroomCount ) * leftEntropy - (float)( rightCount / shroomCount ) * rightEntropy;
    if( gain > maxGain ){
      maxGain = gain;
      highestGainValue = valueIterate;
    }
    valueIterate = valueIterate->next;
  }

  /* pop out the highestGainValue and return it */
  valueIterate = valueList;
  while( valueIterate != NULL ){
    if( valueIterate->next == highestGainValue ){
      valueIterate->next = highestGainValue->next;
      break;
    }
    valueIterate = valueIterate->next;
  }
  return( highestGainValue );
}

/* utility function for log2 */
float logTwo( float value ){
  return( log( value ) / log( 2.0 ) );
}
/************************************************************************************
  Using an information storage function, calculate the 'entropy' of a Value.

  @param {int, int} classCount, shroomCount
  @return {float} entropy 
************************************************************************************/
float calculateEntropy( int classCount, int shroomCount ){
  float classPercent;
  float nonClassPercent;

  /* prevent division by zero, and return if there are no shrooms here */
  if( shroomCount == 0 ){
    return( 0.0 );
  }

  classPercent = (float)( classCount ) / shroomCount;
  nonClassPercent = (float)( shroomCount - classCount ) / shroomCount;
  
  if( ( classPercent == 0 ) || ( nonClassPercent == 0 ) ){
    return( 0.0 );
  }

  return( -( classPercent ) * log2f( classPercent ) - ( nonClassPercent ) * log2f( nonClassPercent ) );
}


/*****************************************************************************
 Print out a decision tree recursively.  Should look something like:
 Split on: attr  1 value  o (count 14)
  Leaf: class is  p          count 4
  Split on: attr  1 value  s (count 10)
    Split on: attr  3 value  h (count 5)
      Leaf: class is  p         count 2
      Leaf: class is  n         count 3
    Split on: attr  4 value  t (count 5)
      Leaf: class is  p         count 3
      Leaf: class is  n         count 2

  @param {DecisionP, int} decisionTree, tab
  @return {} just print
 *****************************************************************************/
void printDecisionTree( DecisionP decisionTree ){
  FILE *outfile;             /* output file (for results) */
  ValueP values;             /* pointer to values for loop */
  values = NULL;

  /* prompt user and read filename */
  if (strcmp(Outfilename, "none") == 0) {  /* file name has not been set */
    printf("Please type the name of the output file: ");
    scanf("%s", Outfilename);
  }

  /* open the output file for writing */
  openFile(&outfile, Outfilename, "w");

  printDecision( outfile, decisionTree, 0 );

  fclose( outfile );
} /* end printDecisionTree */

/*****************************************************************************
  Print the Decision nodes recursively.

  @param {DecisionP, File}  decisionTree, outfile
  @return void
 *****************************************************************************/
void printDecision( FILE * outfile, DecisionP decisionTree, int tab ) {
  int i;
  for( i=0; i<tab; i++){
    fprintf( outfile, " " );
  }

  if( decisionTree->isLeaf == 1 ){
    fprintf( outfile, "Leaf: class is ");
    if( decisionTree->isClass == 1 ){
      fprintf( outfile, "e   count %d\n", decisionTree->mushroomCount );
    } else {
      fprintf( outfile, "p   count %d\n", decisionTree->mushroomCount );
    }
  } else {
    fprintf( outfile, "Split on: attr %d value %c   (count %d )\n", decisionTree->splitValue->attribute, decisionTree->splitValue->letter, decisionTree->mushroomCount );
    printDecision( outfile, decisionTree->left, tab + 1 );
    printDecision( outfile, decisionTree->right, tab + 1 );
  }

} /* end printDecision */


/*****************************************************************************
 Function: freeMushroomss
 Inputs:   a pointer to a linked list
 Returns:  nothing
 Description:
 free the memory in the boxmashroom list note that you have to work from 
 the bottom up if there were substructure to a mushroom, that would need 
 to be free first
 *****************************************************************************/
void freeMushrooms(MushroomP mushroomList) {

	MushroomP head = mushroomList->next;
	MushroomP temp;

	int i; // loop  counter

	/* first, free each of the mushrooms */
	while (head != NULL) {
		temp = head->next;
		free(head->attributes);
		free(head);
		head = temp;
	}

	/* now, you can free the head pointer */
	free(mushroomList->attributes);
	free(mushroomList);

} /* end freeMushrooms */


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

/*****************************************************************************
 Function: openFile
 Inputs:   the file to open, the file name, mode to open the file
 Returns:  nothing
 Description: opens file in specified mode or issues error message
 *****************************************************************************/
void openFile( FILE **fileptr, char *filename, char *mode ) {
  *fileptr = fopen( filename, mode );       /* open the file */

  if ( !(*fileptr )) {
    printf("ERROR: Failed to open file: %s with mode %s.  Exiting.\n\n",
	   filename, mode);
    exit(0);
  }

} /* end openFile */
/* END OF PROGRAM */

