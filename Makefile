
pam_totp.so : src/main.o src/database.o
	$(CC) -o $@ -shared $^ $(LIBS)
