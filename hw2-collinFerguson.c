/*
Collin L. Ferguson
collin.l.ferguson@und.edu
assignment 2: create threads to perform tasks.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


void loadFile(char **fileData, FILE *file, long int *fileSize);
void *findTerms(void *argsv);

struct threadArgs
{
	pthread_t thread;
	char *searchTerm;
	char *fileData;
	long int fileSize;

};

int main(void) {
	system("wget -q http://undcemcs01.und.edu/~ronald.marsh/CLASS/CS451/hw3-data.txt");

	char *fileData = 0;
	long int fileSize = 0;

	FILE *file = fopen("hw3-data.txt", "r");
	if(file==0){
		printf("File not found\n");
		return 0;
	}

	loadFile(&fileData, file, &fileSize);

	pthread_t thread1, thread2;

	struct threadArgs threadStruct1, threadStruct2;
	threadStruct1.thread = thread1;
	threadStruct1.fileData = fileData;
	threadStruct1.fileSize = fileSize;
	threadStruct1.searchTerm = "easy";

	threadStruct2.thread = thread2;
	threadStruct2.fileData = fileData;
	threadStruct2.fileSize = fileSize;
	threadStruct2.searchTerm = "polar";

	pthread_create(&thread1, NULL, findTerms, &threadStruct1);
	pthread_create(&thread2, NULL, findTerms, &threadStruct2);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	free(fileData);
	fclose(file);
	system("unlink hw3-data.txt");


	return 0;
}


void *findTerms(void *argsv)
{
	struct threadArgs *threadArg = ((struct threadArgs *) argsv);
	int counter = 0;

	char *temp;
	temp = threadArg->fileData;


	while((temp = strstr(temp, threadArg->searchTerm)) != NULL)
	{
		counter++;
		temp += strlen(threadArg->searchTerm);
	}

	printf("Search Term: %s\nTotal Count: %d\n", threadArg->searchTerm, counter);

	return NULL;
}


void loadFile(char **fileData, FILE *file, long int *fileSize)
{
	fseek(file,0,SEEK_END); //seeks eof
	*fileSize = ftell(file);
	rewind(file);

	*fileData = malloc(*fileSize+1);

	char* temp; //created to populate the array, while still maintaining the spot
	temp = *fileData;

	char currentChar = fgetc(file);
	while(!feof(file))
	{

		if(currentChar>'A' && currentChar<'Z'){currentChar += 32;} //convert uppercase to lowercase.

		*temp = currentChar;
		temp++; //iterator takes it to the next spot
		currentChar = fgetc(file);
	}

	temp = 0 ;
}
