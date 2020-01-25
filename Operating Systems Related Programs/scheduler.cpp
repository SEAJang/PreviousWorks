/*
 * Sally
 * Assignment 3
 * Question 7
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cstring>
#include <queue>

using namespace std;

enum ProcessState{READY, RUNNING, NONEXIST};

class Process
{
	private:
		int pid;
		int burstTime;
		int arrivalTime;
		int remainingTime;
		int waitTime;
		ProcessState state;

	public:
		Process(int _pid, int _arrivalTime, int _burstTime)
		{
			pid = _pid;
			burstTime = _burstTime;
			arrivalTime = _arrivalTime;
			remainingTime = _burstTime;
			waitTime = 0;
			state = NONEXIST;
		}

		int getPid()
		{
			return pid;
		}
		int getBurstTime()
		{
			return burstTime;
		}
		int getArrivalTime()
		{
			return arrivalTime;
		}
		int getRemainingTime()
		{
			return remainingTime;
		}
		int getWaitTime()
		{
			return waitTime;
		}
		ProcessState getState()
		{
			return state;
		}
		int setState(ProcessState s)
		{
			state = s;
		}
		void incrementWait()
		{
			waitTime++;
		}
		void zRemain()
		{
			remainingTime = 0;
		}
		void run()
		{
			remainingTime--;
			state = RUNNING;
		}
};

Process * pQueue[30];

void SJF(int size)
{
	int currentTime = 0;
	int queueCount = 0;
	bool finish = false;

	while(!finish)
	{
		//find the index of the shortest job/job that has already arrived
		int shortest = -1;
		for(int i = 0; i <= size; i++)
		{
			if(shortest == -1)
			{
				shortest = i;
			}
			else if(pQueue[i]->getRemainingTime() < pQueue[shortest]->getRemainingTime()&&
					pQueue[i]->getRemainingTime()>0 &&
					pQueue[i]->getArrivalTime() <= currentTime)
			{
				shortest = i;
			}
		}
		//check if shortest job arrived yet
		if(pQueue[shortest]->getArrivalTime()<=currentTime)
		{
			cout << currentTime;
			for(int x = 0; x <= size; x++)
			{
				if (pQueue[x]->getArrivalTime() <= currentTime &&
						pQueue[shortest]->getPid() != pQueue[x] ->getPid() &&
							pQueue[x] ->getRemainingTime() > 0)
				{
					pQueue[x]->setState(READY);
					pQueue[x]->incrementWait();
				}
				else if(pQueue[x]->getPid() == pQueue[shortest]->getPid())
				{
					pQueue[x] -> run();
				}
				else
				{
					pQueue[x]->setState(NONEXIST);
				}
				//print
				ProcessState pState = pQueue[x]->getState();
				if(pState == READY)
				{
    					cout << '\t' << "+";
				}
				else if(pState == RUNNING)
				{
					cout << '\t' << ".";
				}
				else
				{
					cout << '\t' << "";
				}
			}
			cout << endl;
			currentTime++;
		}
		else
		{
			cout << currentTime << endl;
			currentTime++;
		}

		//check if all jobs are done
		bool d = true;
		for (int z = 0; z <= size; z++)
		{
			if(pQueue[z] -> getRemainingTime() > 0)
			{
				d = false;
				break;
			}
		}
		finish = d;

	}

}
void RR(int size, int time)
{
	int currentTime = 0;
	int tSlice = time;
	queue<Process*> job;
	//wait for first job to arrive
	while (1)
	{
		if(currentTime < pQueue[0]->getArrivalTime())
		{
			cout << currentTime << endl;
			currentTime++;
		}
		else
		{
			break;
		}
	}
	job.push(pQueue[0]);//add first job to queue

	while(job.size() > 0)
	{
		//work on first job
		Process* temp = job.front();
                job.pop();
		for(int i = 0; i < tSlice; i ++)
		{
			cout << currentTime;
			for(int x = 0; x <= size; x ++)
			{
				if(temp->getPid() != pQueue[x]->getPid() && 
					pQueue[x]->getArrivalTime() <= currentTime &&
						pQueue[x]->getRemainingTime() > 0)
				{
					pQueue[x]->setState(READY);
					pQueue[x]->incrementWait();
				}
				else if(pQueue[x]->getPid() == temp->getPid())
				{
					pQueue[x]->run();
				}
				else
				{
					pQueue[x]->setState(NONEXIST);
				}
				//print				
				ProcessState pState = pQueue[x]->getState();
                                if(pState == READY)
                                {
                               		cout << '\t' << "+";
                                }
                                else if(pState == RUNNING)
                               	{
                                        cout << '\t' << ".";
                                }
                                else
                                {
                                        cout << '\t' << "";
				}
					
			}
			currentTime++;
			cout << endl;
			//push jobs onto queue if arrived
			for (int y = 0; y <= size; y++)
			{
				if(pQueue[y]->getArrivalTime() == currentTime)
				{
					job.push(pQueue[y]);
				}
			}
			//if current job is done, exit loop
			if(temp->getRemainingTime() <= 0)
				break;
		}
		//if current job was not done, put it back into queue
		if(temp->getRemainingTime() > 0)
		{
			job.push(temp);
		}
	}
}


int main(int argc, char ** argv)
{
	char * fileName;
	char * alg;
	bool rr_true;

	if (argc != 4 && argc != 3)
	{
		cerr << "Usage: schedule <filename> <schedule alg> <time quantum>" << endl;
		return -1;
	}
	fileName = argv[1];
	alg = argv[2];
	int tSlice;
	if(argc == 4)
	{
		if (strcmp(alg, "rr") == 0 || strcmp(alg, "RR") == 0)
		{
			rr_true = true;
			tSlice = atoi(argv[3]);	
		}
		else
		{
			cerr << "<Schedule alg> = RR or SJF\nIf SJF, do not have a 4th time quantum" << endl;
			return -1;
		}
	}
	else
	{
		if (strcmp(alg, "SJF") == 0 || strcmp(alg, "sjf") == 0)
		{
			rr_true = false;
		}
		else
		{
			cerr << "<Schedule alg> = RR or SJF\nIf RR, requires 4th entry" << endl;
			return -1;
		}
	}

	//Initialize process queue and arrange according to arrival time

	int pid = -1;
	fstream file(fileName, ios_base::in);

	int arr;
	int burs;

	while (file >> arr && file >> burs)
	{
		pid++;
		pQueue[pid] = new Process(pid, arr, burs);
	}

	//Simulation header
	cout << "Time \t";
	for (int i = 0; i <= pid; i++)
	{
		cout << "P" << i << "\t";
	}
	cout << "\n" << "--------------------------------------------------" << endl;

	if(rr_true)
	{
		RR(pid, tSlice);
	}
	else
	{
		SJF(pid);
	}
	cout << "--------------------------------------------------" << endl;

	float totWaitTime = 0;

	for (int i = 0; i <= pid; i++)
	{
		cout << "P" << i << " waited " << pQueue[i]->getWaitTime() << " seconds." << endl;
		totWaitTime = totWaitTime + pQueue[i]->getWaitTime();
	}
	float avgWaitTime = totWaitTime/(pid+1);
	cout << "Average waiting time = " << avgWaitTime << " seconds." << endl;

	return 0;
		
}
