#include "utility.h"



void sendBuffer(int sock, char* buffer, int position, struct sockaddr_in server_addr){
	printf("\nsend\n");
	uint16_t lengthNet = htons(position);
	uint16_t length = (position > sizeof(buffer))?position:sizeof(buffer);
	unsigned int addrlen = sizeof(server_addr);

	
	int nbytes = sendto(sock, &lengthNet, sizeof(uint16_t), 0, (struct sockaddr*)&server_addr, addrlen);
;
	if(nbytes < sizeof(uint16_t)){
		perror("\nERRORE: Lunghezza dei dati inviati invalida\n");
	}
	
	nbytes = sendto(sock, buffer, length, 0,(struct sockaddr*)&server_addr, sizeof(server_addr));

	/*int m;
	for(m=0; m<position;m++){
		if(m<4 || m==position-1)
			printf("\nlettera:%d %d\n",m,buffer[m]);
		else
			printf("\nlettera:%d %c\n",m,buffer[m]);
	}
	printf("\nlength: %d; nbytes: %d\n", position, nbytes);*/
	if(nbytes < sizeof(buffer)){
		perror("\nERRORE: Invio dei dati non riuscito\n");
	}

}



struct result receiveBuffer(int sock){
	printf("\nreceive\n");
	struct sockaddr_in client_addr;
	uint16_t length;
	char* buffer = NULL;
	unsigned int addrlen = sizeof(client_addr);
	struct result r;
	
	int nbytes = recvfrom(sock, &length, sizeof(uint16_t), 0, (struct sockaddr*)&client_addr, &addrlen);
	
	if(nbytes < sizeof(uint16_t)){
		perror("\nERRORE: Lunghezza dei dati ricevuti invalida\n");
	}
	nbytes = 0;
	length = ntohs(length);

	buffer = (char *)malloc(length);

	if(buffer == NULL){
		perror("\nERRORE! Allocazione non riuscita\n");
		r.buffer = NULL;
		r.length = 0;
		return r;
	}

	memset(buffer, 0, length);
	
	nbytes = recvfrom(sock, buffer, length, 0, (struct sockaddr*)&client_addr, &addrlen);


	/*	
	
	int m;
	for(m=0; m<length;m++){
		
			printf("\nlettera: %d\n",buffer[m]);
	}
	printf("\n nbytes: %d; sizeofbuffer: %d\n",(int) nbytes, (int)sizeof(buffer));
	if(nbytes < sizeof(buffer)){
		perror("\nERRORE: Ricezione dei dati non riuscita\n");
	}*/
	
	r.length = length;
	r.buffer = buffer;
	r.client_addr = client_addr;
	return r;
}




