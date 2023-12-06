/*
 * killSemaphore.c
 *
 *  Created on: Nov 18, 2023
 *      Author: student
 */

#include<stdio.h>
#include<stdlib.h>

#include<sys/mman.h> //needed for shared memory
#include<fcntl.h>


#include<sys/sem.h>
#include<sys/stat.h>
#include<errno.h>
#include <unistd.h> //used to create Pipes.

#define _CHILDNUM_ 9
#define SEM_KEY 12042023
#define MIN_MSG_SIZE 1
#define MAX_MSG_SIZE 16 //10 characters + _p + up to 3 digit child identifier + \0

int main(int argc, char *argv[])
{
	key_t key = SEM_KEY;
	int semIdent = semget(key, 0, 0);
	semIdent = semctl(semget(key, 0, 0), 0, IPC_RMID); //Frees the semaphore
}
