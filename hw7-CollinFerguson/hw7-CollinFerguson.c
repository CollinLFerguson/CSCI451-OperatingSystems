/*
Collin L. Ferguson
Collin.l.ferguson@und.edu
hw7: Big Project.
*/

#include<stdio.h>
#include<stdlib.h>

#include<sys/mman.h> //needed for shared memory
#include<fcntl.h>


#include<sys/sem.h>
#include<sys/stat.h>
#include<errno.h>
#include <unistd.h> //used to create Pipes.

#define SEM_KEY 12042023
#define MIN_MSG_SIZE 1
#define MAX_MSG_SIZE 16 //10 characters + _p + up to 3 digit child identifier + \0

void *sharedMemory;

int main(int argc, char *argv[])
{
	//SHARED MEMORY CREATION//
	int fd = shm_open("/hw7-CollinFerguson", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO); // Gets a file descriptor for shared memory.

	ftruncate(fd, sizeof(int)); //Required for shared memory. Don't know what it does!

	sharedMemory = mmap(0, sizeof(int), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0); //Puts the shared memory into a file descriptor.
	if(sharedMemory == MAP_FAILED){printf("Map Failed\n");}

	*(int*)sharedMemory = 0;

	//Create Semaphore
	int semIdent;
	key_t key = SEM_KEY;
	if((semIdent = semget(key, 1, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
	{perror("---Semaphore Creation Error");}


	//PIPE CREATION//
	int pipes[2]; //[0] = read end, [1] = write end;
	if(pipe(pipes) == -1){(perror("---Pipe Creation Error"));}
	char *pipeWriteEndFD = malloc(sizeof(int));
	*pipeWriteEndFD = pipes[1]; //Converts the write end of the pipe into a string for passing as an arg to child

	//FORK and EXE the children.
	pid_t pid = fork();

	switch(pid)
	{
		case 0:
		{
			char *args[] = {"./cp", "/hw7-CollinFerguson", pipeWriteEndFD, argv[1], NULL}; //execvp requires an array of char*, the first element being the path.
			execvp(args[0], args);

			printf("Child Creation Failure\n");
			exit(0);
		}
		case -1:
		{
			printf("Fork failed\n");
			exit(0);
			break;
		}
	}

	FILE *outFile = fopen("hw7.out", "w");

	struct sembuf semOperations; //structure required to pass semaphore operations
	semOperations.sem_num = 0;
	semOperations.sem_flg = 0;


	semOperations.sem_op = 1; // increments semval to 1 (freeing it)
	if(semop(semIdent,&semOperations, 1) == -1){perror("---Semaphore Unlocking Operation Error:");}


	void *voidBuffer = malloc(MAX_MSG_SIZE); //Buffer for reading from the pipe

	int hasExited = 0;

	while(*(int *)sharedMemory != -1 || !hasExited)
	{


		if(read(pipes[0],voidBuffer, MAX_MSG_SIZE) == -1){perror("---Pipe Read Error:");}
		else
		{
			fprintf(outFile,"%s", (char *) voidBuffer);
			printf("Read Result: %s\n", (char*) voidBuffer);
		}

		msync(sharedMemory, sizeof(int), MS_SYNC);

		if(*(int *)sharedMemory != -1)
		{
			semOperations.sem_op = 1; // increments semval to 1 (freeing it)
			if(semop(semIdent,&semOperations, 1) == -1){perror("---Semaphore Unlocking Operation Error:");}
		}
		else
		{
			semOperations.sem_op = 1; // increments semval to 1 (freeing it)
			if(semop(semIdent,&semOperations, 1) == -1){perror("---Semaphore Unlocking Operation Error:");}

			if(read(pipes[0],voidBuffer, MAX_MSG_SIZE) == -1)
			{perror("---Pipe Read Error:");}
			else
			{
				fprintf(outFile,"%s", (char *) voidBuffer);
				printf("---Num: %s\n", (char*) voidBuffer);
				hasExited++;
			}
		}
	}

	fclose(outFile);
	free(pipeWriteEndFD); //Free Pipes
	shm_unlink("/hw7-CollinFerguson"); //required to free the shared memory.
	semIdent = semctl(semget(key, 0, 0), 0, IPC_RMID); //Frees the semaphore
	return 0;
}
