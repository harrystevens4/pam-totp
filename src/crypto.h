#ifndef _CRYPTO_H
#define _CRYPTO_H

//both crypto_openssl.c and crypto_linux.c provide these

int sha1(const char *message, size_t message_len, unsigned char *message_digest);

#endif
