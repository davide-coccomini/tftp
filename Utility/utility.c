#include "utility.h"




void sendBuffer(int sock, char* buffer, int position, struct sockaddr_in server_addr){
	printf("\nSEND\n");
	uint16_t length = position;
	printf("\nlength: %d\n", length);
	unsigned int addrlen = sizeof(server_addr);

	if(sendto(sock, (void*)&length, sizeof(uint16_t), 0, (struct sockaddr*)&server_addr, addrlen) < 0){
		perror("\nERRORE: Lunghezza dei dati inviati invalida\n");
	}

	if(sendto(sock, (void*)buffer, length, 0,(struct sockaddr*)&server_addr, addrlen) < 0){
		perror("\nERRORE: Invio dei dati non riuscito\n");
	}

}



char* receiveBuffer(int sock){
	printf("\nRECEIVE\n");
	uint16_t length;
	char* buffer = NULL;
	unsigned int addrlen;
	struct sockaddr_in client_addr;

	if(recvfrom(sock, (void*)&length, sizeof(uint16_t), 0, (struct sockaddr*)&client_addr, &addrlen) < 0){
		perror("\nERRORE: Lunghezza dei dati ricevuti invalida\n");
	}
		printf("\nlength: %d\n", length);

	if(length){
		buffer = malloc(length);
		memset(buffer, 0, length);
		if(recvfrom(sock, (void*)&buffer, length, 0, (struct sockaddr*)&client_addr, &addrlen) < 0){
			perror("\nERRORE: Ricezione dei dati non riuscita\n");
		}
	}
	return buffer;
}





