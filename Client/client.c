#include "client.h"
#include "utility.h"
int main(int argc, char** argv){

	int sock;
	struct sockaddr_in server_addr;
	sleep(1);

	char cmd[CMD_SIZE];

	if(argc != 3){
		printf("\nPer avviare il programma digita ./tftp_client <IP_server> <porta_server>\n\n");
		return 0;
	}


	sock = socket(AF_INET, SOCK_DGRAM, 0);
	printf("\nsock: %d\n", sock);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

	
	char transferMode[CMD_SIZE]; 
	strcpy(transferMode,"octet\0");

	initMessage(sock, argv[1], argv[2], server_addr);

	while(1){
	
		memset(&cmd, 0, CMD_SIZE);
		scanf("%s", cmd);

		if(!strcmp(cmd, "!help\0")){
			helpCmd();
		}else if(!strcmp(cmd, "!mode\0")){
			char* mode = argv[1];
			scanf("%s",mode);
			modeCmd(mode, transferMode);
		}else if(!strcmp(cmd, "!get\0")){
			char fileName[BUFFER_SIZE], localName[BUFFER_SIZE];
			memset(&fileName, 0, BUFFER_SIZE);
			scanf("%s %s", fileName, localName);
			getCmd(sock, fileName, localName, transferMode, server_addr);
		}else if(!strcmp(cmd, "!quit\0")){
			quitCmd(sock);
		}

	}
}


void initMessage(int sock, const char* server, const char* port, struct sockaddr_in server_addr){
	printf("Connessione al server %s alla porta %s effettuata con successo\n", server,port);
	helpCmd();
}

void helpCmd(){
	printf("\nSono disponibili i seguenti comandi:\n");
	printf("!help --> mostra l'elenco dei comandi disponibili\n");
	printf("!mode {txt|bin} --> imposta il modo di trasferimento dei files (testo o binario)\n");
	printf("!get filename nome_locale --> richiede al server il nome del file <filename> e lo salva localmente con il nome <nome_locale>\n");
	printf("!quit --> termina il client\n");
}

void modeCmd(char* mode, char* currentMode){
	if(!strcmp(mode, "txt")){	
		printf("Modo di trasferimento testuale configurato.\n");
		strcpy(currentMode,"netascii\0");
	}else if(!strcmp(mode, "bin")){
		printf("Modo di trasferimento binario configurato.\n");
		strcpy(currentMode,"octet\0");
	}else{
		printf("ERRORE: Modo di trasferimento non previsto.\n");
	}
 return;	
}

void getCmd(int sock, char* fileName, char* localName, char* transferMode, struct sockaddr_in server_addr){
	printf("Richiesta file %s al server in corso.\n", fileName);
	char buffer[BUFFER_SIZE];
	memset(&buffer, 0, BUFFER_SIZE);
	int position = 0;
	printf("\nPreparazione del buffer da inviare\n");
	uint16_t errorCode;
	uint16_t opcode = htons(1);
	uint16_t fileNameLength = strlen(fileName);

	memcpy(buffer, (uint16_t*)&opcode, 2);

	position += 2;

	strcpy(buffer + position, fileName);
	position += fileNameLength+1;

	
	strcpy(buffer + position, transferMode);
	position += strlen(transferMode)+1;

	printf("\nInvio richiesta di download al server\n");
	
	sendBuffer(sock, buffer, position, server_addr);
	

	char file[BUFFER_SIZE];

	FILE* fp;
	if(!strcmp(transferMode, "netascii\0"))
		fp = fopen(localName, "w+");
	else
		fp = fopen(localName, "wb+");

	while(1){	
		char* bufferPacket = '\0';
		// Ricezione del blocco	
		position = 0;
	
		memset(&file, 0, BUFFER_SIZE);
	
		struct result r = receiveBuffer(sock);
	
		bufferPacket = r.buffer;
		int length = r.length;
	
		memcpy(&opcode, bufferPacket, 2);
		opcode = ntohs(opcode);
		
		// Errore
		if(opcode == 5){ 
			memcpy(&errorCode, bufferPacket, 2);
			position += 2;
			char message[BUFFER_SIZE];
			strcpy(message, buffer);
			printf("\nERRORE! (%d) %s", errorCode, message);
			return;
		}

		// Lettura del blocco
		position = 2;
		uint16_t block;
		memcpy(&block, bufferPacket + position, 2);
		
		block = ntohs(block);
		position += 2;
		
		if(!strcmp(transferMode, "netascii\0")){
			strcpy(file, bufferPacket+position);
			fprintf(fp, "%s", file);
		}else{
			memcpy(file, bufferPacket+position, FILE_BUFFER_SIZE);
			fwrite(&file,length, 1 ,fp);
		}
		printf("\nRicezione del blocco %d effettuata\n", block);		

		// Invio dell'ACK		
		position = 0;
		char bufferAck[BUFFER_SIZE];
		memset(bufferAck, 0, BUFFER_SIZE);
		opcode = htons(4);
		block = htons(block);
		memcpy(bufferAck + position, (uint16_t*)&opcode, 2);
		position += 2;
		memcpy(bufferAck + position, (uint16_t*)&block, 2);
		position += 2;
		printf("\nInvio dell'ACK\n");
		sendBuffer(sock, bufferAck, sizeof(bufferAck), server_addr);

		// Fine dei trasferimenti
		if(length<PACKET_SIZE){
			printf("\nTrasferimento del file completato\n");
			fclose(fp);
			break;
		}
	}

}

void quitCmd(int sock){
	close(sock);
	printf("\nDisconnessione effettuata\n");
	exit(0);
}



