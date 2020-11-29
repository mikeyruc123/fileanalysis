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
  double occurence;
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
pthread_barrier_t bar;
list database;
int isEmpty = 1;

// functions

void addFile(char *file){
// adds file to database
	pthread_mutex_lock(mut);
	list *cur = &database;
	if(isEmpty)
	{
		database.name = malloc(sizeof(file));
		strcpy(database.name, file);
		database.tokens = 0;
		database.n = NULL;
		database.next = NULL;
		isEmpty = 0;
		return;
	}
	while(cur->next != NULL)
	{
		cur = cur->next;
	}
	cur->next = malloc(sizeof(list));
	cur->next->name = malloc(sizeof(file));
	strcpy(cur->next->name, file);
	cur->next->tokens = 0;
	cur->next->n = NULL;
	cur->next->next = NULL;
	pthread_mutex_unlock(mut);
	return;

}

void addToken(char *file, char *token){
// adds token to database
	// assumes file exists
	list *cur = &database;
	while(strcmp(cur->name, file) != 0)
	{
		cur = cur->next;
	}

	node *cur_token = cur->n;

	if(cur_token == NULL)
	{
		cur->n = malloc(sizeof(node));
		cur->n->name = mallloc(sizeof(token));
		strcpy(cur->n->name, token);
		cur->n->occurence++;
		cur->n->next = NULL;
		return;
	}

	while(strcmp(cur_token->name, token) != 0 && cur_token->next != NULL)
	{
		cur_token = cur_token->next;
	}

	if(strcmp(cur_token->name, token) == 0)
	{
		cur->tokens++;
		cur_token->occurence++;
	}

	else
	{
		cur->tokens++;
		cur_token->next = malloc(sizeof(node));
		cur_token->next->name = malloc(sizeof(token));
		strcpy(cur_token->next->name, token);
		cur_token->next->occurence++;
		cur_token->next->next = NULL;
	}
	return;

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

  pthread_barrier_wait(&bar);

  return NULL;

}

int main(int argc, char **argv){

  // check the given path to see if it is valid or accessable

  pthread_mutex_init(&mut, NULL);

  DIR *directory = opendir(argv[1]);

  if (directory == NULL) {
    printf("%s", "Error: Invalid or inaccessable directory");
    return 1;
  }

  pthread_t *a;

  pthread_create(a, NULL, dirHandler, (void *)directory);

  pthread_barrier_wait(&bar);

}
