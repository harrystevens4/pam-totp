#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

int load_database(database_t *db_return, const char *path){
	memset(db_return,0,sizeof(database_t));
	//====== open and read database ======
	int dbfd = open(path,O_RDONLY);
	//get filesize
	struct statbuf stats;
	if (fstat(dbfd,&stats) < 0){
		close(dbfd);
		return -errno;
	}
	size_t filesize = stats.st_size;
	//read
	char *database_content = malloc(filesize+1);
	if (read(dbfd,database_content,filesize) < 0){
		free(database_content);
		close(dbfd);
		return -errno;
	}
	//make sure it ends with a null
	database_content[filesize] = '\0';
	//cleanup
	close(dbfd);
	//====== parse database ======
	//process as individual lines
	char *line_start = database_content;
	for (;;){
		line_end = strchr(line_start,'\n');
		*line_end = '\0';
		size_t line_length = line_end - line_start;
		//read the user and key
		char *seperator = strchr(line_start,":");
		if (seperator == NULL) goto next_line; //no seperator found (skip line)
		char *user = malloc(seperator-line_start);
		memcpy(user,line_start,seperator-line_start);
		char *key = malloc(line_end-seperator);
		memcpy(key,seperator+1,line_end-seperator-1);
		//add it to the database
		db->record_count++;
		db->records = realloc(db->records,db->record_count*sizeof(database_record_t));
		db->records[db->record_count-1].name = name;
		db->records[db->record_count-1].key = key;
		//move onto next line
		next_line:
		line_start = line_end;
		if (line_start != NULL) break;
		line_start++;
	}
	//cleanup
	free(database_content);
	return 0;
}
void free_database(database_t *db){
	//foreach record
	for (size_t i = 0; i < db->record_count; i++){
		free(db->records[i].name);
		free(db->records[i].key);
		free(db->records[i]);
	}
}
