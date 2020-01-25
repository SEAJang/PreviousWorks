/*
 * Sally Jang

 * Assignment 1 
 * Question 5

 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char * const argv[])
{
	char * filename;
	//handle input error
	if (argc != 2)
	{
		cerr << "Usage: readFile <input file>" << endl;
		return -1;
	}
	else
	{
		filename = argv[1];
	}

	int fd = open(filename, O_RDONLY);
	//handle file presence error
	if (fd < 0) 
	{
		cerr << "Could not open file" << filename << "\n";
		exit(-1);
	}
	//figure out file size
	struct stat filestat;
	stat(filename, & filestat);

	int fileleng = filestat.st_size;

	char filecontent[fileleng];
	//read whole file into array
	read(fd, &filecontent, fileleng);
	//close file
	close(fd);

	int count = 0;
	//go through char array and find new lines
	for (int i = 0; i < fileleng; i++)
	{

		if (filecontent[i] == '\n')
		{
			count++;
		}
	}
	//print onto screen
	cout << count << " " << filename << "\n";
	return 0;
}
