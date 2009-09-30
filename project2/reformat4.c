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
       int edibleCount; /* the number of mushrooms in the set that have this value and are also edible */
       int column; /* the column that the attribute appears in */
       AttributeP next; /* pointer to the next attribute in the linked list */
};
typedef struct Node *NodeP;
struct Node {
 char c;
 int column;
 NodeP left;
 NodeP right;
};
typedef struct Info *InfoP;
struct Info {
 MushroomP list;
 AttributeP * array;
};

/*****************************************************************************/
/* prototypes */
InfoP readMushroomsAttributes(int, char**,int*);
void printMushroomsAttributes(int, char**, MushroomP);
void freeMushrooms(MushroomP);
void *emalloc(long size);
MushroomP createNewMushroom(void);
void extractAttributes(char* a, char* b);
void printAttributes(MushroomP, int, FILE *);

AttributeP newAttribute(void);

void addToArrayOfLinkedLists(MushroomP last, AttributeP *attributeArray, int NumAttributes);

AttributeP highestGainAttribute(AttributeP *, int);
void  printArrayOfLinkedList(int, char**, AttributeP*, int);


/*****************************************************************************
 main
 ******************************************************************************/
int main(int argc, char* argv[]) {

 InfoP info;
 int n;
 printf("This program processes data about mushrooms.\n");

 /*read mushroom attributes from file to linked list*/

 info = readMushroomsAttributes(argc, argv , &n);
 printArrayOfLinkedList(argc, argv, info->array, n);

 freeMushrooms(info->list); /* free the memory in the linked list */

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
InfoP readMushroomsAttributes(int argc, char** argv, int *n) {

 AttributeP* attributeArray ;
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
   printf("ERROR: Failed to open file: %s with mode 'r'.  Exiting.\n\n",
          infilename);
   exit(1);
 }

 /* read the number of attributes to follow in the file */
 fscanf(infile, "attributes: %d\n", &NumAttributes);

 /* read the number of lines to follow in the file */
 fscanf(infile, "lines: %d\n", &NumMushrooms);

 /* now you know how big the array of attribures is -- allocate the memory
    the size multiplied by 2 because of the ',' between the alphabatical
    characters and plus the 0 at the end of the string */
 *n = NumAttributes;
 mAttributes = (char *) emalloc(2 * NumAttributes);
 attributeArray = (AttributeP *) emalloc(sizeof(struct Attribute *)* NumAttributes);




 /* read the first mushroom information from the file, store the information */

 mushroomList = createNewMushroom();
 fscanf(infile, "%s", mAttributes);
 mushroomList->mushroomNum = 0;
 mushroomList->attributes = (char*) emalloc(NumAttributes + 1);
 extractAttributes(mushroomList->attributes, mAttributes);
 mushroomList->next = NULL;

 //initialize the attribute array with first mushroom's attributes
 for(i = 0; i < NumAttributes; i++) {
   attributeArray[i] = (AttributeP) emalloc(sizeof(struct Attribute));
   attributeArray[i]->value = mushroomList->attributes[i];
   attributeArray[i]->count = 1;
   attributeArray[i]->column = i;
   attributeArray[i]->next = NULL;
   if( mushroomList->attributes[0] == 'e')
     attributeArray[i]->edibleCount = 1;
   else
     attributeArray[i]->edibleCount = 0;
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
 //    printArrayOfLinkedList(argc, argv, attributeArray, *n);

 fclose(infile);
 InfoP info;
 info = (InfoP) emalloc(sizeof(struct Info));
 info->list = mushroomList;
 info->array = attributeArray;
 return (info);


}

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
                                       if( last->attributes[0] == 'e')
                                         head->edibleCount += 1;
                                       break;  // break while loop
                               }
                                       temp = head;
                                       head = head->next;

                       }
                       if(head == NULL) {
                               newAttr = (AttributeP) emalloc(sizeof(struct Attribute));
                               newAttr->value = last->attributes[i];
                               newAttr->count = 1;
                               newAttr->column = i;
                               temp->next = newAttr;
                               newAttr->next = NULL;
                               if( last->attributes[0] == 'e')
                                 newAttr->edibleCount = 1;
                               else
                                 newAttr->edibleCount = 0;

                       }

               }
}/* end of addToArrayOfLinkedLists */

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
                                       if( last->attributes[0] == 'e')
                                         head->edibleCount += 1;
                                       break;  // break while loop
                               }
                                       temp = head;
                                       head = head->next;

                       }
                       if(head == NULL) {
                               newAttr = (AttributeP) emalloc(sizeof(struct Attribute));
                               newAttr->value = last->attributes[i];
                               newAttr->count = 1;
                               newAttr->column = i;
                               temp->next = newAttr;
                               newAttr->next = NULL;
                               if( last->attributes[0] == 'e')
                                 newAttr->edibleCount = 1;
                               else
                                 newAttr->edibleCount = 0;

                       }

               }
}/* end of addToArrayOfLinkedLists */

