/* Code based off of Prof's TCP server*/
#include <cstdlib>
#include <iostream>
#include <map>
#include <math.h>
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>

using namespace std;

/* Global manifest constants */
#define MAX_MESSAGE_LENGTH 281
#define A_ 'A'
#define a_ 'a'
#define Z_ 'Z'
#define z_ 'z'
#define ZERO '0'
#define NINE '9'
#define f_ 'f'
#define F_ 'F'

/* Global variables */
map<int, string> seq;
map<int, string> sumdictionary;
map<int, string> unk;

int ind;
void catcher(int sig)
{
	exit(0);
}
//Does the sequential hashing
string sequentialHash(string x)
{
	string ans;
	bool found = false;
	map<int, string>::iterator temp;
	//iterate through the keys to check if value is present in dictionary
	for (int i = 0; i < ind; i++)
	{
		temp = seq.find(i);
		if (x.compare(temp->second) == 0)
		{
			found = true;
			break;
		}
	}
	stringstream tempstream;
	//if value was present in dictionary
	if (found)
	{
		tempstream << "0x" << hex << temp->first;
		ans = tempstream.str();
		return ans;
	}
	//if value was not present
	else
	{
		ind = ind % 16777215; //0xffffff  - Done to limit dictionary size
		seq[ind] = x;
		tempstream << "0x" << hex << ind;
		ans = tempstream.str();
		ind++;		//increment index for next word
		return ans;
	}
}
//returns the value hidden in the sequential hash
string seqUnHash(string x)
{
	string ans;
	int temp;
	stringstream s;
	s << hex << x;
	s >> temp;
	ans = seq.at(temp);
	return ans;
}
//Word sum hashing
string sumHash(string x)
{
	string ans;
	int tempsum = 0;
	for (int i = 0; i < x.length(); i++)
	{
		tempsum += (int)x.at(i);
	}
	tempsum = tempsum % 16777215; //0xffffff - done to limit dictionary size

	auto temp = sumdictionary.find(tempsum);

	stringstream tempstream;
	//if found in dictionary
	if (temp != sumdictionary.end())
	{
		tempstream << "0x" << hex << temp->first;
		ans = tempstream.str();
	}
	else
	{
		sumdictionary[tempsum] = x;
		tempstream << "0x" << hex << tempsum;
		ans = tempstream.str();
	}
	return ans;
}
string sumUnHash(string x)
{
	string ans;
	int temp;
	stringstream s;
	s << hex << x;
	s >> temp;
	ans = sumdictionary.at(temp);
	return ans;
}
string unkHash(string x)
{
	string ans;
	//Hash based off of Java's implementation of hashcode()
	int PRIME = 23;
	int n = x.length();
	int hashnum = 0;
	//hash = sum( word * 23 ^ (n - i))
	for (int y = 0; y < n; y++)
	{
		hashnum += ((int)x.at(y) * pow(PRIME, (n - y)));
	}

	hashnum = hashnum % 16777215; //0xffffff
	auto temp = unk.find(hashnum);
	stringstream tempstream;
	if (temp != unk.end())
	{
		tempstream << "0x" << hex << temp->first;
		ans = tempstream.str();
	}
	else
	{
		unk[hashnum] = x;
		tempstream << "0x" << hex << hashnum;
		ans = tempstream.str();
	}
	return ans;
}
string unkUnHash(string x)
{
	string ans;
	int temp;
	stringstream s;

	s << std::hex << x;
	s >> temp;

	ans = unk.at(temp);
	return ans;
}

