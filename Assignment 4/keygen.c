#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(int argc, char* argv[])
{
	time_t t;
	srand(time(&t));
	
	if(argc >= 1)
	{
		char* key;
		int char_num;
		char_num = atoi(argv[1]);
		int i;
		key = malloc(sizeof(sizeof(char) * char_num));
		for(i=0; i<char_num; i++)
		{
			char new_char;
			new_char = 'A' + (rand()%27);
			if(new_char == '[')
				new_char = ' ';
			key[i] = new_char;	
		}
		printf("%s\n", key);
	}	
}
