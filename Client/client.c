#include "client.h"

int main(int argc, char** argv){

	int sock;
	struct sockaddr_in server_addr;
	sleep(1);

	char cmd[CMD_SIZE];
	char* command = NULL;


	pthread_t thread;



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

	initMessage(argv[1], argv[2]);	
	

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
			char* fileName = argv[1];
			getCmd(sock, server_addr, fileName);
		}else if(!strcmp(cmd, "!quit\0")){
			quitCmd(sock);
		}
	}
}


void initMessage(const char* server, const char* port){
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
	}else if(!strcmp(mode, "bin")){
		printf("Modo di trasferimento binario configurato.\n");
	}else{
		printf("ERRORE: Modo di trasferimento non previsto.\n");
		return;
	}
 
	currentMode = mode;
	
}

void getCmd(int sock, struct sockaddr_in server_addr, char* fileName){
	printf("Richiesta file %s al server in corso.\n", fileName);
	char buf[BUFFER_SIZE];

	printf("Trasferimento del file in corso.\n");
	int len = sendto(sock, buf, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

	printf("Trasferimento completato.\n");

	printf("Salvataggio %s completato.\n", fileName);

}

void quitCmd(int sock){
	close(sock);
	printf("Disconnessione effettuata\n");
	exit(0);
}




