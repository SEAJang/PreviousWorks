#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <iostream>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

#include "FileEdit.h"


//Base code based off of tutorial codes

int main(int argc, char * argv[])
{

	int MYPORTNUM;

	if(argc != 3 && argc != 4 )	//checks for proper number of parameters
	{
		printf("<programname> <port number> <max num of errors> <optional>\n");
		exit(-1);
	}

	FileEdit fd = FileEdit(atoi(argv[2]));
	if(argc == 4)
	{
		fd.options();	//Turns on the optional flag to enable bolded errors
	}
	MYPORTNUM = atoi(argv[1]);
	/* Address initialization */
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(MYPORTNUM);
	client.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create the listening socket */
	int lstn_sock;
	lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_sock < 0) {
		printf("Error in socket() while creating lstn_sock\n");
		exit(-1);
	}
	printf("Socket created successfully.\n");
	//Code based off of an online tutorial. Used to force rebinding of socket
	int opt = 1;
	if (setsockopt(lstn_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/* Bind the socket to address and port */
	int status;
	status = bind(lstn_sock, (struct sockaddr *) &client, sizeof(struct sockaddr_in));
	if (status < 0) {
		printf("Error in bind()\n");
		exit(-1);
	}
	printf("Binding completed.\n");

	/* Connect to TCP server */
	status = listen(lstn_sock, 5);
	if (status < 0) {
		printf("Error in listen()\n");
		exit(-1);
	}
	printf("Listening for connection requests...\n");

	struct hostent *p;

	/* Main Loop for listening */
	while (1)
	{
		/* Accept a connection */
		int client_sock;
		client_sock = accept(lstn_sock, NULL, NULL);
		if (client_sock < 0)
		{
			printf("Error in accept()\n");
			exit(-1);
		}
		printf("Connection established.\n");

		char rcv_msg_serv[5000];
		char msg_out[5000];

		int ini = recv(client_sock, rcv_msg_serv, sizeof(rcv_msg_serv), 0);
		bool test = false;
		while (test)		//keeps receving message from client until it'a a GET request
		{
			printf("Not get request\n");
			client_sock = accept(lstn_sock, NULL, NULL);
			ini = recv(client_sock, rcv_msg_serv, sizeof(rcv_msg_serv), 0);
			test = fd.getReq(rcv_msg_serv);
			printf("cs %d \n", client_sock);
		}
		strcpy(msg_out, rcv_msg_serv);

		//parse HTTP message to extract HOST name
		char * pghead = fd.parseHead(rcv_msg_serv);
		//printf(pghead);
		p = gethostbyname(pghead);

		if (p == NULL)
		{
			printf("No host found\n");
			exit(-1);
		}

		/* Address initialization for web server */
		struct sockaddr_in proxy;
		memset(&proxy, 0, sizeof(proxy));
		proxy.sin_family = AF_INET;
		proxy.sin_port = htons(80);
		memcpy((char*)&proxy.sin_addr.s_addr, (char *)p->h_addr, p->h_length);

		int server_sock;
		int status_s;
		server_sock = socket(AF_INET, SOCK_STREAM, 0);


		/* Connect to TCP server for server */
		status_s = connect(server_sock, (struct sockaddr *) &proxy, sizeof(struct sockaddr_in));
		if (status_s < 0)
		{
			printf("Error in connect()\n");
			exit(-1);
		}
		else
		{
			printf("Connected.\n");
		}
		int count;
		//send server the request from client
		count = send(server_sock, msg_out, sizeof(msg_out), 0);

		if (count < 0)
		{
			printf("Error in send\n");
			exit(-1);
		}
		else
		{
			printf("Sending HTTP request to server done\n");
		}
		char rcv_message[500000];	//message buffer
		for (int q = 0; q < 500000; q++)
		{
			rcv_message[q] = '\0';
		}
		int count_recv;

		count_recv = recv(server_sock, rcv_message, sizeof(rcv_message), 0);

		if (count_recv < 0)
		{
				 printf("Error in recv");
				 exit(-1);
		}
		else
		{
				printf("Recieving web server HTTP response\n");
        }

		close(server_sock);
		printf("server_sock closed");
		//send client the message from server with spelling mistakes	
		count = send(client_sock, fd.spellMistakes(rcv_message), sizeof(rcv_message)+1, 0);
		if (count < 0)
		{
			printf("Erorr in send()\n");
			exit(-1);
		}
		else
		{
			printf("Sending HTTP to client\n");
		}

		close(client_sock);
		printf("client_sock is closed\n");

	}

	close(lstn_sock);
	printf("lstn_sock is closed\n");
	return 0;
}
