#include "client.h"
#include "utility.h"
int main(int argc, char** argv){

	int sock;
	struct sockaddr_in server_addr;
	sleep(1);

	char cmd[CMD_SIZE];


	//pthread_t thread;



	if(argc != 3){
		printf("\nPer avviare il programma digita ./tftp_client <IP_server> <porta_server>\n\n");
		return 0;
	}


	sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
		perror("ERRORE! Connessione non riuscita");
		exit(1);	
	}
	
	char* transferMode = "txt";

	initMessage(sock, argv[1], argv[2]);	
	

	while(1){
		memset(&cmd, 0, CMD_SIZE);
		scanf("%s", cmd);
		sendCommand(sock, cmd);
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
			getCmd(sock, fileName, transferMode, server_addr);
		}else if(!strcmp(cmd, "!quit\0")){
			quitCmd(sock);
		}
	}
}


void initMessage(int sock, const char* server, const char* port){
	printf("Connessione al server %s alla porta %s effettuata con successo\n", server,port);
	helpCmd();
	char cmd[CMD_SIZE];
	memset(&cmd, 0, CMD_SIZE);
	memcpy(cmd,"!connect\0",CMD_SIZE);
	sendCommand(sock,cmd);
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
		currentMode = "netascii\0";
	}else if(!strcmp(mode, "bin")){
		printf("Modo di trasferimento binario configurato.\n");
		currentMode = "octet\0";
	}else{
		printf("ERRORE: Modo di trasferimento non previsto.\n");
	}
 return;	
}

void getCmd(int sock, char* fileName, char* transferMode, struct sockaddr_in server_addr){
	printf("Richiesta file %s al server in corso.\n", fileName);
	char buffer[BUFFER_SIZE];
	memset(&buffer, 0, BUFFER_SIZE);
	int position = 0;
	printf("\nPreparazione del buffer da inviare\n");
	uint16_t errorCode;
	uint16_t opcode = htons(1);
	
	fileName[strlen(fileName)+1]=0;
	uint16_t fileNameLength = strlen(fileName);


	memcpy(buffer, (uint16_t*)&opcode, 2);

	position += 2;

	strcpy(buffer + position, fileName);
	position += fileNameLength+3;
	
	//memcpy(buffer + position, 0, 1);
	//position++;
	
	strcpy(buffer + position, transferMode);
	position += strlen(transferMode)+1;


	//memcpy(buffer + position, 0, 1);
	printf("\nlength buffer %d\n", (int)strlen(buffer));
	printf("\nInvio richiesta di download al server\n");
	sendBuffer(sock, buffer, position, server_addr);
	
	char* file = NULL;
	while(1){	

		// Ricezione del blocco
		char* bufferPacket;
		position = 0;
		memset(&bufferPacket, 0, BUFFER_SIZE);
	
		bufferPacket = receiveBuffer(sock);
		memcpy(&opcode, (uint16_t*)&bufferPacket, 2);

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
		position = 0;
		uint16_t block;
		memcpy(&block, bufferPacket, 2);
		block = ntohs(block);
		position += 2;
		char data[BUFFER_SIZE];
		strcpy(data, bufferPacket);
		strcat(data, file);
		
		printf("\nRicezione del blocco %d effettuata", block);		
		
		// Invio dell'ACK		
		position = 0;
		char bufferAck[BUFFER_SIZE];
		opcode = htons(4);
		block = htons(block);
		memcpy(bufferAck + position, (uint16_t*)&opcode, 2);
		position += 2;
		memcpy(bufferAck + position, (uint16_t*)&block, 2);
		position += 2;
		sendBuffer(sock, bufferAck, position, server_addr);
	
		// Fine dei trasferimenti
		if(strlen(data)<FILE_BUFFER_SIZE){
			printf("\nTrasferimento del file terminato");
			break;
		}
	}

	FILE* fp;
	fp = fopen(fileName, "w+");
	fprintf(fp, "%s", file);
	fclose(fp);

}

void quitCmd(int sock){
	close(sock);
	printf("\nDisconnessione effettuata\n");
	exit(0);
}


void sendCommand(int sock, char* cmd){
	uint16_t length = htons(strlen(cmd)+1);
	if(send(sock, (void*)&length, sizeof(uint16_t),0)<0){
		perror("\nERRORE! Invio della lunghezza del comando non riuscita");
		return;
	}
	

	if(send(sock, (void*)cmd, strlen(cmd)+1, 0) < 0){
		perror("\nERRORE! Invio del comando non riuscito");
		return;
	}
}	



