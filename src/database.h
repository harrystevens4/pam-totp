#ifndef _DATABSE_H
#define _DATABSE_H

#include <stddef.h>

typedef struct {
	char *user;
	char *key;
} database_record_t;

typedef struct {
	database_record_t *records;
	size_t record_count;
} database_t;

#endif
