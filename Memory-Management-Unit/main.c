#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdint.h>
#include <math.h>

void* memory_malloc(void* threadNum);
void memory_free(int currentThread);
void defragmentation();

typedef struct{
    int threadNum[20];          //organizes the thread number
    int memoryRequest[20];      //organizes the memory Request
    int memoryPtr[20];          //organizes the memory pointers
    int fit_Type;               //stores fit type
    int defrag;                 //determines defragmentation
}threadInfo;

sem_t input,delete,request;     // used semaphores
threadInfo memoryThreads;       // globally initializes the global structure
int* memoryStack;               // global memory

int SYSTEM_MAX_MEMORY_SIZE = 512;   // maximum memory

int main(int argc , char* argv[])
{ 
    int numThreads = atoi(argv[1]);             //gets num threads
    memoryThreads.fit_Type = atoi(argv[2]);     //gets fit type
    memoryThreads.defrag = atoi(argv[3]);       //gets defragmentation toggle
    pthread_t thread[numThreads];               //creates an array of threads to request
    sem_init(&input, 1 , 1);                    //initializes used semaphores
    sem_init(&delete, 1 ,1);
    sem_init(&request, 1 ,1);
                                                        	//creating a memory bank of 512 total bits made up of 4 bit integers
	int maxInts = SYSTEM_MAX_MEMORY_SIZE / sizeof(int);     //total of 128 memory blocks
    memoryStack = (int*) calloc(maxInts , sizeof(int)); 	//dynamically assigns memory

	//print functions to return fit type
	if(memoryThreads.fit_Type == 1) 
	{
		printf("Selecting First Fit...\n");
		sleep(1);
    }
	else if(memoryThreads.fit_Type == 2)
	{
		printf("Selecting Best Fit...\n");
		sleep(1);
    }
	else if(memoryThreads.fit_Type == 3)
	{
		printf("Selecting Worst Fit...\n");
		sleep(1);
    }
	else{
		printf("Incorrect Fit type, ending program...\n");
		exit(0);
    }

	//actually creates the threads
	for(int i = 0 ; i < numThreads ; i++)
	{
		memoryThreads.threadNum[i] = i+1;
		pthread_create(&thread[i] , NULL , &memory_malloc , (void*) memoryThreads.threadNum[i]);
	}
	//joins the threads
	for( int i = 0 ; i < numThreads ; i++)
	{
		pthread_join(thread[i] , NULL);
	}
	//frees the memory
    free(memoryStack);
    
return 0;
}

