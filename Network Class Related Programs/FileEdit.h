#ifndef FILEEDIT_H_
#define FILEEDIT_H_

#include <string.h>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include <time.h>

using namespace std;

class FileEdit {
public:
	FileEdit(int x);		//Constructor
	virtual ~FileEdit();	//Deconst
	char * parseHead(char * fc);	//Parses head for hostname, returns host name
	bool getReq(char *);		//Parses message to figure out if message was a GET request, returns bool
	void options();		//set for optional flag, if called, turns optional bold flag to true
	char * spellMistakes(char * fc);	//Main function used to insert spelling mistakes into a message
private:
	string edited;		//String used for editing
	int maxM;			//Maximum errors allowed
	bool op;			//Optional bold flag
	void changeL(int);	//Changes the head of message to account for number of errors
};

#endif /*FILEEDIT_H_*/
