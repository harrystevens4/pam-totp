#ifndef _DATABSE_H
#define _DATABSE_H

typedef struct {
	char *name;
	char *key;
} database_record_t;

typedef struct {
	databse_record_t *records;
	size_t record_count;
} database_t;

#endif
