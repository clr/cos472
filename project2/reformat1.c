/*****************************************************************************
 Adnan Fazeli

 File:   reformat.c

 Description:
 Read data from a file, reformat the data and print this back out to a new file.

 Notes:
 run this program with the file mushrooms.data. You can pick the output file
 name. Some functions are from program P4.c .

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

/*****************************************************************************
 data structures:
 *****************************************************************************/
typedef struct Mushroom *MushroomP; /* pointer to a Mushroom structure */
struct Mushroom {
	int mushroomNum; /* line number of this mushroom */
	char* attributes; /* attributes of the mushroom */
	MushroomP next; /* pointer to the next mushroom */
};
typedef struct Attribute * AttributeP; /* pointer to an Attribute structure */
struct Attribute {
	char value; /* the letter of the value */
	int count; /* the number of mushrooms in the set that have this value */
  //	int edibleCount; /* the number of mushrooms in the set that have this value and are also edible */
	int column; /* the column that the attribute appears in */
	AttributeP next; /* pointer to the next attribute in the linked list */
};

/*****************************************************************************/
/* prototypes */
MushroomP readMushroomsAttributes(int, char**, AttributeP*);
void printMushroomsAttributes(int, char**, MushroomP);
void freeMushrooms(MushroomP);
void *emalloc(long size);
MushroomP createNewMushroom(void);
void extractAttributes(char* a, char* b);
void printMushroomsInformation(int, char**, MushroomP);
void printMushroomsDifferently(int, char**, MushroomP);
void printAttributes(MushroomP, int, FILE *);

AttributeP newAttribute(void);
AttributeP appendAttribute(AttributeP firstList, AttributeP secondList);

MushroomP getLeftList(AttributeP list, char value);
MushroomP getRightList(AttributeP list, char value);
void addToArrayOfLinkedLists(MushroomP last, AttributeP *attributeArray, int NumAttributes);

AttributeP highestGainAttribute(AttributeP *);

/*****************************************************************************
 main
 ******************************************************************************/
int main(int argc, char* argv[]) {

	MushroomP mushroomList;
	AttributeP * attributeArray;

	printf("This program processes data about mushrooms.\n");
	/*read mushroom attributes from file to linked list*/

	mushroomList = readMushroomsAttributes(argc, argv, attributeArray);

	/* print from the linked list */
	//printMushroomsAttributes(argc, argv,mushroomList);

	//printMushroomsInformation(argc, argv,mushroomList);    
	//	printMushroomsDifferently(argc, argv, mushroomList);

	freeMushrooms(mushroomList); /* free the memory in the linked list */

	return (0); /* return a value */

} /* end main */

/*****************************************************************************
 Function: readMushroomsAttributes
 Input: 
 the number of command line argumants and an
 array of the command line arguments 
 
 Returns: a pointer to a linked list
 Discription:  Read data from a file to a linked list
 *****************************************************************************/
MushroomP readMushroomsAttributes(int argc, char** argv,
	AttributeP* attributeArray) {

	char infilename[STRLEN];
	FILE *infile;

	int i; /* loop counter */
	int NumAttributes;
	int NumMushrooms;
	char *mAttributes; /*temporary storage for attributes read from file */
	MushroomP mushroomList; /* pointer to the mashrooms linked list */

	if (argc != 5) {
		/* prompt user and read filename */
		printf("Please type the name of the input file: \n");
		printf("  (Hint: it is mushrooms.data)\n");
		printf("Name of the input file:");
		scanf("%s", infilename);
	} else {
		strcpy(infilename, argv[2]);
	}

	/* open file for reading */
	infile = fopen(infilename, "r"); /* open the file */

	if (!infile) {
		printf("ERROR: Failed to open file: %s with mode 'r'.  Exiting.\n\n", infilename);
		exit(1);
	}

	/* read the number of attributes to follow in the file */
	fscanf(infile, "attributes: %d\n", &NumAttributes);

	/* read the number of lines to follow in the file */
	fscanf(infile, "lines: %d\n", &NumMushrooms);

	/* now you know how big the array of attribures is -- allocate the memory
	 the size multiplied by 2 because of the ',' between the alphabatical 
	 characters and plus the 0 at the end of the string */

	mAttributes = (char *) emalloc(2 * NumAttributes);
	attributeArray = (AttributeP *) emalloc(sizeof(struct Attribute) * NumAttributes);

	/* read the first mushroom information from the file, store the information */

	mushroomList = createNewMushroom();
	fscanf(infile, "%s", mAttributes);
	mushroomList->mushroomNum = 0;
	mushroomList->attributes = (char*) emalloc(NumAttributes + 1);
	extractAttributes(mushroomList->attributes, mAttributes);
	mushroomList->next = NULL;
	
	//initialize the attribute array with first mushroom's attributes
	for(i = 0; i < NumAttributes; i++) {
		attributeArray[i]->value = mushroomList->attributes[i];
		attributeArray[i]->count = 1;
		attributeArray[i]->column = i;
		attributeArray[i]->next = NULL;
	}
 
	/* read data from file and create a linked list  */
	MushroomP last = mushroomList;
	MushroomP newMushroom = NULL;
	for (i = 1; i < NumMushrooms; i++) {
		fscanf(infile, "%s", mAttributes);
		newMushroom = createNewMushroom();
		newMushroom->mushroomNum = i;
		newMushroom->attributes = (char *) emalloc(NumAttributes + 1);
		extractAttributes(newMushroom->attributes, mAttributes);
		newMushroom->next = NULL;
		last->next = newMushroom;
		last = newMushroom;
		
		// create the array of attributes values linked list		
		addToArrayOfLinkedLists(last, attributeArray, NumAttributes);
	}
	fclose(infile);
	return (mushroomList);
}

