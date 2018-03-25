#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>

//error message function
void error(const char *msg, int a) { perror(msg); exit(a); }


int main(int argc, char* argv[])
{

	//memsetting for sockets and addresses
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char* buffer;
	char  check_buffer[10];

	memset ((char*)&serverAddress, '\0', sizeof(serverAddress));
	portNumber = atoi(argv[3]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverHostInfo = gethostbyname("localhost");
	
	if(serverHostInfo == NULL) { fprintf(stderr, "ERROR, no such host\n"); exit (2);}
	
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	FILE* fp = fopen(argv[1], "r");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *string = malloc(fsize + 1);
	fread(string, fsize, 1, fp);
	string[strcspn(string, "\n")] = '\0';
	
	FILE* fp2 = fopen(argv[2], "r");
	fseek(fp2, 0, SEEK_END);
	long f2size = ftell(fp2);
	fseek(fp2, 0, SEEK_SET);

	char *key_string = malloc(f2size + 1);
	fread(key_string, f2size, 1, fp2);
	key_string[strcspn(key_string, "\n")] = '\0';
	fclose(fp);
	fclose(fp2);
	//checking key and string length
	if(strlen(key_string) < strlen(string))
	{	
		error("Key longer than plain text.", 1);
	}
	
	int j;
	//checking for bad characters
	for(j=0; j < strlen(string); j++)
	{
		if(string[j] != 32 &&  string[j] != 10 && !(string[j] > 64 && string[j]<=90))
		{
				error("Text file has bad characters", 1);
		}
	}

	int k;
	
	for(k=0; k < strlen(string); k++)
	{
		if(key_string[k] != 32 &&  key_string[k] != 10 && !(key_string[k] > 64 && key_string[k]<=90))
		{
				error("Key file has bad characters", 1);
		}

	}
	
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) error("Error opening socket", 2);
	//actual socket connection
	if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		error("Client: error connection", 2);

	buffer = malloc((sizeof(char) * strlen(string)));
	
	memset(buffer, '\0', sizeof(buffer));
	int n;

	
	charsWritten = send(socketFD, argv[1], strlen(argv[1]), 0);

	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
	//read in the cipher text
	memset(buffer, '\0',(sizeof(char)*strlen(string)));
	charsWritten = send(socketFD, argv[2], strlen(argv[2]), 0);
	charsRead =  recv(socketFD, buffer, (sizeof(char)*strlen(string)), 0);
	
	printf("%s\n", buffer);	
	close(socketFD);
	return 0;

}
