#include "init.h"

char *read_file_content(char *filename, int *length)
{
	if(!filename)
		return NULL;

	FILE *stream = fopen(filename, "r");
	if(!stream)
		return NULL;

	fseek(stream, 0, SEEK_END);
	int sz = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	char *data = (char *)malloc(sz + 1);
	if(!data)
	{
		printf("Malloc err\n");
		fclose(stream);
		return NULL;
	}

	int bytes = fread(data, 1, sz, stream);
	data[bytes] = '\0';
	fclose(stream);

	*length = bytes;

	if(bytes == 0)
	{
		free(data);
		return NULL;
	}

	return data;
}

void *to_heap(void *p, int size)
{
	void *ptr = malloc(size);
	memcpy(ptr, p, size);

	return ptr;
}

void free_arr(void **arr)
{
    for(int i = 0; arr[i] != NULL; i++)
        free(arr[i]);

    free(arr);
}

int trim_idx(char *buffer, int index)
{
	if(!buffer)
		return 0;

	int idx = 0, len = strlen(buffer);
	char *data = (char *)malloc(len + 1);
	for(int i = 0; i < len; i++) {
        	if(i != index)
        	data[idx++] = buffer[i];
	}

    data[idx] = '\0';
    memcpy(buffer, data, idx);
        return 1;
}

char **__split(char *buffer, char *delim, int *idx)
{
    if(!buffer || !delim)
        return NULL;

    char **arr = (char **)malloc(sizeof(char *) * 1);
    if(!arr)
    {
        return NULL;
    }
    *idx = 0;

    char *token, *copy = strdup(buffer), *rest = copy;
    while((token = strtok_r(rest, delim, &rest)))
    {
        int len = strlen(token);

        if(!(arr[*idx] = malloc(len + 1)))
            printf("Malloc err\n");
        memcpy(arr[*idx], token, len + 1);

        (*idx)++;

        if(!(arr = realloc(arr, sizeof(char *) * (*idx + 1)))) {
            free(copy);
            free_arr((void *)arr);
            return NULL;
        }

    }

	if(*idx == 0)
	{
		free(copy);
		free(arr);
		return NULL;
	}

    free(copy);
    arr[*idx] = NULL;
    return arr;
}

void struct_Destructor(void *ptr, void *(*destructor)())
{
    if(destructor)
        destructor(ptr);

    free(ptr);
}

void field_Destruct(_key *q) 
{
	if(q->key)
		free(q->key);

	if(q->value)
		free(q->value);

	free(q);
}

void map_Destruct(map_t map)
{
	for(int i = 0; map[i] != NULL; i++)
		field_Destruct(map[i]);

	free(map);
}