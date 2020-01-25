/*
Sally
A5 - Q6
*/

#include <stdio.h>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace std;

class PageReplacement
{
	private:
	int * frames;
	int faults;
	int framel;
	
	void printFormat(string alg)
	{
		stringstream temp; 
		for(int i = 0; i < framel; i++)
		{
			temp << frames[i] << " ";
		}
		string tempf = temp.str();
		
		cout << alg << ":" << endl;
		cout << " - frames: " << tempf << endl;
		cout << " - page faults: " << faults << endl;
	}
	public:
	PageReplacement(int * f, int l)
	{
		frames = f;
		faults = 0;
		framel = l;	
	}
	void Optimal(int * refs, int nr)
	{
		faults = 0;
		int reference[nr][3];
		int c = 0;
	
		for(int i = 0; i < nr; i++)	//initialize the reference array to be used
		{
			bool found = false;
			for(int j = 0; j < c; j++)
			{
				if (reference[j][0] == refs[i])
				{
					reference[j][1]++;
					found = true;
					if(reference[j][2] > i)
					{
						reference[j][2] = i;
					}
					break;
				}
			}
			if (!found)
			{
				reference[c][0] = refs[i];
				reference[c][1] = 1;
				reference[c][2] = i;
				c++;
			}
		}
		for(int i = 0; i < framel; i++)	//initialize the frames
		{
			frames[i] = -1;
		}
		for(int i = 0; i < nr; i ++)	//start simulation
		{
			bool loop = true;
			bool checked = false;
			int k = 0;
			while(loop)
			{
				//check though frames
				if(frames[k] == refs[i])	//if in frame
				{
					for(int j = 0; j < c; j++)	//update the reference
					{
						if(reference[j][0] == refs[i])
						{
							reference[j][1]--;	//decrement the count
							if(reference[j][1]<=0)	//if none left, set to -1
							{
								reference[j][2] = -1;
							}
							else			//if some are still left, find the next it appears
							{
								for(int l = (i+1); l < nr; l++)	
								{
									if(refs[l] == reference[j][0])
									{
										reference[j][2] = l;
										break;
									}
								}
							}
						}
					}
					loop = false;	
				}
				else if(frames[k] == -1)	//if empty frame
				{
					faults++;
					loop = false;
					frames[k] = refs[i];
 					for(int j = 0; j < c; j++)      //update the reference
					{
						if(reference[j][0] == refs[i])
						{ 
							reference[j][1]--;      //decrement the count
							if(reference[j][1]<=0)  //if none left, set to -1
							{
							   reference[j][2] = -1;
							}
							else                    //if some are still left, find the next it appears
							{
							   for(int l = (i+1); l < nr; l++)
								{
									if(refs[l] == reference[j][0])
									{
										reference[j][2] = l;
										break;
									}
								}
							}
						}
					}
				}
				else if(checked)			//if all frames are filled
				{
					bool foundreplace = false;
					int replaceframe = -1;
					int referenceind = -1;
					int j = 0;				//will be used to get frame index
					while(!foundreplace)
					{
						int tempr = -1;
						for(int k = 0; k < c; k++)		//find the index of the page in reference
						{
							if(reference[k][0] == frames[j])
							{
								tempr = k;
								break;
							}
						}
						if(reference[tempr][1] <= 0)	//if there are no more references to page
						{
							replaceframe = j;
							referenceind = tempr;		
							foundreplace = true;
						}
						else
						{
							if(replaceframe == -1)
							{
								replaceframe = j;
								referenceind = tempr;
							}
							else if(reference[referenceind][2] < reference[tempr][2])		//obtain the furthest referenced page
							{
								replaceframe = j;
								referenceind = tempr;
							}
						}

						j++;
						if(j == framel)
						{
							foundreplace = true;
						}
					}
					frames[replaceframe] = refs[i];
					faults++;

					for(int j = 0; j < c; j++)      //update the reference
					{
						if(reference[j][0] == refs[i])
						{
							reference[j][1]--;      //decrement the count
							if(reference[j][1]<=0)  //if none left, set to -1
							{
							    reference[j][2] = -1;
							}
							else                    //if some are still left, find the next it appears
							{
							   for(int l = (i+1); l < nr; l++)
							   {
								   if(refs[l] == reference[j][0])
								   {
									   reference[j][2] = l;
									   break;
								   }
							   }
							}
						}
					 }
	
					loop = false;
				}
				else	//increment k
				{
					k++;
					if(k%framel == 0)
					{
						k = k % framel;
						checked = true;
					}
				}
			}		
		}
		string t = "Optimal";
		printFormat(t);
	}
	void LRU(int * refs, int nr)
	{
		faults = 0;
		int time[framel];
		
		for(int i = 0; i < framel;i++)
		{
			frames[i] = -1;
		}
		for(int i = 0; i < nr; i++)
		{
			int j = 0;
			bool first = false;
			bool loop = true;
			while(loop)
			{
				if(frames[j] == refs[i])	//page is already in frame
				{
					time[j] = i;
					loop = false;
				}
				else if(frames[j] == -1)	//frame is empty
				{
					frames[j] = refs[i];
					time[j] = i;
					faults++;
					loop = false;
				}
				else if(first)			//checked through all frames
				{
					int temp = nr;
					int tempi = -1;
					for(int k = 0; k < framel; k ++)	//check though the times
					{
						if(time[k] < temp)
						{
							temp = time[k];
							tempi = k;
						}
					}
					time[tempi] = i;
					frames[tempi] = refs[i];
					faults++;
					loop = false;
				}
				else
				{
					if(j < framel)
					{
						j++;
					}
					else 
					{
						first = true;
						j = 0;
					}
				}
			}
		}
		
		string t = "LRU";
		printFormat(t);
	}
	void Clock(int * refs, int nr)
	{
		faults = 0;
		int refb[framel];
		int clock = 0;
		for(int i = 0; i < framel; i++)
		{
			frames[i] = -1;
			refb[i] = 0;
		}
		for(int i = 0; i < nr; i++)
		{
			bool hit = false;

			for(int k = 0; k < framel; k++)
			{
				if(frames[k] == refs[i])
				{
					hit = true;
				}
			}
			while(!hit)
			{
				if(refb[clock] == 0)
				{
					faults++;
					refb[clock] = 1;
					frames[clock] = refs[i];
					hit = true;
				}
				else
				{
					refb[clock] = 0;
					clock++;
					clock = clock % framel;
				}
			}
		}		
		string t = "Clock";
		printFormat(t);
	}
};

int main(int argc, char * const argv[])
{
	if (argc != 2)
	{
		cerr << "Usage: pagesim <#frames>" << endl;
		return -1;
	}
	else
	{
		if(atoi(argv[1])<1 || atoi(argv[1])> 20)
		{
			cerr << "Number of frames range 1-20"<<endl;
			return -1;
		}
	}
	int numbers[5000];
	int tot = 0;
	bool loop = true;
	while (loop)
	{
		int num;
		if (1 != scanf("%ld", &num))
		{
			loop = false;
		}
		else
		{
			numbers[tot] = num;
			tot++;
		}
			
	}
	int temp[atoi(argv[1])];
	PageReplacement sim = PageReplacement(temp, atoi(argv[1]));
	
	sim.Optimal(numbers, tot);;
	sim.LRU(numbers, tot);
	sim.Clock(numbers, tot);
	
	return 0;
}
