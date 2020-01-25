/* TCP-based client    */
/* This client interacts with the word length server, */
/* which needs to be running first.                   */
/* Based off of prof's example code for client              */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <map>
#include <iostream>

/* Some generic error handling stuff */
extern int errno;
void perror(const char *s);

/* Manifest constants used by client program */
#define MAX_HOSTNAME_LENGTH 64
#define MAX_CHAR_LENGTH 281

/* Menu selections */
#define ALLDONE 0
#define SEQ 1
#define SUM 2
#define UNK 3

/* Prompt the user to enter a word */
void printmenu()
{
	printf("\n");
	printf("Please choose from the following selections:\n");
	printf("  3 - Enter your tweet(Unk)\n");
	printf("  2 - Enter your tweet(Sum)\n");
	printf("  1 - Enter your tweet(Seq)\n");
	printf("  0 - Exit program\n");
	printf("Your desired menu selection? ");
}

/* Main program of client */
int main(int argc, char * argv[])
{
	int decision = 0;
	int MYPORTNUM;

	if(argc != 3 && argc != 2)
	{
		printf("<programname> <port num> optional:<IP address>\n");
		exit(-1);
	}
	MYPORTNUM = atoi(argv[1]);
	if (argc == 3)
	{
		decision++;
	}
	int sockfd, sockfd2;
	char c;
	struct sockaddr_in server;
	struct hostent *hp;
	char hostname[MAX_HOSTNAME_LENGTH];
	char message[MAX_CHAR_LENGTH];
	char messageback[MAX_CHAR_LENGTH];
	int choice, len, bytes;

	/* Initialization of server sockaddr data structure */
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(MYPORTNUM);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (decision == 0)
	{
		/* use a resolver to get the IP address for a domain name */
		/* I did my testing using csx1 (136.159.5.25)    Carey */
		strcpy(hostname, "csx1.cpsc.ucalgary.ca");
		hp = gethostbyname(hostname);
		if (hp == NULL)
		{
			fprintf(stderr, "%s: unknown host\n", hostname);
			exit(1);
		}
		/* copy the IP address into the sockaddr structure */
		bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	}
	else
	{
		/* hard code the IP address so you don't need hostname resolver */
		server.sin_addr.s_addr = inet_addr(argv[2]);
	}

	/* create the client socket for its transport-level end point */
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "client: socket() call failed!\n");
		exit(1);
	}

	/* connect the socket to the server's address using TCP */
	if (connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "client: connect() call failed!\n");
		perror(NULL);
		exit(1);
	}

	/* Print welcome banner */
	printf("Welcome!\n\n");
	printmenu();

	scanf("%d", &choice);

	/* main loop*/
	while (choice != ALLDONE)
	{
		len = 0;
		char tempch = (choice - 1) + '1';
		message[len] = tempch;
		len++;
		std::cout << "choice: " << choice << std::endl;
		if (choice == SEQ || choice == SUM || choice == UNK)
		{
			/* get rid of newline after the (integer) menu choice given */
			c = getchar();
			/* prompt user for the input */
			printf("Enter your Tweet: ");

			while ((c = getchar()) != '\n')
			{
				message[len] = c;
				len++;
			}
			/* make sure the message is null-terminated in C */
			message[len] = '\0';

			/* send it to the server via the socket */
			send(sockfd, message, len, 0);
			/* see what the server sends back */
			if ((bytes = recv(sockfd, messageback, sizeof(messageback), 0)) > 0)
			{
				//std::cout << "from server" <<messageback << std::endl;
				/* make sure the message is null-terminated in C */
				//messageback[bytes] = '\0';
				printf("Answer received from server: ");
				std::cout << messageback<< std::endl;
				bzero(messageback, sizeof(messageback));
			}
			else
			{
				/* an error condition if the server dies unexpectedly */
				printf("Sorry. Server failed!\n");
				close(sockfd);
				exit(1);
			}
		}
		else
		{
			 printf("Invalid menu selection. Please try again.\n");
		}
		printmenu();
		scanf("%d", &choice);
		bzero(message, MAX_CHAR_LENGTH);
	}

	/* Program all done, so clean up and exit the client */
	close(sockfd);
	exit(0);
}
