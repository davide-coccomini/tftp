#include <server.h>




int main(int argc, char** argv){
	


	if(argc != 3){
		printf("Per avviare il programma digita ./tftp_server <porta> <directory files>");
		return 0;
	}
	
	char* directory = argv[2]; // Directory dei files



	fd_set master;
	fd_set read_fds;
	int fdmax;
	


	int listener, new_sock;

	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	char buf[512];
	int nbytes;
	int addrlen;
	int i;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	
	printf("Creazione socket di ascolto");
	listener = socket(AF_INET, SOCK_STREAM,0);
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(argv[1]));
	my_addr.sin_addr.s_addr = INADDR_ANY;
	printf("Socket di ascolto creato");

	if(bind(listener, (struct sockaddr*)&my_addr, sizeof(my_addr))<0){
		perror("ERRORE: Bind non riuscito");
		return -1;
	}
	
	if(listen(listener, 10) < 0){
		perror("ERRORE: Listen non riuscita");
		return -1;
	}
	FD_SET(listener, &master);
	fdmax = listener;


	for(;;){
		
		// (1) Arriva connessione
		// (2) Ricevi modalità
		// (3) Controlli presenza file ed eventualmente mandi errore
		// (4) Converti file nella modalità richiesta se necessario
		// (5) Invii file
		// (6) Ricevi l'ACK ed eventualmente stampi messaggio di errore
		read_fds = master;	
		select(fdmax +1, &read_fds, NULL, NULL, NULL);
		for(i=0; i<=fdmax; i++){
			if(i==listener){
				addrlen = sizeof(client_addr);
				newfd = accept(listener, (struct sockaddr *)&client_addr, &addrlen);
				FD_SET(newfd, &master);
				if(newfd > fdmax){
					fdmax = newfd;
				}else{
					nbytes = recv(i, buf, sizeof(buf));
					close(i);
					FD_CLR(I, &master);
				}
			}
			
		}	
	}

}
/*
void binaryToText(char* fileName, char* outputFile){
	unsigned char str[256];
	unsigned int num;
	int i, len;
	
	FILE* finp = fopen(fileName, "rb");
	FILE* fout = fopen(outputFile, "w");
	
	while((len = fgetc(finp)) != EOF){
		fread(str, len, 1, finp);
		str[len] = '\0';
		num = (unsigned int)fgetc(finp)<<24;
		num |= fgetc(finp) << 16;
		num |= fgetc(finp) << 8;
		num |= fgetc(finp);
		fprintf(fout, "%s %d\n", (char*)str, num);
	}
	fclose(finp);
	fclose(fout);



}*/
void sendFile(char* fileName, char* buf, int s, char* mode){
	
	
}