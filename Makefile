CFLAGS=-g -Wall -Wextra
LIBS=-lcrypto

pam_totp.so : src/main.o src/database.o src/totp.o src/util.o
	$(CC) -o $@ -shared $^ $(LIBS)

test : src/test.o src/database.o src/totp.o src/util.o
	$(CC) -o $@ $^ $(LIBS) -fsanitize=address

