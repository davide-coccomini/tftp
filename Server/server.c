#include "server.h"
#include "utility.h"



int main(int argc, char** argv){
	

	if(argc != 3){
		printf("\nPer avviare il programma digita ./tftp_server <porta> <directory files>\n\n");
		return 0;
	}
	
	char* directory = argv[2]; // Directory dei files



	fd_set read_fds;
	int fdmax, newfd;
	


	int listener;
	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	int i;
	int position;
	
	FD_ZERO(&read_fds);


	printf("\nCreazione socket di ascolto\n");
	listener = socket(AF_INET, SOCK_DGRAM,0);

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(argv[1]));
	my_addr.sin_addr.s_addr = INADDR_ANY;
	printf("\nSocket di ascolto creato\n");

	if(bind(listener, (struct sockaddr*)&my_addr, sizeof(my_addr))<0){
		perror("ERRORE: Bind non riuscito");
		return -1;
	}

	FD_SET(listener, &read_fds);
	fdmax = listener;

	char fileName[BUFFER_SIZE];
	char mode[BUFFER_SIZE];	
	char bufferPacket[PACKET_SIZE];
	char bufferFile[FILE_BUFFER_SIZE];
	char bufferError[BUFFER_SIZE];
	char message[FILE_BUFFER_SIZE];
	for(;;){	
	 select(fdmax+1, &read_fds, NULL, NULL, NULL);
		
	 for(i=0; i<=fdmax; i++){
	 
	//printf("\ni: %d, listener: %d \n", i, listener);
	  if(FD_ISSET(i, &read_fds)){
		
		struct result ret = receiveBuffer(listener);	
		sleep(0.3);
		client_addr = ret.client_addr;
		char* buffer = ret.buffer;
		uint16_t opcode, errorCode;
		memcpy(&opcode, buffer, 2);
	
		opcode = ntohs(opcode);
		// Errore
		if(opcode != 1 && opcode != 4){
			printf("\nOperazione TFTP non prevista\n");
			opcode = htons(5);
			errorCode = htons(1);
			char* message = "Operazione TFTP non prevista";	
				
	
			memcpy(bufferError, (uint16_t*)&opcode, 2);
			position += 2;
			memcpy(bufferError+position, (uint16_t*)&errorCode, 2);
			position += 2;
			strcpy(bufferError+position, message);
			position += strlen(message);
	
			sendBuffer(newfd, bufferError, position, client_addr);
			close(newfd);
			FD_CLR(newfd, &read_fds);
			i=0;
			memset(bufferFile, 0, FILE_BUFFER_SIZE);
			continue;
		}

		if(opcode == 1) i=listener;
		if(i==listener && opcode == 1){ // Il descrittore pronto è il listener
			printf("\n i==listener\n");
		 	newfd =  socket(AF_INET, SOCK_DGRAM, 0);
		
			
			FD_SET(newfd, &read_fds);
			if(newfd > fdmax) fdmax = newfd;	
			position = 0;
	
	
			
			memset(fileName, 0, BUFFER_SIZE);
			strcpy(fileName, buffer+2);
			strcpy(mode, buffer + (int)strlen(fileName)+3);
			free(buffer);
			
			
			printf("\nRichiesta di download del file %s in modalità %s\n", fileName, mode);
			char* path = malloc(strlen(directory)+strlen(fileName)+2);
			strcpy(path, directory);
			strcat(path, "/");
			strcat(path, fileName);
			FILE* fp;
			if(!strcmp(mode, "netascii\0"))
				fp = fopen(path, "r");
			else
				fp = fopen(path, "rb");

			free(path);
			if(fp == NULL){
				position = 0;
				printf("\nERRORE! Lettura del file %s non riuscita\n", fileName);
				opcode = htons(5);
				errorCode = htons(1);
				
				strcpy(message, "File non trovato");
				
				
				memcpy(bufferError, (uint16_t*)&opcode, 2);
				position += 2;
				memcpy(bufferError+position, (uint16_t*)&errorCode, 2);
				position += 2;
				
				strcpy(bufferError+position, message);
				position += strlen(message);
				sendBuffer(newfd, bufferError, position, client_addr);	
				close(newfd);
				FD_CLR(newfd, &read_fds);
				i=0;
				memset(bufferFile, 0, FILE_BUFFER_SIZE);
				continue;
			}else{
				printf("\nLettura del file %s riuscita\n", fileName);
				// Lettura della lunghezza del contenuto del file
		
				fseek(fp, 0 , SEEK_END);
				unsigned int length = ftell(fp);
				fseek(fp, 0 , SEEK_SET);

				unsigned int nchars = (length > FILE_BUFFER_SIZE)?FILE_BUFFER_SIZE:length;
				addRequest(newfd, client_addr, fp, length-nchars, mode, 0);
			
		
				if(!strcmp(mode, "netascii\0")){	
				
					int k;
					for(k = 0; k<nchars; k++){
						bufferFile[k] = fgetc(fp);
					}


					// Lettura ed invio di un blocco
					position = 0;

			
				
					opcode = htons(3);
					memcpy(bufferPacket, (uint16_t*)&opcode, 2);
					position += 2;
					uint16_t blockSend = htons(1);
					memcpy(bufferPacket + position, (uint16_t*)&blockSend, 2);
					position += 2;

					strcpy(bufferPacket + position, bufferFile);
					position += nchars;
					sendBuffer(newfd, bufferPacket, position, client_addr);
		
			
					memset(bufferFile, 0, FILE_BUFFER_SIZE);

			
				}else{
					fread(bufferFile, nchars, 1, fp);

					// Lettura ed invio di un blocco
					position = 0;

			
			
			
					opcode = htons(3);
					memcpy(bufferPacket, (uint16_t*)&opcode, 2);
					position += 2;
					uint16_t blockSend = htons(1);
					memcpy(bufferPacket + position, (uint16_t*)&blockSend, 2);
					position += 2;

					memcpy(bufferPacket + position, bufferFile, nchars);
					position += nchars;
					sendBuffer(newfd, bufferPacket, position, client_addr);
			
					memset(bufferFile, 0, FILE_BUFFER_SIZE);
			
				
				}
		 	 FD_CLR(listener, &read_fds);	
			}

		}else if(i!=listener && opcode == 4){ // Il descrittore pronto è un altro socket
			//printf("\nRicezione dell'ACK\n");
			// Attesa dell'ACK
			char* bufferAck = ret.buffer;
			if(!bufferAck){
				printf("\nERRORE! ACK invalido\n");	
			}
			free(bufferAck);
			struct request* r = findRequest(i);
			removeRequest(i);
			if(r->packets > 0){
				unsigned int nchars = (r->packets > FILE_BUFFER_SIZE)?FILE_BUFFER_SIZE:r->packets;
				
				r->block++;
				//printf("\r[%d]Invio del blocco", r->block);
				addRequest(r->sock, r->client_addr, r->fp, r->packets-nchars, r->mode, r->block);		
	
				
				
				// Lettura ed invio di un blocco
				position = 0;
				
				
				opcode = htons(3);
				uint16_t blockSend = htons(r->block);
				if(!strcmp(r->mode, "netascii\0")){	


					int k;
					for(k = 0; k<nchars; k++){
						bufferFile[k] = fgetc(r->fp);
					}
					memcpy(bufferPacket, (uint16_t*)&opcode, 2);
					position += 2;
					memcpy(bufferPacket + position, (uint16_t*)&blockSend, 2);
					position += 2;

					strcpy(bufferPacket + position, bufferFile);
					position += nchars;
					sendBuffer(i, bufferPacket, position, r->client_addr);

				}else{
				
					fread(bufferFile, nchars, 1, r->fp);
					memcpy(bufferPacket, (uint16_t*)&opcode, 2);
					position += 2;
					memcpy(bufferPacket + position, (uint16_t*)&blockSend, 2);
					position += 2;

					memcpy(bufferPacket + position, bufferFile, nchars);
					position += nchars;
					sendBuffer(i, bufferPacket, position, r->client_addr);

				}
				memset(bufferFile, 0, FILE_BUFFER_SIZE);

			}else{
				if(i == fdmax){
					fdmax = findMaxSocket(listener);
					if(fdmax == listener){
						resetRequestList();
						FD_SET(listener, &read_fds);
					}
				}
				close(i);
			   	FD_CLR(i, &read_fds);
				i=0;
				printf("\nL'intero file è stato trasferito con successo\n");
			}	
		 
		}
		if(i==findMaxSocket(listener)) i=0;		
	  }
	 }
		
	}
 close(listener);
}


