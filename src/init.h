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

/* cWebRequest */
typedef struct
{

} _cwr;

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
cws_t init_ws(char *ip, int port);
int find_route(cws_t web, char *name);
int add_route(cws_t web, char *name, void *handler);
void run_server(cws_t web, int buff_len);

// parser.c
map_t create_map(int len, char *buff[restrict len][2]);
map_t parse_headers(char *data);

// utils.c
void *to_heap(void *p, int size);
void free_arr(void **arr);
int trim_idx(char *buffer, int index);
char **__split(char *buffer, char *delim, int *idx);

#endif