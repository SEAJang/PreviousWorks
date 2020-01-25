/*
 * Assignment 2
 * Question 6
 * Sally 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <string.h>

using namespace std;

const int MAX_NUM_FILES = 1000;
const int MAX_PATHNAME = 512;

//Data to be saved for each file found
struct AFile
{
	char * fileName;
	int fileSize;
};

//Sort by File Name
bool sortFileName(const AFile &l, const AFile &r)
{
	return l.fileName > r.fileName;
}
//Sort by File Size
bool sortFileSize(const AFile &l, const AFile &r)
{
	return l.fileSize < r.fileSize;
}

int main(int argc, char ** argv)
{
	char * file_ex;
	int file_c;

	//handle command line arguments
	if (argc != 3)
	{
		cerr << "Usage: scan <file extension> <number of files>" << endl;
		return -1;
	}
	else
	{
		file_ex = argv[1];
		file_c = atoi(argv[2]);
	}

	//concatenate string, then change back to char *
	string temp = "find . -type f -name '*.'";
	temp = temp + file_ex;
	const char * t = temp.c_str();

	FILE * fp = popen(t, "r");
	if(fp == NULL) 
	{
		cerr << "popen failed " << endl;
		return -1;
	}

	char buff[MAX_PATHNAME];

	int nFiles = 0;

	struct AFile files[MAX_NUM_FILES];
	struct stat st;

	while(fgets(buff, MAX_PATHNAME, fp))
	{
		int len = strlen(buff) - 1;
		files[nFiles].fileName = strndup(buff, len);	//save file name
		if (0 != stat(files[nFiles].fileName, &st))
		{
			cerr << "Stat failed" << endl;
			return -1;
		}
		files[nFiles].fileSize = st.st_size;	//save file size
		nFiles++;	//increment number of files found
	}
	fclose(fp);

	int totalSize = 0;
	sort(files, files + nFiles, sortFileName);	//Sort alphabetically, then
	sort(files, files + nFiles, sortFileSize);	//sort based on file size

	//print out file paths & sizes up to either max number of files found or number specified
	for (int i = 0;(i < file_c) && (i < nFiles); i++)
	{
		totalSize += files[i].fileSize;
		cout << files[i].fileName << "\t" << files[i].fileSize << endl;
	}
	//print out total
	cout << "Total size:\t" << totalSize << endl;
	return 0;
}

