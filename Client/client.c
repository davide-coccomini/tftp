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

			printf("FILENAME: %s", fileName);
			getCmd(sock, fileName, transferMode);
			printf("wewewew");
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
	if(!strcmp(mode, "netascii")){	
		printf("Modo di trasferimento testuale configurato.\n");
	}else if(!strcmp(mode, "octet")){
		printf("Modo di trasferimento binario configurato.\n");
	}else{
		printf("ERRORE: Modo di trasferimento non previsto.\n");
		return;
	}
 
	currentMode = mode;
	
}

void getCmd(int sock, char* fileName, char* transferMode){
	printf("Richiesta file %s al server in corso.\n", fileName);
	char buf[BUFFER_SIZE];

	printf("\nPreparazione del buffer da inviare\n");
	uint16_t errorCode;
	uint16_t opcode = htons(1);
	
	uint16_t fileNameLength = strlen(fileName)+1;
	char* bufferCopy = buffer;

	memcpy(buffer, (uint16_t*)&opcode, 2);
	buffer += 2;

	strcpy(buffer, fileName);
	buffer += fileNameLength;

	memcpy(buffer, 0, 1);
	buffer++;

	strcpy(buffer, &transferMode);
	buffer += 3;

	memcpy(buffer, 0, 1);
		
	buffer = bufferCopy;
	printf("\nInvio richiesta di download al server\n");
	sendBuffer(sock, buffer);
	
	char* file = NULL;
	while(1){	
		// Ricezione del blocco
		memset(&buffer, 0, BUFFER_SIZE);
		buffer = receiveBuffer(sock);
		memcpy(opcode, (uint16_t*)&buffer, 2);
		buffer += 2;
		opcode = ntohs(opcode);

		// Errore
		if(opcode == 5){ 
			memcpy(errorCode, buffer);
			buffer += 2;
			char* message;
			strcpy(message, buffer);
			printf("\nERRORE! (%d) %s", errorCode, message);
			return;
		}
	
		// Lettura del blocco
		uint16_t block;
		memcpy(block, buffer);
		block = ntohs(block);
		buffer += 2;
		char* data;
		strcpy(data, buffer);
		strcat(data, file);
		
		printf("\nRicezione del blocco %d effettuata", block);		
		
		// Invio dell'ACK		
		char bufferAck[BUFFER_SIZE];
		char* bufferCopy = bufferAck;
		opcode = htons(4);
		block = htons(block);
		memcpy(bufferAck, (uint16_t*)&opcode, 2);
		bufferAck += 2;
		memcpy(bufferAck, (uint16_t*)&block, 2);
		bufferAck += 2;
		sendBuffer(bufferCopy);
	
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



