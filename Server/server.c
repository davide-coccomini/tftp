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
					getCmd(i, directory, my_addr);
				}else if(!strcmp(buf, "!quit\0")){
					printf("\nRicezione comando quit\n");
					close(i);
					FD_CLR(i, &master);
				}else{
					//printf("\nConnessione in ingresso\n");
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


void getCmd(int sock, char* directory, struct sockaddr_in server_addr){
	
	char* buffer = receiveBuffer(sock);
	
	uint16_t opcode, errorCode;
    char fileName[BUFFER_SIZE];
	char mode[BUFFER_SIZE];
	int position = 0;

	memcpy(&opcode, (uint16_t*)&buffer, 2);
	opcode = ntohs(opcode);
	printf("\n opcode ricevuto:: %d\n",opcode);
	// Errore
	if(opcode != 1){
		opcode = htons(5);
		errorCode = htons(1);
		char* message = "Richiesta invalida";
	
		char bufferError[BUFFER_SIZE];	
	
		//unsigned int zero = 0;

		memcpy(bufferError, (uint16_t*)&opcode, 2);
		position += 2;
		memcpy(bufferError+position, (uint16_t*)&errorCode, 2);
		position += 2;
		strcpy(bufferError+position, message);
		position += strlen(message)+1;
		//memcpy(buffer, zero, 1);
	
		sendBuffer(sock, bufferError, position, server_addr);
		return;
	}

	printf("\ntest\n");
	
	strcpy(fileName, buffer+2);
	printf("\nnome ricevuto: %s\n", fileName);
	strcpy(mode, buffer + strlen(fileName)+1);
	printf("\nmode ricevuto: %s\n", mode);
	printf("\nRicezione filename %s\n", fileName);

	char* path = strcat(directory,"/");
	path = strcat(path, fileName);

	FILE* fp;
	printf("\nTentativo di aprire %s", path);
	fp = fopen(path, "r");
	printf("\nFile aperto");
	if(fp == NULL){
		position = 0;
		printf("\nERRORE! Lettura del file %s non riuscita\n", fileName);
		opcode = htons(5);
		char* message = "File non trovato";

		char errorBuffer[BUFFER_SIZE];	

		memcpy(errorBuffer, (uint16_t*)&opcode, 2);
		position += 2;
		strcpy(errorBuffer+position, message);

		sendBuffer(sock, errorBuffer, position, server_addr);
		return;
	}
	printf("\nLettura del file %s riuscita", fileName);


	if(!strcmp(mode, "netascii")){	
		// Lettura della lunghezza del contenuto del file
		uint16_t block = 1;
		fseek(fp, 0 , SEEK_END);
	  	unsigned int length = ftell(fp);
	  	fseek(fp, 0 , SEEK_SET);
		unsigned int nchars = (length > FILE_BUFFER_SIZE)?FILE_BUFFER_SIZE:length;
		char bufferFile[nchars];

		// Trasferimenti
		while(length > 0){
			// Lettura ed invio di un blocco
			//fseek(fp, nbytes, fp);
			position = 0;
			int k;
			for(k=0; k<nchars; k++){		
				fscanf(fp, "%s", bufferFile+k);
			}
			
			char bufferPacket[BUFFER_SIZE];
			
			
			opcode = htons(3);
			memcpy(bufferPacket, (uint16_t*)&opcode, 2);
			position += 2;

			strcpy(bufferPacket + position, bufferFile);
			position += nchars;
			
			sendBuffer(sock, bufferPacket, position, server_addr);
			
			// Attesa dell'ACK
			char* bufferAck = receiveBuffer(sock);
			if(!bufferAck){
				printf("\nERRORE! Ack invalido");	
				return;		
			}
			
			// Passaggio al blocco successivo
			length -= nchars;
			nchars = (length > FILE_BUFFER_SIZE)?FILE_BUFFER_SIZE:length;
			block++;			
		}
	}else if(!strcmp(mode, "octet")){
		// OPCODE (3)

	}


	fclose(fp);
}




