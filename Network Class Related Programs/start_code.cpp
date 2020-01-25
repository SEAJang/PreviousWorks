/* -------------------------------------------------------------------------
 * This program provides a basic structure for a discrete-event simulator.
 *
 * Name              : start_code.c
 * Author            : Reza Gholizadeh
 * Language          : C
 * Latest Revision   : 11-20-2018
 * -------------------------------------------------------------------------
 */

/* -------------- Libraries ------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

/* -------------- Global variables ------------------------ */
#define MAX_INT 2147483647 /* Maximum positive integer 2^31 - 1 */

/* Defining event type indicators */
#define SILENT 0
#define MELODY 1
#define SCREECH 2

/* Debugging flag */
#define DEBUG 0
#define BONUS 0			//turn to 1 to make yodel time always 10.0

using namespace std;

/* -------------- Structures ------------------------------ */
struct Bozon {
	double sleeptime;		//duration the bozon sleeps
	double startsleep;		//time of when they fall asleep
	double endsleep;		//time of when they start yodelling
	double yodeltime;		//duration of the yodel
	double endyodel;		//time when they go back to sleep
	bool yodel;				//set if bozon is yodelling
	bool screech;			//set if yodelling overlapped with another bozon 
};
struct Atmosphere {
	double silent;			//sum of all silent time
	double melodious;		//sum of all melodious
	double screechy;		//sum of all screech
	int attemptyodel;		//# of attempted yodels
	int perfectyodel;		//#of perfect yodels
};
/* -------------- Function declarations ------------------- */
double uniform01(void);
double getExponentialVar(double mean);
/* -------------- Function definitions ------------------- */
/* ============== */
int main(int argc, char * const argv[])
/* ============== */
{
	if (argc != 5)
	{
		cerr << "Invalid usage." << endl;
		cerr << "./<program_name> <M(#bozon)> <S(mean sleep)> <Y(mean yodel)> <SimDurationTime>" << endl;
		return -1;
	}
	/*Set up constants from command line*/
	int M = atoi(argv[1]);
	double S = atof(argv[2]);
	double Y = atof(argv[3]);
	double SIM_END_TIME = atof(argv[4]);
	

	/* *************** Defining Variables *************************** */
	// You should define simulator variables (e.g. time, event list, entities, statistical counters and control variables)
	double curr_time;
	struct Atmosphere thisPlanet;
	struct Bozon colony[M];
	vector<Bozon*> yodellers;
	struct Bozon* nextBozo;
	struct Bozon imaginaryTester;
	/* ************************************************************** */
	/* *************** Initialization ******************************* */
	// You should initialize ALL VARIABLES before the main loop starts
	curr_time = 0.0;
	
	//initializing statistical counters for current planet/simulation
	thisPlanet.silent = 0.0;
	thisPlanet.melodious = 0.0;
	thisPlanet.screechy = 0.0;
	thisPlanet.attemptyodel = 0;
	thisPlanet.perfectyodel = 0;
	
	imaginaryTester.endsleep = 999999.0;
	imaginaryTester.yodel = false;
	
	// initialize random number generator seed
//	srand(time(NULL));
	srand(0);	
	// You should generate INITIAL EVENTS before the main loop starts
	/* ************************************************************** */
	for (int i = 0; i < M; i++)
	{
		colony[i].sleeptime = getExponentialVar(S);
		colony[i].startsleep = 0.0;
		colony[i].endsleep = colony[i].startsleep + colony[i].sleeptime;
		colony[i].yodeltime = getExponentialVar(Y);
#if BONUS
		colony[i].yodeltime = 10.0;
#endif
		colony[i].endyodel = colony[i].endsleep + colony[i].yodeltime;
		colony[i].yodel = false;
		colony[i].screech = false;
	}
	nextBozo = &imaginaryTester;
	for(int i = 0; i < M; i++)	//find the first bozon to wake up/start yodelling
	{
		if(nextBozo->endsleep > colony[i].endsleep)
		{
			nextBozo = &colony[i];
		}
	}
	/* Main loop */
	while (curr_time < SIM_END_TIME) 
	{
		//if current state is Silent
		if(yodellers.size() == SILENT)	
		{

			thisPlanet.silent +=  nextBozo->endsleep - curr_time;		//add to silent time total
			curr_time = nextBozo->endsleep;		//increment time to the next bozon to yodel
#if DEBUG
			cout << "SiCurr time" << curr_time << endl;
#endif
			nextBozo->yodel = true;
			yodellers.push_back(nextBozo);
			thisPlanet.attemptyodel++;
			if(curr_time > SIM_END_TIME)
			{
				float t = curr_time - SIM_END_TIME;
				thisPlanet.silent -= t;
				curr_time = SIM_END_TIME;
			}
		}
		//if current state is Melodious
		else if(yodellers.size() == MELODY)
		{
			//if current Bozon finishes yodelling before the next one wakes
			if(yodellers.front()->endyodel <= nextBozo->endsleep)
			{
				thisPlanet.melodious += (yodellers.front()->endyodel - curr_time);
#if DEBUG
				cout << yodellers.front()->endyodel << " end" << curr_time << "curr " << endl;
#endif
				if(!yodellers.front()->screech)
				{
					thisPlanet.perfectyodel++;
				}
				//increment time
				curr_time = yodellers.front()->endyodel;
#if DEBUG
				cout << "MCurr time" << curr_time << endl;
#endif

				//randomize Bozon's sleep time & yodel
				yodellers.front()->sleeptime = getExponentialVar(S);
				yodellers.front()->startsleep = curr_time;
				yodellers.front()->endsleep = yodellers.front()->startsleep + yodellers.front()->sleeptime;
				yodellers.front()->yodeltime = getExponentialVar(Y);
#if BONUS
				yodellers.front()->yodeltime = 10.0;
#endif
				yodellers.front()->endyodel = yodellers.front()->endsleep + yodellers.front()->yodeltime;
				yodellers.front()->yodel = false;
				yodellers.front()->screech = false;
				
				yodellers.erase(yodellers.begin());
				if(curr_time > SIM_END_TIME)
				{
					float t = curr_time - SIM_END_TIME;
					thisPlanet.melodious -= t;
					curr_time = SIM_END_TIME;
				}
			}
			else
			{
#if DEBUG
				cout << nextBozo->endsleep << " end" << curr_time << "curr " << endl;
#endif

				//add the amount of time the Bozon was alone
				thisPlanet.melodious += nextBozo->endsleep - curr_time;
				//update current time 
				curr_time = nextBozo->endsleep;
#if DEBUG
				cout << "MCurr time" << curr_time << endl;
#endif

				//update Bozons...
				yodellers.front()->screech = true;
				
				nextBozo->screech = true;
				nextBozo->yodel = true;
				yodellers.push_back(nextBozo);
				thisPlanet.attemptyodel++;
				if(curr_time > SIM_END_TIME)
				{
					float t = curr_time - SIM_END_TIME;
					thisPlanet.melodious -= t;
					curr_time = SIM_END_TIME;
				}
			}
		}
		//if current state is Screechy
		else if (yodellers.size() >= SCREECH)
		{
			//find the closest event to occur
			struct Bozon* temp;
			int z = -1;		//keep track of which index the Bozon is kept in the vector
			if(nextBozo->endsleep < yodellers.front()->endyodel)
			{
				temp = nextBozo;
			}
			else
			{
				temp = yodellers.front();
				z = 0;
			}
			for(int i = 0; i < yodellers.size(); i++)
			{
				if((temp->endyodel > yodellers.at(i)->endyodel && z != -1)||(temp->endsleep > yodellers.at(i)->endyodel && z == -1))
				{
					temp = yodellers.at(i);
					z = i;
				}
			}
   			//-----------------------------------------
			if(temp == nextBozo)	//if the closest event is to be another yodeller
			{
				thisPlanet.screechy +=(nextBozo->endsleep - curr_time);		//add to screech time total
				curr_time = nextBozo->endsleep;		//increment time
#if DEBUG
                cout << "SCCurr time" << curr_time << endl;
#endif

				nextBozo->yodel = true;
				nextBozo->screech = true;
				yodellers.push_back(nextBozo);
				thisPlanet.attemptyodel++;
			}
			else	//else
			{

				thisPlanet.screechy += (yodellers.at(z)->endyodel - curr_time);
				//increment time
				curr_time = yodellers.at(z)->endyodel;
#if DEBUG
                cout << "SC2Curr time" << curr_time << endl;
#endif

				//randomize Bozon's sleep time & yodel
				yodellers.at(z)->sleeptime = getExponentialVar(S);
				yodellers.at(z)->startsleep = curr_time;
				yodellers.at(z)->endsleep = yodellers.at(z)->startsleep + yodellers.at(z)->sleeptime;
				yodellers.at(z)->yodeltime = getExponentialVar(Y);
#if BONUS
                yodellers.at(z)->yodeltime = 10.0;
#endif

				yodellers.at(z)->endyodel = yodellers.at(z)->endsleep + yodellers.at(z)->yodeltime;
				yodellers.at(z)->yodel = false;
				yodellers.at(z)->screech = false;
	
				yodellers.erase(yodellers.begin()+z);

			}
			if (curr_time > SIM_END_TIME)
			{
				float t = curr_time - SIM_END_TIME;
				thisPlanet.screechy -= t;
				curr_time = SIM_END_TIME;
			}
		}
		//Error event...
		else
		{
			printf("Invalid type of event:");
			printf("%d", yodellers.size());
		}
		//find the next bozon to wake up/start yodelling
		nextBozo = &imaginaryTester;
		for(int i = 0; i < M; i++)	
		{
			if(nextBozo->endsleep > colony[i].endsleep && !colony[i].yodel)
			{
				nextBozo = &colony[i];
			}
		}
	}

	// You may need to update your statistical counters one last time based on the most recent state of the system
	// You can report your results at the end of the program
	cout << "M: " << M << endl;
	cout << "S: " << S << endl;
	cout << "Y: " << Y << endl;
	cout << "Total simulation time: " << curr_time << endl;
	cout << "Average silent time(%): " << (thisPlanet.silent/curr_time * 100.00) <<"%"<< endl;
	cout << "Average melodious time(%): " << (thisPlanet.melodious/curr_time * 100.00) <<"%"<< endl;
	cout << "Average screechy time(%): " << (thisPlanet.screechy/curr_time*100.00) <<"%"<< endl;
	cout << "Number of attempted yodels: " << thisPlanet.attemptyodel << endl;
	cout << "Perfect yodels(%): " << ((float)thisPlanet.perfectyodel / (float)thisPlanet.attemptyodel * 100.00)<<"%"<< endl;
	cout << "Number of perfect yodels: "<<thisPlanet.perfectyodel << endl;
	
	return 0;
}

/* Generate a random floating point number uniformly distributed in [0,1] */
/* ============== */
double uniform01()
/* ============== */
{
	double randnum;

	/* Get a random positive integer from random() */
	randnum =  1.0 * (double)rand();

	/* Divide by max integer to get something in the range 0.0 to 1.0 */
	randnum = randnum / (1.0 * MAX_INT);
	return randnum;
}

/* Generate a random floating point number exponentially distributed with mean = m */
/* ============== */
double getExponentialVar(double m)
/* ============== */
{
	double randnum, ans;
	randnum = uniform01();
	ans = -(m) * log(randnum);
	return ans;
}
