#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>

int sha1(const char *message, size_t message_len, unsigned char *message_digest){
	//====== initialise ======
	//message digest context
	EVP_MD_CTX *context;
	context = EVP_MD_CTX_new();
	if (context == NULL) return -1;
	//initialise our context for sha1
	if (EVP_DigestInit_ex(context,EVP_sha1(),NULL) != 1) return -1;
	//====== feed it our message ======
	if (EVP_DigestUpdate(context,message,message_len) != 1) return -1;
	//====== copy the result ======
	unsigned int digest_size = EVP_MD_size(EVP_sha256());
	if (EVP_DigestFinal_ex(context,message_digest,&digest_size) != 1) return -1;
	//====== cleanup ======
	EVP_MD_CTX_free(context);
	return 0;
}
