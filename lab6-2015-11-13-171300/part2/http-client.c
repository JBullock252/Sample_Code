#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <netdb.h> 

static void die(const char *errorMessage)  /* Error handling function */
{
		perror(errorMessage);
		exit(1);
}

int main(int argc, char *argv[])
{
		int sock;                        /* Socket descriptor */
		struct sockaddr_in echoServAddr; /* Echo server address */
		char *servIP;                    /* Server IP address (dotted quad) */
		char echoString[100];                /* String to send to echo server */
		char echoBuffer[4096];			     /* Buffer for echo string */
		unsigned int echoStringLen;      /* Length of string to echo */
		
		if (argc != 4)    /* Test for correct number of arguments */
		{
				fprintf(stderr, "Usage: %s <Host Name> <Port Number> <File Path>\n", argv[0]);
				exit(1);
		}
		
		struct hostent *he;
		char *hostName = argv[1];             /* First arg: Host Name */
		int servPort = atoi(argv[2]);         /* Second arg: Port Number*/
		char *filePath = argv[3];						 /* Third org: File Path*/
		echoStringLen = sprintf(echoString, "GET %s HTTP/1.0\r\nHost: %s:%d\r\n\r\n", filePath, hostName, servPort);	
		if((he = gethostbyname(hostName)) == NULL)
				die("gethostbyname");
		servIP = inet_ntoa(*(struct in_addr *)he->h_addr);

		/* Create a reliable, stream socket using TCP */
		if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
				die("socket() failed");
		
		/* Construct the server address structure */
		memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
		echoServAddr.sin_family      = AF_INET;             /* Internet address family */
		echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
		echoServAddr.sin_port        = htons(servPort);     /* Server port */

		/* Establish the connection to the echo server */
		if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
				die("connect() failed");

		echoStringLen = strlen(echoString);          /* Determine input length */
		
		/* Send the string to the server */
		if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
				die("send() sent a different number of bytes than expected");

		FILE *input = fdopen(sock, "r");
		if(input == NULL)
				die("fdopen failed");
		FILE *htmlFile = fopen("make.html", "w");
		if(htmlFile == NULL)
				die("make.html");

		char *returnStatus = "200";
		
		if(fgets(echoBuffer, sizeof(echoBuffer), input) == NULL){
				printf("%s", echoBuffer);
				exit(1);
		}
		
		if(strstr(echoBuffer, returnStatus) == NULL){
					printf("%s", echoBuffer);
					exit(1);
		}
						
		char *blankLn = "\r\n";
		while (fgets(echoBuffer, sizeof(echoBuffer), input)){
			if(strncmp(echoBuffer, blankLn, sizeof(echoBuffer)) == 0)
						break;
		}
		
		size_t n;
		while((n = fread(echoBuffer, 1, sizeof(echoBuffer), input)) > 0){
				if(fwrite(echoBuffer, 1, n, htmlFile) != n)
						die("fread failed");
		}
		if(ferror(input))
				die("fread failed");
		
		printf("\n");    /* Print a final linefeed */
		fclose(input);
		close(sock);
		fclose(htmlFile);
		exit(0);

}
