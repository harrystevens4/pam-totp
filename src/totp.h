#ifndef _TOTP_H
#define _TOTP_H

int sha1(const char *message, size_t message_len, unsigned char *message_digest);

#endif
