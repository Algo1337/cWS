#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> //
#include <pthread.h>
#include <execinfo.h> //
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define U8 __uint8_t

typedef int (handler_t);

typedef enum
{
	__M_NULL 	= 0,
	__M_INT 	= 1,
	__M_CHAR 	= 2,
	__M_FLOAT 	= 3,
	__M_STRING 	= 4
} map_types;

typedef struct
{
	char 		*key;
	char 		*value;
	map_types 	type;
} _key;

typedef _key **map_t;

typedef struct
{
	char 		*name;
	handler_t	(*handler)();
} _route;

typedef struct
{
	char 		*ip;
	int 		port;
	int 		socket;
	struct 		sockaddr_in addr;

	/* Routes */
	_route 		**routes;
	int 		route_count;

	/* Middleware */
	handler_t	(*middleware)();
} _cws;

typedef _cws *cws_t;

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

cws_t init_ws(char *ip, int port)
{
	if(!port)
		return NULL;

	cws_t ws = (cws_t)malloc(sizeof(_cws));

	if((ws->socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		printf("%d\n", ws->socket);
		printf("[ x ] Error, Unable to create socket...!\n");
		return NULL;
	}

	ws->addr.sin_family = AF_INET;
	ws->addr.sin_port = htons(port);

    if(!ip) {
        if(!inet_aton("127.0.0.1", &ws->addr.sin_addr))
            printf("[ - ] Error, Unable to set web-server IP...!\n");
    } else {
        if(!inet_aton(ip, &ws->addr.sin_addr))
            printf("[ - ] Error, Unable to set web-server IP...!\n");
    }

    int reuse = 1;
    if(setsockopt(ws->socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0) {
    	printf("Error enabling REUSEADDR\n");
        return NULL;
    }

    if (setsockopt(ws->socket, SOL_SOCKET, SO_REUSEPORT, (const char *)&reuse, sizeof(reuse)) < 0) {
    	printf("Error enabling REUSEPORT\n");
        return NULL;
    }

    if(bind(ws->socket, (struct sockaddr *)&ws->addr, sizeof(ws->addr)) < 0)
    {
    	printf("Error Binding\n");
        return NULL;
    }

    ws->routes = (_route **)malloc(sizeof(_route));
    ws->route_count = 0;

    return ws;
}

int find_route(cws_t web, char *name)
{
	if(!web || !name)
		return -1;

	for(int i = 0; i < web->route_count; i++)
	{
		if(!strcmp(web->routes[i]->name, name))
			return i;
	}

	return -1;
}

int add_route(cws_t web, char *name, void *handler)
{
	if(!web || !handler)
		return 0;

	_route *route = (_route *)malloc(sizeof(_route));
	route->name = strdup(name);
	route->handler = handler;

	web->routes[web->route_count++] = route;
	if(!(web->routes = (_route **)realloc(web->routes, sizeof(_route *) * (web->route_count + 1))))
		printf("[ x ] Error, unable to reallocate...!\n");

	web->routes[web->route_count] = NULL;
	return 1;
}

void run_server(cws_t web, int buff_len)
{
	if(listen(web->socket, 20) < 0)
        return;

	struct sockaddr_in client;
	int sz = sizeof(client);
	while(1)
	{
		printf("Listening for new connection...!\n");
		int sock = accept(web->socket, (struct sockaddr *)&client, &sz);
		if(sock <= 0)
			continue;

		char buffer[buff_len];
		int bytes = read(sock, buffer, buff_len);
		if(bytes == 0)
		{
			close(sock);
			continue;
		}
		buffer[bytes] = '\0';

		if(strstr(buffer, "GET") || strstr(buffer, "POST"))
		{
			int arg_c;
			char **args = __split(buffer, " ", &arg_c);
			if(arg_c > 0)
			{
				int pos = find_route(web, args[1]);
				if(pos == -1) { close(sock); continue; }
				int p = (int)web->routes[pos]->handler(sock, buffer);
				if(!p) { close(sock); continue; }
			}

		}

		close(sock);
	}
}

_key **parse_headers(char *data)
{
	if(!data)
		return NULL;

	_key **map = (_key **)malloc(sizeof(_key));
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

handler_t handler(int sock, char *buffer)
{
	map_t headers = parse_headers(buffer);
	for(int i = 0; headers[i] != NULL; i++)
	{
		printf("[ HEADER_%d ]: %s -> %s\n", i, headers[i]->key, headers[i]->value);
	}
	char resp[] = "<html>BEEP BOOP @algo1337</html>\r\n";
	int len = strlen(resp);

	char new_headers[1024];
	sprintf(new_headers, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\nContent-Length: %d\r\n\r\n", len);

	write(sock, new_headers, strlen(new_headers));
	write(sock, resp, len);

	return 1;
}

int main()
{
	cws_t web = init_ws("0.0.0.0", 80);
	if(!web)
	{
		printf("[ x ] Error, Unable to create socket...!\n");
		return 1;
	}
	add_route(web, "/", handler);
	run_server(web, 1024);
	return 0;
}
