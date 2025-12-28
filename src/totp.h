#ifndef _TOTP_H
#define _TOTP_H

#include <time.h> 

int sha1(const char *message, size_t message_len, unsigned char *message_digest);
int hmac_sha1(const char *key, size_t key_len, const char *message, size_t message_len, unsigned char *digest);
int generate_totp(const char *key, size_t key_len, time_t time, int *digits, size_t digit_count);

#endif
