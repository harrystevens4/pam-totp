#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){
	if (argc < 2){
		fprintf(stderr,"Please provide the name of a user.\n");
		return 1;
	}
	//====== load db ======
	database_t totp_key_db;
	if (load_database(&totp_key_db,"totp_keys") < 0){
		perror("load_database");
		goto end;
	}
	//====== fetch user's record ======
	database_record_t *user_record = get_record_for_user(&totp_key_db,argv[1]);
	if (user_record == NULL){
		fprintf(stderr,"Record not found.");
		goto end;
	}
	printf("user: %s\n",user_record->user);
	printf("key: %s\n",user_record->key);
	//====== free db ======
	end:
	free_database(&totp_key_db);
	return 0;
}
