/*
Collin L. Ferguson
collin.l.ferguson@und.edu
Assignment 3: Synchronize threads using mutex
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void *readingThread(void *args);
void *writingThread(void *args);

pthread_mutex_t mutexLock;

int globalInt = 1;
int threadingStep = 0;

int main(void)
{
	if(pthread_mutex_init(&mutexLock, NULL) != 0)
	{
		printf("There was an issue initializing the mutex!!\n");
		return 0;
	}

	FILE *outFile = fopen("hw3.out", "w");
	if(outFile == NULL)
	{
		printf("The hw3.out file could not be found\n");
		return 0;
	}

	pthread_t threadRead, threadWrite;

	//pthread_mutex_lock(&mutexLock);
	pthread_create(&threadRead, NULL, readingThread, NULL);

	pthread_create(&threadWrite, NULL, writingThread, outFile);

	pthread_join(threadRead, NULL);
	pthread_join(threadWrite, NULL);

	fclose(outFile);

	return 0;
}

void *readingThread(void *args)
{
	FILE *inFile = fopen("hw3.in", "r");
	if(inFile==NULL)
	{
		printf("The hw3.in file could not be found\n");
		return 0;
	}
	//pthread_mutex_unlock(&mutexLock);
	while(!feof(inFile))
	{
		if(threadingStep == 0)
		{
			pthread_mutex_lock(&mutexLock);
			fscanf(inFile, "%d\n", &globalInt);
			threadingStep = 1;
			pthread_mutex_unlock(&mutexLock);
		}
	}
	threadingStep = 3;
	fclose(inFile);
	return NULL;
}

void *writingThread(void *args)
{
	FILE *outFile = (FILE*) args;

	while(threadingStep!= 4)
	{
		if(threadingStep == 1 || threadingStep == 3)
		{
			pthread_mutex_lock(&mutexLock);
			fprintf(outFile, "%d\n", globalInt);
			if(globalInt%2 == 0)
			{
				fprintf(outFile, "%d\n", globalInt);
			}
			if(threadingStep == 3)
			{
				threadingStep = 4;
			}
			else
			{
				threadingStep = 0;
			}
			pthread_mutex_unlock(&mutexLock);
		}
	}
	return NULL;
}
