#include <security/pam_modules.h>
#include <security/pam_misc.h>
#include <ctype.h>
#include "database.h"
#include "totp.h"

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *handle, int flags, int argc, const char **argv){
	//====== get user ======
	const char *user;
	int pam_result = pam_get_user(handle, &user, NULL);
	if (pam_result != PAM_SUCCESS) return pam_result;
	//====== get conversation function ======
	//conversation
	struct pam_conv *conv;
	struct pam_message message;
	struct pam_response *response = NULL;
	pam_result = pam_get_item(handle,PAM_CONV,(const void **)&conv);
	if (pam_result != PAM_SUCCESS) return pam_result;
	//====== load db ======
	database_t totp_key_db;
	if (load_database(&totp_key_db,"/etc/totp_keys") < 0){
		perror("pam_totp: load_database");
		return PAM_AUTHINFO_UNAVAIL;
	}
	//====== fetch user's record ======
	database_record_t *user_record = get_record_for_user(&totp_key_db,user);
	if (user_record == NULL){
		dprintf(STDERR_FILENO,"pam_totp: key not found for \"%s\"\n",user);
		free_database(&totp_key_db);
		return PAM_USER_UNKNOWN;
	}
	//key is base32 encoded
	char *decoded_key;
	size_t decoded_key_len = base32decode(user_record->key,&decoded_key);
	//====== request totp ======
	message.msg_style = PAM_PROMPT_ECHO_ON;
	message.msg = "6-digit otp:";
	const struct pam_message *message_ptr = &message;
	pam_result = (*conv->conv)(1,&message_ptr,&response,conv->appdata_ptr);
	if (pam_result != PAM_SUCCESS) return pam_result;
	int totp_digits[6] = {0};
	for (size_t i = 0; (i < 6) && (i < strlen(response->resp)); i++){
		//copy the digits from the response into an array
		if (isdigit(response->resp[i])){
			//convert from char to int
			totp_digits[i] = response->resp[i] - '0';
		}
	}
	//====== calculate totp ======
	unsigned char digest[20] = {0};
	//sha1 has a digest length of 20
	sha1(user_record->key,strlen(user_record->key),digest);
	//calculate totp
	int calculated_digits[6] = {0};
	generate_totp(decoded_key,decoded_key_len,time(NULL),calculated_digits,6);
	//====== cleanup ======
	free_database(&totp_key_db);
	free(decoded_key);
	free(response->resp);
	free(response);
	//====== verify totp ======
	for (int i = 0; i < 6; i++){
		//does each digit given match the calculated digit?
		if (calculated_digits[i] != totp_digits[i]) return PAM_AUTH_ERR;
	}
	return PAM_SUCCESS;
}

//====== this is an authentication module so these dont do anything ======

PAM_EXTERN int pam_sm_setcred(pam_handle_t *handle, int flags, int argc, const char **argv){
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *handle, int flags, int argc, const char **argv){
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *handle, int flags, int argc, const char **argv){
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *handle, int flags, int argc, const char **argv){
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *handle, int flags, int argc, const char **argv){
	return PAM_SUCCESS;
}
