#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>

int BUFFER[4];
sem_t empty, full, lock; // initilize the buffer and active semaphores

void *produce(); // initialize the functions for the producer and consumer
void *consume(void* threadNum);
int main(int argc , char *argv[])
{
	int count = atoi(argv[1]); // gets number of desired threads from user
	pthread_t producer; 
	pthread_t consumer[count]; // varying size array of consumers to allow for user input
	int *threadNum[count];
	sem_init(&empty , 1 , 1); // initialize semophores and their starting values
	sem_init(&full , 1 , 0);
	sem_init(&lock , 1 , 1);
	pthread_create(&producer , NULL , &produce , NULL); // runs producer code
	
	for(int i = 0 ; i < count ; i++) // runs consumer code
	{
		sleep(1);
		threadNum[i] = i+1;
		pthread_create(&consumer[i] , NULL , &consume , (void*)threadNum[i]);
	}
	
	pthread_join(producer,NULL); //joins producer
	
	for( int i = 0 ; i < count ; i++)
	{
		pthread_join(consumer[i],NULL); //joins consumer
	}
	return 0;
}

void *produce()
{
	int currentVal = 1;
	while(1)
	{
		sem_wait(&empty); // signals producer that the buffer is empty and is time to produce
		sem_wait(&lock); // locks critical section 
		for(int i = 0 ; i < 5 ; i++){
		BUFFER[i % 5] = currentVal; // always stay within the fixed buffer area
		currentVal++; // arbitrarily puts numbers in the buffer
		}
		printf("Produced numbers: %d %d %d %d %d\n",
		BUFFER[0],BUFFER[1],BUFFER[2],BUFFER[3],BUFFER[4]); // list what values are
		produced
		
		sem_post(&lock); // unlocks critial section
		sem_post(&full); // signals to consumers that the buffer is full and time to consume
		sleep(2); // sleep to allow consumers to consume.
	}
}

void *consume(void* threadNum)
{
	int num = 0;
	while(1)
	{
		sem_wait(&full); // Wait until buffer is full to enter the semophore
		sem_wait(&lock); // lock and enter cirical section
		for(int i=0 ; i < 5 ; i++) //scan through the buffer and find a nonzero number
		{
			if(BUFFER[i] > 0) //after you find one, obtain the value and set it to zero
			{
				num = BUFFER[i]; //once you find a number, skio down to
				the logcal statmenet 
				BUFFER[i] = 0; // at the bottom.
				printf("Consumer number %i has ID number %ld and consumes
				%d\n",threadNum , pthread_self() , num);
				goto next;
			}
		}
		next:
		sleep(1);
		//CRITICAL SECTION ENDS
		sem_post(&lock); // unlock semophore
		if(BUFFER[0] == 0 && BUFFER[1] == 0 && BUFFER[2] == 0 && BUFFER[3] == 0 && BUFFER[4] == 0)  // condition for empty to be true
		{
			sem_post(&empty); //if buffer is empty, send signal to producer
			sleep(3);
		}
		else
		{
			sem_post(&full); // if buffer is not empty, send signal to other consumers to consume
			numbers
			sleep(3);
		}
	}
}