void* memory_malloc(void* threadNum)
{
    while(1)
	{
        sem_wait(&request);
		uintptr_t arrayIndex = threadNum;                   //creates usable integer from pointer to thread num
		arrayIndex--;                                       //decrement to get array index
		sleep(1);                   
		srand(time(0));
		int threadRequest;
        newNum: threadRequest = 2 << (rand() % 9);              //randomly ask for an integer of 2^any power less than 9

		if(threadRequest == 2) // must request full integer (4 bits)
		{
			goto newNum;
		}
		
		memoryThreads.memoryRequest[arrayIndex] = threadRequest;            //store the memory request
		sem_post(&request);                                                 //enter critical zone

        Start:
		sem_wait(&input);                                               	//print requests
		printf("Thread %d requests %d bytes of data\n",threadNum, memoryThreads.memoryRequest[arrayIndex]);
		sleep(1);
		printf("Requesting %d bytes of data... \n" , memoryThreads.memoryRequest[arrayIndex]);
		sleep(1);


		if(memoryThreads.fit_Type == 1)
		{ //FIRST FIT
			for(int i = 0 ; i < 128 ; i++)
			{
        		SEARCHING:      
				if(memoryStack[i] == 0) //Scan the whole memory and see if there is enough space
				{
					if((i + (memoryThreads.memoryRequest[arrayIndex] / 4)) > 128)
					{
						printf("Not enough space to allocate memory yet, waiting for room...\n\n");
						sem_post(&input);
						sleep(5);
						goto Start; // if not go back and wait for more space
					}
					for(int j = i ; j <= (i + (memoryThreads.memoryRequest[arrayIndex] / 4)) ; j++)
					{
						if(memoryStack[j] != 0 ) // if there is not enough space, check the next spot
						{
							i++;
							goto SEARCHING;
						}
						else //memory is available, allocating the data
						{
							if( j == (i + (memoryThreads.memoryRequest[arrayIndex] / 4)))
							{
								printf("Memory space available, allocating...\n");
								memoryThreads.memoryPtr[arrayIndex] = i;
								for( int k = i ; k < (i + (memoryThreads.memoryRequest[arrayIndex] / 4)) ; k++)
								{
									memoryStack[k] = threadNum;
									printf("%d stored in location %d\n",memoryStack[i] , k);
								}
								printf("\n");
								goto Wait;
							}
						}
					}
				}
				else
				{
					if( i == 127) //if i = 127, the entire memory has been scanned without room
					{
	                    printf("Not enough space to allocate memory yet, waiting for room...\n\n");
	                    sem_post(&input);                           //exit the critical zone and wait your turn again
	                    sleep(1);
	                    srand(time(0));
	                    sleep(rand() % 4);
	                    goto Start;
	                    exit(0);
					}
				}
			}
		}
		else if(memoryThreads.fit_Type == 2) 					//Best fit
		{
			int holePtr = 0;                                    //Indicates where the hole starts
			int holeSize = 0;                                   //indicate the size of the hole
			int minSize = 128;                                  // minimum size of the hole is currently initialized to
			int minSizePtr = 0;                                 //the entire stack; minSizeptr is the pointer to the
			int j = 0;                                          //smallest hole.

			for(int i = 0 ; i < 128 ; i++)                 		// go and find the holes in the data and return their pointer
			{
				if(memoryStack[i] == 0)
				{
					holePtr = i;
		
					j = i;                                      //j indexes all elements of data starting from i
					while(memoryStack[j] == 0 && j != 127)     //use a while loop to determine how big the hole is
					{
						holeSize++;
						j++;
						i = j+1;
					}

					if((minSize > holeSize) && (holeSize > (memoryThreads.memoryRequest[arrayIndex] / 4)))
					{
						minSize = holeSize;                 //determine smallest hole that will fit the request
						minSizePtr = holePtr;
					}
					if(holeSize < (memoryThreads.memoryRequest[arrayIndex] / 4) && i == 127)
					{
							printf("No room for allocation");//traversed the whole memory without success
							goto Start;
					}
				}
			}

			if(holeSize < (memoryThreads.memoryRequest[arrayIndex] / 4))  //if at the end the space is too small, start over
			{
				sem_post(&input);
				sleep(5);
				goto Start;
			}

			if(minSizePtr + (memoryThreads.memoryRequest[arrayIndex] / 4) > 128) //if the request exceeds the bounds, start over
			{
				printf("No room for allocation\n");
				sem_post(&input);
				sleep(5);
				goto Start;
			}
			memoryThreads.memoryPtr[arrayIndex] = minSizePtr;   //store the pointer of the hole that will fit the data
			for(int i = minSizePtr ; i < (minSizePtr + (memoryThreads.memoryRequest[arrayIndex] / 4)) ; i++)
			{
				memoryStack[i] = threadNum;                                 //actually allocate the data
				printf("%d stored in location %d\n",memoryStack[i] , i);    //put the thread num in the data
			}
			goto Wait;                                                  //allow other threads to come in
		}
		else if(memoryThreads.fit_Type == 3)                       //Worst Fit
		{
			int holePtr = 0;
			int holeSize = 0;
			int maxSize = 0;
			int maxSizePtr = 0;
			int j = 0;

			for(int i = 0 ; i < 128 ; i++)
			{
				if(memoryStack[i] == 0)
				{
					holePtr = i;
                
					j = i;
					while(memoryStack[j] == 0 && j != 127)
					{
						holeSize++;
						j++;
						i = j+1;
					}
					//works EXACTLY like best fit but looking for the biggest hole.		
					if((maxSize < holeSize) && (holeSize > (memoryThreads.memoryRequest[arrayIndex] / 4)))
					{
						maxSize = holeSize;
						maxSizePtr = holePtr;
					}
		
					if(holeSize < (memoryThreads.memoryRequest[arrayIndex] / 4) && i == 127)
					{
						printf("No room for allocation");
						goto Start;
					}             
				}
			}

			if(holeSize < (memoryThreads.memoryRequest[arrayIndex] / 4))
			{
				sem_post(&input);
				sleep(5);
				goto Start;
			}

			if(maxSizePtr + (memoryThreads.memoryRequest[arrayIndex] / 4) > 128)
			{
				printf("No room for allocation\n");
				sem_post(&input);
				sleep(5);
				goto Start;
			}

			memoryThreads.memoryPtr[arrayIndex] = maxSizePtr;
			for(int i = maxSizePtr ; i < (maxSizePtr + (memoryThreads.memoryRequest[arrayIndex] / 4)) ; i++)
			{
				memoryStack[i] = threadNum;
				printf("%d stored in location %d\n",memoryStack[i] , i);
        	}
			goto Wait;
		}
        else
		{
			exit(0);
        }
		Wait:
		sem_post(&input);

		srand(time(0));
		sleep(rand()%4);
		memory_free(threadNum);
    }
}

