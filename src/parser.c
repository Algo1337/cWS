#include "init.h"

map_t create_map(int len, char *buff[restrict len][2])
{
	map_t map = (map_t)malloc(sizeof(_key *));
	int idx = 0;

	for(int i = 0; i < len; i++)
	{
		_key *field = (_key *)malloc(sizeof(_key));
		field->key = strdup(buff[i][0]);
		field->value = strdup(buff[i][0]);

		map[idx++] = field;
		map = (map_t)realloc(map, sizeof(_key *) * (idx + 1));
		if(!map)
		{
			printf("Realloc err\n");
			break;
		}
	}

	if(idx == 0)
	{
		free(map);
		return NULL;
	}

	return map;
}

map_t parse_headers(char *data)
{
	if(!data)
		return NULL;

	map_t map = (map_t)malloc(sizeof(_key));
	int idx = 0, line_count;
	char **lines = __split(data, "\n", &line_count);

	for(int i = 0; i < line_count; i++)
	{
		if(!lines[i])
			break;

		if(lines[i][0] == ' ') break;

		int arg_c;
		char **args = __split(lines[i], " ", &arg_c);

		int len = strlen(args[0]);
		for(int c = 0; c < len; c++) trim_idx(lines[i], 0);

		_key *field = (_key *)malloc(sizeof(_key));
		field->key = strdup(args[0]);
		field->value = strdup(lines[i]);
		field->type = __M_STRING;

		map[idx++] = field;
		map = (_key **)realloc(map, sizeof(_key *) * (idx + 1));
		if(!map)
			printf("[ x ] Error, Unable to realloc...!\n");
	}

	if(idx == 0)
	{
		free(map);
		return NULL;
	}

	return map;
}