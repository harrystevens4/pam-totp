CFLAGS=-g -Wall -Wextra
LIBS=-lcrypto

test : src/test.o src/database.o src/totp.o
	$(CC) -o $@ $^ $(LIBS)

pam_totp.so : src/main.o src/database.o src/totp.o
	$(CC) -o $@ -shared $^ $(LIBS)

