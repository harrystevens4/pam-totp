#include "database.h"
#include "totp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

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
	//====== calculate totp ======
	unsigned char digest[20] = {0};
	//sha1 has a digest length of 20
	sha1(user_record->key,strlen(user_record->key),digest);
	printf("key hash: ");
	for (int i = 0; i < 20; i++) printf("%02x",digest[i]);
	printf("\n");
	//calculate hmac
	hmac_sha1(user_record->key,strlen(user_record->key),"hello",5,digest);
	printf("key hmac: ");
	for (int i = 0; i < 20; i++) printf("%02x",digest[i]);
	printf("\n");
	//calculate totp
	int digits[6] = {0};
	generate_totp(user_record->key,strlen(user_record->key),time(NULL),digits,6);
	printf("totp: %d%d%d %d%d%d\n",digits[0],digits[1],digits[2],digits[3],digits[4],digits[5]);
	//====== free db ======
	end:
	free_database(&totp_key_db);
	return 0;
}
