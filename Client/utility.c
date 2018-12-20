#include "utility.h"




void sendGetBuffer(int sock, char* buffer){

	uint16_t length = strlen(buffer)+1;


	while(sendto(sock, (void*)&length, sizeof(uint16_t),0) < 0){
		perror("ERRORE: Lunghezza dei dati invalida");
	}

	while(sendto(sock, (void*)buffer, length, 0) < 0){
		perror("ERRORE: Invio dei dati non riuscito");
	}

}



char* receiveBuffer(int sock){
	uint16_t length;
	char* s = NULL;


	while(recvfrom(sock, (void*)&length, sizeof(uint16_t), 0) < 0){
		perror("ERRORE: Lunghezza dei dati invalida");
	}

	if(length){
		buffer = malloc(length);
		memset(buffer, 0, length);
		while(recvfrom(sock, (void*)buffer, length, 0) < 0){
			perror("ERRORE: Ricezione dei dati non riuscita");
		}
	}
	return buffer;
}





