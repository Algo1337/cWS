#include <stdio.h>

typedef enum
{
	_MNULL = 0,
	_MINT = 1,
	_MFLOAT = 2,
	_MCHAR = 3,
	_MSTRING = 4,
	_MINT_ARRAY = 5,
	_MSTRING_ARRAY = 6
} map_value_types;

void *map[][3] = {
	{"NAME", {
		"VALUE"
	}, _MSTRING},
	{"KEY", "VALUE", _MSTRING},
	NULL
};

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