/*****************************************************************************
 File:   minimax.c
 Author: Clare Bates Congdon
 Date:   Mon Sep 28 21:05:55 2009

 Description:  Play minimax Tic-Tac-Toe, for Lab 3
   FWIW, Edited from p6.c

 Notes:
 1. Change DIMENSION to move from 3x3 to 4x4, etc.
    but then also have to change gameOver function, etc.
 2. players are case-sensitive 'x' or 'o'
    'x' is maximizing
    'o' is minimizing

 Data Structures:
 Node for the search tree holds a board, a 2d array of characters
   node->board[i][j] is the mark at row=i, col=j, which might be x, o, or blank

Yes, this is not optimal code.

Incomplete functions to be filled out:
1. getMinimaxScore
   return the best score for this player

2. bestMove
   given the children (with their minimax scores), pick the best move for this
   player 

(*. Cleaning up memory; not a distinct function.)

3. staticEvaluator
   given a board, return a score
     high scores good for x (the maximizing player)
   (this does not have to be the smartest function ever, but you can do better
   than returning a random number)

4. gameOver function is cheating, and just counting the number of moves
   means that we have to play to nine moves, even with a win, until this is
   fixed.
   
Should do, but we probably won't have time:
1. Free (or reuse)  memory from lookahead search
2. We're creating a chain of boards, but not using them... should also clean
   up that memory
3. Lookahead should check to see if game is over, but doesn't
4. There are plenty of refinements we could make to this, e.g., in terms of
   what to print when
5. You can't actually play this game; the computer does both players
   if you allowed human players, you'd also need error checking, etc.

******************************************************************************/

/*****************************************************************************/
/* include files */
#include <stdio.h>   /* standard input and output */
#include <stdlib.h>  /* standard library, including memory allocation */
#include <math.h>    /* math library */

/*****************************************************************************/
/* constants -- no semicolons at the ends of the lines */
#define TRUE 1       
#define FALSE 0
#define STRLEN 100   /* length (in chars) for strings */
#define DIMENSION 3  /* "real" TTT is 3x3, but we can change that here */
#define DEPTH 2      /* how far to lookahead for minimax */

/*****************************************************************************
 data structures:
 *****************************************************************************/
typedef struct Node *NodeP; /* pointer to a Node structure, for Tree */
struct Node {
  char **board;           /* the board at this state (2d array of chars) */  
  int staticScore;        /* score of this board from the static evaluator */
  int mmScore;            /* score of this board from minimax */
  NodeP* offspring;       /* array of offspring nodes */
};

/*****************************************************************************/
/* global variables */
int MoveCount=0;   /* hacky way to know the end of the game */

/*****************************************************************************/
/* prototypes */

void playGame (NodeP node, char player);
int gameOver(NodeP node);
int bestMove(NodeP node, char player);
void lookahead (NodeP node, char player, int depth);
int staticEvaluator (NodeP node);
int getMinimaxScore (NodeP node, char player);
char opponent (char player);

NodeP newNode (void);
NodeP makeNodeCopy (NodeP original);
void freeNode (NodeP node);
void printNode (NodeP node);
void openFile(FILE **fileptr, char *filename, char *mode);
void *emalloc(long size);

/*****************************************************************************
 main
******************************************************************************/
int main(int argc, char* argv[]) {

  NodeP root;  /* the root of the search tree */

  root = newNode(); /* make the initial blank board */

  playGame(root, 'x');  /* take turns with x and o; x is first */

  freeNode(root);   /* free the tree */

  return(0);        /* return a value */

}  /* end main */


/*****************************************************************************
 Function: playGame
 Inputs:   node for this board, plus player (to get to this board)
 Returns:  nothing
 Description: play the game of TTT
 *****************************************************************************/
void playGame (NodeP node, char player) {

  int bestChild;   /* which of the children to move to */
  NodeP nextNode;  /* the node for that child */

  if (!gameOver(node)) { /* if we're not yet done */

    printNode(node);     /* print the board and scores */

    printf("Next move by player %c\n", player);
    
    lookahead(node, player, DEPTH);      /* construct the lookahead tree */

    bestChild = bestMove(node, player);  /* the best move for the player */
    nextNode = node->offspring[bestChild];

    /**** should free offspring of bestChild, plus all the other children */

    MoveCount++;
    playGame(nextNode, opponent(player));  /* recurse */
  }

  else {  /* game is over */
    printf("Final board is: \n");
    printNode(node);
  }

} /* end playGame */

/*****************************************************************************
 Function: gameOver
 Inputs:   node
 Returns:  1 if game is over; 0 if game is not over
 Description: cheating for now; just counting moves
   to fix: should check for three in a row or full board
     should also print out who won or that it's a cat's game
 *****************************************************************************/
int gameOver(NodeP node) {
  if (MoveCount == DIMENSION*DIMENSION)
    return 1;
  else
    return 0;
}

