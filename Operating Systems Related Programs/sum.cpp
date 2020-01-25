/*
 * Assignment 2
 * Question 8
 * Sally 
 *
 * README:
 * 	Program assumes that the input file will have numerical values on all lines which are part of the file
 * 	Therefore if there are any empty lines, results may be different.
 * compile:
 * 	g++ sum.cpp -o sum -lpthread
 *
 * 	USage: sum <filename> <threadcount>
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <pthread.h>

using namespace std;

//filecontent will hold the ints from file
int filecontent[1000000];
//sum_val will hold ints that are the sum of each thread
int * sum_val;
//mutex for printing in threads
pthread_mutex_t p_mutex = PTHREAD_MUTEX_INITIALIZER;

//data to be past into the thread
//start/end points to the index values of the numbers the thread needs to find the sum of
struct thread_data
{
	int thread_id;
	int start;
	int end;
};

void * SumThread(void * targ)
{
	struct thread_data *thisdata;
	int tsum = 0;		//keeps track of the sum of this thread
	thisdata = (struct thread_data *) targ;
	for (int i = (thisdata -> start); i < (thisdata -> end); i++)
	{
		tsum = tsum + filecontent[i];
	}
	int x = thisdata -> thread_id;
	sum_val[x] = tsum;
	pthread_mutex_lock(&p_mutex);			//makes sure only one thread is printing
	cout << "Thread " << thisdata -> thread_id << ":" << tsum << endl;
	pthread_mutex_unlock(&p_mutex);
}

int main(int argc, char * const argv[])
{
	char * filename;
	int threadcount;
	int sum = 0;	//initialize total sum as 0
	int fileints = 0;	//variable used to keep track of how many ints read in from the file

	if  (argc != 3)		//check commandline input
	{
		cerr << "Usage: sum <input file> <#threads>" << endl;
		return -1;
	}
	else			//initialize variablea
	{
		filename = argv[1];
		threadcount = atoi(argv[2]);
	}

	ifstream file;		//open file for read only
	file.open(filename, ifstream::in);

	char temp[20];
	while(!file.eof())		//save ints from file into an array
	{
		fileints++;
		file.getline(temp, 20);
		filecontent[fileints-1] = atoi(temp);
	}
	fileints--;
	file.close();		//close file
	
	int sum_arry[threadcount];
	sum_val = sum_arry;

	pthread_t thread[threadcount];		//make array of threads
	struct thread_data td[threadcount];	//make array of structures for args to be used in thread

	int e;
	int divisor = fileints/threadcount;	//int used to increment the number of ints read for threads

	int s = 0;
	int l = s + divisor + (fileints % threadcount);	//first thread gets all extra remainders

	for (int i = 0; i < threadcount; i++)
	{	
		//initialize struct/args to be passed in
		td[i].start = s;
		td[i].end = l;
		td[i].thread_id = i;
		//create thread
		e = pthread_create(&thread[i], NULL, SumThread, (void *)&td[i]);
		//thread error check
		if(e)
		{
			cout << "Error creating thread: " << e << endl;
			exit(-1);
		}
		//increment ints for next struct
		s = l;
		l = l + divisor;
	}

	//wait for all threads to finish adding
	for (int i = 0; i < threadcount; i++)
	{
		void * status;
		int r = pthread_join(thread[i], &status);
	}
	//do a final sum
	for (int i = 0; i < threadcount; i++)
	{
		sum = sum + sum_val[i];
	}
	cout << "Sum: " << sum << endl;

	return 0;
}	
