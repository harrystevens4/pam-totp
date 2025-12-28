#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <string.h>
#include <endian.h>
#include <ctype.h>

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

int generate_totp(const char *key, size_t key_len, time_t time, int *digits, size_t digit_count){
	const int DIGITS_POWER[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000};
	const time_t timestep = 30;
	uint64_t timesteps = htobe64(time / timestep);
	//generate hmac message
	char hmac_message[8] = {0};
	size_t hmac_message_len = 8;
	memcpy(hmac_message,&timesteps,8);
	//char hmac_message[1024] = {'0'};
	//char time_as_string[1024];
	//size_t hmac_message_len = snprintf(time_as_string,sizeof(time_as_string),"%lu",timesteps);
	//if (hmac_message_len < 16){
	//	memcpy(hmac_message+(16-hmac_message_len),time_as_string,hmac_message_len);
	//	hmac_message_len = 16;
	//}else{
	//	memcpy(hmac_message,time_as_string,hmac_message_len);
	//}
	//calculate the hmac
	unsigned char digest[20];	
	hmac_sha1(key,key_len,hmac_message,hmac_message_len,digest);
	//extract the otp
	int offset = digest[20-1] & 0xf;
	int binary = 
		((digest[offset] & 0x7f) << 24) |
		((digest[offset + 1] & 0xff) << 16) |
		((digest[offset + 2] & 0xff) << 8) |
		((digest[offset + 3] & 0xff));
	//restrict the otp to only be as long as the number of digits requested
	int otp = binary % DIGITS_POWER[digit_count];
	char digit_buffer[1024];
	snprintf(digit_buffer,1024,"%0*d",(int)digit_count,otp);
	for (size_t i = 0; i < digit_count; i++){
		digits[i] = digit_buffer[i] - '0';
	}
	return 0;
}

void write_bit_to_array(char *array, int bit, size_t index){
	array[index/8] |= bit << (7-(index%8));
}

size_t base32decode(const char *input,char **output){
	const char base32_table[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','2','3','4','5','6','7','\0'};
	size_t bit_count = 0;
	size_t output_size = 0;
	*output = malloc(1);
	//for each group of 8 bytes
	for (size_t i = 0; i < strlen(input); i+=8){
		char output_byte_group[5] = {0};
		char input_byte_group[8] = {0};
		memcpy(input_byte_group,input+i,8);
		for (int j = 0; j < 8; j++){
			char input_byte = tolower(input_byte_group[j]);
			//padding
			if (input_byte == '=') continue;
			//find corresponding index in table
			char *position = strchr(base32_table,input_byte);
			if (position == NULL) continue;
			uint8_t index = position-base32_table;
			for (size_t bit_index = 0; bit_index < 5; bit_index++){
				write_bit_to_array(output_byte_group,(index >> (4-bit_index)) & 1,bit_index+(5*j));
			}
			bit_count += 5;
		}
		output_size += 5;
		*output = realloc(*output,output_size);
		memcpy(*output+output_size-5,output_byte_group,5);
	}
	//ceil division
	output_size = (bit_count/8) + (bit_count % 8 != 0);
	return output_size;
}

