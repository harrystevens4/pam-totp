#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <string.h>

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

int hmac_sha1(const char *key, size_t key_len, const char *message, size_t message_len, unsigned char *digest){
	const unsigned int block_size = 64;
	const unsigned int hash_size = 20;
	unsigned char block[block_size];
	memset(block,0,sizeof(block));
	unsigned char inner_padding[block_size];
	unsigned char outer_padding[block_size];
	//magic numbers
	memset(inner_padding,0x36,block_size);
	memset(outer_padding,0x5c,block_size);
	//====== generate block sized key ======
	if (key_len > block_size){
		sha1(key,key_len,block);
	}else {
		memcpy(block,key,key_len);
	}
	//====== digestion gaming ======
	size_t key_plus_message_len = message_len+block_size;
	unsigned char *key_plus_message = malloc(key_plus_message_len);
	//xor inner_padding with block sized key
	for (unsigned int i = 0; i < block_size; i++) inner_padding[i] ^= block[i];
	//concatenate block sized key and message
	memcpy(key_plus_message,inner_padding,block_size);
	memcpy(key_plus_message+block_size,message,message_len);
	//hash result
	unsigned char inner_digest[hash_size];
	memset(inner_digest,0,hash_size);
	sha1((char *)key_plus_message,key_plus_message_len,inner_digest);
	free(key_plus_message);
	//xor outer_padding with block sized key
	for (unsigned int i = 0; i < block_size; i++) outer_padding[i] ^= block[i];
	//contatenate with previous hash
	char outer_padding_plus_inner_hash[hash_size + block_size];
	memcpy(outer_padding_plus_inner_hash,outer_padding,block_size);
	memcpy(outer_padding_plus_inner_hash+block_size,inner_digest,hash_size);
	//hash concatenated total
	sha1(outer_padding_plus_inner_hash,hash_size+block_size,digest);
	return 0;
}
