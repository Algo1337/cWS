#include "src/init.h"

const char *PRE_SET_HEADER = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\nContent-Length: %d\r\n";

int send_response(int sock, map_t headers, char *body)
{
	int len = strlen(PRE_SET_HEADER);
	char *data = (char *)malloc(len + 1);

	int body_len = strlen(body);
	sprintf(data, PRE_SET_HEADER, body_len);

	if(headers)
	{
		for(int i = 0; headers[i] != NULL; i++)
		{
			_key *field = headers[i];
			len += strlen(field->key) + strlen(field->value) + 3;
			data = (char *)realloc(data, len + 1);
			if(!data)
			{
				printf("Realloc err\n");
				break;
			}

			strcat(data, field->key);
			strcat(data, ": ");
			strcat(data, field->value);
			strcat(data, "\r\n");
		}
	}

	strcat(data, "\r\n");
	strcat(data, body);
	strcat(data, "\r\n\r\n");
	len += body_len + 6;
	write(sock, data, len);

	free(data);
	return 1;
}

handler_t handler(int sock, char *buffer)
{
	char *n[][2] = {
		{"TEST", "NIG"},
		NULL
	};

	map_t headers = create_map(1, n);
	printf("%s\n", headers[0]->key);
	//map_t headers = parse_headers(buffer);
	char *file_content = read_file_content("frontend/index.html");
	int length = strlen(file_content);
	send_response(sock, headers, file_content);

	free(file_content);
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

	add_route(web, "/", handler, 1, 1);
	run_server(web, 1024);
	return 0;
}
