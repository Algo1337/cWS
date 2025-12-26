#include "src/init.h"

const char *PRE_SET_HEADER = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\nContent-Length: %d\r\n";

int send_response(int sock, int status_code, map_t headers, char *body)
{
	int body_len = strlen(body);
	char data[strlen(PRE_SET_HEADER) + body_len * 2];

	sprintf(data, PRE_SET_HEADER, body_len);

	if(headers)
	{
		for(int i = 0; headers[i] != NULL; i++)
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

int send_web_file(int sock, int status_code, map_t headers, char *web_file)
{
    int length = 0;
    char *file_content = read_file_content(web_file, &length);

    int ret = send_response(sock, status_code, headers, file_content);
	free(file_content);

	return ret;
}

handler_t middle_ware(int sock)
{
	return 1;
}

handler_t mobile_handler(int sock, cwr_t req)
{
	int length = 0;
	char *file_content = read_file_content("frontend/mobile.html", &length);

	send_response(sock, 200, NULL, file_content);
	free(file_content);

	return 1;
}

handler_t index_handler(int sock, cwr_t req)
{
	char *n[][2] = {
		{"TEST", "NIG"},
		{"NIG", "TEST"},
		NULL
	};
	printf("%s\n", req->content);

//	map_t headers = create_map(2, n);
//	if(!headers)
//	{
//		printf("Error\n");
//		return 1;
//	}

	char *ip = sock_get_client_ip(sock);
	printf("USER_IP: %s\n", ip);

	/* Parse Headers */

	map_t headers = parse_headers(req);
	for(int i = 0; headers[i] != NULL; i++)
	{
		printf("%s -> %s\n", headers[i]->key, headers[i]->value);
	}

	send_web_file(sock, 200, NULL, "frontend/index.html");
	return 1;
}

int main()
{
	cws_t web = init_ws("192.168.1.88", 80);
	if(!web)
	{
		printf("[ x ] Error, Unable to create socket...!\n");
		return 1;
	}

	add_route(web, "/", index_handler, 1, 1);
	add_route(web, "/mobile", mobile_handler, 1, 1);
	run_server(web, 1024);
	return 0;
}
