#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); }


//this is the encrypt function
char* encrypt(char* key, char* text)
{
	int key_length = strlen(key);
	int text_length = strlen(text);
	char* cipher = malloc(sizeof(char) * text_length);
	int i;
	for(i=0; i<=text_length; i++)
	{

		char t1 = text[i];
		char k1 = key[i];
		
		if(t1 == ' ')
			t1 = '[';
	
		if (k1 == ' ')
			k1 = '[';
		
		unsigned int a = t1 - 'A';
		unsigned int b = k1 - 'A';
		unsigned int c = 'A' + ((a + b) % 27);

		if(c == '[')
			c = ' ';
		cipher[i] = c;


	}
	
	cipher[i] = '\0';
	return cipher;
}



int main(int argc, char * argv[])
{


	//setting up addresses and sockets
	int portNumber, listenSocketFD, establishedConnectionFD, first_file, second_file, send_check;
	struct sockaddr_in serverAddress, clientAddress;
	socklen_t sizeOfClientInfo;
	char buffer [256];

	memset((char *)&serverAddress, '\0', sizeof(serverAddress));
	portNumber = atoi(argv[1]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverAddress.sin_addr.s_addr = INADDR_ANY;	
	
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if(listenSocketFD < 0) error("ERROR opening socket");

	//bind the socket
	if(bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		error("Error on binding");

	listen(listenSocketFD,5);

	int socket_count;
	socket_count = 0;

	//multiserver while loop
	while(socket_count <= 5){
		//I FORK HERE FORK FORK FORK 
		pid_t spawnPID = -5;
		spawnPID = fork();
			
		switch(spawnPID)
		{
			case 0:
				sizeOfClientInfo = sizeof(clientAddress);
				
				establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
				socket_count++;

				if(establishedConnectionFD < 0) error("ERROR on accept");	
				
					
				memset(buffer,'\0', 256);
				first_file = recv(establishedConnectionFD, buffer, 255, 0);

				if(first_file < 0) error("ERROR reading from socket");
				 // open the files
				FILE* fp = fopen(buffer, "r");
				fseek(fp, 0, SEEK_END);
				long fsize = ftell(fp);
				fseek(fp, 0, SEEK_SET);
			
				char *string = malloc(fsize + 1);
				fread(string, fsize, 1, fp);
				string[strcspn(string, "\n")] = '\0';

				first_file = send(establishedConnectionFD, "k", 1,0);

				memset(buffer, '\0', 256);
				second_file = recv(establishedConnectionFD, buffer, 255, 0);

				FILE* fp2 = fopen(buffer, "r");
				fseek(fp2, 0, SEEK_END);
				long f2size = ftell(fp2);
				fseek(fp2, 0, SEEK_SET);
			
				char *key_string = malloc(f2size + 1);
				fread(key_string, f2size, 1, fp2);
				key_string[strcspn(key_string, "\n")] = '\0';

				fclose(fp);
				fclose(fp2);
				//send the cipher back to the client			
				int q;
				char* c = encrypt(key_string, string);
				q = send(establishedConnectionFD, c, (sizeof(char)*strlen(string)),0);				
				
				
				if(second_file < 0) error("ERROR writing to socket");

				close(establishedConnectionFD);
				socket_count--;
				exit(0);
			
			default:
				wait(NULL);	
		}
	}
	close(listenSocketFD);
	return 0;
}
