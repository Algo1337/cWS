#include "src/init.h"

int main()
{
	int len = 2;
	char *arr[][2] = {
		{"DICK", "BALLS"},
		{"BALLS", "DICK"},
		{NULL, NULL}
	};

	map_t m = create_map(len, arr);
	for(int i = 0; m[i] != NULL; i++)
	{
		printf("%s -> %s\n", m[i]->key, m[i]->value);
	}

}