/*****************************************************************************
Function: highestGainAttribute



******************************************************************************/
AttributeP highestGainAttribute(AttributeP * array, int len){

 AttributeP highestInfoGainedAttribute = NULL , attr;
 float gain, entropyP,entropyL, entropyR ;
 float max = FLOAT_MIN;
 int i;  // loop counter
 int total;
 int edible;
 int leftEdible;
 int rightEdible;

 if(array[0]->next == NULL)
   return NULL;   // when all remain mushrooms are edible or all poison

 total = array[0]->count + array->next->count;
 if(array[0]->value == 'e')
   edible = array[0]->count;
 else
   edible = array[0]->next->count;

 for(i = 1; i < len; i++){
   attr = array[i];

  if(!attr->next){
     contiue;
   }

   leftEdible =  attr->edibleCount;
   rightEdible = edible - attr->edibleCount;

   while(attr != NULL){
     entropyP = -((float)(array[0]->count)/total)*log2((float)(array[0]->count)/total)
       - ((float)(array[0]->next->count)/total)*log2((float)(array[0]->next->count)/total);

     entropyL =  -((float)(leftEdible)/attr->count)*log2((float)(leftEdible)/attr->count)
       -((float)(attr->count - leftEdible)/attr->count)*log2((float)(attr->count - leftEdible)/attr->count);

     entropyR =  -((float)(rightEdible)/(total - attr->count))*log2((float)(rightEdible)/(total - attr->count))
       -((float)(total - attr->count - rightEdible)/(total - attr->count))*log2((float)(total - attr->count - rightEdible)/(total - attr->count));

     gain = entropyP - (float)(attr->count/total)*entropyL - (float)(total - attr->count/total)*entropyR;

     if(max < gain){
       max = gain;
       highestInfoGainedAttribute = attr ;
     }
   }
 }
 }


 return(highestInfoGainedAttribute);
} /* end of highestInfoGainedAttribute */

/************************************************************************************
Function: calculateEntropy


************************************************************************************/
float  calculateEntropy(AttributeP target, AttributeP attr){
 float gain;
 float ES = 0;
 int numMushrooms = 0, i;
 AttributeP temp;


 if(attr->next || target->next)
   return ES;  /* if all the mushrooms left has the same attribute at the this column or if all of them either edible or poisonous */

 temp = target;
 while(temp){
   numMushrooms += (temp->count);
   temp = temp->next;
 }

 temp = attr;
 while(temp){

   ES + = -((float)(temp->edibleCount)/numMushrooms)* log2((float)(temp->edibleCount)/numMushrooms));
   temp = temp->next;
   }

return ES;

}
/***************************************************************************
 Function: extractAttributes
 Inputs: two arrays of charecters
 Results: none
 Description: extract the attributes from array b and eliminate the commas.
 ****************************************************************************/
void extractAttributes(char* a, char* b) {

       int i, j;

       for (i = 0; i < strlen(b); i += 2)
               a[i / 2] = b[i];

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
               printf("ERROR: Failed to open file: %s with mode 'w'.  Exiting.\n\n",
                               outfilename);
               exit(1);
       }

  for(i = 0; i<n; i++){

       /* print attributes with there number of repeatation */
       head = attributeArray[i];

       while (head != NULL) {
               fprintf(outfile, " %c=%d", head->value, head->count);
               head = head->next;
       }
       fprintf(outfile, "\n");
       /*free memory */
       /*      head = attributeArray[i]-> next;
       while (head != NULL) {
               temp = head->next;
               free(head);
               head = temp;
               }*/

       //      free(attributeArray[i]);
  }
  fclose(outfile);
 }




/* END OF PROGRAM */