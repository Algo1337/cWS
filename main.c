#include "src/init.h"

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
	cws_t web = init_ws(NULL, 80);
	if(!web)
	{
		printf("[ x ] Error, Unable to create socket...!\n");
		return 1;
	}

	add_route(web, "/", handler);
	run_server(web, 1024);
	return 0;
}
