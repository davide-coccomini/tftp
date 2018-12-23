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


	for(;;){	
	 	select(fdmax+1, &read_fds, NULL, NULL, NULL);
		
	 for(i=0; i<=fdmax; i++){
	  if(FD_ISSET(i, &read_fds)){		
		struct result ret = receiveBuffer(listener);	
		client_addr = ret.client_addr;
		char* buffer = ret.buffer;
		uint16_t opcode, errorCode;
		char fileName[BUFFER_SIZE];
		char mode[BUFFER_SIZE];
		if(i==listener){ // Il descrittore pronto è il listener
			
		 	newfd =  socket(AF_INET, SOCK_DGRAM, 0);
		
			
			FD_SET(newfd, &read_fds);
			if(newfd > fdmax) fdmax = newfd;	
			position = 0;
	
			memcpy(&opcode, buffer, 2);
			opcode = ntohs(opcode);
			// Errore
			if(opcode != 1){
				opcode = htons(5);
				errorCode = htons(1);
				char* message = "Richiesta invalida";	
				char bufferError[BUFFER_SIZE];	
	
				memcpy(bufferError, (uint16_t*)&opcode, 2);
				position += 2;
				memcpy(bufferError+position, (uint16_t*)&errorCode, 2);
				position += 2;
				strcpy(bufferError+position, message);
				position += strlen(message)+1;

	
				sendBuffer(newfd, bufferError, position, client_addr);

			}

	
			strcpy(fileName, buffer+2);
			strcpy(mode, buffer + (int)strlen(fileName)+3);
			

			
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
			if(fp == NULL){
				position = 0;
				printf("\nERRORE! Lettura del file %s non riuscita\n", fileName);
				opcode = htons(5);
				char* message = "File non trovato";

				char errorBuffer[BUFFER_SIZE];	

				memcpy(errorBuffer, (uint16_t*)&opcode, 2);
				position += 2;
				strcpy(errorBuffer+position, message);

				sendBuffer(newfd, errorBuffer, position, client_addr);
		
			}

			printf("\nLettura del file %s riuscita\n", fileName);
			// Lettura della lunghezza del contenuto del file
		
			fseek(fp, 0 , SEEK_END);
			unsigned int length = ftell(fp);
			fseek(fp, 0 , SEEK_SET);

			unsigned int nchars = (length > FILE_BUFFER_SIZE)?FILE_BUFFER_SIZE:length;
			addRequest(newfd, fp, length-nchars, mode, 0);
			
			if(!strcmp(mode, "netascii\0")){	
				char bufferFile[FILE_BUFFER_SIZE];
				
				int k;
				for(k = 0; k<nchars; k++){
					bufferFile[k] = fgetc(fp);
				}


				// Lettura ed invio di un blocco
				position = 0;

			
				char bufferPacket[PACKET_SIZE];
			
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
				unsigned char bufferFile[FILE_BUFFER_SIZE];
				fread(bufferFile, nchars, 1, fp);

				// Lettura ed invio di un blocco
				position = 0;

			
				char bufferPacket[PACKET_SIZE];
			
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
		 printf("\nTrasferimento del primo blocco completato\n");


		}else{ // Il descrittore pronto è un altro socket
			printf("\nRicezione dell'ACK\n");
			// Attesa dell'ACK
			char* bufferAck = ret.buffer;
			if(!bufferAck){
				printf("\nERRORE! ACK invalido");	
				
			}
			struct request* r = findRequest(i);
			removeRequest(i);

			if(r->packets > 0){
				
				unsigned int nchars = (r->packets > FILE_BUFFER_SIZE)?FILE_BUFFER_SIZE:r->packets;
				
				r->block++;
				printf("\nInvio del blocco %d\n", r->block);
				addRequest(r->sock, r->fp, r->packets-nchars, r->mode, r->block);		
	
				
				
				// Lettura ed invio di un blocco
				position = 0;
				
			
				opcode = htons(3);
				uint16_t blockSend = htons(r->block);
				if(!strcmp(r->mode, "netascii\0")){	
				
					char bufferPacket[PACKET_SIZE];
					char bufferFile[FILE_BUFFER_SIZE];

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
					sendBuffer(i, bufferPacket, position, client_addr);

					memset(bufferFile, 0, FILE_BUFFER_SIZE);
				}else{
				
					unsigned char bufferPacket[PACKET_SIZE];
					unsigned char bufferFile[FILE_BUFFER_SIZE];
					fread(bufferFile, nchars, 1, r->fp);
					memcpy(bufferPacket, (uint16_t*)&opcode, 2);
					position += 2;
					memcpy(bufferPacket + position, (uint16_t*)&blockSend, 2);
					position += 2;

					memcpy(bufferPacket + position, bufferFile, nchars);
					position += nchars;
					sendBuffer(i, bufferPacket, position, client_addr);

					memset(bufferFile, 0, FILE_BUFFER_SIZE);
				}
				FD_SET(i, &read_fds);
				i--;
			}else{
				FD_SET(listener, &read_fds);
			   	FD_CLR(i, &read_fds);
				printf("\nL'intero file è stato trasferito con successo\n");
			}	
		 
		}
	  }
	 }
		
	}
 close(listener);
}


