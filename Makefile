CFLAGS=-g -Wall -Wextra
LIBS=

test : src/test.o src/database.o
	$(CC) -o $@ $^ $(LIBS)

pam_totp.so : src/main.o src/database.o
	$(CC) -o $@ -shared $^ $(LIBS)