void memory_free(int currentThread)
{
    sem_wait(&input);                                                   //enter the critical zone
    printf("Deallocating thread %d and its data\n" , currentThread);    
    int threadIndex = currentThread - 1;                                //the thread is stored at index 1-threadNum
                                                                        //goes through all the allocated data and sets it to 0
	for (int i = memoryThreads.memoryPtr[threadIndex] ; i < (memoryThreads.memoryPtr[threadIndex] + (memoryThreads.memoryRequest[threadIndex] / 4)) ; i++)
	{
		memoryStack[i] = 0;
    }
    if(memoryThreads.defrag == 1) //determines weather or not defraagmentation will occur
	{
        printf("Defragmenting the Data Stack...\n");
        sleep(2);
        defragmentation(currentThread);
	}
    sem_post(&input);
}

void defragmentation(int currentThread)
{
	int threadIndex = currentThread - 1;    //gives the thread index in relation to the thread number
	int tempPtr = 0;                        //temperary pointer
	int ptrToMove = 128;                    //determines the pointer to the data that needs to be moved.
	int ptrThreadIndex;                     //determines the thread that correlates with the pointer needing to be moved
	int nextThreadNum;                      //indicates the thread that is next to run the recursive function         
	int complete = 1;                       //indicates that defragmentation is complete.
        
	for(int i = 0 ; i < 20 ; i++) // traverse through all the threads
	{

	/*If you find a pointer that is greater than the pointer that is currently being removed, store it and then pass it to the next if statement.
 	The if statement asks if the thread pointer that is bigger than the thread pointer being removed, is the smallest of the pointers,
  	this makes it the one that needs to be moved first. This statement is essentially looking for the smallest pointer bigger than the pointer being removed.*/

		if(memoryThreads.memoryPtr[i] > memoryThreads.memoryPtr[threadIndex])
		{
			tempPtr = memoryThreads.memoryPtr[i];
			complete = 0;
			if(ptrToMove > tempPtr)
			{
				ptrToMove = tempPtr;
				ptrThreadIndex = i;
			}
		}
		else if(i ==19 && complete == 1)            //if Complete was marked as still 1 then there is no bigger pointer
		{
			goto end;                               // and therefore is complete
		}
	}

	//delete all of the data that is allocated to the thread that is moving
	for (int i = memoryThreads.memoryPtr[ptrThreadIndex] ; i < (memoryThreads.memoryPtr[ptrThreadIndex] + (memoryThreads.memoryRequest[ptrThreadIndex] / 4)) ; i++)
	{
		memoryStack[i] = 0;
	}

	//set the pointer of the thread being moved equal to the pointer of the thread that was deleted.
	memoryThreads.memoryPtr[ptrThreadIndex] = memoryThreads.memoryPtr[threadIndex];
	memoryThreads.memoryPtr[threadIndex] = 0;           //reset the pointer of the thread that was deleted.

	//Go through and re-allocate the memory for the thread that is being moved.
	for(int i = memoryThreads.memoryPtr[ptrThreadIndex] ; i < (memoryThreads.memoryPtr[ptrThreadIndex] + (memoryThreads.memoryRequest[ptrThreadIndex] / 4)) ; i++)
	{
		memoryStack[i] = ptrThreadIndex + 1;
	}

	nextThreadNum = ptrThreadIndex + 1; 	//pass in the pointer of the thread that is being moved and repeat the process so 
	end:                                    // that all threads after the one being removed, are moved up
	if(complete != 1)                  		//If complete is not HIGH then the process is not over and will call itself again.
	{
		defragmentation(nextThreadNum);
	}
	if(complete == 1)
	{
		for(int i = 0 ; i < 128 ; i++)
		{
			printf("%d stored in location %d\n" , memoryStack[i] , i);      //reprint the whole stack to see defrag occur
		}
	}
}