/*****************************************************************************
 Function: printMushroomsAttributes
 Inputs:   number of command line argumants,an 
 array of command line arguments  
 and a pointer to linked list
 Returns:  nothing
 Description:
 print out the information stored in the linked list
 *****************************************************************************/
void printMushroomsAttributes(int argc, char *argv[], MushroomP mushroomList) {

	char outfilename[STRLEN]; /* name of output file */
	FILE *outfile; /* output file (for results) */
	int i; /* loop counter */

	if (argc != 5) {
		/* prompt user and read filename */
		printf("Please type the name of the output file: ");
		scanf("%s", outfilename);
	} else {
		strcpy(outfilename, argv[4]);
	}

	/* open the output file for writing */
	outfile = fopen(outfilename, "w"); /* open the file */

	if (!outfile) {
		printf("ERROR: Failed to open file: %s with mode 'w'.  Exiting.\n\n",	outfilename);
		exit(1);
	}

	MushroomP head = mushroomList;

	/* print out each mushroom's attributes in the list */
	while (head != NULL) {
		int i;
		for (i = 0; i < strlen(head->attributes); i++) {
			fprintf(outfile, "%c ", head->attributes[i]);
    }
		fprintf(outfile, "\n");
		head = head->next; // move head to the next mushroom
	} /* end of while loop */
	fclose(outfile);

} /* end printMushroomsAttributes */

/*****************************************************************************
 Function: printMushroomsInformation
 Inputs:   number of command line argumants,an 
 array of command line arguments  
 and a pointer to linked list
 Returns:  nothing
 Description:
 print out the information stored in the linked list
 *****************************************************************************/
void printMushroomsInformation(int argc, char *argv[], MushroomP mushroomList) {

	char outfilename[STRLEN]; /* name of output file */
	FILE *outfile; /* output file (for results) */
	int i; /* loop counter */

	if (argc != 5) {
		/* prompt user and read filename */
		printf("Please type the name of the output file: ");
		scanf("%s", outfilename);
	} else {
		strcpy(outfilename, argv[4]);
	}

	/* open the output file for writing */
	outfile = fopen(outfilename, "w"); /* open the file */
	if (!outfile) {
		printf("ERROR: Failed to open file: %s with mode 'w'.  Exiting.\n\n", outfilename);
		exit(1);
	}

	/* print out each attribute repeatation in the list */
	for (i = 0; i < strlen(mushroomList->attributes); i++) {
		fprintf(outfile, "attribute %3d:", i);
		printAttributes(mushroomList, i, outfile);
		fprintf(outfile, "\n");
	} /* end of while loop */

	fclose(outfile);
} /* end printMushroomsInformation */

/*****************************************************************************
 Function: printMushroomsDifferently
 Inputs:   number of command line argumants,an 
 array of command line arguments  
 and a pointer to linked list
 Returns:  nothing
 Description:
 print out the information stored in the linked list
 *****************************************************************************/
void printMushroomsDifferently(int argc, char *argv[], MushroomP mushroomList) {

	char outfilename[STRLEN]; /* name of output file */
	FILE *outfile; /* output file (for results) */
	int i; /* loop counter */

	if (argc != 5) {
		/* prompt user and read filename */
		printf("Please type the name of the output file: ");
		scanf("%s", outfilename);
	} else {
		strcpy(outfilename, argv[4]);
	}

	/* open the output file for writing */
	outfile = fopen(outfilename, "w"); /* open the file */
	if (!outfile) {
		printf("ERROR: Failed to open file: %s with mode 'w'.  Exiting.\n\n", outfilename);
		exit(1);
	}

	/* print out each attribute repeatation in the list */
	for (i = 0; i < strlen(mushroomList->attributes); i++) {
		fprintf(outfile, "attribute %3d:", i);
		printAttributes(mushroomList, i, outfile);
		fprintf(outfile, "\n");
	} /* end of while loop */

	fclose(outfile);
} /* end printMushroomsInformation */

/************************************************************
 Function: printAttributes
 Inputs: array of attributes, length of the array and a pointer to output file
 Returns: none
 Discription: print the attributes of mushrooms in the format of 
 'attribute = number of repeat'
 *****************************************************************/
