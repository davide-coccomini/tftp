#include "server.h"
#include "utility.h"



int main(int argc, char** argv){
	

	if(argc != 3){
		printf("\nPer avviare il programma digita ./tftp_server <porta> <directory files>\n\n");
		return 0;
	}
	
	char* directory = argv[2]; // Directory dei files
	printf("\nDirectory files: %s", directory);

	fd_set master;
	fd_set read_fds;
	int fdmax;
	


	int listener;

	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	unsigned int addrlen = sizeof(client_addr);
	char buf[BUFFER_SIZE];
	int i;
	int newfd;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);


	printf("\nCreazione socket di ascolto\n");
	listener = socket(AF_INET, SOCK_STREAM,0);
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(argv[1]));
	my_addr.sin_addr.s_addr = INADDR_ANY;
	printf("\nSocket di ascolto creato\n");

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
		 if(FD_ISSET(i, &read_fds)){
			if(i==listener){ // Il descrittore pronto è il listener
				addrlen = sizeof(client_addr);
				newfd = accept(listener, (struct sockaddr *)&client_addr, &addrlen);
				FD_SET(newfd, &master);
				if(newfd > fdmax){
					fdmax = newfd;
				}
			}else{ // Il descrittore pronto è un altro socket
				memset(buf, 0, BUFFER_SIZE);
				cmd(i, buf, &master);

				if(!strcmp(buf, "!help\0")){
					printf("\nRicezione comando help\n");
				}else if(!strcmp(buf, "!mode\0")){
					printf("\nRicezione comando mode\n");
				}else if(!strcmp(buf, "!get\0")){
					printf("\nRicezione comando get\n");
					getCmd(i, directory);
				}else if(!strcmp(buf, "!quit\0")){
					printf("\nRicezione comando quit\n");
					close(i);
					FD_CLR(i, &master);
				}else{
					printf("\nConnessione in ingresso\n");
				}
			}
		 }
		}	
	}
 close(listener);
}
void cmd(int sock, char* buffer, fd_set* master){
	uint16_t length;
	int ret;

	memset(&length, 0, sizeof(uint16_t));

	ret = recv(sock, (void*)&length, sizeof(uint16_t),0);
	if(ret < 0){
		perror("ERRORE! Ricezione della lunghezza del comando non riuscita\n");
		return;
	}
	if(recv(sock, (void*)buffer, ntohs(length),0) < 0){
		perror("ERRORE! Ricezione del comando non riuscita\n");
	}

	return;
}


void getCmd(int sock, char* directory){
	// Ricezione della modalità di trasferimento
	char* mode = receiveString(sock);
	printf("\nRicezione mode %s", mode);
	//Invio del segnale di ACK per conferma
	sendACK(sock);
	printf("\nInvio ACK");
	//Ricezione del nome del file richiesto
	char* fileName = receiveString(sock);
	printf("\nRicezione filename %s", fileName);

	char* path = strcat(directory,"/");
	path = strcat(path,fileName);

	FILE* fp;
	printf("\nTentativo di aprire %s", path);
	fp = fopen(path, "r");
	printf("\nFile aperto\n");
	if(fp == NULL){
		printf("\nERRORE! Lettura del file %s non riuscita\n", fileName);
		return;
	}
	printf("\nLettura del file %s riuscita", fileName);

	fseek(fp, 0 , SEEK_END);
  	unsigned int fileSize = ftell(fp);
  	fseek(fp, 0 , SEEK_SET);
	
	// Invio della dimensione del file richiesto
	printf("\nInvio dimensione del file %d", fileSize);
	sendSize(sock, fileSize);

	int transfers = ((int)(fileSize/512))+1;


	if(!strcmp(mode, "txt")){	
		char buffer[FILE_BUFFER_SIZE];
		int i, fsize;
		for(i=1; (fsize = fread(buffer, sizeof(char), sizeof(buffer), fp) > 0); i++){
			printf("\nInvio file %d/%d",i,transfers);
			sendFileTxt(sock, buffer);
		}
	}else if(!strcmp(mode, "bin")){


	}


	fclose(fp);
}


