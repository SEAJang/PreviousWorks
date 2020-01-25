/*
 * Sally
 * Assignment 3
 * Qustion 5
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>

using namespace std;
pthread_mutex_t debug_mut = PTHREAD_MUTEX_INITIALIZER;


//Global variables
//Mutex
pthread_mutex_t count_mut = PTHREAD_MUTEX_INITIALIZER;

//Threadpool
int tPool[256];

int numbPrime = 0;

//array of bools which are used to keep track of if thread managed to figure out if numb was not prime
//at end of doing all threads, bool is still left false
//increment nubmer of primes as the number given failed to be proven as not a prime
bool * primeCheck;

struct threadInfo
{
	int pid;
	int64_t inputNum;
	int index;
	int64_t checkNumU;
	int64_t checkNumL;
	int64_t m;
};

void * onecheck(void * tInfo)
{
	struct threadInfo * thisdata;
	thisdata = (struct threadInfo *)tInfo;
	int64_t n = thisdata -> inputNum;
	int x = thisdata -> index;
	
	if (n <= 1)
	{
		primeCheck[x] = true;
	}
	if (n <= 3 && n > 1)
	{
		pthread_mutex_lock(&count_mut);
		numbPrime++;
		pthread_mutex_unlock(&count_mut);
		primeCheck[x] = true;
	}
	if(n % 2 == 0 || n % 3 == 0)
	{
		primeCheck[x] = true;
	}

	tPool[thisdata -> pid] = 0;
        return 0;

}
void * generalcheck(void * tInfo)
{
	struct threadInfo * thisdata;
	thisdata = (struct threadInfo *)tInfo;
	int64_t n = thisdata -> inputNum;
	int64_t u = thisdata -> checkNumU;
	int64_t l = thisdata -> checkNumL;
	int x = thisdata -> index;
	int64_t max = thisdata -> m;
	while (l <=  max && l < u)
	{
		if (n % l == 0 || n % (l + 2) == 0)
		{
			primeCheck[x] = true;
			break;
		}
		l += 6;
	}
	tPool[thisdata -> pid] = 0;
	return 0;
}

int main(int argc, char ** argv)
{
	//parse command line arguments
	int nThreads = 1;
	if (argc != 1 && argc != 2)
	{
		cerr << "Usage: count [nThreads]" << endl;
		return -1;
	}
	if (argc == 2)
	{
		nThreads = atoi(argv[1]);
	}
	//handle invalid arguments
	if(nThreads < 1 || nThreads > 256)
	{
		cerr << "Bad arguments. 1 <= nthreads <= 256" << endl;
		return -1;
	}

	//count the primes
	printf("Counting primes using %d thread%s.\n", nThreads, nThreads == 1 ? "": "s");
	
	pthread_t threads[nThreads];
	
	bool checkdone[nThreads];
	for(int i = 0; i < nThreads; i++)
	{
		checkdone[i] = false;
		threads[i] = -1;
	}
	primeCheck = checkdone;
	
	
	struct threadInfo tiPool[nThreads];
	for (int i = 0; i < nThreads; i++)
	{
		tPool[i] = 0;
	}
	int ind = -1;
	while (1)
	{
		int64_t num;
		if (1 != scanf("%ld", &num))
		{
			break;
		}
		ind++;
		bool fir = false;
		bool tFound = false;
		
		int64_t max = sqrt(num);

		int64_t z = 5;
		int64_t u = 23;

		bool threadsMade = false;

		while(!threadsMade)
		{
			tFound = false;
			while(!tFound)
			{
				for (int i = 0; i < nThreads; i++)
				{
					if(tPool[i] == 0)
					{	
						if(threads[i] != -1)
						{
							int r = pthread_join(threads[i] , NULL);
						}
						tFound = true;
						int status;
						//initialize values to be passed into thread
						tiPool[i].pid = i;
						tiPool[i].inputNum = num;
						tiPool[i].index = ind;
						tiPool[i].checkNumU = u;
						tiPool[i].checkNumL = z;
						tiPool[i].m = max;
						tPool[i] = 1;
						
						if (!fir)
						{
							status = pthread_create(&threads[i], NULL, onecheck,(void*) &tiPool[i]);
							fir = true;
						}
						else if(z <= max)
						{
							status = pthread_create(&threads[i], NULL, generalcheck, (void*) &tiPool[i]);
							z = u;
							u += 60000;
						}
						else
						{
							threadsMade = true;	//all threads for particular number finished
							tPool[i] = 0;
							status = 0;
						}
						if(status)
						{
							cerr << "PID:" << i << "inputnum:"<<num << "fir true?" << fir << "U:" << u<< endl;
							cerr << "Error!" << status <<endl;
							exit(-1);
						}
					}
				}
			}
		}
	}
	//wait for all threads to finish
	for (int i = 0; i < nThreads; i++)
	{
		if(tPool[i] == 1)
		{
			int r = pthread_join(threads[i], NULL);
		}
	}
	//count how many proved themselves to be a prime
	for (int i = 0; i <= ind; i++)
	{
		if(primeCheck[i] == false)
                {
                        numbPrime++;
                }
	}
	//report results
	cout << "Found "<< numbPrime << " primes." << endl;
	
	return 0;
}
	