void printAttributes(MushroomP mushroom, int attributeIndex, FILE *outfile) {

	/* structure to store the attribute, number of repeat 
	 and pointer to the next structure */
	struct anAttr {
		char a;
		int n;
		struct anAttr *next;
	};

	struct anAttr *attrList; /* pointer to the linked list of attributes */
	MushroomP mushTemp;

	/* allocate memory and assign values*/
	attrList = (struct anAttr *) emalloc(sizeof(struct anAttr));
	attrList->a = mushroom->attributes[attributeIndex];
	attrList->n = 1;
	attrList->next = NULL;

	/*create a linked list for a mushroom attributes */
	struct anAttr *head, *last, *temp;
	last = attrList;

	mushTemp = mushroom->next; /* point to the second mushroom of the mushroom's linked list */

	while (mushTemp != NULL) {
		head = attrList; /* point to the front of the attributes linked list */
		while (head != NULL) {
			if (head->a == mushTemp->attributes[attributeIndex]) {
				(head->n)++;
				break;
			}
			head = head->next;
		}
		if (head == NULL) { /* a new attribute */
			temp = (struct anAttr *) emalloc(sizeof(struct anAttr));
			temp->a = mushTemp->attributes[attributeIndex];
			temp->n = 1;
			temp->next = NULL;
			last->next = temp;
			last = temp;
		}
		mushTemp = mushTemp->next;

	} /* end of while(mushTemp != NULL) */

	/* print attributes with there number of repeatation */
	head = attrList;
	while (head != NULL) {
		fprintf(outfile, " %c=%d", head->a, head->n);
		head = head->next;
	}

	/*free memory */
	head = attrList->next;
	while (head != NULL) {
		temp = head->next;
		free(head);
		head = temp;
	}

	free(attrList);

} /* end of printAttributes */

/***************************************************************************
 Function: extractAttributes
 Inputs: two arrays of charecters
 Results: none
 Description: extract the attributes from array b and eliminate the commas.
 ****************************************************************************/
void extractAttributes(char* a, char* b) {

	int i, j;

	for (i = 0; i < strlen(b); i += 2) {
		a[i / 2] = b[i];
  }
	a[i / 2] = 0;

} // end of extractAttributes


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
	void *p = (void *) malloc(size); /* request memory */

	if (p == NULL) { /* quit if request failed */
		printf("Unable to intitialize...out of memory.");
		exit(0);
	}

	return p;

} /* end emalloc */

/*****************************************************************************
 Function: newMushroom
 Inputs:   none
 Returns:  pointer to the new Mushroom
 Description: allocates and initializes memory or fails with error message
 *****************************************************************************/
MushroomP createNewMushroom(void) {
	MushroomP mushroom; /* the new object to return */

	mushroom = (MushroomP) emalloc(sizeof(struct Mushroom));

	if (mushroom == NULL) {
		printf("ERROR: emalloc failed for new Mushroom\n");
		exit(0);
	}

	/* initialize fields */
	mushroom->mushroomNum = 0;
	mushroom->attributes = NULL;
	mushroom->next = NULL;

	return (mushroom);

} /* end newMushroom */


/*********************************************************************************
Function: addToArrayOfLinkedLists
Inputs: pointer to Mushroom structure, array of attributes linked list and number of attributes in each mushroom
Output: none
***********************************************************************************/
void addToArrayOfLinkedLists(MushroomP last, AttributeP *attributeArray, int NumAttributes){
		
	AttributeP head, temp, newAttr;	
	int i; // loop counter

	// create the array of attributes values linked list
	for(i = 0; i < NumAttributes; i++) {
		head = attributeArray[i];
		while(head != NULL){
			if(head->value == last->attributes[i]) {
				(head->count)++;
				break;  // break while loop			
				temp = head;
				head = head->next;
			}
		}
		if(head == NULL) {
			newAttr = (AttributeP) emalloc(sizeof(struct Attribute));
			newAttr->value = last->attributes[i];
			newAttr->count = 1;
			newAttr->column = i;
			temp->next = newAttr;
			newAttr->next = NULL;
		}
	}
}

/* end of addToArrayOfLinkedLists */

/*********************************************************************/
void printArrayOfLinkedList(int argc, char *argv[], AttributeP * attributeArray, int n) {

  AttributeP head, temp;

	char outfilename[STRLEN]; /* name of output file */
	FILE *outfile; /* output file (for results) */
	int i; /* loop counter */

	if (argc != 5) {
		/* prompt user and read filename */
		printf("Please type the name of the output file: ");
		scanf("%s", outfilename);
	} else {
		strcpy(outfilename, argv[4]);
	}

	/* open the output file for writing */
	outfile = fopen(outfilename, "w"); /* open the file */
	if (!outfile) {
		printf("ERROR: Failed to open file: %s with mode 'w'.  Exiting.\n\n", outfilename);
		exit(1);
	}

  for(i = 0; i<n; i++){
	/* print attributes with there number of repeatation */
  	head = attributeArray[i];

	  while (head != NULL) {
		  fprintf(outfile, " %c=%d", head->value, head->count);
		  head = head->next;
	  }

  	/*free memory */
	  head = attributeArray[i]->next;
  	while (head != NULL) {
	  	temp = head->next;
		  free(head);
		  head = temp;
  	}

	  free(attributeArray[i]);
  }
  fclose(outfile);
}

/* END OF PROGRAM */

