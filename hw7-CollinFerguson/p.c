/*
Collin L. Ferguson
Collin.l.ferguson@und.edu
hw7: Big Project.
*/

#include<stdio.h>
#include<stdlib.h>

#include<sys/mman.h> //needed for shared memory
#include<fcntl.h>

#include<sys/sem.h> //Used for semaphore
#include<sys/stat.h>
#include<errno.h>

#include <unistd.h> //used to create Pipes.

#include<time.h> //used for random number generation

#define _CHILDNUM_ 9
#define SEM_KEY 12042023 //Due date for assignment.
#define MIN_MSG_SIZE 1
#define MAX_MSG_SIZE 10
#define PAD_MSG_SIZE 6


int getRandom(int min, int max)
{
	return (rand() % max + min);
}

void freeAll(char *pipeWriteBuffer, char *tempWriteBuffer, char *childName, FILE* fileName)
{
	free(childName);
	free(pipeWriteBuffer);
	free(tempWriteBuffer);
	fclose(fileName);
}

int main(int argc, char* argv[])
{

	if(argc < 2){printf("It seems like you ran the incorrect file.\n"); exit(0);}

	//SEMAPHORE FETCH//
		key_t key = SEM_KEY;
		int semIdent;
		if((semIdent = semget(key, 0, 0)) == -1){perror("---Semaphore Fetch Failure:");}

		//SEMAPHORE OPERATION STRUCT//
		struct sembuf semOperations;
		semOperations.sem_num = 0;
		semOperations.sem_flg = 0;


	//CHILD CREATION//
	int childNum = 0;
	int pid = -1;
	while(pid != 0 && childNum < _CHILDNUM_)
	{
		childNum++;
		//*(int*)sharedMemory = (*(int*)sharedMemory + 1);
		if(childNum != _CHILDNUM_)
		{
			pid = fork();
		}

		if(pid == -1){printf("child %d could not be created\n", childNum); exit(0);}
	}

	//PIPE FETCH//
	int writePipe = (int) *argv[2]; //Casts the file descriptor into an int from temporary char* storage for passing.

	//SHARED MEMORY FETCH//
	int fd = shm_open(argv[1], O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO); // Gets a file descriptor for shared memory.
	void *sharedMemory = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //puts file descriptor into a void pointer

	//OPEN FILE
	FILE *inFile = fopen(argv[3], "r");


	//Buffer for holding file data
	char *pipeWriteBuffer = malloc(MAX_MSG_SIZE + PAD_MSG_SIZE);
	char *tempWriteBuffer = malloc(MAX_MSG_SIZE + PAD_MSG_SIZE);

	char *childName = malloc(6); //Enough of a buffer to hold _p, a 3 digit int, and the \0;
	if(snprintf(childName, PAD_MSG_SIZE, "_p%d", childNum) < 0){perror("---Child ID creation Error");}

	// SETS UP RANDOM NUMBER GENERATION //
	time_t t;
	srand((unsigned) time(&t) + childNum); //Seeds Random number.
	int randomNum = getRandom(MIN_MSG_SIZE,MAX_MSG_SIZE);

	// MAIN READING BLOCK //
	while(!feof(inFile) && (*(int *)sharedMemory) != -1)
	{
		sleep(0); //Forces the current child to give up the thread so another child can run. Prevents last created child from being the only thread to run.
		randomNum = getRandom(MIN_MSG_SIZE, MAX_MSG_SIZE);

		semOperations.sem_op = -1; // Waits for sem to be 1 (unlocked) then reduces it to zero.
		if(semop(semIdent, &semOperations, 1) == -1)
		{
			//perror("Semaphore Locking Error");
			freeAll(pipeWriteBuffer, tempWriteBuffer, childName, inFile);
			exit(0);
		}

		fseek(inFile, (*(int *)sharedMemory), SEEK_SET);

		if(!feof(inFile) && *(int *)sharedMemory != -1)
		{
			//printf("---Child %s has started RN:%d  SM:%d\n",childName, randomNum, *(int *)sharedMemory);
			//UPDATE SHARED MEMORY
			for( int x = 0; x<randomNum; x++)
			{

				tempWriteBuffer[x] = (char) fgetc(inFile);

				//printf("%s---%d-----------------sharedMemory: ----%d----", childName, randomNum, *(int *)sharedMemory);
				//printf("---Child: %s, \"%s\"\n",childName, tempWriteBuffer);

				if(x==randomNum-1)
				{
					tempWriteBuffer[x+1] = '\0';
				}

				if(feof(inFile))
				{
					//printf("---feof\n");
					tempWriteBuffer[x] = '\0';

					snprintf(pipeWriteBuffer, 16, "%s%s", tempWriteBuffer, childName);

					int temp = *(int *)sharedMemory;
					*(int *)sharedMemory = -1;
					msync(sharedMemory, sizeof(int), MS_SYNC);

					if(write(writePipe,(void *)pipeWriteBuffer, MAX_MSG_SIZE + PAD_MSG_SIZE) == -1){perror("---Pipe Write Error");}

					snprintf(pipeWriteBuffer, 16, "%d", temp); //TEMP

					//snprintf(pipeWriteBuffer, 16, "%s %d",childName, *(int *)sharedMemory); //TEMP
					//*(int *)sharedMemory = -1;

					semOperations.sem_op = -1; // Waits for sem to be 1 (unlocked) then reduces it to zero.
					if(semop(semIdent, &semOperations, 1) == -1){perror("Semaphore Lock Error");}

					if(write(writePipe,(void *)pipeWriteBuffer, MAX_MSG_SIZE + PAD_MSG_SIZE) == -1){perror("---Pipe Write Error");}

					freeAll(pipeWriteBuffer, tempWriteBuffer, childName, inFile);
					exit(0);
					//break;
				}

				*(int *)sharedMemory = (*(int *)sharedMemory) + 1;
			}
				//printf("--------------------sharedMemory: ----%d----, %d\n",*(int *)sharedMemory, randomNum);

			snprintf(pipeWriteBuffer, 16, "%s%s", tempWriteBuffer, childName);

			if(write(writePipe,(void *)pipeWriteBuffer, MAX_MSG_SIZE + PAD_MSG_SIZE) == -1){perror("---Pipe Write Error");}
		}
		else
		{
			semOperations.sem_op = 1; // increments semval to 1 (freeing it)
			if(semop(semIdent,&semOperations, 1) == -1){perror("---Semaphore Unlocking Operation Error:");}

			freeAll(pipeWriteBuffer, tempWriteBuffer, childName, inFile);
			exit(0);
		}
	}

	printf("Here %s\n", childName);
	freeAll(pipeWriteBuffer, tempWriteBuffer, childName, inFile);
	exit(0);
	return 0;
}
