
/******************************************************************************
 * Boggle.c
 * This file contains a program that can be used to list every word in the
 * standard input that is a "Boggle word" for the NROWS x NCOLS array BOARD.
 *
 * Harrison Miller
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Boggle.h"

/*
 *  Function: getWord
 *  --------------------
 *  reads and parses words from FILE object
 *
 *  fp: file stream
 *
 *  returns: word to lower case with terminal newlines removed
 */
char *getWord(FILE *fp) {
    // modified getLine
    char *input;
    int size;
    int c, i;

    size = sizeof(double);
    input = malloc (size);
    for (i = 0;  (c = getc(fp)) != EOF; )  {
        if (i == size-1) {
            size *= 2;
            input = realloc (input, size);
        }
	      input[i++] = tolower(c);
        if (c == '\n') {
            ungetch(c, fp);
            break;
        }
    }

    if (c == EOF && i == 0)  {
        free (input);
        return NULL;
    }

    input[i++] = '\0';
    input = realloc(input, i);
    return input;
}

/*
 *  Function: isValidWord
 *  --------------------
 *  reads and parses words from stdin
 *
 *  str: string to be checked
 *
 *  returns: whether string is a valid input
 */
int isValidWord(char *str) {
  int c = 0;
  if(!str) return 0;
  if(!isalpha(str[0])) return 0;
  while(*str) {
    c = tolower(*str);
    if(!isalpha(c)) return 0;
    if (!(c >= 'a' && (c <= 'z'))) return 0;
    *str++ = c;
  }
  return 1;
}

/*
 *  Function: makeNode
 *  --------------------
 *  makes and initializes a new trie node
 *
 *  returns: pointer to the new trie node
 */
trieNodePtr makeNode(void) {
  trieNodePtr t = (trieNodePtr) malloc(sizeof(TrieNode));
  for (int i = 0; i < ALPHABET_SIZE; i++) t->children[i] = NULL;
  t->count = 0;
  t->word = NULL;
  return t;
}

/*
 *  Function: insertWord
 *  --------------------
 *  reads and parses words from stdin
 *
 *  root: root of the trie
 *  word: string to be inserted
 *
 */
void insertWord(trieNodePtr root, char *word) {
  int pos; /* position of char in node children array */
  trieNodePtr child; /* child node of current trie node */

  for(int i = 0; i < strlen(word); i++) {
    pos = tolower(word[i]) - 'a';
    child = root->children[pos];
    if(!child) {
      child = makeNode();
      root->children[pos] = child;
    }
    // if at the end of the word (i = strlen-1)
    // and t->word is null, t->word = word
    if(i == strlen(word)-1 && !root->word) {
      root->word = word;
      return;
    }
    root = child;
  }
}

/*
 *  Function: makeBoard
 *  --------------------
 *  makes and initializes a new Boggle board
 *
 *  NROWS: number of rows
 *  NCOLS: number of columns
 *  letters: board characters as one-dimensional string
 *
 *  returns: pointer to the new Boggle board
 */
boardPtr makeBoard(int NROWS, int NCOLS, char *letters) {
  boardPtr board = (boardPtr) malloc(sizeof(Board));
  board->NROWS = NROWS;
  board->NCOLS = NCOLS;

  for(int i = 0; i < NROWS; i++) {
    for(int j = 0; j < NCOLS; j++) {
     board->grid[i][j] = letters[i*NROWS+j];
    }
  }
  return board;
}

/*
 *  Function: traverseUtil
 *  --------------------
 *  helper function for traverse that recursively walks the board
 *
 *  board: the Boggle board
 *  trie: the trie of input words
 *  row: index of row
 *  col: index of column
 *  seen: matrix to mark which board positions have been visited
 *  noReuse: flag to indicate whether board letters can be revisited
 *
 */
void traverseUtil(boardPtr board, trieNodePtr trie, int row, int col,
  int seen[], int noReuse) {
  if(!trie) return;
  if(noReuse && seen[row * board->NROWS + col] == 1) return;

  trie->count++;

  int nrow, ncol;
  int nseen[board->NROWS * board->NCOLS];
  int pos;

  // update the seen positions matrix
  for(int i = 0; i < board->NROWS * board->NCOLS; i++) nseen[i] = seen[i];

  // check all letters around current position
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      nrow = row + i;
      ncol = col + j;
      // if neighbor position is valid
      if(nrow >= 0 && nrow < board->NROWS && ncol >= 0 && ncol < board->NCOLS){
        if(board->grid[nrow][ncol] == '_') {
          for(int i = 0; i < ALPHABET_SIZE; i++) {
            traverseUtil(board, trie->children[i], nrow, ncol, nseen, noReuse);
          }
        }
        // if noReuse flag, ensure that neighbor has not been visited
        if(noReuse && seen[nrow * board->NROWS + ncol] == 0) {
          seen[nrow * board->NROWS + ncol] = 1;
          pos = board->grid[nrow][ncol] - 'a';
          traverseUtil(board, trie->children[pos], nrow, ncol, nseen, noReuse);
        } else if (!noReuse) {
          seen[nrow * board->NROWS + ncol] = 1;
          pos = board->grid[nrow][ncol] - 'a';
          traverseUtil(board, trie->children[pos], nrow, ncol, nseen, noReuse);
        }
      }
    }
  }
}

