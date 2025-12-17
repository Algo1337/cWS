#pragma once

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

#ifndef C_WEB_SERVER
#define C_WEB_SERVER

typedef int (handler_t);

typedef enum {
    _NONE = 0,
    _GET = 1,
    _POST = 2,
    _HEAD = 3,
} req_t;

typedef enum {
    CONTINUEE                       = 100,
    SWITCH_PROTOCOL                 = 101,
    PROCESSING                      = 102,
    EARLY_HINT                      = 103,

    OK                              = 200,
    CREATED                         = 201,
    ACCEPTED                        = 202,
    NON_AUTHORIZED_INFO             = 203,
    NO_CONTENT                      = 204,
    RESET_CONTENT                   = 205,
    PARTIAL_CONTENT                 = 206,
    MULTI_STATUS                    = 207,
    ALREADY_REPRORTED               = 208,
    IM_USED                         = 226,

    MULTIPLE_CHOICES                = 300,
    MOVED_PERMANENTLY               = 301,
    FOUND                           = 302,
    SEE_OTHER                       = 303,
    NOT_MODIFIED                    = 304,
    USE_PROXY                       = 305,
    SWITCH_PROXY                    = 306,
    TEMP_REDIRECT                   = 307,
    PERM_REDIRECT                   = 308,

    BAD_REQUEST                     = 400,
    UNAUTHORIZED                    = 401,
    PAYMENT_REQUIRED                = 402,
    FORBIDDEN                       = 403,
    NOT_FOUND                       = 404,
    METHOD_NOT_ALLOWED              = 405,
    NOT_ACCEPTABLE                  = 406,
    PROXY_AUTH_REQUIRED             = 407,
    REQUIRE_TIMEOUT                 = 408,
    CONFLICT                        = 409,
    GONE                            = 410,
    LENGTH_REQUIRED                 = 411,
    PRECONDITION_FAILED             = 412,
    PAYLOAD_TOO_LARGE               = 413,
    URI_TOO_LONG                    = 414,
    UNSUPPORTED_MEDIA_TYPE          = 415,
    RANGE_NOT_SATISFIABLE           = 416,
    EXPECTATION_FAILED              = 417,
    IM_A_TEAPOT                     = 418,
    MISDIRECTED_REQUEST             = 421,
    UNPROCESSABLE_ENTITY            = 422,
    LOCKED                          = 423,
    FAILED_DEPENDENCY               = 424,
    TOO_EARLY                       = 425,
    UPGRADE_REQUIRED                = 426,
    PROCONDITION_REQUIRED           = 428,
    TOO_MANY_REQUEST                = 429,
    REQ_HEADER_FIELD_TOO_LARGE      = 431,
    UNAVAILABLE_FOR_LEGAL_REASON    = 451,
    
    INTERNAL_SERVER_ERR             = 500,
    NOT_IMPLEMENTED                 = 501,
    BAD_GATEWAY                     = 502,
    SERVER_UNAVAILABLE              = 503,
    GATEWAY_TIMEOUT                 = 504,
    HTTP_VERSION_NOT_SUPPORTED      = 505,
    VARIANT_ALSO_NEGOTIATES         = 506,
    INSUFFICIENT_STORAGE            = 507,
    LOOP_DETECTED                   = 508,
    NOT_EXTENDED                    = 510,
    NETWORK_AUTHENTICATION_REQUIRED = 511
} StatusCode;

#define STATUS_CODE_COUNT 63
extern void *StatusCodeDef[][2];

typedef enum
{
	__M_NULL 	        = 0,
	__M_INT 	        = 1,
	__M_CHAR 	        = 2,
	__M_FLOAT 	        = 3,
	__M_STRING 	        = 4,
    __M_INT_ARRAY       = 5,
    __M_STRING_ARRAY    = 6,
    __M_FLOAT_ARRAY     = 7
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


    int         parse_status_code;
    int         parse_headers;
	char		*http_version;
	char		*path;
	int 		status_code;
} _route;

/* cWebRequest */
typedef struct
{
    req_t       req_type;
	int 		status_code;
	int 		path;
	int 		http_version;
	map_t 		headers;
	map_t 		post_data;
    char        *body;
} _cwr;

typedef _cwr *cwr_t;

/* cWebserver */
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

// init.c
cws_t 	init_ws(char *ip, int port);
int 	find_route(cws_t web, char *name);
int 	add_route(cws_t web, char *name, void *handler, int req_info, int headers);
char    *sock_get_client_ip(int sock);
void 	run_server(cws_t web, int buff_len);

// parser.c
map_t 	create_map(int len, char *buff[restrict len][2]);
map_t 	parse_headers(char *data);
char 	**parse_req_info(char *data);

// utils.c
void 	*to_heap(void *p, int size);
void 	free_arr(void **arr);
int 	trim_idx(char *buffer, int index);
char 	**__split(char *buffer, char *delim, int *idx);
char 	*read_file_content(char *filename, int *length);

#endif
