/*****************************************************************************
 Adnan Fazeli
 Casey Rosenthal
 
 File:   reformat.c
 
 Description:
	This program reads the training data from the specified input file into
 a linked list in the root node of the decision tree, 
	uses highest information gain, Alpphabatical or a function picks the 
'fattest' value (the one that represents the most mushrooms).
	to determine which attribute-value pair to split on, the program does 
not use the class	attribute (the one we are trying to
	predict) for splitting. Then split the linked list into two sublists, 
according to the attribute-value pair chosen above and 
	create right and left offspring nodes, one with each of the linked 
lists. Recurse with each offspring node. The stopping criteria
	is all the examples in the node are the same class (e.g., they are all 
poisonous mushrooms or they are all edible mushrooms) 
	or there are no attribute-value pairs left to split on, this way we will 
have the decision tree completed. Then our program 
	reads the testing data from the specified input file into a	linked list 
in the root node and filter down testing data to the
	leaf nodes and since each leaf node has stored (from the training data) 
the class of the examples that should be there. we can
	calcuate the number and percentage of correctly classified examples of 
each class, and the overall percentage of correctly classified
	examples.

 
 
 Data Structures:
	This program has three structures, one to store information about each 
mushroom and a pointer to next mushroom, second to store information
	about each attribute and also a pointer to next attribute, the last one 
a structure which is used as decision tree node. We have two linked 
	lists of mashrooms and thier attributes and a decision tree.
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
  int isClass; /* -1 for maybe, 0 for no, 1 for yes */
  int mushroomCount; /* keep track of how many pass through here */
  ValueP splitValue; /* pointer to the Value on which the decision splits */
  DecisionP left; /* pointer to positive matches */
  DecisionP right; /* pointer to negative matches */
};
/*****************************************************************************/
/* prototypes */
MushroomP readMushrooms( char infilename[STRLEN] );
MushroomP appendMushroom( MushroomP list, MushroomP newshroom );
int mushroomCount( MushroomP list );
MushroomP newMushroom( int attributeCount );
MushroomP randomlyReduce( MushroomP list, int reductionCount );
void snipShroom( MushroomP shroom, MushroomP list );
int classifyMushroom( DecisionP decisionTree, MushroomP shroom );
void freeMushrooms( MushroomP mushroomList );

DecisionP createDecisionTree( char letter, int attribute, MushroomP shroomList, ValueP availableValues );
DecisionP createLeafNode( int isClass, int mushroomCount );
DecisionP createSplitNode( char letter, int attribute, ValueP splitValue, MushroomP positiveShrooms, MushroomP negativeShrooms, ValueP availableValues, int mushroomCount );
DecisionP newDecision( );
void printDecisionTree( DecisionP decisionTree, char letter[STRLEN], char outfilename[STRLEN] );
void printDecision( FILE * outfile, char letter[STRLEN], DecisionP decisionTree, int tab );
void printTestResults( char letter, int attribute, DecisionP decisionTree, int trainingShroomCount, MushroomP testingList, char outfilename[STRLEN] );
int freeDecisions( DecisionP decisionTree );
 
ValueP findAvailableValues( MushroomP shroomList );
ValueP newValue( char letter, int attribute );
ValueP appendValue( ValueP list, ValueP newvalue );
ValueP updateValue( ValueP list, char letter, int attribute, int isClass );
ValueP cloneValues( ValueP list );
void freeValues( ValueP value );
 
float calculateEntropy( int classCount, int shroomCount );
ValueP findHighestGain( char letter, int attribute, ValueP valueList, MushroomP shroomList );
ValueP findAlphabetical( ValueP valueList );
ValueP findFattest( ValueP valueList );
float logTwo( float value );
 
void *emalloc( long size );
void openFile( FILE **fileptr, char *filename, char *mode );
 
/*****************************************************************************
  We run the program by calling something along the lines of:
  guessMushroom -test moreMushrooms.data -out mushrooms.out 
 ******************************************************************************/
