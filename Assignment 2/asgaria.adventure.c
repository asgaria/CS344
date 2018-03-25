#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pthread.h>

//This is the room structure
struct Room {
 	
	char room_name[11];
	int  num_of_connections;
	char type[11];
	struct Room* connections[7];
	int  max_connections;
};

struct Room Rooms[7]; 			//Initial Rooms
struct Room Game_Rooms[7];		//Rooms created with info from file (this is the array used in the game)

//MUTEX MUTEX MUTEX
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;	//initialize mutex

//Checks if all the rooms have a proper amount of connections
bool IsGraphFull()
{
	int i;
	for(i=0; i < 7; i++)
	{
		if(Rooms[i].num_of_connections < 3)
			return false;
	}	
	return true;
}

//This function sees if two rooms are already connected
bool Check_Connection(int a, int b)
{
	if(a == b)
		return true;
	int i;
	for(i=0; i < Rooms[a].num_of_connections; i++)
	{
		if((Rooms[a].connections[i] == &Rooms[b]))
		{
			return true;
		}
	}
	
	return false;
}

//This functions takes two ints (indexs) and connects them if possible
bool Connect(int a, int b)
{
	
	struct Room* RoomA = &Rooms[a];
	struct Room* RoomB = &Rooms[b];

	if(Check_Connection(a, b))
	{
		return false;
	}
	if(RoomA->num_of_connections == RoomA->max_connections)
	{	
		return false;
	}
	if(RoomB->num_of_connections == RoomB->max_connections)
	{
		return false;
	}
	if((RoomA->num_of_connections == 6) || (RoomB->num_of_connections == 6))
		return false;


	RoomA->connections[RoomA->num_of_connections] = RoomB;		
	RoomB->connections[RoomB->num_of_connections] = RoomA;		
	RoomA->num_of_connections++;
	RoomB->num_of_connections++;

	return true;	
}

//This function initializes the sets up an array of Rooms			
void Create_Rooms()
{
	/*set up an array of room names that will be randomly picked */
        char *room_choices[10];
        room_choices[0] = "Compton";
        room_choices[1] = "Bompton";
        room_choices[2] = "Atlanta";
        room_choices[3] = "Detroit";
        room_choices[4] = "Chicago";
        room_choices[5] = "Newark";
        room_choices[6] = "LBC";
        room_choices[7] = "Baltimore";
        room_choices[8] = "Houston";
        room_choices[9] = "Philly";	
	
	int i=0;
        int rooms_filled = 0;

        while(rooms_filled < 7)
        {
                int random_num;
                random_num = rand() % 10;

                if(strcmp(room_choices[random_num],"0"))
                {
                        strcpy(Rooms[i].room_name, room_choices[random_num]);
                        room_choices[random_num] = "0";
                        Rooms[i].max_connections = 6;
			Rooms[i].num_of_connections = 0;

                        if(i == 0)
                        {
                                strcpy(Rooms[i].type,"START_ROOM");
                        }
                        else if (i == 6)
                        {
                                strcpy(Rooms[i].type, "END_ROOM");
                        }
                        else
                        {
                              strcpy(Rooms[i].type,"MID_ROOM");
                        }

                        i++;
                        rooms_filled++; 
               }
			
        }

	while(!IsGraphFull())
	{
		int rand_a, rand_b;
		rand_a = rand() % 7;
		rand_b = rand() % 7;
		Connect(rand_a, rand_b);
	}
	

}

//This function creates the files for each room and fills them with info
void Create_Files(int i)
{
	FILE *file_pointer =  fopen(Rooms[i].room_name, "w");
	fprintf(file_pointer, "ROOM NAME: %s\n", Rooms[i].room_name);
	
	int j;
	for(j=0; j < Rooms[i].num_of_connections; j++)
		fprintf(file_pointer, "CONNECTION %i: %s\n", j+1, Rooms[i].connections[j]->room_name);
	fprintf(file_pointer, "ROOM TYPE: %s\n", Rooms[i].type);
	fclose(file_pointer);
}
		
//This function creates the directory where all the room files are stored			
void Create_Directory()
{
	char dir_name[30];
	char processID[10];
	strcpy(dir_name, "./asgaria.adventure.");
	sprintf(processID, "%d", getpid());
	strcat(dir_name, processID);
	mkdir(dir_name, 0777);		
	chdir(dir_name);

	int i;
	for(i=0; i < 7; i++)
	{
		Create_Files(i);
	}
	
}

/* THIS IS WHERE I READ IN FROM THE FILES. RIGHT HERE! 
 * Open each file and assign names and room types to each index of an array of rooms called Game_Rooms.
 * Then, after the rooms have been named, I go through each file again and assign connections
 */
