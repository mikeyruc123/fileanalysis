#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

// linked list struct

typedef struct node{
  char *name;
  double occurence;
  double meanProb;
  struct node *next;
} node;

typedef struct linkedlist{

  char *name;
  int tokens;
  node *n; 
  struct linkedlist *next;

} list;

typedef struct JSD_list
{
	double value;
	char *file1;
	char *file2;
	int TotalTokens;
	struct JSD_list *next;

}JSD_list;

// global variables

pthread_mutex_t mut;
pthread_barrier_t bar;
list database;
int isEmpty = 1;

// functions

void addFile(char *file){
// adds file to database
	pthread_mutex_lock(&mut);
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
	pthread_mutex_unlock(&mut);
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
		cur->n->name = malloc(sizeof(token));
		strcpy(cur->n->name, token);
		cur->n->occurence = 1;
		cur->n->next = NULL;
		cur->tokens++;
		return;
	}
	if(strcmp(cur_token->name, token) > 0)
	{
		node *new_node = malloc(sizeof(node));
		new_node->name = malloc(sizeof(token));
		strcpy(new_node->name, token);
		new_node->occurence = 1;
		new_node->next = cur_token;
		cur->tokens++;
		cur->n = new_node;
	}

	while(strcmp(cur_token->name, token) != 0 && cur_token->next != NULL && strcmp(cur_token->next->name, token) < 0)
	{
		cur_token = cur_token->next;	
	}

	if(strcmp(cur_token->name, token) == 0)
	{
		cur->tokens++;
		cur_token->occurence++;
	}

	else if (cur_token->next == NULL)
	{
		cur->tokens++;
		cur_token->next = malloc(sizeof(node));
		cur_token->next->name = malloc(sizeof(token));
		strcpy(cur_token->next->name, token);
		cur_token->next->occurence = 1;
		cur_token->next->next = NULL;	
	}
	else
	{
		node *new_node = malloc(sizeof(node));
		new_node->name = malloc(sizeof(token));
		strcpy(new_node->name, token);
		new_node->occurence = 1;
		cur->tokens++;
		new_node->next = cur_token->next;
		cur_token->next = new_node;
	}
	return;

}

void *fileHandler(void *input){

  // assume file is valid (DT_REG) and accessable

  

  char *buf = malloc(256);

  FILE *fp = fopen(input, "r");
  addFile(input);

  int i = 0;
  while (fscanf(fp, "%s", buf) != EOF){

    for (i = 0; i < strlen(buf); i++){
      buf[i] = tolower(buf[i]);
    }

    addToken(input, buf);

  }

  return NULL;

}

void *dirHandler(void *input){

  DIR *directory = input;

  struct dirent *current;
current = readdir(directory);
  while (current != NULL){

    if (current->d_type == DT_DIR){
      // start new pthread
      pthread_t id;
      pthread_create(&id, NULL, dirHandler, (void *)current->d_name);
    } else if (current->d_type == DT_REG) {
      pthread_t id;
      pthread_create(&id, NULL, fileHandler, (void *)current->d_name);
    }
	current = readdir(directory);
  }

  return NULL;

}

