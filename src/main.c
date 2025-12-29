#include <security/pam_modules.h>
#include <security/pam_misc.h>
#include <ctype.h>

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
	//====== request totp ======
	message.msg_style = PAM_PROMPT_ECHO_ON;
	message.msg = "6-digit otp:";
	const struct pam_message *message_ptr = &message;
	pam_result = (*conv->conv)(1,&message_ptr,&response,conv->appdata_ptr);
	if (pam_result != PAM_SUCCESS) return pam_result;
	//====== verify totp ======
	int totp_digits[6] = {0};
	for (size_t i = 0; (i < 6) && (i < strlen(response->resp)); i++){
		//copy the digits from the response into an array
		if (isdigit(response->resp[i])){
			//convert from char to int
			totp_digits[i] = response->resp[i] - '0';
		}
	}
	free(response->resp);
	free(response);
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *handle, int flags, int argc, const char **argv){
	return PAM_SUCCESS;
}
