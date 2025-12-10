#include "init.h"

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

        arr[*idx] = malloc(len + 1);
        memcpy(arr[*idx], token, len + 1);

        (*idx)++;

        if(!(arr = realloc(arr, sizeof(char *) * (*idx + 1)))) {
            free(copy);
            free_arr((void *)arr);
            return NULL;
        }

    }

    free(copy);
    arr[*idx] = NULL;
    return arr;
}