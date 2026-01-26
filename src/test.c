#include "database.h"
#include "totp.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

int main(int argc, char **argv){
	if (argc < 2){
		fprintf(stderr,"Please provide the name of a user.\n");
		return 1;
	}
	//====== check utility functions ======
	char *test_string = strdup(" abcd   e fg    27?? 89 ");
	str_remove_predicate(test_string,isspace);
	str_remove_predicate(test_string,isalpha);
	str_remove_predicate(test_string,ispunct);
	printf("[%s]\n",test_string);
	free(test_string);
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
	//key is base32 encoded
	char *decoded_key;
	size_t decoded_key_len = base32decode(user_record->key,&decoded_key);
	printf("user: %s\n",user_record->user);
	printf("key: %.*s\n",(int)decoded_key_len,decoded_key);
	//====== calculate totp ======
	unsigned char digest[20] = {0};
	//sha1 has a digest length of 20
	sha1(user_record->key,strlen(user_record->key),digest);
	printf("key hash: ");
	for (int i = 0; i < 20; i++) printf("%02x",digest[i]);
	printf("\n");
	//calculate totp
	int digits[6] = {0};
	generate_totp(decoded_key,decoded_key_len,time(NULL),digits,6);
	printf("totp: %d%d%d %d%d%d\n",digits[0],digits[1],digits[2],digits[3],digits[4],digits[5]);
	//====== cleanup ======
	end:
	free_database(&totp_key_db);
	free(decoded_key);
	return 0;
}
