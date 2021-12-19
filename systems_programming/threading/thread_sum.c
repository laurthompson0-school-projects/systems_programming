/* 
Laura Thompson
CS 332, FA 2020
Lab 12, 11/18/2020

To Compile: gcc -Wall -o lab12 thompson_lab12.c -lpthread
To Run: ./lab 12 <# of elements> <# of threads>
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER; //initialize global mutex

typedef struct vars{

    //Struct passed as arguments to thread start_routine function
    
    double *a; //double array [1, 2, 3, ..., N] of elements to be added
    double sum; //total sum
    int N; //Number of elements being summed
    int size; //number of threads used
    long tid; // id (1 to size) of current thread
    
} VARS;

void *compute(void *arg) {

    //Function that each thread executes to compute partial sum

    //Static variable that will have mysum  added to it for each thread
    static double totalsum = 0.0;
    
    int myStart, myEnd, myN, i; //initialize local variables
    VARS *args = (VARS *)arg; //assign struct to variables passed in to function

    // determine start and end of computation for the current thread
    myN = args->N/args->size;
    myStart = args->tid*myN;
    myEnd = myStart + myN;
    if (args->tid == (args->size-1))
	myEnd = args->N;

    // compute partial sum
    double mysum = 0.0;
    for (i=myStart; i<myEnd; i++)
      mysum += args->a[i];

    // grab the lock, update sum value passed and total sum, and release lock
    pthread_mutex_lock(&mutex);
    totalsum += mysum;
    args->sum = totalsum;   
    pthread_mutex_unlock(&mutex);

    //Last thread will have true total sum

    return (NULL);
}

int main(int argc, char **argv) {

    //Initialize local variables
    long i;
    VARS *args;
    double *a;
    pthread_t *ptid;

    //make sum static so all threads can add to it
    static double sum = 0.0;

    //Check usage
    if (argc != 3) {
       printf("Usage: %s <# of elements> <# of threads>\n",argv[0]);
       exit(-1);
    }

    //Get inputs
    int N = atoi(argv[1]); //N = num elements to be summed
    int size = atoi(argv[2]); //size = num threads to be used

    
    //allocate memory for arrays 
    ptid = (pthread_t *)malloc(sizeof(pthread_t)*size); //array of threads
    args = (VARS *)malloc(sizeof(VARS)*size); //array of argument structs
    a = (double *)malloc(sizeof(double)*N); //array to be summed [1, 2,..,N]

    //initialze values to be summed in array 'a' as [1, 2, ..., N]
    for (i=0; i<N; i++)
      a[i] = (double)(i + 1);
    
    //initialize arguments for threads and then create threads
    for ( i = 0; i < size; i++)
    {
	//Constant/Do not change for each thread
	args[i].a = a;
	args[i].N = N;
	args[i].size = size;

	//Changes for each iteration, identifies the thread
	args[i].tid = i;

	//passed in as 0, and will be assigned to total sum within function
	args[i].sum = sum;

	//Create threads
	pthread_create(&ptid[i], NULL, compute, (void *)&args[i]);
    }
    
    //wait for threads to complete
    for ( i = 0; i < size; i++)
	pthread_join(ptid[i], NULL);

    //print total, stored in final thread
    printf("The total is %g, it should be equal to %g\n", 
           args[size-1].sum, ((double)N*(N+1))/2);
    
    return 0;
}

