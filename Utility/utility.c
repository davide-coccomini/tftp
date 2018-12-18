#include "utility.h"


void sendString(int sock, char* s){ // Nome del file e modalità

	uint16_t length;
	uint16_t lenCheck = (s)?strlen(s)+1 : 0;

	length = htons(lenCheck);

	while(send(sock, (void*)&length, sizeof(uint16_t),0) < 0){
		perror("ERRORE: Lunghezza dei dati invalida");
	}

	if(lenCheck){
		while(send(sock, (void*)s, lenCheck, 0) < 0){
			perror("ERRORE: Invio dei dati non riuscito");
		}
	}
}



char* receiveString(int sock){
	uint16_t length;
	char* s = NULL;


	while(recv(sock, (void*)&length, sizeof(uint16_t), 0) < 0){
		perror("ERRORE: Lunghezza dei dati invalida");
	}

	if(ntohs(length)){
		s = malloc(ntohs(length));
		memset(s, 0, ntohs(length));
		while(recv(sock, (void*)s, ntohs(length), 0) < 0){
			perror("ERRORE: Ricezione dei dati non riuscita");
		}
	}
	return s;
}

void sendSize(int sock, unsigned int val){
	unsigned int uint = htons(val);
	while(send(sock, &uint, sizeof(unsigned int), 0) < 0){
		perror("ERRORE: Invio della dimensione non riuscito");
	}
	
}

unsigned int receiveSize(int sock){

	unsigned int size;
	while(recv(sock, &size, sizeof(unsigned int), 0) < 0){
		perror("ERRORE: Ricezione della dimensione non riuscita");
	}
	return size;
}
void sendFileTxt(int sock, char* buffer){
	uint16_t length;
	uint16_t lenCheck = (buffer)?strlen(buffer)+1 : 0;
	length = htons(lenCheck);

	while(send(sock, (void*)&length, sizeof(uint16_t),0) < 0){
		perror("ERRORE: Lunghezza dei dati invalida");
	}

	if(lenCheck){
		while(send(sock, (void*)buffer, lenCheck, 0) < 0){
			perror("ERRORE: Invio dei dati non riuscito");
		}
	}

}

char* receiveFileTxt(int sock){
	uint16_t length;
	char* buffer;
	while(recv(sock, &length, sizeof(uint16_t),0) < 0){
		perror("ERRORE! Ricezione della lunghezza dei dati non riuscita");
	}
	buffer = malloc(ntohs(length));
	memset(buffer, 0, ntohs(length));
	while(recv(sock, buffer, ntohs(length),0) < 0){
		perror("ERRORE! Ricezione del file non riuscita");
	}
	return buffer;
}


void sendFileBin(int sock, FILE* buffer){
	int nbytes;
	if ((nbytes = write(sock, &buffer, sizeof(FILE)) != sizeof(buffer))){
	  perror("ERRORE: Invio dei dati non riuscito");
	}

}


FILE* receiveFileBin(int sock, unsigned int size){
	int transfers = ((int)(size/512))+1;
	int nbytes;
	FILE* file;
	while(size > 0){
		if((nbytes = recv(sock, (void*)&file, sizeof(FILE), 0) >= 0)){
			// Ricezione
			printf("Ricevuti %d bytes", nbytes);
		}
		size -= 512;
	}

	printf("Trasferimento effettuato con successo (%d/%d)", transfers, transfers);
	return file;

}


void sendACK(int sock){
	unsigned int ack = 555;
	while(send(sock, &ack, sizeof(unsigned int), 0) < 0){
		perror("ERRORE: Invio dell'ACK non riuscito");
	}
}

unsigned int receiveACK(int sock){
	unsigned int ack;
	while(recv(sock, &ack, sizeof(unsigned int), 0) < 0){
		perror("ERRORE: Ricezione dell'ACK non riuscita");
	}
	return ack;

}