void Read_In()
{	
	int e;
	for(e = 0; e < 7; e++)
	{
		char line[256];
		char* filename = Rooms[e].room_name;
		FILE *file_pointer = fopen(filename, "r");
		int j = Rooms[e].num_of_connections + 2;
		int i = 0;
		
		while(fgets(line, sizeof(line), file_pointer))
		{
			if(i == 0){
				char* token;
				token = strtok(line, " ");
				token = strtok(NULL, " ");
				token = strtok(NULL, "\n");
				Game_Rooms[e].num_of_connections = j-2;
				strcpy(Game_Rooms[e].room_name, token);
			}

			if(i == j-1){
				char* token;
				token = strtok(line, " ");
				token = strtok(NULL, " ");
				token = strtok(NULL, "\n");
				
				strcpy(Game_Rooms[e].type, token);
			}
			i++;
		}

	fclose(file_pointer);
	}


	int f;
	for(f=0; f<7; f++)
	{
		char con_line[256];
		char* filename = Rooms[f].room_name;
		FILE *connect_file_pointer = fopen(filename, "r");
		int g = Rooms[f].num_of_connections + 2;
		int k =0;
		while(fgets(con_line, sizeof(con_line), connect_file_pointer))
		{
			if(k >=1 && k < g-1)
			{
				char* token;
				token = strtok(con_line, " ");
				token = strtok(NULL, " ");
				token = strtok(NULL, "\n");
				int q;
				for(q=0; q <7; q++)
				{
					if(strcmp(Game_Rooms[q].room_name, token) == 0)	
						Game_Rooms[f].connections[k-1] = &Game_Rooms[q];
				}
			}
			k++;
		}
		fclose(connect_file_pointer);
	}
}

// This is the function that writes to currentTime.txt
void* print_time(void* arg)
{
	pthread_mutex_lock(&myMutex);
	FILE* time_file = fopen("currentTime.txt", "w");

	char buffer[252];
	struct tm *info;
	time_t timestamp;
	time( &timestamp );

	info = localtime(&timestamp);
	strftime(buffer, 252, "%I:%M%p, %A, %B %d, %Y\n", info);
	fprintf(time_file, buffer);
	fclose(time_file);
	pthread_mutex_unlock(&myMutex);
	pthread_exit(NULL);
}	

//Goes through the game prompts, opens THREAD if time is called and prints time
int Game_Prompt(struct Room *cr, int* steps, char** path)
{
	(*steps) = 0;
	bool game_finished = false;
	while(!game_finished)
	{
		char response[252];	
		bool response_bool = false;	
		printf("\n");	 		
		printf("CURRENT LOCATION: %s:\n", cr->room_name);
		printf("POSSIBLE CONNECTIONS: ");
		int i;
		for(i=0; i < cr->num_of_connections; i++)
		{
			printf("%s", cr->connections[i]->room_name);
			if(i != cr->num_of_connections - 1)
				printf(", ");
			else
				printf(".\n");
		}
		printf("WHERE TO? >");
		fgets(response, 252, stdin);

		char* token;
		token = strtok(response, "\n");
		strcpy(response, token);	
		printf("\n");

		int j;
		for(j=0; j < cr->num_of_connections; j++)
		{
			if((strcmp(cr->connections[j]->room_name, response) == 0))
			{
				cr = cr->connections[j];
				path[(*steps)] =  cr->room_name;
				(*steps)++;
				response_bool=true;
			}
		}

		if((strcmp(response, "time") == 0))
		{
			pthread_t time_thread;
			if(pthread_create(&time_thread, NULL, print_time, NULL))
			{
				printf("error");
			}
			pthread_join(time_thread,NULL);
			char buffer [252];
			FILE* time_file_pointer = fopen("currentTime.txt","r");
			fgets(buffer, 252, time_file_pointer);
			printf("%s", buffer);
			response_bool = true;
		}

		if(!response_bool)
		{
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		} 

		if(strcmp(cr->type,"END_ROOM") == 0)
		{
			game_finished = true;
		}

 	}
	return 0;
}
	  	
int Play_Game(){

	bool game_finished = false;
	int* steps;
	int s = 0;
	steps = &s;
	char* path[100];
	struct Room*  current_room = &Game_Rooms[0];
	Game_Prompt(current_room, steps, path);
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");	
	printf("YOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS: \n", (*steps));
	int i;
	for(i=0; i < (*steps); i++)
	{
		printf("%s \n", path[i]);
	}

	return 0;
}

int main(void)
{
	
	srand(time(NULL));
	Create_Rooms();
	Create_Directory();
	Read_In();
	chdir("..");
	Play_Game();
	
	pthread_exit(NULL);	
	return 0;
}
