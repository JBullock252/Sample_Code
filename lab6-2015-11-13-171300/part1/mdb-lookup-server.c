
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <signal.h>
#include "mylist.h"
#include "mdb.h"

#define MAXPENDING 5    /* Maximum outstanding connection requests */


static void die(const char *message)
{
		perror(message);
		exit(1);
}

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
		struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    
		if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
				die("signal() failed");
		
		if (argc != 3)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Database Name> <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[2]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        die("socket() failed");
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        die("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        die("listen() failed");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            die("accept() failed");
				
    		char *filename = argv[1];
    		FILE *fp = fopen(filename, "rb");
    		if (fp == NULL) 
        		die(filename);

				FILE *input = fdopen(clntSock, "r");
				if(input == NULL)
						die("Client Socket");
    		/*
     		* read all records into memory
     		*/

    		struct List list;
    		initList(&list);

 	  		int loaded = loadmdb(fp, &list);
    		if (loaded < 0)
        		die("loadmdb");
    
  		  fclose(fp);

    		/*
    		* lookup loop
     		*/

    		char line[1000];
   		 	char key[6];

    		while (fgets(line, sizeof(line), input) != NULL) {

        		// must null-terminate the string manually after strncpy().
        		strncpy(key, line, sizeof(key) - 1);
        		key[sizeof(key) - 1] = '\0';

        		// if newline is there, remove it.
        		size_t last = strlen(key) - 1;
        		if (key[last] == '\n')
            		key[last] = '\0';

        		// traverse the list, printing out the matching records
        		struct Node *node = list.head;
        		int recNo = 1;
						char outPut[57];
        		while (node) {
            		struct MdbRec *rec = (struct MdbRec *)node->data;
            		if (strstr(rec->name, key) || strstr(rec->msg, key)) {	
										int y;
										if((y = sprintf(outPut, "%4d: {%s} said {%s}\n", recNo,
														rec->name, rec->msg)) < 0)
												die("sprint failed");
										if(send(clntSock, outPut, y, 0) < 0)
												die("send failed");
            		}
            		node = node->next;
            		recNo++;
        		}

						//char blankLn[1];
						//blankLn[0] = '\n';
						if(send(clntSock, "\n", sizeof("\n"), 0) < 0)
								die("send blankline failed");
				}	
				
				freemdb(&list);
				fclose(input);
				
			
		}
}
