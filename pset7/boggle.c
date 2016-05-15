
/*
File: boggle.c
Description: This file contains a program that can be used to list every word in the standard input that is a "Boggle
word" for the NROWS x NCOLS array BOARD.
Name: Harrison Miller, hmm29
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "boggle.h"
#include "/c/cs223/Hwk3/getLine.h"

void initializeNode(trieNodePtr t) {
  for (int i = 0; i < ALPHABET_SIZE; i++) t->children[i] = NULL;
  t->isTerminal = t->used = false;
}

void insert(trieNodePtr root, char *string) {
  int pos; /* position of char in node children array */
  trieNodePtr child; /* child node of current trie node */

  for(int i = 0; i < strlen(string); i++) {
    pos = tolower(string[i]) - 'a';
    child = root->children[pos];
    if(!child) {
      child = (trieNodePtr) malloc(sizeof(trieNode));
      initializeNode(child);
      root->children[pos] = child;
    }
    root = child;
    if(i == strlen(string)-1) {
      root->isTerminal = true;
    }
  }
}

int trieNodeHasChildren(trieNodePtr t) {
 for (int i = 0; i<ALPHABET_SIZE; i++) {
   if (t->children[i] != NULL) {
     return true;
   }
 }
 return false;
}

int search(trieNodePtr root, char *string) {
  int pos; /* position of char in node children array */
  int found; /* search result */

  for(int i = 0; i < strlen(string); i++) {
    pos = tolower(string[i]) - 'a';
    if(root->children[pos] == NULL) {
      found = 0;
      return found;
    } else {
      root = root->children[pos];
    }
  }
  found = (int) root->isTerminal || trieNodeHasChildren(root);
  return found;
}

void removeNewline(char *s) {
  while(*s && *s != '\n' && *s != '\r') s++;
  *s = 0;
}

int isValidWord(char *s) {
  int c = 0;

  removeNewline(s);

  if (strlen(s) < 3) return 0;

  while(*s) {
    c = tolower(*s);
    if (!(c >= 'a' && (c <= 'z'))) return 0;
    *s++ = c;
  }
  return 1;
}

boardPtr createBoard(int NROWS, int NCOLS, char *string) {
  boardPtr board = (boardPtr) malloc(sizeof(board));
  board->NROWS = NROWS;
  board->NCOLS = NCOLS;
  board->size = NROWS*NCOLS;
  for(int i = 0; i < NROWS; i++) {
    for(int j = 0; j < NCOLS; j++) {
      board->grid[i][j] = string[i * NROWS + j];
    }
  }
  return board;
}

int main(int argc, char *argv[]) {

  int intArg; /* current arg as int */
  int NROWS = 0; /* ARG: number of board rows */
  int NCOLS = 0; /* ARG: number of board columns */
  int inputCount = 0; /* number of stdin inputs */
  bool showNonBoggle = false; /* ARG: flag to print non-Boggle words */
  bool useLettersOnce = false; /* ARG: flag to only use letters once */
  bool isValid; /* flag to signify stdin word (dictionary entry) is valid */
  char *board = NULL; /* ARG: board */
  char *input = NULL; /* STDIN: dictionary word */
  char *dict[MAX_NUM_WORDS]; /* array of inputs */

  // check for valid argument count
  if(argc < 4 || argv > 6) {
    fprintf(stderr, "Usage: %s filename. Invalid number of arguments: %d.", argv[0], argc);
    exit(EXIT_FAILURE);
  }

  for(int i = 1; i < argc; i++) {
    intArg = atoi(argv[i]);

    if(i == 1) {
      if(intArg == 0 && strcmp(argv[i], "-c") == 0) {
        showNonBoggle = true;
      } else if (intArg == 0) {
        fprintf(stderr, "Usage: %s filename. Invalid flag: %s.", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      } else {
        NROWS = intArg;
      }
    } else if(i == 2) {
      if(intArg == 0 && strcmp(argv[i], "-t") == 0) {
        useLettersOnce = true;
      } else if (intArg == 0){
        fprintf(stderr, "Usage: %s filename. Invalid flag: %s.", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      } else {
        NCOLS = intArg;
      }
    } else if(i == argc-1) {
      if(intArg == 0 && !board && strlen(argv[i]) >= NROWS * NCOLS) {
        board = argv[i];
      } else {
        fprintf(stderr, "Usage: %s filename. Invalid board argument: %s.", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      }
    } else {
      if(NROWS == 0 && intArg > 0) {
        NROWS = intArg;
      } else if(NCOLS == 0 && intArg > 0) {
        NCOLS = intArg;
      } else {
        fprintf(stderr, "Usage: %s filename. Invalid board dimension argument: %s.", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      }
    }
  }

  // ensure we have necessary setup
  if(NROWS <= 0 || NCOLS <= 0 || !board) {
    fprintf(stderr, "Usage: %s filename. Error occurred with Boggle setup.", argv[0])
    exit(EXIT_FAILURE);
  }

  boardPtr boggleBoard = createBoard(NROWS, NCOLS, board);
  trieNodePtr root = (trieNodePtr) malloc(sizeof(trieNode));
  initializeNode(root);
  root->isTerminal = true;

  int i = 0;

  // read words from stdin
  while((input = getLine(stdin)) != NULL) {
     inputCount++;

     // normalize input (make lower case and remove newlines)
     // check if it's valid, meaning no non-alphabetical characters
     isValid = isValidWord(input);

     // skip to next word in dictionary if invalid
     if (!isValid) {
       if(showNonBoggle) {
         dict[i++] = input;
       }
       continue;
     }

     insert(root, input);
     dict[i++] = input;
   }

    // walk board and check for boggle words
    // if it's a boggle word -> key: hash(input), value: 1
    // else if key already there then increment value by 1
    // else -> key: hash(input), value: 0

   int histogram[inputCount];

   // sort the hash table with radix sort
   // print values using specified format

   // free everything here

  return EXIT_SUCCESS;
}