/*****************************************************************************
 Function: lookahead
 Inputs:   rootNode, and player
 Returns:  nothing
 Description: build the lookahead search tree
 *****************************************************************************/
void lookahead (NodeP node, char player, int depth) {

  int i, j;      /* loop counters, for the positions of the board */
  int childNum;  /* count how many children (same as # of blanks) */

  childNum = 0;  /* initialize */

  /* get the static score of this node */
  node->staticScore = staticEvaluator(node);

  /* if we haven't reached the max depth, create the offspring and recurse */
  if (depth != 0) {

    for (j=0; j<(DEPTH-depth); j++)  /* indent */
      printf("  ");
    printf("making offspring for player %c at level %i\n", player, depth);

    /* make a new child node for each blank spot, 
       same as parent, but with an extra mark in that blank */
    for (i=0; i<DIMENSION; i++) {
      for (j=0; j<DIMENSION; j++) {
        if (node->board[i][j] == ' ') {
          node->offspring[childNum] = makeNodeCopy(node);
          node->offspring[childNum]->board[i][j] = player;
          childNum++;
        } 
      } /* done with columns */
    }   /* done with all rows */

    /* recurse with each child
       it'll be the opponent's turn at the next level */
    for (i=0; i<childNum; i++) {
      lookahead(node->offspring[i], opponent(player), depth-1);

      /* here is where the score propagates up */
      node->offspring[i]->mmScore 
        = getMinimaxScore(node->offspring[i], opponent(player));

      /* print the score for this node */
      for (j=0; j<=(DEPTH-depth); j++)  /* indent */
        printf("  ");
      printf("-->score of child is %i\n", node->offspring[i]->mmScore);
    }

  } /* end, max depth not reached */


} /* end lookahead */


/*****************************************************************************
 Function: staticEvaluator
 Inputs:   a board
 Returns:  score of the board
 Description: send back a score; remember that this is independent of the
   player. High scores are good for player 'x' (maximizing) and low scores are
   good for player 'o' (minimizing)
 *****************************************************************************/
int staticEvaluator (NodeP node) {
  int score;

  score = rand() % 9;   /* random score for now */

  return (score);
} /* end staticEvaluator */


/*****************************************************************************
 Function: bestMove
 Inputs:   node, and player
 Returns:  integer index of the best move (offspring) to take
 Description:
   chose the best move for the given player from among the offspring
 *****************************************************************************/
int bestMove(NodeP node, char player) {
  int index, value, i, score;

  /* shouldn't happen, but let's check for it anyway */
  if (node == NULL) {
    printf("error in bestMove! node is NULL!\n");
    printf("exiting...\n");
    exit(0);
  }

  /*** here you determine the best move for this player and return that index
       (index into the offspring array) ***/
  /* remember that x is maximizing (so find the highest score)
     and o is minimizing (so find the lowest score) */
  /* this is a lot like getMinimaxScore, but you're returning the move, not
     the score */
  value = 0;
  index = 0;
  score = 0;
  i = 0;

  for( i=0; i < DIMENSION*DIMENSION; i++ ){
    if( node->offspring[ i ] != NULL ){
      if( player == 'x' ){
        value = node->offspring[ i ]->mmScore;
        if( value > score ){
          score = value;
          index = i;
        }
      } else {
        value = node->offspring[ i ]->mmScore;
        if( value < score ){
          score = value;
          index = i;
        }
      }
    }
  }

  return index;
} /* end bestMove*/


/*****************************************************************************
 Function: getMinimaxScore
 Inputs:   a board and a player
 Returns:  score of the board, using minimax
 Description: look at all the offspring of this node and the player at this
   level... propagate the best score for this player

   x should return the highest score
   o should return the lowest score
 *****************************************************************************/
int getMinimaxScore (NodeP node, char player) {
  int score;  /* the score to return */
  int i;
  int value;

  /* shouldn't happen, but let's check for it anyway */
  if (node == NULL) {
    printf("error in getMinimaxScore! node is NULL!\n");
    printf("exiting...\n");
    exit(0);
  }

  /* if there are no offspring, mmScore is staticScore */
  else if (node->offspring[0] == NULL) {
    score = node->staticScore;
    return(score);
  }

  value = 0;
  i = 0;
  for( i=0; i < DIMENSION*DIMENSION; i++ ){
    if( node->offspring[ i ] != NULL ){
      if( player == 'x' ){
        value = getMinimaxScore( node->offspring[ i ], 'y' );
        if( value > score ){
          score = node->offspring[ i ]->mmScore;
        }
      } else {
        value = getMinimaxScore( node->offspring[ i ], 'x' );
        if( value < score ){
          score = node->offspring[ i ]->mmScore;
        }
      }
    }
  }


/*  score = node->offspring[0]->mmScore;  / * take the first score for now */

  /*** here, you return the correct minimax score ***/
  /* remember that x is maximizing (so find the highest score)
     and o is minimizing (so find the lowest score) */

  return (score);
} /* end getMinimaxScore */



