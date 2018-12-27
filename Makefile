PATH_UTILITY=./Utility/
PATH_CLIENT=./Client/
PATH_SERVER=./Server/

CC=gcc
CFLAGS=-Wall


all: utility.o $(PATH_SERVER)request.o $(PATH_SERVER)server.o $(PATH_CLIENT)client.o
		$(CC) $(PATH_CLIENT)utility.o $ $(PATH_CLIENT)client.o -o $(PATH_CLIENT)tftp_client
		$(CC) $(PATH_SERVER)utility.o $(PATH_SERVER)request.o $(PATH_SERVER)server.o -o $(PATH_SERVER)tftp_server
	
tftp_client: utility.o client.o
		$(CC) $ $(PATH_CLIENT)utility.o $(PATH_CLIENT)client.o -o $(PATH_CLIENT)tftp_client

client.o: $(PATH_CLIENT)client.c $(PATH_CLIENT)client.h
		$(CC) $(CFLAGS) $ -c $(PATH_CLIENT)client.c -o $(PATH_CLIENT)client.o

tftp_server: request.o utility.o server.o
		$(CC) $ $(PATH_SERVER)request.o $(PATH_SERVER)utility.o $(PATH_SERVER)server.o -o $(PATH_SERVER)tftp_server


server.o: $(PATH_SERVER)server.c $(PATH_SERVER)server.h $(PATH_SERVER)request.o
		$(CC) $(CFLAGS) $ -c $(PATH_SERVER)server.c -o $(PATH_SERVER)server.o


utility.o: $(PATH_UTILITY)utility.c $(PATH_UTILITY)utility.h
		 cp $(PATH_UTILITY)utility.c $(PATH_CLIENT)utility.c
		 cp $(PATH_UTILITY)utility.h $(PATH_CLIENT)utility.h
		 cp $(PATH_UTILITY)utility.c $(PATH_SERVER)utility.c
		 cp $(PATH_UTILITY)utility.h $(PATH_SERVER)utility.h
		 $(CC) $(CFLAGS) -c $(PATH_CLIENT)utility.c -o $(PATH_CLIENT)utility.o
		 $(CC) $(CFLAGS) -c $(PATH_SERVER)utility.c -o $(PATH_SERVER)utility.o

request.o: $(PATH_SERVER)request.c $(PATH_SERVER)request.h
		 $(CC) $(CFLAGS) -c $(PATH_SERVER)request.c -o $(PATH_SERVER)request.o

clean:
	rm -f $(PATH_CLIENT)utility.c $(PATH_CLIENT)utility.h
	rm -f $(PATH_SERVER)utility.c $(PATH_SERVER)utility.h	
	rm -f $(PATH_CLIENT)*.o $(PATH_CLIENT)tftp_client
	rm -f $(PATH_SERVER)*.o $(PATH_SERVER)tftp_server



