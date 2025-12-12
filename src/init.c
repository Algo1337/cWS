#include "init.h"

cws_t init_ws(char *ip, int port)
{
	if(!port)
		return NULL;

	cws_t ws = (cws_t)malloc(sizeof(_cws));

	if((ws->socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
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

int add_route(cws_t web, char *name, void *handler, int req_info, int headers)
{
	if(!web || !handler)
		return 0;

	_route *route = (_route *)malloc(sizeof(_route));
	route->name = strdup(name);
	route->handler = handler;
	route->parse_status_code = req_info;
	route->parse_headers = headers;

	web->routes[web->route_count++] = route;
	if(!(web->routes = (_route **)realloc(web->routes, sizeof(_route *) * (web->route_count + 1))))
		printf("[ x ] Error, unable to reallocate...!\n");

	web->routes[web->route_count] = NULL;
	return 1;
}

char *sock_get_client_ip(int sock, struct sockaddr_in *client) {
    int sz = sizeof(*client);
    if(getpeername(sock, (struct sockaddr *)client, &sz) != 0)
        return NULL;

    char ip[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &(client->sin_addr), ip, INET_ADDRSTRLEN);

	char *n = strdup(ip);
	if(!n)
		return NULL;

	return n;
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

		if(strstr(buffer, "GET") || strstr(buffer, "POST"))
		{
			int arg_c;
			char **lines = __split(buffer, "\n", &arg_c);
			if(arg_c == 0)
			{
				close(sock);
				if(lines) free_arr((void *)lines);
				continue;
			}
			printf("%s\n", buffer);
			char **args = __split(buffer, " ", &arg_c);
			int pos = find_route(web, args[1]);
			if(pos == -1)
			{
				close(sock);
				free_arr((void *)args);
				free_arr((void *)lines);
				continue;
			}

			int status = web->routes[pos]->handler(sock);
			if(!status)
			{
				close(sock);
				free_arr((void *)args);
				free_arr((void *)lines);
				continue;
			}
		}
		buffer[bytes] = '\0';
		close(sock);
	}
}