/*****************************************************************************
 Function: opponent
 Inputs:   player, 'x' or 'o'
 Returns:  the other player's mark
 Description: return the other player's mark or signal an error
 *****************************************************************************/
char opponent (char player) {
  if (player == 'x')
    return 'o';
  else if (player == 'o')
    return 'x';
  else {
    printf("error! player is %c\n", player);
    return 'E';
  }

} /* end opponent */

/*****************************************************************************
 node allocation, freeing, printing
 *****************************************************************************/

/*****************************************************************************
 Function: makeNodeCopy
 Inputs:   a node
 Returns:  pointer to the new Node, a copy of the original board
 Description: copy the board, but leave scores and offspring 0/null
 *****************************************************************************/
NodeP makeNodeCopy (NodeP original) {
  NodeP copy;   /* the copied node */
  int i, j;     /* loop counters */

  copy = newNode();   /* get a new node */

  /* copy the board */
  for (i=0; i<DIMENSION; i++) {
    for (j=0; j<DIMENSION; j++) {   /* fill in with blanks */
      copy->board[i][j] = original->board[i][j];
    }
  }

  return (copy);

} /* end makeNodeCopy */


/*****************************************************************************
 Function: newNode
 Inputs:   none
 Returns:  pointer to the new Node for the tree
 Description: allocates and initializes memory or fails with error message
 *****************************************************************************/
NodeP newNode (void) {
  NodeP node;  /* the new node to return */
  int i,j;       /* loop counter */
        
  node = (NodeP) emalloc (sizeof (struct Node));
        
  if (node == NULL) {
    printf("ERROR: emalloc failed for new Node\n");
    exit(0);
  }

  /* initialize fields */

  /* board is a 2d array of chars
     means that 1st index is an array of strings
     need one more for 2nd dimension for null termination
     ... not required, but easier printing in some situations */
  node->board = (char **) emalloc (DIMENSION * sizeof(char *));
  for (i=0; i<DIMENSION; i++) {
    node->board[i] = (char *) emalloc ((DIMENSION+1) * sizeof(char));
    for (j=0; j<DIMENSION; j++) {   /* fill in with blanks */
      node->board[i][j] = ' ';      
    }
    node->board[i][j] = '\0';       /* null termination for string */
  }

  node->staticScore = 0;
  node->mmScore = 0;
  
  /* max number of children is at the root node -- one per square */
  /* not all the pointers will be used, so be sure to set them to NULL */
  node->offspring = (NodeP*) emalloc (DIMENSION*DIMENSION * sizeof(NodeP));
  for (i=0; i<DIMENSION*DIMENSION; i++) {
    node->offspring[i] = NULL;
  }

  return (node);

} /* end newNode */


/*****************************************************************************
 Function: freeNode
 Inputs:   Node
 Returns:  nothing
 Description:
   free the memory in the Node
   note that you have to work from the bottom up
   --> all the frees are in the opposite order of the emallocs above
 *****************************************************************************/
void freeNode (NodeP node) {

  int i;  /* loop counter */

  /* free all the offspring, if they are there, and then the array itself */
  for (i=0; i<DIMENSION*DIMENSION; i++) {
    if (node->offspring[i] != NULL)
      freeNode(node->offspring[i]);
  }
  free(node->offspring);

  /* free the rows, and then the board itself */
  for (i=0; i<DIMENSION; i++) {
    free(node->board[i]);
  }
  free(node->board);

  /* free this node */
  free(node);

} /* end freeNode */


/*****************************************************************************
 Function: printNode
 Inputs:   node with a board in it
 Returns:  nothing
 Description:
   print out the board for this node
 *****************************************************************************/
void printNode (NodeP node) {

  int i,j;  /* loop counters */

  /* just to test node->board[0][1] = 'o'; */

  printf("Board is: \n");

  for (i=0; i<DIMENSION; i++) {
    /* possible, but doesn't look as "pretty":
       printf("  %s\n", node->board[i]);
    */

    printf("  ");
    for (j=0; j<DIMENSION; j++) {
      printf("%c ", node->board[i][j]);
    }
    printf("\n");

  }
  printf("static score: %i\n", node->staticScore);
  printf("minimax score: %i\n", node->mmScore);

  printf("\n");

} /* end printNode */


/*****************************************************************************
 utility functions
 *****************************************************************************/

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
void openFile(FILE **fileptr, char *filename, char *mode) {
  *fileptr = fopen(filename,mode);       /* open the file */

  if ( !(*fileptr)) {
    printf("ERROR: Failed to open file: %s with mode %s.  Exiting.\n\n",
           filename, mode);
    exit(0);
  }

} /* end openFile */


/****** END OF FILE **********************************************************/