int main(int argc, char* argv[]) {
  srand( time( 0 ) ); 
  
  MushroomP trainingShroomList;
  trainingShroomList = NULL;
  MushroomP testingShroomList;
  testingShroomList = NULL;
  ValueP availableValues;
  DecisionP decisionTree;
  availableValues = NULL;
  decisionTree = NULL;
  char infilenameTraining[STRLEN]; /* input file name for training data */
  char infilenameTesting[STRLEN]; /* input file name for testing data */
  char outfilename[STRLEN]; /* output file name for printing the results */
  int trainingShroomCount;
  ValueP nonClassAvailableValues; /* available values minus the 0-attribute */
  ValueP valueIterate;
  nonClassAvailableValues = NULL;
  valueIterate = NULL;

  printf( "This program processes data about the attributes.\n" );
  
  /* initialize filenames to dummy strings */
  strcpy( infilenameTraining, "none" );
  strcpy( infilenameTesting, "none" );
  strcpy( outfilename, "none" );
  /* TAKEN FROM p5.c EXAMLPE
     process command-line arguments */
  while( *++argv ) {             /* while there are still command-line args */
    if ( !strcmp( *argv, "-train" ) )          /* set input file name */
      strcpy( infilenameTraining, *++argv );
    else if( !strcmp( *argv,"-test" ) )          /* set input file name */
      strcpy( infilenameTesting, *++argv );
    else if( !strcmp( *argv,"-out" ) )     /* set output file name */
      strcpy( outfilename, *++argv );
    else {  /*  error checking */
      printf( "\nERROR: option %s not recognized\n", *argv );
      printf( "...exiting program\n\n" );
      exit( 0 );
    }
  } /* end, while there are still command-line args */
 
  /* this is the testing data */
  testingShroomList = readMushrooms( infilenameTesting );
 
  /* we randomly extract some of the nodes for training data */
  trainingShroomList = randomlyReduce( testingShroomList, (int)( 0.2 * mushroomCount( testingShroomList ) ) );

  /* this is needed for the report later */
  trainingShroomCount = mushroomCount( trainingShroomList );

  availableValues = findAvailableValues( trainingShroomList );

  /* remove the 0-attribute from our attributes to check for splitting on */
  nonClassAvailableValues = cloneValues( availableValues );
  /* fast-forward to a non-0 attribute */
  while( nonClassAvailableValues->attribute == availableValues->attribute ){
    nonClassAvailableValues = nonClassAvailableValues->next;
  }
  /* strip out the 0s */
  valueIterate = nonClassAvailableValues;
  while( valueIterate->next ){
    if( valueIterate->next->attribute == availableValues->attribute ){
      valueIterate->next = valueIterate->next->next;
    }
    valueIterate = valueIterate->next;
  }

  /* seed the tree looking for the first value to classify */
  decisionTree = createDecisionTree( availableValues->letter, availableValues->attribute, trainingShroomList, nonClassAvailableValues );
 
 // printDecisionTree( decisionTree, &availableValues->letter, outfilename );
 
  printTestResults( availableValues->letter, availableValues->attribute, decisionTree, trainingShroomCount, testingShroomList, outfilename );
 
  /* free the list of test mushrooms */
  freeMushrooms( trainingShroomList );
  freeMushrooms( testingShroomList );
  /* note that Decision structs are freed in printDecisionTree */
  /* free value list */
  freeValues( availableValues );
 
	return (0);
 
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
  Count the number of mushrooms in the list.
  
  @param {MushroomP} list
  @return {int} count
 *****************************************************************************/
int mushroomCount( MushroomP list ){
  int i;
  i = 0;

  while( list ){
    i++;
    list = list->next;
  }
  return( i );
}
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
 
  /* make sure the newshroom will be at the end of the list */
  newshroom->next = NULL;
  
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
  Iterate through the mushroom list. If a random float is less that the 
  threshold, then pop the given node and return those in a new list.
 
  @param { MushroomP } list
  @return { MushroomP } resultList a pointer to the first Mushroom in the list
 *****************************************************************************/
MushroomP randomlyReduce( MushroomP list, int reductionCount ){
  MushroomP iterateList;
  MushroomP resultList;
  MushroomP selectedShroom;
  double randomSelector;
  int i;

  iterateList = list;
  resultList = NULL;
  selectedShroom = NULL;

  while( mushroomCount( resultList ) < reductionCount ){
    randomSelector = (double)random() * (double)mushroomCount( list ) / ((double)RAND_MAX + 1);
    i = 0;
    iterateList = list;
    while( ( i < randomSelector ) && iterateList ){
      i++;
      iterateList = iterateList->next;
    }
    snipShroom( iterateList, list );
    resultList = appendMushroom( resultList, iterateList );
  }
  return( resultList );
}

/*****************************************************************************
  Find the given node in a list, and remove it from the list, or NULL if 
  not found.
  
  @param { MushroomP, MushroomP } shroom, list
  @return { int } snipped
 *****************************************************************************/
void snipShroom( MushroomP shroom, MushroomP list ){

  while( list != NULL ){
    if( list->next == shroom ){
      list->next = shroom->next;
      return;
    }
    list = list->next;
  }
  return;

}

/*****************************************************************************
  This is our main function in a 2-part recursion.  We decide here whether and
  where to split, and prepare the decision nodes accordingly.
 
  @param {char, int, MushroomP, ValueP} letter, attribute, shroomList, availableValues
  @return {DecisionP}
 *****************************************************************************/
DecisionP createDecisionTree( char letter, int attribute, MushroomP shroomList, ValueP availableValues ){
  MushroomP shroomIterate;
  shroomIterate = NULL;
  MushroomP shroom;
  shroom = NULL;
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
    return createLeafNode( -1, shroomCount );
  }
  /* return a positive leaf node if all of the children have this class */
  if( edibleCount == shroomCount ){
    return createLeafNode( 1, shroomCount );
  }
  /* return a negative leaf node if none of the children have this class */
  if( edibleCount == 0 ){
    return createLeafNode( 0, shroomCount );
  }
 
  /* choose a value on which to split */
  /* we have three functions to choose from: highest gain, alphabetical, and fattest */
  //splitValue = findAlphabetical( availableValues );
  //splitValue = findHighestGain( letter, attribute, availableValues, shroomList );
  splitValue = findFattest( availableValues );

  /* go through the mushroom list, and sort them into one list or the other */
  shroom = shroomList;
  while( shroom ){
    shroomIterate = shroom->next;
    shroom->next = NULL;
    if( shroom->attributes[ splitValue->attribute ] == splitValue->letter ){
      positiveShroomList = appendMushroom( positiveShroomList, shroom );
    } else {
      negativeShroomList = appendMushroom( negativeShroomList, shroom );
    }
    shroom = shroomIterate;
  }
 
  return createSplitNode( letter, attribute, splitValue, positiveShroomList, negativeShroomList, availableValues, shroomCount );
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
DecisionP createSplitNode( char letter, int attribute, ValueP splitValue, MushroomP positiveShrooms, MushroomP negativeShrooms, ValueP availableValues, int mushroomCount ){
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
ValueP findAvailableValues( MushroomP shroomList ){
  MushroomP shroomIterate;
  shroomIterate = shroomList;
  ValueP list, listIterate;
  list = NULL;
  listIterate = NULL;
  int i;
 
  /* for each mushroom, iterate through its attributes, and update the list of values for that attribute */
  shroomIterate = shroomList;
  while( shroomIterate != NULL ){
    i = 0;
    while( shroomIterate->attributes[ i ] != 0 ){
      /* precount whether these values are in the first class or not */
      if( ( list == NULL ) || ( shroomIterate->attributes[ list->attribute ] == list->letter ) ){
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
      
    gain = (float)currentEntropy - ( (float)leftCount / (float)shroomCount ) * (float)leftEntropy - ( (float)rightCount / (float)shroomCount ) * (float)rightEntropy;
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

/*****************************************************************************
  Iterate the values, choose the lowest alphabetically,  and pop it out of 
  the list.
  
  @param { ValueP } valueList
  @return { ValueP } splitValue
 *********************************************************************************/
ValueP findAlphabetical( ValueP valueList ){
  ValueP valueIterate;
  valueIterate = valueList;
  ValueP splitValue;
      
  splitValue = valueIterate;
  /* go through each available attribute, and set it to the splitValue if it
   * is less than the previous value */
  while( valueIterate != NULL ){
	  if( valueIterate->letter < splitValue->letter ){
		  splitValue = valueIterate;
	  }
	  valueIterate = valueIterate->next;
  }

  /* pop out the splitValue from the list and then return it */
  valueIterate = valueList;
  while( valueIterate != NULL ){
    if( valueIterate->next == splitValue ){
      valueIterate->next = splitValue->next;
      break;
    }
    valueIterate = valueIterate->next;
  }
  return( splitValue );

}

/*****************************************************************************
  Iterate the values, find the one representative of the most items in the
  original data set, and pop it out of the list.
  
  @param { ValueP } valueList
  @return { ValueP } splitValue
 *********************************************************************************/
ValueP findFattest( ValueP valueList ){
  ValueP valueIterate;
  valueIterate = valueList;
  ValueP splitValue;
      
  splitValue = valueIterate;
  /* go through each available attribute, and set it to the splitValue if it
   * is less than the previous value */
  while( valueIterate != NULL ){
	  if( valueIterate->count > splitValue->count ){
		  splitValue = valueIterate;
	  }
	  valueIterate = valueIterate->next;
  }

  /* pop out the splitValue from the list and then return it */
  valueIterate = valueList;
  while( valueIterate != NULL ){
    if( valueIterate->next == splitValue ){
      valueIterate->next = splitValue->next;
      break;
    }
    valueIterate = valueIterate->next;
  }
  return( splitValue );

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
 
  classPercent = (float)classCount / (float)shroomCount;
  nonClassPercent = ( (float)shroomCount - (float)classCount ) / (float)shroomCount;
  
  if( ( classPercent == 0.0 ) || ( nonClassPercent == 0.0 ) ){
    return( 0.0 );
  }
 
  return( -( classPercent ) * log2f( classPercent ) - ( nonClassPercent ) * log2f( nonClassPercent ) );
}
 
/*****************************************************************************
  Recurse through a decision tree, and estimate the class of this mushroom.
 
  @param {DecisionP, MushroomP}  decisionTree, shroom
  @return {int} isClass
 *****************************************************************************/
int classifyMushroom( DecisionP decisionTree, MushroomP shroom ) {
 
  if( decisionTree->isLeaf == 1 ){
    return( decisionTree->isClass );
  } else {
    if( shroom->attributes[ decisionTree->splitValue->attribute ] == decisionTree->splitValue->letter ){
      classifyMushroom( decisionTree->left, shroom );
    } else {
      classifyMushroom( decisionTree->right, shroom );
    }
  }
 
} /* end classifyMushroom */
 
 
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
void printDecisionTree( DecisionP decisionTree, char letter[STRLEN], char outfilename[STRLEN]  ){
  FILE *outfile;             /* output file (for results) */
  ValueP values;             /* pointer to values for loop */
  values = NULL;
 
  /* prompt user and read filename */
  if( strcmp( outfilename, "none" ) == 0 ){  /* file name has not been set */
    printf( "Please type the name of the output file: " );
    scanf( "%s", outfilename );
  }
 
  /* open the output file for writing */
  openFile( &outfile, outfilename, "w" );
 
  printDecision( outfile, letter, decisionTree, 0 );
 
  fclose( outfile );
} /* end printDecisionTree */
 
/*****************************************************************************
  Print the Decision nodes recursively.
 
  @param {DecisionP, File}  decisionTree, outfile
  @return void
 *****************************************************************************/
void printDecision( FILE * outfile, char letter[STRLEN], DecisionP decisionTree, int tab ) {
  int i;
  for( i=0; i<tab; i++){
    fprintf( outfile, " " );
  }
 
  if( decisionTree->isLeaf == 1 ){
    fprintf( outfile, "Leaf: class is ");
    if( decisionTree->isClass == 1 ){
      fprintf( outfile, "%s count %d\n", letter, decisionTree->mushroomCount );
    } else {
      fprintf( outfile, "not-%s count %d\n", letter, decisionTree->mushroomCount );
    }
  } else {
    fprintf( outfile, "Split on: attr %d value %c   (count %d )\n", decisionTree->splitValue->attribute, decisionTree->splitValue->letter, decisionTree->mushroomCount );
    printDecision( outfile, letter, decisionTree->left, tab + 1 );
    printDecision( outfile, letter, decisionTree->right, tab + 1 );
  }
 
} /* end printDecision */
 
/*****************************************************************************
 Print out a decision tree recursively.  Should look something like:
 
There are 14 examples in the training data.
There are 5 rules in the decision tree.
There is an average of 2.8 examples per rule.
100.0 percent of n examples were classified correctly (5/5)
100.0 percent of p examples were classified correctly (9/9)
100.0 percent of all the data were classified correctly (14/14)
 
  @param {DecisionP, MushroomP, char} decisionTree, shroomList, outfile
  @return {} just print
 *****************************************************************************/
void printTestResults( char letter, int attribute, DecisionP decisionTree, int trainingShroomCount, MushroomP testingList, char outfilename[STRLEN]  ){
  FILE *outfile;             /* output file (for results) */
  ValueP values;             /* pointer to values for loop */
  values = NULL;
  int testingShroomCount, decisionCount;
  int classifyPositiveCount, classifyNegativeCount;
  int positiveCorrectCount, negativeCorrectCount;
  int isClass;
 
  testingShroomCount = mushroomCount( testingList );
  decisionCount = 0;
  classifyPositiveCount = 0;
  classifyNegativeCount = 0;
  positiveCorrectCount = 0;
  negativeCorrectCount = 0;
  isClass = 0;
 
  /* collect the statistics */
  MushroomP shroom;
  shroom = testingList;
  while( shroom ){
 
    isClass = classifyMushroom( decisionTree, shroom );
    if( isClass == 1 ){
      classifyPositiveCount++;
      if( shroom->attributes[ attribute ] == letter ){
        positiveCorrectCount++;
      }
    } else {
      classifyNegativeCount++;
      if( shroom->attributes[ attribute ] != letter ){
        negativeCorrectCount++;
      }
    }
    shroom = shroom->next;
  }  
 
  /* prompt user and read filename */
  if( strcmp( outfilename, "none" ) == 0 ){  /* file name has not been set */
    printf( "Please type the name of the output file: " );
    scanf( "%s", outfilename );
  }
 
  /* open the output file for writing */
  openFile( &outfile, outfilename, "w" );
 
  printDecision( outfile, &letter, decisionTree, 0 );
  decisionCount = freeDecisions( decisionTree );
  fprintf( outfile, "\n\nTest Results:\n" );
  fprintf( outfile, "There are %d examples in the training data.\n", trainingShroomCount );
  fprintf( outfile, "There are %d rules in the decision tree.\n", decisionCount );
  fprintf( outfile, "There is an average of %3.2f examples per rule.\n", ( (float)trainingShroomCount / decisionCount ) );
  fprintf( outfile, "%3.2f percent of not-class examples were classified correctly (%d/%d)\n", 
    ( (float)negativeCorrectCount * 100 / classifyNegativeCount ),
    negativeCorrectCount,
    classifyNegativeCount
  );
  fprintf( outfile, "%3.2f percent of class examples were classified correctly (%d/%d)\n", 
    ( (float)positiveCorrectCount * 100 / classifyPositiveCount ),
    positiveCorrectCount,
    classifyPositiveCount
  );
  fprintf( outfile, "%3.2f percent of all the data were classified correctly (%d/%d)\n", 
    ( (float)( negativeCorrectCount + positiveCorrectCount ) * 100 / testingShroomCount ),
    ( negativeCorrectCount + positiveCorrectCount ),
    testingShroomCount
  );
 
  fclose( outfile );
} /* end printDecisionTree */
 
 
/*****************************************************************************
 Function: freeMushroomss
 Inputs:   a pointer to a linked list
 Returns:  nothing
 Description:
 free the memory in the boxmashroom list note that you have to work from 
 the bottom up if there were substructure to a mushroom, that would need 
 to be free first
 *****************************************************************************/
void freeMushrooms( MushroomP mushroomList ) {
 
	if( !mushroomList ){
    return;
  }

  /* first, free each of the mushrooms */
	if( mushroomList->next ){
    freeMushrooms( mushroomList->next );
  }
  free( mushroomList->attributes );
  free( mushroomList );
 
} /* end freeMushrooms */
 
/******************************************************************
  Iterate through a linked list of values, and free the memory
  for all of them.
 
  param {ValueP} value
  return 
********************************************************************/
void freeValues( ValueP value ){
	ValueP temp = NULL;
 
	while( value ){
		temp = value->next;
		free( value );
		value = temp;
	}
 
}
 
/*****************************************************************************
  Iterate through the decision tree and free the memory for each node,
  keeping count so we can return the number of nodes which we freed.
 
  param {DecisionP} decisionTree
  return {int} decisionCount
 *****************************************************************************/
int freeDecisions( DecisionP decisionTree ) {
 
  if( decisionTree->isLeaf == 1 ){
    free( decisionTree );
    return( 1 );
  } else {
    return( freeDecisions( decisionTree->left ) + freeDecisions( decisionTree->right ) );
  }
} /* end freeDecisions */
 
 
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
 
