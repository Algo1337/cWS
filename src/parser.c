#include "init.h"  

map_t create_map(int len, char *buff[restrict len][2])
{
    if(len <= 0 || !buff)
        return NULL;

	map_t map = (map_t)malloc(sizeof(_key *));
    if(!map)
    {
        printf("Malloc err\n");
        return NULL;
    }

	int idx = 0;
	for(int i = 0; i < len; i++)
	{
		_key *field = (_key *)malloc(sizeof(_key));
        if(!field)
        {
            printf("Malloc err\n");
            break;
        }
		field->key = strdup(buff[i][0]);
		field->value = strdup(buff[i][1]);

		map[idx++] = field;
		map = (map_t)realloc(map, sizeof(_key *) * (idx + 1));
		if(!map)
		{
			printf("Realloc err\n");
			break;
		}

		map[idx] = NULL;
	}

    map[idx] = NULL;
	if(idx == 0)
	{
		free(map);
		return NULL;
	}

	return map;
}

/* Parse Top Line Of Content */
char **parse_req_info(cwr_t req)
{
    if(!req)
        return NULL;

    int line_count = 0;
    char **args = __split(req->lines[0], " ", &line_count);
    
    return args;
}

/* Parse Headers Only */
map_t parse_headers(cwr_t req)
{
	if(!req->content)
		return NULL;

	map_t map = (map_t)malloc(sizeof(_key));
	int idx = 0, line_count;
	char **lines = __split(req->content, "\n", &line_count);

	for(int i = 1; i < line_count; i++)
	{
		if(!lines[i])
			break;

		if(lines[i][0] == ' ') break;
		if(!strstr(lines[i], ":")) break;

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

		map[idx] = NULL;
	}

	map[idx] = NULL;
	if(idx == 0)
	{
		free(map);
		return NULL;
	}

	return map;
}

int parse_post_request(cwr_t req)
{
	if(!req)
		return 0;

	int arg_c;
	char *post_data = req->lines[req->body_line];
	int len = strlen(post_data);
	if(len == 0)
		return 0;

	if(strstr(post_data, "&"))
	{
		char **params = __split(post_data, "&", &arg_c);
		if(arg_c == 0)
		{
			printf("Error parsing POST data...!\n");
			return 0;
		}

		// Iterate and store params
		map_t map = (map_t)malloc(sizeof(_key));
		int idx = 0;
		for(int i = 0; i < arg_c; i++) {
			int argc;
			char **args = __split(params[i], "=", &argc);
			
			map[idx] = (_key *)malloc(sizeof(_key *));
			map[idx]->key = strdup(args[0]);
			map[idx]->value = strdup(args[1]);
			map[idx++]->type = __M_STRING;

			if(!(map = realloc(map, sizeof(_key *) * (idx + 1))))
				printf("Realloc err\n");

			map[idx] = NULL;
			free_arr((void *)args);
		}

		req->post = map;
		free_arr((void *)params);
		return 1;
	}

	if(!strstr(post_data, "="))
		return 0;

	// Only one param
	char **args = __split(post_data, "=", &arg_c);
	
	map_t map = (map_t)malloc(sizeof(_key));
	map[0] = (_key *)malloc(sizeof(_key *));
	map[0]->key = strdup(args[0]);
	map[0]->value = strdup(args[1]);
	map[0]->type = __M_STRING;

	req->post = map;
	free_arr((void *)args);
	return 1;
}