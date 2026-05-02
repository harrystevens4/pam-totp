CFLAGS=-g -Wall -Wextra
LIBS=
OUTPUT_NAME := "pam_totp.so"

auto:
	#use either native linux encryption or libcrypto from openssl
	if pkg-config --exists libcrypto && [ "$(USE_LINUX_CRYPTO)" != "1" ]; then\
		make openssl_pam_totp.so;\
	else\
		make linux_pam_totp.so;\
	fi

openssl_pam_totp.so : src/main.o src/database.o src/totp.o src/util.o src/crypto_openssl.o
	$(CC) -o $(OUTPUT_NAME) -shared $^ $(LIBS) -lcrypto

linux_pam_totp.so : src/main.o src/database.o src/totp.o src/util.o src/crypto_linux.o
	$(CC) -o $(OUTPUT_NAME) -shared $^ $(LIBS)

test : src/test.o src/database.o src/totp.o src/util.o src/crypto_linux.o
	$(CC) -o $@ $^ $(LIBS) -fsanitize=address
