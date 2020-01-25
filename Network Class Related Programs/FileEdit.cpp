#include "FileEdit.h"	//will contain the description for all functions/variables
//for debug
#include "iostream"

//int x = the maximum errors given by initial consol parameter
FileEdit::FileEdit(int x)
{
	string edited;
	srand(time(NULL));		//initialize time stamp for rand
	maxM = x;
	op = false;
}

FileEdit::~FileEdit()
{

}
void FileEdit::options()
{
	op = true;
}
char * FileEdit::parseHead(char * fc)
{
	edited = fc;
	int t = edited.find("Host: ");
	edited.erase(0, (t + 6));
	t = edited.find("\r\n");
	edited.erase(t, edited.length() - 1);
	//std::cout<<edited<<std::endl;
	return (char *)edited.c_str();
}
bool FileEdit::getReq(char * fc)
{
	int t = -1;
	t = edited.find("GET");
	if (t == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}
char * FileEdit::spellMistakes(char * fc)
{
	edited = string(fc);
	int f = -1;
	//std::cout<<edited << std::endl;
	f = edited.find("200 OK");
	if (f != -1)	//check if 200 OK message, otherwise return whole message back
	{
		f = -1;
		f = edited.find("html");
		int y = -1;
		y = edited.find("text/plain");
		int k = 0;
		if (f != -1)	//check if html message
		{

			int s = f + 4;
			int e = -1;
			s = edited.find("<p>", s);		//find the paragraph tags
			if (s != -1)
			{
				s = s + 3;
			}
			e = edited.find("</p>", s);
			while (s != -1 && k < maxM)
			{
				int temp = (rand() % (e - s)) + s;
				char cmp = 'A';
				char cmp2 = 'Z';
				char cmp3 = 'a';
				char cmp4 = 'z';

				//std::cout<<edited.substr(s, e-s) << std::endl;
				//only edit if it is an alphabetical character
				if (((edited.at(temp) >= cmp && edited.at(temp) <= cmp2) || (edited.at(temp) >= cmp3 && edited.at(temp) <= cmp4)) && s != e)
				{
					char tempchar = abs(rand() % 25) + 97;
					string tempch(1, tempchar);
					if (op)	//bolded
					{
						string tc = "<b></b>";
						tc.insert(3, tempch);
						//std::cout<<"LENG"<<tc.length()<<std::endl;

						edited.erase(temp, 1);
						edited.insert(temp, tc);
					}
					else	//not bolded
					{
						edited.erase(temp, 1);
						edited.insert(temp, tempch);
					}
					k++;
				}
				//std::cout<<edited.substr(s, e-s) << std::endl;
				//to find the next paragraph to insert spell error 
				s = edited.find("<p>", s);
				if (s != -1)
				{
					s = s + 3;
				}
				e = edited.find("</p>", s);
			}
			//if bold characters were added, add to the content length
			if (op)
			{
				this->changeL(k);
			}
		}
		else if (y != -1)	//check if plain/text message
		{
			int s = y + 27;
			int e = -1;
			s = edited.find("\n", s);
			if (s != 0)
			{
				s++;
			}
			e = edited.find("\n", s);
			//insert error for each line of text, only change text if an alphabetical character
			while (s != 0 && s != -1 && e != -1 && k < maxM)
			{
				int temp = (rand() % (e - s - 1)) + s;
				char tempchar = abs(rand() % 25) + 97;
				char tc[2] = "A";
				tc[0] = tempchar;
				char cmp = 'A';
				char cmp2 = 'Z';
				char cmp3 = 'a';
				char cmp4 = 'z';
				if (s != e && (edited.at(temp) >= cmp && edited.at(temp) <= cmp2) || (edited.at(temp) >= cmp3 && edited.at(temp) <= cmp4))
				{
					edited.replace(temp, 1, (const char *)tc);
					k++;
				}
				s = edited.find("\n", e);
				if (s != 0)
				{
					s++;
				}
				e = edited.find("\n", s);
			}

		}
		else	//else return (likely gif) whole message back
		{
			return fc;
		}
	}
	else
	{
		return fc;
	}

	//std::cout<<edited<<std::endl;
	char * result = strdup(edited.c_str());
	return result;
}
//int k = number of errors inserted
void FileEdit::changeL(int k)
{
	int x = edited.find("Content-Length: ");
	int z = edited.find("\n", x + 16);

	string temp = edited.substr(x + 16, z);
	int y = k * 7;
	int w = atoi((char *)temp.c_str()) + y;

	edited.erase(x + 16, (z - x - 16));
	edited.insert(x + 16, std::to_string(w));
}