int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		cerr << "<programname> <portnum>" << endl;
		exit(-1);
	}
	int MYPORTNUM;
	MYPORTNUM = atoi(argv[1]);

	struct sockaddr_in server;
	static struct sigaction act;
	int serversocket;

	ind = 0;

	//Set up a signal handler to catch some weird termination conditions
	act.sa_handler = catcher;
	sigfillset(&(act.sa_mask));
	sigaction(SIGPIPE, &act, NULL);

	//Initialize server sockaddr structure
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(MYPORTNUM);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//Set up the transport-level end point to use TCP
	if ((serversocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		cerr << "Server: socket() call failed!" << endl;
		exit(1);
	}
	cout << "Serversocket created successfully" << endl;

	//Code based off of an online tutorial - used to force rebinding of socket
	int opt = 1;
	if (setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//Bind specific address and port to the end point
	if (bind(serversocket, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
	{
		cerr << "Server: bind() call failed!" << endl;
			exit(1);
	}

	//Start listening for incomming connections from clients
	if (listen(serversocket, 5) == -1)
	{
		cerr << "Server: listen() call failed" << endl;
			exit(1);
	}

	cout << "Welcome!!" << endl;
	cout << "Server listening on port " << MYPORTNUM << endl;

	int clientsocket;
	while (true)
	{
		//Accept a connection
		clientsocket = accept(serversocket, NULL, NULL);
		if (clientsocket < 0)
		{
			cerr << "Server: accept() call failed " << endl;
			exit(1);
		}
		cout << "Connection established" << endl;
		char rcv_message[MAX_MESSAGE_LENGTH];
		bzero(rcv_message, MAX_MESSAGE_LENGTH);

		string messageout(MAX_MESSAGE_LENGTH, '\0');
		string messagein(MAX_MESSAGE_LENGTH, '\0');
		string aword(MAX_MESSAGE_LENGTH, '\0');
		string ahash((MAX_MESSAGE_LENGTH * 3), '\0');

		int obt = recv(clientsocket, rcv_message, MAX_MESSAGE_LENGTH, 0);

		int choice;
		bool endstr;

		messageout.clear();
		messagein.clear();
		bool added = false;
		while (obt > 0)
		{
			messagein = string(rcv_message);

			choice = atoi(messagein.substr(0, 1).c_str());
			messagein.erase(0, 1);

			cout << "Message recieved: " << messagein << endl;

			endstr = false;

			int x = 0;

			while (!endstr)
			{
				bool hashedval = false;
				//if end of message
				if (messagein.length() == 0)
				{
					messageout += "\0";		//make message null terminated
					hashedval = false;
					endstr = true;
				}
				if (!endstr)
				{
					//Check if hex input
					if (messagein.length() > 1 && messagein.at(x) == ZERO && messagein.at(x + 1) == 'x')
					{
						messagein.erase(0, 2);	//erase the 0x
						ahash.clear();
						added = false;
						//add into hash the rest of the chars part of the number
						while ((messagein.length() > x) && ((messagein.at(x) >= ZERO && messagein.at(x) <= NINE) || (messagein.at(x) >= A_ && messagein.at(x) >= F_) || (messagein.at(x) >= a_ && messagein.at(x) >= f_)))
						{
							ahash += messagein.at(x);
							messagein.erase(0, 1);
						}
						hashedval = true;
					}
					//check if it's an alphabetical char
					else if ((messagein.at(x) >= A_ && messagein.at(x) <= Z_) || (messagein.at(x) >= a_ && messagein.at(x) <= z_))
					{
						aword.clear();
						added = false;
						while ((messagein.length() > 0)&&((messagein.at(x) >= A_ && messagein.at(x) <= Z_) || (messagein.at(x) >= a_ && messagein.at(x) <= z_)))
						{
							aword += messagein.at(x);
							messagein.erase(x, 1);
						}
					}
					if(messagein.length() >= 0)
					{
						//append the message depending on type of hash/unhash
						if ((!added) && choice == 1)		//seq hash
						{
							if (!hashedval)
							{
								messageout += sequentialHash(aword);
							}
							else
							{
								messageout += seqUnHash(ahash);
								hashedval = false;
							}
							added = true;
						}
						else if ((!added) && choice == 2)	//sum hash
						{
							if (!hashedval)
							{
								messageout += sumHash(aword);
							}
							else
							{
								messageout += sumUnHash(ahash);
								hashedval = false;
							}
							added = true;
						}
						else if ((!added) && choice == 3)		//my hash
						{
							if (!hashedval)
							{
								messageout += unkHash(aword);
							}
							else
							{
								messageout += unkUnHash(ahash);
								hashedval = false;
							}
							added = true;	
						}
						//if messagein still has leftovers, add the unknown character to the out message
						if (messagein.length() > 0)
						{
							messageout += messagein.substr(0, 1);
							messagein.erase(0, 1);
						}
					}	

				}
			}

			cout << "Server is sending: " << messageout << endl;
			
			int statusSend;

			statusSend = send(clientsocket, (char *)messageout.c_str(), messageout.length(), 0);

			if (statusSend < 0)
			{
				cerr << "Server: error in send()" << endl;
				exit(-1);
			}

			messageout.clear();

			bzero(rcv_message, MAX_MESSAGE_LENGTH);
			obt = recv(clientsocket, rcv_message, MAX_MESSAGE_LENGTH, 0);
		}

		close(clientsocket);
	}
	
}
