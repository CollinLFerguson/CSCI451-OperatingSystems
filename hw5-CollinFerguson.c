/*
Collin L. Ferguson
Collin.l.ferguson@und.edu
HW5
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

void *dataThread1(void *args);
void *dataThread2(void *args);
void *dataThread3(void *args);

pthread_mutex_t mutexLock[4];

char globalChar = 't';


int main()
{

	if(pthread_mutex_init(&mutexLock[0], NULL) != 0 &&
			pthread_mutex_init(&mutexLock[1], NULL) != 0 &&
			pthread_mutex_init(&mutexLock[2], NULL) != 0 &&
			pthread_mutex_init(&mutexLock[3], NULL) != 0)
			{
				printf("There was an issue initializing the mutexs!!\n");
				return 0;
			}
	pthread_t thread[3];

	FILE *file = fopen("hw5.out", "w");
	if(file == NULL)
	{
		printf("There was an issue opening hw5-1.in");
	}

	int isRunning = 1; // Keeps track of whether the last thread is still running.

	pthread_mutex_lock(&mutexLock[0]);
	pthread_mutex_lock(&mutexLock[1]);
	pthread_mutex_lock(&mutexLock[2]);
	pthread_mutex_lock(&mutexLock[3]);

	pthread_create(&thread[0], NULL,dataThread1,NULL);
	pthread_create(&thread[1], NULL,dataThread2,NULL);
	pthread_create(&thread[2], NULL,dataThread3,&isRunning);

	while(isRunning)
	{
		pthread_mutex_unlock(&mutexLock[1]);
		pthread_mutex_lock(&mutexLock[0]);
		fprintf(file, "%c\n", globalChar);
		printf("1 - %c\n", globalChar);

		pthread_mutex_unlock(&mutexLock[2]);
		pthread_mutex_lock(&mutexLock[0]);
		fprintf(file, "%c\n", globalChar);
		printf("2 - %c\n", globalChar);

		pthread_mutex_unlock(&mutexLock[3]);
		pthread_mutex_lock(&mutexLock[0]);
		fprintf(file, "%c\n", globalChar);
		printf("3 - %c\n", globalChar);
		printf("------------------------------\n\n");

		if(!isRunning)
		{
			pthread_join(thread[0], NULL);
			pthread_join(thread[1], NULL);
			pthread_join(thread[2], NULL);
		}
	}
	return 0;
}

void *dataThread1(void *args)
{
	FILE *file = fopen("hw5-1.in", "r");
	if(file == NULL)
	{
		printf("There was an issue opening hw5-1.in");
	}
	char currentChar;
	while(!feof(file))
	{
		pthread_mutex_lock(&mutexLock[1]);
		fscanf(file, "%c\n", &currentChar);
		globalChar = currentChar;

		pthread_mutex_unlock(&mutexLock[1]);
		pthread_mutex_lock(&mutexLock[1]);
		if(!feof(file)){pthread_mutex_unlock(&mutexLock[0]);}
	}
	fclose(file);
	pthread_mutex_unlock(&mutexLock[0]);
	printf("--- Thread 1 is done\n");
	return NULL;
}

void *dataThread2(void *args)
{
	FILE *file = fopen("hw5-2.in", "r");
	if(file == NULL)
	{
		printf("There was an issue opening hw5-2.in");
	}
	char currentChar;

	while(!feof(file))
	{
		pthread_mutex_lock(&mutexLock[2]);
		fscanf(file, "%c\n", &currentChar);
		globalChar = currentChar;

		pthread_mutex_unlock(&mutexLock[2]);
		pthread_mutex_lock(&mutexLock[2]);
		if(!feof(file)){pthread_mutex_unlock(&mutexLock[0]);}
	}

	fclose(file);
	pthread_mutex_unlock(&mutexLock[0]);
	printf("--- Thread 2 is done\n");
	return NULL;
}

void *dataThread3(void *args)
{
	int *isRunning = (int *) args;
	FILE *file = fopen("hw5-3.in", "r");
	if(file == NULL)
	{
		printf("There was an issue opening hw5-3.in");
	}
	char currentChar;
	while(!feof(file))
	{
		pthread_mutex_lock(&mutexLock[3]);
		fscanf(file, "%c\n", &currentChar);
		globalChar = currentChar;

		pthread_mutex_unlock(&mutexLock[3]);
		pthread_mutex_lock(&mutexLock[3]);
		if(!feof(file)){pthread_mutex_unlock(&mutexLock[0]);}

	}
	*isRunning = 0;
	fclose(file);
	pthread_mutex_unlock(&mutexLock[0]);
	printf("--- Thread 3 is done\n");

	return NULL;
}