int main(int argc, char **argv){

  // check the given path to see if it is valid or accessable

  pthread_mutex_init(&mut, NULL);

  DIR *directory = opendir(argv[1]);

  if (directory == NULL) {
    printf("%s\n", "Error: Invalid or inaccessable directory");
    return 1;
  }

  pthread_t a;

  pthread_create(&a, NULL, dirHandler, (void *)directory);

  pthread_barrier_wait(&bar);
	printf("test");
	
	/*list *cur = &database;

	JSD_list answers;
	JSD_list *head = &answers;
	JSD_list *ptr = &answers;
	int size = 0;
	

	while(cur != NULL)
	{
		list *pair = cur->next;
		while(pair != NULL)
		{
			int meanEmpty = 1;
			node meanTokens;
			node *meanptr = &meanTokens;
			node *cur_token = cur->n;
			node *pair_token = pair->n;
			while(cur_token != NULL)
			{
				meanptr = &meanTokens;
				if(meanEmpty)
				{
					meanTokens.name = malloc(sizeof(cur_token->name));
					strcpy(meanTokens.name, cur_token->name);
					meanTokens.meanProb =(cur_token->occurence / cur->tokens);
					meanTokens.next = NULL;
					meanEmpty = 0;
				}
				else
				{
					while(meanptr != NULL)
					{
						if(strcmp(meanptr->name, cur_token->name) == 0)
						{
							meanptr->meanProb += (cur_token->occurence / cur->tokens);
							break;
						}
						else if(meanptr->next == NULL)
						{
							meanptr->next = malloc(sizeof(node));
							meanptr->next->name = malloc(sizeof(cur_token->name));
							strcpy(meanptr->next->name, cur_token->name);
							meanptr->next->meanProb = (cur_token->occurence / cur->tokens);
							meanptr->next->next = NULL;
							break;
						}
						else
						{
							meanptr = meanptr->next;
						}
					}		
				}
				cur_token = cur_token->next;
			}
			while(pair_token != NULL)
			{
				meanptr = &meanTokens;
				if(meanEmpty)
				{
					meanTokens.name = malloc(sizeof(pair_token->name));
					strcpy(meanTokens.name, pair_token->name);
					meanTokens.meanProb =(pair_token->occurence / pair->tokens);
					meanTokens.next = NULL;
					meanEmpty = 0;
				}
				else
				{
					while(meanptr != NULL)
					{
						if(strcmp(meanptr->name, pair_token->name) == 0)
						{
							meanptr->meanProb += (pair_token->occurence / pair->tokens);
							break;
						}
						else if(meanptr->next == NULL)
						{
							meanptr->next = malloc(sizeof(node));
							meanptr->next->name = malloc(sizeof(pair_token->name));
							strcpy(meanptr->next->name, pair_token->name);
							meanptr->next->meanProb = (pair_token->occurence / pair->tokens);
							meanptr->next->next = NULL;
							break;
						}
						else
						{
							meanptr = meanptr->next;
						}
					}		
				}
				pair_token = pair_token->next;
			}
			meanptr = &meanTokens;
			while(meanptr !=NULL)
			{
				meanptr->meanProb/=2;
				meanptr = meanptr->next;
			}
			double curKLD= 0;
			double pairKLD= 0;
			cur_token = cur->n;
			pair_token = pair->n;
			while(cur_token != NULL)
			{
				meanptr = &meanTokens;
				while(meanptr!=NULL)
				{
					if(strcmp(cur_token->name, meanptr->name) == 0)
					{
						double FirstX = cur_token->occurence / cur->tokens;
						double MeanX = meanptr->meanProb;
						curKLD += FirstX * (log(FirstX/MeanX));
						break;
					}
					else
					{
						meanptr = meanptr->next;
					}
				}
				cur_token = cur_token->next;
			}
			while(pair_token != NULL)
			{
				meanptr = &meanTokens;
				while(meanptr!=NULL)
				{
					if(strcmp(pair_token->name, meanptr->name) == 0)
					{
						double SecondX = pair_token->occurence / pair->tokens;
						double MeanX = meanptr->meanProb;
						pairKLD += SecondX * (log(SecondX/MeanX));
						break;
					}
					else
					{
						meanptr = meanptr->next;
					}
				}
				pair_token = pair_token->next;
			}
			double JSD = (curKLD + pairKLD)/2;
			ptr = head;
			if(size == 0)
			{
				answers.value = JSD;
				answers.file1 = malloc(sizeof(cur->name));
				answers.file2 = malloc(sizeof(pair->name));
				strcpy(answers.file1, cur->name);
				strcpy(answers.file2, pair->name);
				answers.TotalTokens = cur->tokens + pair->tokens;
				answers.next = NULL;
				
			}
			else if(cur->tokens + pair->tokens < head->TotalTokens)
			{
				JSD_list *new_head = malloc(sizeof(JSD_list));
				new_head->value = JSD;
				new_head->file1 = malloc(sizeof(cur->name));
				new_head->file2 = malloc(sizeof(pair->name));
				strcpy(new_head->file1, cur->name);
				strcpy(new_head->file2, pair->name);
				new_head->TotalTokens = cur->tokens + pair->tokens;
				new_head->next = head;
				head = new_head;	
			}
			else
			{
				while(ptr->next != NULL && ptr->next->TotalTokens < (cur->tokens + pair->tokens))
				{
					ptr = ptr->next;
				}
				if(ptr->next == NULL)
				{
					JSD_list *new_node = malloc(sizeof(JSD_list));
					new_node->value = JSD;
					new_node->file1 = malloc(sizeof(cur->name));
					new_node->file2 = malloc(sizeof(pair->name));
					strcpy(new_node->file1, cur->name);
					strcpy(new_node->file2, pair->name);
					new_node->TotalTokens = cur->tokens + pair->tokens;
					new_node->next = NULL;
					ptr->next = new_node;
				}
				else
				{
					JSD_list *new_node = malloc(sizeof(JSD_list));
					new_node->value = JSD;
					new_node->file1 = malloc(sizeof(cur->name));
					new_node->file2 = malloc(sizeof(pair->name));
					strcpy(new_node->file1, cur->name);
					strcpy(new_node->file2, pair->name);
					new_node->TotalTokens = cur->tokens + pair->tokens;
					new_node->next = ptr->next;
					ptr->next = new_node;
				}
			}
			size++;
			pair = pair->next;
			
		}
		cur = cur->next;
	}
	ptr = head;
	while(ptr != NULL)
	{
		double value = ptr->value;
		if(value >= 0 && value <= 0.1)
		{
			//print value in red
			printf("%s%f", "\x1B[31m", value);
			printf("%s" , "\x1B[0m");
		}
		if(value > 0.1 && value <= 0.15)
		{
			//print value in yellow
			printf("%s%f", "\x1B[33m", value);
			printf("%s" , "\x1B[0m");
		}
		if(value > 0.15 && value <= 0.2)
		{
			//print value in green
			printf("%s%f", "\x1B[32m", value);
			printf("%s" , "\x1B[0m");
		}
		if(value > 0.2 && value <= 0.25)
		{
			//print value in cyan
			printf("%s%f", "\x1B[36m", value);
			printf("%s" , "\x1B[0m");
		}
		if(value > 0.25 && value <= 0.3)
		{
			//print value in blue
			printf("%s%f", "\x1B[34m", value);
			printf("%s" , "\x1B[0m");
		}
		if(value > 0.3)
		{
			//print value in white
			printf("%s%f", "\x1B[37m", value);
			printf("%s" , "\x1B[0m");
		}
		printf("%s%s%s%s%s%s\n" , " \"" , ptr->file1 , "\" " , "and \"" , ptr->file2 , "\"");
		ptr = ptr->next;
	}*/
	return 0;
	

}
