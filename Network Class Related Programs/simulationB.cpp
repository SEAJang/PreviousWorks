#include <stdio.h>
#include <queue>
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
using namespace std;
int main(int argc, char * const argv[])
{
	char * filename;
	char * bonusFile;
	bool bonus = false;
	int buffers[12]	= {0, 1, 4, 6, 25, 50, 75, 100, 100, 100, 100, 100};
	long wifi[12] = {11, 11, 11, 11, 11, 11, 11, 11, 6, 11, 30, 54};
	FILE	*fp;
	FILE	*bfp;
	//handle input error
	if (argc != 2 && argc != 3)
	{
		cerr << "Usage: readFile <input file> optional: <input file>" << endl;
		return -1;
	}
	else
	{
		filename = argv[1];
		if (argc == 3)
		{
			bonus = true;
			bonusFile = argv[2];
		}
	}
	if(!bonus)
	{
		cout << "WifiCap(Mbps)\t WiFi(bytes/s)\tBufferSize\tTotPacket\tDelivered\tPacketLost\tPackLoss%\tAverageDelay(ms)" << endl;
	}
	else
	{
		cout << "WifiCap(Mbps)\tWifi(bytes/s)\tBufferSize\tTotPackets\tFirDeliv\tSecDeliv\tFirstLost\tSecondLost\tTotLoss%\tAvgDelay(ms)" << endl;
	}
	for(int s = 0; s < 12; s++)					//iterate through the different senarios
	{
		int buffSize = buffers[s];
		double wifiCap = (double)wifi[s];
		wifiCap = 125000 * wifiCap;
		fp = fopen(filename, "r");
	
		 //handle file presence error
		if (fp == NULL)
		{
			cerr << "Could not open file" << filename << "\n";
			exit(-1);
		}
		if(bonus)
		{
			bfp = fopen(bonusFile, "r");
			//handle file presence error
			if (bfp == NULL)
			{
				cerr << "Could not open file" << filename << "\n";
				exit(-1);
			}
		}
		long ind = 0;			//queue size
		long packLost = 0;		//# packets lost
		long totpack = 0;		//total packets sent
		
		double currentT = 0;		//time read in from file
		double ttl = 0;			//time to leave queue
		double packSize = 0;		//size of packets(bytes)
		
		double averageWait = 0;		//sum of time packets spent in queue
		queue<double> thebufferL;	//queue for transmission times of packets
		queue<double> thebufferI;	//queeu for initial arrival time of packets
		long anotherind = 0;		//keeps track of packets delivered properly
		double actualTime = 0;
		
		bool setStart = false;		//bool used to keep track of if initial time set
		long fdel = 0;			//#packets delivered  for first file(if bonus)
		long bdel = 0;			//#packets delivered for second file(if bonus)
		long floss = 0;			//#packets lost for first(if bonus)
		long bloss = 0;			//#packets lost for second(if bonus)
		if(!bonus)
		{
			while(!feof(fp))
			{
				
				fscanf(fp, "%lf", &currentT);			//Read Time Arrived
				fscanf(fp, "%lf\n", &packSize);			//Read Packet size
				totpack++;								//Increment Packet Count
				if(!setStart)							//Set Start time
				{
					actualTime = currentT;
					setStart = true;
				}
				bool checkingBuff = true;
				while (checkingBuff)
				{
					if (thebufferL.size() == 0)		//if buffer is empty
					{
						actualTime = currentT;
						checkingBuff = false;
					}
					else if (thebufferI.front() > actualTime)	//if packet arrival is greater, increment actual time
					{
						actualTime = thebufferI.front();
					}
					else if (((thebufferL.front()+ actualTime) <= currentT )&& (actualTime >= thebufferI.front()))
					{
						actualTime += thebufferL.front();
						double temp = max(actualTime - thebufferI.front()-thebufferL.front(), 0.0);
					
						averageWait += temp;
								
						thebufferL.pop();
						thebufferI.pop();
						anotherind++;
						ind--;
					}
					else					//if time hasn't been enough
					{
						checkingBuff = false;
					}
				}
				if(ind < buffSize)
				{
					ind++;                                  //Increment numb of packets in buffer
					thebufferI.push(currentT);              //Time arrived of packet
					ttl = packSize / wifiCap;
					thebufferL.push(ttl);                   //Push time for packet to leave
				}
				else
				{
					packLost++;				//increment packetloss
				}
			}
			fclose(fp);//Close file
		}
		else
		{
			//Additional variables to use for second file
			double ttlbonus = 0;
			double currentTbonus = 0;
			double packSizebonus = 0;

			bool doneRdOri = true;
			bool doneRdBon = true;
			while(!feof(fp) || !feof(bfp))
			{
				if (doneRdOri == true && !feof(fp))
				{
					fscanf(fp, "%lf", &currentT);			//Read Time Arrived
					fscanf(fp, "%lf\n", &packSize);			//Read Packet size
					totpack++;					//Increment Packet Count
					doneRdOri = false;
				}
				else if(feof(fp))
				{
					currentT = numeric_limits<double>::max();
				}
				if(doneRdBon == true && !feof(bfp))
				{
					fscanf(bfp, "%lf", &currentTbonus);		//Read Time Arrived
					fscanf(bfp, "%lf\n", &packSizebonus);		//Read Packet size
					totpack++;					//Increment Packet Count
					doneRdBon = false;
				}
				else if(feof(bfp))
				{
					currentTbonus = numeric_limits<double>::max();
				}

				if(!setStart)						//Set Start time
				{
					if(currentT <= currentTbonus)			//Pick the one with smaller time
					{
						actualTime = currentT;
					}
					else
					{
						actualTime = currentTbonus;
					}
					setStart = true;
				}
				bool checkingBuff = true;
				long thisint = 0;
				while (checkingBuff)
				{	
					if (thebufferL.size() == 0)		//if buffer is empty
					{
						if (currentT < actualTime)	//checks if packet arrived before the incremented times
						{
							checkingBuff = false;
							thisint = ind + 1;
						}
						else if (currentTbonus < actualTime)
						{
							checkingBuff = false;
							thisint = ind + 1;
						}
						else if((currentT <= currentTbonus)&&(currentT >= actualTime) && !feof(bfp))	//move timer up if required
						{
							actualTime = currentT;
						}
						else if (!feof(fp) && feof(fp)&&(currentT >= actualTime))
						{
							actualTime = currentT;
						}
						else if (feof(fp) && !feof(bfp) && (currentTbonus >= actualTime))
						{
							actualTime = currentTbonus;
						}
						else if ((currentTbonus <= currentT) && (currentT>=actualTime) && !feof(fp))
						{
							actualTime = currentTbonus;
						}
						checkingBuff = false;
					}
					else if (thebufferI.front() > actualTime)	//if packet arrival is greater, increment actual time
					{
						actualTime = thebufferI.front();
					}
					else if((((thebufferL.front()+ actualTime) <= currentT ) || ((thebufferL.front() + actualTime) <= currentTbonus))&& (actualTime >= (thebufferI.front())))
					{
						actualTime += thebufferL.front();			//increment time by packet transmisison time
						double temp = max(actualTime - thebufferI.front()-thebufferL.front(), 0.0);	//do this to account if 0 loops around
						averageWait += temp;			//add to sum of time packet spent in queue
						thebufferL.pop();		//pop packet from queues
						thebufferI.pop();
						anotherind++;			//increment number of delivered packets
						ind--;				//decrement number of packets in queue
					}
					else					//if time hasn't been enough
					{
						// cout << "ActualTime: " << actualTime << " BuffL: " << thebufferL.front() << " bufferI: " << thebufferI.front() << endl;
						checkingBuff = false;
					}
				}
				if((ind < buffSize)&&(thisint < buffSize))		//If there's room in buffer to recieve packet, or if there was if packet arrived before
				{
					ind++;                                  //Increment numb of packets in buffer
					if(currentT <= currentTbonus)		//choose whichever packet arrived first
					{
						thebufferI.push(currentT);              //Time arrived of packet
						ttl = packSize / wifiCap;
						thebufferL.push(ttl);                   //Push time for packet to leave
						doneRdOri = true;
						fdel++;
					}
					else
					{
						thebufferI.push(currentTbonus);
						ttlbonus = packSizebonus / wifiCap;
						thebufferL.push(ttlbonus);
						doneRdBon = true;
						bdel++;
					}
				}
				else
				{
					if (currentT == currentTbonus)		//if both packets arrived at same time, both are lost
					{
						packLost++;
						packLost++;
						floss++;
						bloss++;
						doneRdOri = true;
						doneRdBon = true;
					}
					else if(currentT < currentTbonus)	//loss of the packet which arrived first
					{
						floss++;
						doneRdOri = true;
						packLost++;
					}
					else
					{
						packLost++;
						bloss++;
						doneRdBon = true;
					}
				}
			}
			fclose(fp);		//close files
			fclose(bfp);
		}
		while(thebufferL.size() > 0)				//Deal with the rest of the packets in buffer 
		{
			if (actualTime < thebufferI.front())
			{
				actualTime = thebufferI.front();
			}
			else
			{
				actualTime += thebufferL.front();
				double temp = max(actualTime - thebufferI.front() - thebufferL.front(), 0.0);
				averageWait += temp;
				thebufferL.pop();
				thebufferI.pop();
				anotherind++;
			}
		}
		long temp = wifi[s];	
		double loss = (double)packLost/(double)totpack * 100;	//calc packets lost %
		double wait = averageWait / (double)anotherind * 1000;	//calc the average waittime
		//Print out stats
		printf("%-10d\t", temp);
		printf("%10.0f\t", wifiCap);
		printf("%-10d\t", buffSize);
		printf("%-10d\t", totpack);
		if (!bonus)
		{
			printf("%-10d\t", anotherind);
			printf("%-10d\t", packLost);
			printf("%-10.5f\t", loss);
			printf("%-10.10f\t\n", wait);
		}
		else
		{
			printf("%-10d\t",fdel);
			printf("%-10d\t",bdel);
			printf("%-10d\t",floss);
			printf("%-10d\t",bloss);
			printf("%-10.5f\t", loss);
			printf("%-10.10f\t\n", wait);
		}
	}
	return 0;                                               
}
