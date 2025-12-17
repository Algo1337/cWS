#include "init.h"

void *StatusCodeDef[][2] = {
    {(void *)CONTINUEE,                         "Continue" },
    {(void *)SWITCH_PROTOCOL,                   "Switching Protocols" },
    {(void *)PROCESSING,                        "Processing" },
    {(void *)EARLY_HINT,                        "Early Hints" },
    {(void *)OK,                                "OK" },
    {(void *)CREATED,                           "Created" },
    {(void *)ACCEPTED,                          "Accepted" },
    {(void *)NON_AUTHORIZED_INFO,               "Non-Authoritative Information" },
    {(void *)NO_CONTENT,                        "No Content" },
    {(void *)RESET_CONTENT,                     "Reset Content" },
    {(void *)PARTIAL_CONTENT,                   "Partial Content" },
    {(void *)MULTI_STATUS,                      "Multi-Status" },
    {(void *)ALREADY_REPRORTED,                 "Already Reported" },
    {(void *)IM_USED,                           "IM Used" },
    {(void *)MULTIPLE_CHOICES,                  "Multiple Choices" },
    {(void *)MOVED_PERMANENTLY,                 "Moved Permanently" },
    {(void *)FOUND,                             "Found" },
    {(void *)SEE_OTHER,                         "See Other" },
    {(void *)NOT_MODIFIED,                      "Not Modified" },
    {(void *)USE_PROXY,                         "Use Proxy" },
    {(void *)SWITCH_PROXY,                      "Switch Proxy" },
    {(void *)TEMP_REDIRECT,                     "Temporary Redirect" },
    {(void *)PERM_REDIRECT,                     "Permanent Redirect" },
    {(void *)BAD_REQUEST,                       "Bad Request" },
    {(void *)UNAUTHORIZED,                      "Unauthorized" },
    {(void *)PAYMENT_REQUIRED,                  "Payment Required" },
    {(void *)FORBIDDEN,                         "Forbidden" },
    {(void *)NOT_FOUND,                         "Not Found" },
    {(void *)METHOD_NOT_ALLOWED,                "Method Not Allowed" },
    {(void *)NOT_ACCEPTABLE,                    "Not Acceptable" },
    {(void *)PROXY_AUTH_REQUIRED,               "Proxy Authentication Required" },
    {(void *)REQUIRE_TIMEOUT,                   "Request Timeout" },
    {(void *)CONFLICT,                          "Conflict" },
    {(void *)GONE,                              "Gone" },
    {(void *)LENGTH_REQUIRED,                   "Length Required" },
    {(void *)PRECONDITION_FAILED,               "Precondition Failed" },
    {(void *)PAYLOAD_TOO_LARGE,                 "Content Too Large" },
    {(void *)URI_TOO_LONG,                      "URI Too Long" },
    {(void *)UNSUPPORTED_MEDIA_TYPE,            "Unsupported Media Type" },
    {(void *)RANGE_NOT_SATISFIABLE,             "Range Not Satisfiable" },
    {(void *)EXPECTATION_FAILED,                "Expectation Failed" },
    {(void *)IM_A_TEAPOT,                       "I'm a teapot" },
    {(void *)MISDIRECTED_REQUEST,               "Misdirected Request" },
    {(void *)UNPROCESSABLE_ENTITY,              "Unprocessable Content" },
    {(void *)LOCKED,                            "Locked" },
    {(void *)FAILED_DEPENDENCY,                 "Failed Dependency" },
    {(void *)TOO_EARLY,                         "Too Early" },
    {(void *)UPGRADE_REQUIRED,                  "Upgrade Required" },
    {(void *)PROCONDITION_REQUIRED,             "Precondition Required" },
    {(void *)TOO_MANY_REQUEST,                  "Too Many Requests" },
    {(void *)REQ_HEADER_FIELD_TOO_LARGE,        "Request Header Fields Too Large" },
    {(void *)UNAVAILABLE_FOR_LEGAL_REASON,      "Unavailable For Legal Reasons" },

    {(void *)INTERNAL_SERVER_ERR,               "Internal Server Error" },
    {(void *)NOT_IMPLEMENTED,                   "Not Implemented" },
    {(void *)BAD_GATEWAY,                       "Bad Gateway" },
    {(void *)SERVER_UNAVAILABLE,                "Service Unavailable" },
    {(void *)GATEWAY_TIMEOUT,                   "Gateway Timeout" },
    {(void *)HTTP_VERSION_NOT_SUPPORTED,        "HTTP Version Not Supported" },
    {(void *)VARIANT_ALSO_NEGOTIATES,           "Variant Also Negotiates" },
    {(void *)INSUFFICIENT_STORAGE,              "Insufficient Storage" },
    {(void *)LOOP_DETECTED,                     "Loop Detected" },
    {(void *)NOT_EXTENDED,                      "Not Extended" },
    {(void *)NETWORK_AUTHENTICATION_REQUIRED,   "Network Authentication Required" },
    NULL
};

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

char *sock_get_client_ip(int sock) {
    struct sockaddr_in client;
    int sz = sizeof(client);
    if(getpeername(sock, (struct sockaddr *)&client, &sz) != 0)
        return NULL;

    char ip[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);

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

		if(strstr(buffer, "GET") || strstr(buffer, "POST") || strstr(buffer, "HEAD"))
		{
			int arg_c;
			char **lines = __split(buffer, "\n", &arg_c);
			if(arg_c == 0)
			{
				close(sock);
				if(lines) free_arr((void *)lines);
				continue;
			}

			char **args = __split(lines[0], " ", &arg_c);
            if(arg_c == 0)
			{
				close(sock);
				free_arr((void *)lines);
				continue;
			}

            cwr_t req = (cwr_t)malloc(sizeof(_cwr));
            req->req_type = strdup(args[0]);
            req->path = strdup(args[1]);
            req->http_version = strdup(args[2]);
            req->body = strdup(buffer);

			printf("[ HANDLER ]: %s\n", args[1]);
			int pos = find_route(web, args[1]);
			if(pos == -1)
			{
				close(sock);
				free_arr((void *)args);
				free_arr((void *)lines);
				continue;
			}

			int status = web->routes[pos]->handler(sock, req);
			if(!status)
			{
				close(sock);
				free_arr((void *)args);
				free_arr((void *)lines);
				continue;
			}

			free_arr((void *)args);
			free_arr((void *)lines);
		} else {

        }

		buffer[bytes] = '\0';
		close(sock);
	}
}