/*
 *  Function: traverse
 *  --------------------
 *  walk the Boggle board
 *
 *  board: the Boggle board
 *  trie: the trie of input words
 *  noReuse: flag to indicate whether board letters can be revisited
 *
 */
void traverse(boardPtr board, trieNodePtr trie, int noReuse) {
  int seen[board->NROWS * board->NCOLS];

  for (int row = 0; row < board->NROWS; row++) {
    for (int col = 0; col < board->NCOLS; col++) {
        // initialize all seen matrix positions to 0 (unseen)
        for (int i = 0; i < board->NROWS * board->NCOLS; i++) seen[i] = 0;

        // mark current letter as seen
        seen[(row * board->NROWS) + col] = 1;

        // traverse all child nodes
        for(int i = 0; i < ALPHABET_SIZE; i++) {
          traverseUtil(board, trie->children[i], row, col, seen, noReuse);
        }
    }
  }
}

/*
 *  Function: printWords
 *  --------------------
 *  print trie words in alphabetical order
 *
 *  root: root of the trie
 *  showNonBoggleWords: flag to indicate if non-Boggle words should be printed
 *
 */
void printWords(trieNodePtr root, int showNonBoggleWords) {
  int i;
  if (!root) return;

  // check that word exists
  if(showNonBoggleWords && root->count == 0 && root->word) {
    printf("%s\n", root->word);
  } else if (!showNonBoggleWords && root->count > 0 && root->word) {
    printf("%s: %d\n", root->word, root->count);
  }

  for (i = 0; i < ALPHABET_SIZE; i++)
  {
    printWords(root->children[i], showNonBoggleWords);
  }

}

int main(int argc, char *argv[]) {

  int intArg; /* current arg as int */
  int NROWS = 0; /* ARG: number of board rows */
  int NCOLS = 0; /* ARG: number of board columns */
  int showNonBoggleWords = 0; /* ARG: flag to print all non-Boggle words */
  int noReuse = 0; /* ARG: flag to only use letters once */
  char *letters = NULL; /* ARG: board letters */
  char *input = NULL; /* STDIN: dictionary word */

  // check for valid argument count
  if(argc < 4 || argc > 6) {
    fprintf(stderr,
      "Usage: %s filename.
      Invalid number of arguments: %d.\n", argv[0], argc);
    exit(EXIT_FAILURE);
  }

  for(int i = 1; i < argc; i++) {
    intArg = atoi(argv[i]);

    if(i == 1) {
      if(intArg == 0 && strcmp(argv[i], "-c") == 0) {
        showNonBoggleWords = 1;
      } else if (intArg == 0) {
        fprintf(stderr,
          "Usage: %s filename.
          Invalid flag: %s.\n", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      } else {
        NROWS = intArg;
      }
    } else if(i == 2) {
      if(intArg == 0 && strcmp(argv[i], "-t") == 0) {
        noReuse = 1;
      } else if (intArg == 0){
        fprintf(stderr,
          "Usage: %s filename.
          Invalid flag: %s.\n", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      } else {
        NCOLS = intArg;
      }
    } else if(i == argc-1) {
      if(intArg == 0 && !letters && strlen(argv[i]) == NROWS * NCOLS) {
        letters = argv[i];
      } else {
        fprintf(stderr,
          "Usage: %s filename.
          Invalid board argument: %s.\n", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      }
    } else {
      if(NROWS == 0 && intArg > 0) {
        NROWS = intArg;
      } else if(NCOLS == 0 && intArg > 0) {
        NCOLS = intArg;
      } else {
        fprintf(stderr,
          "Usage: %s filename.
          Invalid board dimension argument: %s.\n", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      }
    }
  }

  // ensure we have necessary setup
  if(NROWS < 1 || NCOLS < 1 || !letters) {
    fprintf(stderr,
      "Usage: %s filename.
      Error occurred with Boggle setup.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // make board
  boardPtr board = makeBoard(NROWS, NCOLS, letters);

  // make trie
  trieNodePtr root = makeNode();

  // read words from stdin
  while((input = getWord(stdin)) != NULL) {
     if (!isValidWord(input)) {
      continue;
     }
     insertWord(root, input);
   }

   // walk board and count words
   traverse(board, root, noReuse);

   // print
   printWords(root, showNonBoggleWords);

  return EXIT_SUCCESS;
}