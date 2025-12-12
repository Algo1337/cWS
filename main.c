#include "src/init.h"

typedef struct
{
	char **ips;
	int idx;
} _prot;

typedef _prot *prot_t;

prot_t protection = NULL;

int is_ip_blocked(prot_t protection, char *ip)
{
	if(!protection)
		return -1;

	for(int i = 0; i < protection->idx; i++)
	{
		if(!strcmp(protection->ips[i], ip))
			return i;
	}

	return -1;
}

const char *PRE_SET_HEADER = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\nContent-Length: %d\r\n";

int send_response(int sock, int status_code, int header_len, map_t headers, char *body)
{
	char data[1024 * 3];

	int body_len = strlen(body);
	sprintf(data, PRE_SET_HEADER, body_len);

	if(headers)
	{
		for(int i = 0; i < header_len; i++)
		{
			_key *field = headers[i];

			strcat(data, field->key);
			strcat(data, ": ");
			strcat(data, field->value);
			strcat(data, "\r\n");
		}
	}

	strcat(data, "\r\n");
	strcat(data, body);
	strcat(data, "\r\n\r\n");
	
	int len = strlen(data);
	write(sock, data, len);

	return 1;
}

handler_t middle_ware(int sock)
{
	char *ip = sock_get_client_ip(sock);
	if(!is_ip_blocked(protection, ip)) {
		printf("[ REQ_ATTEMP] IP Is Blocked: %s\n", ip);
		return 0;
	}

	return 1;
}

handler_t handler(int sock, char *buffer)
{
	char *n[][2] = {
		{"TEST", "NIG"},
		{"NIG", "TEST"},
		NULL
	};

	map_t headers = create_map(1, n);
	if(!headers)
	{
		printf("Error\n");
		return 1;
	}

	/* Parse Headers */
	//map_t headers = parse_headers(buffer);
	char *file_content = read_file_content("frontend/index.html");
	int length = strlen(file_content);
	send_response(sock, 200, 1, headers, file_content);

	free(file_content);
	return 1;
}

int main()
{
	protection = malloc(sizeof(_prot));
	protection->ips = malloc(sizeof(char *));
	protection->idx = 0;
	cws_t web = init_ws("192.168.1.88", 80);
	if(!web)
	{
		printf("[ x ] Error, Unable to create socket...!\n");
		return 1;
	}

	add_route(web, "/", handler, 1, 1);
	run_server(web, 1024);
	return 0;
}
