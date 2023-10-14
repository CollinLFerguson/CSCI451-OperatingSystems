/*
Collin L. Ferguson
collin.l.ferguson@und.edu
assignment 4: use threads
 */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>

pthread_mutex_t mutexLock;
float accountTotal = 0;

void *accountChangeThreads(void *args);

int main()
{
	if(pthread_mutex_init(&mutexLock, NULL) != 0)
		{
			printf("There was an issue initializing the mutex!!\n");
			return 0;
		}

	pthread_t threads[5];

	pthread_create(&threads[1], NULL,accountChangeThreads,(void *) fopen("data1.in", "r"));
	pthread_create(&threads[2], NULL,accountChangeThreads,(void *) fopen("data2.in", "r"));
	pthread_create(&threads[3], NULL,accountChangeThreads,(void *) fopen("data3.in", "r"));
	pthread_create(&threads[4], NULL,accountChangeThreads,(void *) fopen("data4.in", "r"));
	pthread_create(&threads[5], NULL,accountChangeThreads,(void *) fopen("data5.in", "r"));

	pthread_exit(0);
	return 0;
}

/*
Creates a linear chain of threads that run right after another.
**To make them run at the same time, the R critical section should be skipped and a DeltaBalance variable should be created
**to keep track of the account changes in the file.
**The only critical section should be when adding deltaBalance to the global storage.
 */

void *accountChangeThreads(void *args)
{
	FILE *file = (FILE*) args;
	char *currentLine = NULL;
	size_t junk = 0;
	getline(&currentLine, &junk, file); // == R

	while(!feof(file))
	{
		pthread_mutex_lock(&mutexLock);
		getline(&currentLine, &junk, file);
		float newAccountTotal = accountTotal;

		if(!feof(file))
		{
			while(strcmp(currentLine, "W\n"))
			{
				//printf("---|%s|\n", currentLine);
				newAccountTotal += atoi(currentLine);
				getline(&currentLine, &junk, file);
				//printf("---|%s|\n", currentLine);
				//printf("----------------------\n");
			}

			accountTotal = newAccountTotal;
			printf("Account balance after thread %lu is $%0.2f\n", pthread_self(), accountTotal);
			pthread_mutex_unlock(&mutexLock);
			getline(&currentLine, &junk, file);
			sleep(1);
		}
	}
	free(currentLine);
	fclose(file);
	return NULL;
}
