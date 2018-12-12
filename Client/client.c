#include "client.h"

int main(int argc, char** argv){

	int sock;
	struct sockaddr_in serverAddr;
	sleep(1);

	char* cmd = malloc(CMD_SIZE);
	char* command = NULL;


	pthread_t thread;
	struct thread_args args;



	if(argc != 3){
		printf("Per avviare il programma digita ./tftp_client <IP_server> <porta_server>");
		return 0;
	}


	sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serverAddr, 0, sizeof(svAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[2]]));
	
	inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
	
	if(connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)<0)){
		perror("ERRORE: Connessione non riuscita");
		exit(1);	
	}
	
	char* transferMode = "txt";

	initMessage(argv[1], argv[2]);	
	

	while(1){
		memset(&cmd, 0, CMD_SIZE);
		SCANF("%s", cmd);
	
		if(strcmp(cmd, "!help\0")){
			helpCmd();
		}else if(strcmp(cmd, "!mode\0")){
			char* mode = argv[1];
			modeCmd(mode);
		}else if(strcmp(cmd, "!get\0")){
			char* fileName = argv[1];
			getCmd(fileName);
		}else if(strcmp(cmd, "!quit\0")){
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

void modeCmd(char* mode){

	if(strcmp(mode, "txt")){	
		printf("Modo di trasferimento testuale configurato.");
	}else if(strcmp(mode, "bin")){
		printf("Modo di trasferimento binario configurato.");
	}else{
		perror("ERRORE: Modo di trasferimento non previsto.");
		return;	
	}
 
	transferMode = mode;
	
}

void getCmd(char* fileName){
	printf("Richiesta file %s al server in corso.", fileName);
	struct sockaddr_in myAddr;
	int sock;
	char* buffer;

	


	printf("Trasferimento del file in corso");

	printf("Trasferimento completato");

	printf("Salvataggio %s completato", fileName);

}

void quitCmd(int sock){
	close(sock);
	printf("Disconnessione effettuata\n");
	exit(0);
}


/*
void getCommand(const char* cmd, char** arg_cmd){
	char *temp;
	*(strchr(cmd, '\n') = '\0');
	temp = strchr(cmd, ' ');
	if(temp)
		*temp = '\0';
	else
		return;

	if(!(int)strlen(temp+1) || strchr(temp+1, ' ')){
		return;	
	}
	
}
*/


