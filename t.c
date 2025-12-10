#include <stdio.h>


void create_map(int len, char *buff[restrict len][2])
{
	for(int i = 0; i < len; i++)
	{
		printf("%s -> %s\n", buff[i][0], buff[i][1]);
	}
}

int main()
{
	int len = 2;
	char *arr[][2] = {
		{"DICK", "BALLS"},
		{"BALLS", "DICK"},
		{NULL, NULL}
	};

	create_map(len, arr);
}