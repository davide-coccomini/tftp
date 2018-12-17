CC=gcc
CFLAGS=-Wall
LDFLAGS=-lpthread
TERM=x-terminal-emulator

PUTILITY=./Utility/
PCLIENT=./Client/
PSERVER=./Server/

all: utility.o $(PSERVER)server.o $(PCLIENT)client.o
		$(CC) $(PCLIENT)utility.o $(LDFLAGS) $(PCLIENT)client.o -o $(PCLIENT)tftp_client
		$(CC) $(PSERVER)utility.o $(PSERVER)server.o -o $(PSERVER)tftp_server
	
tftp_client: utility.o client.o
		$(CC) $(LDFLAGS) $(PCLIENT)utility.o $(PCLIENT)client.o -o $(PCLIENT)tftp_client

client.o: $(PCLIENT)client.c $(PCLIENT)client.h
		$(CC) $(CFLAGS) $(LDFLAGS) -c $(PCLIENT)client.c -o $(PCLIENT)client.o

tftp_server: utility.o server.o
		$(CC) $(LDFLAGS) $(PSERVER)utility.o $(PSERVER)server.o -o $(PSERVER)tftp_server


server.o: $(PSERVER)server.c $(PSERVER)server.h
		$(CC) $(CFLAGS) $(LDFLAGS) -c $(PSERVER)server.c -o $(PSERVER)server.o


utility.o: $(PUTILITY)utility.c $(PUTILITY)utility.h
		 cp $(PUTILITY)utility.c $(PCLIENT)utility.c
		 cp $(PUTILITY)utility.h $(PCLIENT)utility.h
		 cp $(PUTILITY)utility.c $(PSERVER)utility.c
		 cp $(PUTILITY)utility.h $(PSERVER)utility.h
		 $(CC) $(CFLAGS) -c $(PCLIENT)utility.c -o $(PCLIENT)utility.o
		 $(CC) $(CFLAGS) -c $(PSERVER)utility.c -o $(PSERVER)utility.o

clean:
	rm -f $(PCLIENT)utility.c $(PCLIENT)utility.h
	rm -f $(PSERVER)utility.c $(PSERVER)utility.h	
	rm -f $(PCLIENT)*.o $(PCLIENT)tftp_client
	rm -f $(PSERVER)*.o $(PSERVER)tftp_server


test: all
	$(TERM) -e sh ./test.sh
	# sh ./stop.sh
