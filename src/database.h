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

int load_database(database_t *db, const char *path);
void free_database(database_t *db);
database_record_t *get_record_for_user(database_t *db, const char *user);

#endif
