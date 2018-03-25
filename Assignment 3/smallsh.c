#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>	
#include <fcntl.h>
#include <signal.h>

int childExitMethod = -10;

//This function executes when the child process is killed by a SIGINT

void carrot_c_handler(int sig_num)
{	
	char* message = "terminated by signal 2.\n";
	write(STDOUT_FILENO, message, 25);
	childExitMethod = sig_num;
}


//This function is what the child process executes and takes the necessary steps before the exec() function is called

void execute(char** argv, int arg_count)
{


	if(arg_count > 1)
	{
		if(strcmp(argv[1], ">") == 0)
		{
			int fd;
			fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			fcntl(fd, F_SETFD, FD_CLOEXEC);

			int result = dup2(fd, 1);
			execlp(argv[0], argv[0], NULL);
			perror("Execution failed.");
			fflush(stdout);
			exit(1);
		}

		else if(strcmp(argv[1], "<") == 0)
		{
			int fd;
			fd = open(argv[2], O_RDONLY);
			fcntl(fd, F_SETFD, FD_CLOEXEC);

			int result = dup2(fd, 0);

			if(result == -1)
			{
				printf("Cannot open bad file for input.\n");
				exit(1);
			}
			execlp(argv[0], argv[0], NULL);
			perror("Execution failed.");
			fflush(stdout);
			exit(1);
		}

		else
		{
			execvp(argv[0], argv);
			perror("Execution failed.");
			fflush(stdout);
			exit(1);
		}
				
	}
	else
	{
		execvp(argv[0], argv);
		perror("Execution failed.");
		fflush(stdout);
		exit(1);
	}
}


	

int  main() {

	int P_IDS[15]; //An array of process IDs
	int P_IDS_Count = 0;	// The amount of processes in the array
	int BGP_Check = 0;	// A check of whether or not there are background processes

	//Set up sigaction for interruptions
	struct sigaction SIGINT_action;
	SIGINT_action.sa_handler = carrot_c_handler;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags=0;		
	sigaction(SIGINT, &SIGINT_action, NULL);

	int pid_check, i;

	//while loop that prompts user and delegates processes
	
	while(true)
	{

		//If there are background processes, check if any of them are completed, then tell the user which process is completed 
		
		if (BGP_Check == 1)
		{
			for(i=0; i<P_IDS_Count; i++)
			{	
				pid_check = waitpid(P_IDS[i], &childExitMethod, WNOHANG);
				
				if(pid_check != 0)
				{
					printf("background pid %i is done: exit value %i\n", P_IDS[i], childExitMethod);	
					fflush(stdout);
					P_IDS[i] = 0;
					P_IDS_Count--;
					if(P_IDS_Count == 0)
						BGP_Check = 0;
				}
			}
		}
		
	
		fflush(stdin);
		char response[2048];
		char cwd[1024];
		char* arguments[512];
		
		memset(response, '\0', sizeof(response)); //set all of response to null terminators
		fflush(stdin);
		fflush(stdout);
		printf(":");
		fflush(stdout);
	

		fgets(response, 2048, stdin);

		if(response[0] == '\n')	//if the response is a blank line, reprompt the user
		{
			continue;
		}

		char* token;
		token = strtok(response, " \n");
		strcpy(response, token);	

		char first_char;
		first_char = response[0];
		int arg_count; 
		arg_count=0;

		//parse the users string by spaces, input each argument as it's own index in the argument array
		while(token != NULL)
		{
				
			arguments[arg_count] = token;
			token = strtok(NULL," \n"); 
			arg_count++;
		}
	
		arguments[arg_count] = NULL; //set the final argument to null (makes execution a lot easeier)
		
		//Built in choose directory function

		if(strcmp(arguments[0], "cd") == 0)
		{
			fflush(stdout);

			if(arg_count == 1)
			{
				chdir(getenv("HOME"));
				fflush(stdout);
			}
			else
			{
	 			chdir(arguments[1]);
			}
		}
	
		//Prints status to user
		
		else if(strcmp(arguments[0], "status") == 0)
		{

			if(WIFEXITED(childExitMethod) != 0)
			{
				int val = WEXITSTATUS(childExitMethod);
				printf("exit value: %i\n", val);
				fflush(stdout);
			}	

			else
			{
				printf("terminated by signal %d \n",childExitMethod);
			}
		}

		//Built in exit function that exists

		else if(strcmp(arguments[0], "exit") == 0)
		{
			exit(0);
		}

		// if the character is a comment, just reprompt user

		else if(first_char == '#')
		{
			continue;
		}	

		// if none of the above, fork off a child process that executes the command.

		else
		{
			pid_t pid=-5;
			pid = fork();
			switch(pid)
			{

				case -1:
				{
					printf("ruh roh");
				}
				case 0:
				{
					if(strcmp(arguments[arg_count-1], "&") == 0)
						arguments[arg_count-1] = NULL;	
					execute(arguments, arg_count);
				}

				default:
				{
					if(strcmp(arguments[arg_count-1], "&") == 0)
					{
						BGP_Check = 1;
						printf("background pid is %i\n", pid);
						P_IDS[P_IDS_Count] = pid;
						P_IDS_Count++;	
					}
			
					else
					{
						waitpid(pid, &childExitMethod, 0);
					}
				}	
			}
		}	 	
	}
		
	return 0;
	
}
