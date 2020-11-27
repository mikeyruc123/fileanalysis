#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

// linked list struct

typedef struct node{
  char *name;
  struct node *next;
} node;

typedef struct linkedlist{

  char *name;
  int tokens;
  node *n;
  struct linkedlist *next;

} list;

// global variables

pthread_mutex_t mut;
list database;

// functions

void addFile(char *file){

  // adds file to database

}

void addToken(char *file, char *token){

  // adds token to database

}

void *fileHandler(void *input){

  // assume file is valid (DT_REG) and accessable

  struct dirent *current = input;

  return NULL;

}

void *dirHandler(void *input){

  DIR *directory = input;

  struct dirent *current;
  while ((current = readdir(directory)) != NULL){

    if (current->d_type == DT_DIR){
      // start new pthread
      pthread_t *id;
      pthread_create(id, NULL, dirHandler, NULL);
    } else if (current->d_type == DT_REG) {
      pthread_t *id;
      pthread_create(id, NULL, fileHandler, NULL);
    }
  }

  return NULL;

}

int main(int argc, char **argv){

  // check the given path to see if it is valid or accessable

  DIR *directory = opendir(argv[1]);

  if (directory == NULL) {
    printf("%s", "Error: Invalid or inaccessable directory");
    return 1;
  }

  dirHandler((void *)directory);

}
