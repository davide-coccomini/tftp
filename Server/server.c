#include "server.h"
#include "utility.h"



int main(int argc, char** argv){
	

	if(argc != 3){
		printf("\nPer avviare il programma digita ./tftp_server <porta> <directory files>\n\n");
		return 0;
	}
	
	char* directory = argv[2]; // Directory dei files


	fd_set master;
	fd_set read_fds;
	int fdmax;
	


	int listener, new_sock;

	struct sockaddr_in my_addr;
	struct sockaddr_in client_addr;
	int addrlen = sizeof(client_addr);
	char buf[BUFFER_SIZE];
	int nbytes;
	int i;
	int newfd;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);


	printf("\nCreazione socket di ascolto\n");
	listener = socket(AF_INET, SOCK_STREAM,0);
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(argv[1]));
	my_addr.sin_addr.s_addr = INADDR_ANY;
	printf("\nSocket di ascolto creato\n");

	if(bind(listener, (struct sockaddr*)&my_addr, sizeof(my_addr))<0){
		perror("ERRORE: Bind non riuscito");
		return -1;
	}
	
	if(listen(listener, 10) < 0){
		perror("ERRORE: Listen non riuscita");
		return -1;
	}
	FD_SET(listener, &master);
	fdmax = listener;


	for(;;){
		
		// (1) Arriva connessione
		// (2) Ricevi modalità
		// (3) Controlli presenza file ed eventualmente mandi errore
		// (4) Converti file nella modalità richiesta se necessario
		// (5) Invii file
		// (6) Ricevi l'ACK ed eventualmente stampi messaggio di errore
		read_fds = master;	
		select(fdmax +1, &read_fds, NULL, NULL, NULL);
		for(i=0; i<=fdmax; i++){
		 if(FD_ISSET(i, &read_fds)){
			if(i==listener){ // Il descrittore pronto è il listener
				addrlen = sizeof(client_addr);
				newfd = accept(listener, (struct sockaddr *)&client_addr, &addrlen);
				FD_SET(newfd, &master);
				if(newfd > fdmax){
					fdmax = newfd;
				}
			}else{ // Il descrittore pronto è un altro socket
				printf("\nConnessione in ingresso accettata\n");
				memset(buffer, 0, BUFFER_SIZE);
				cmd(i, buffer, &master);

				if(!strcmp(buffer, "!help\0")){
					printf("Ricezione comando help");
				}else if(!strcmp(buffer, "!mode\0")){
					printf("Ricezione comando mode");
				}else if(!strcmp(buffer, "!get\0")){
					printf("Ricezione comando get");
				}else if(!strcmp(buffer, "!quit\0")){
					printf("Ricezione comando quit");
				}
				
				nbytes = recvfrom(i, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &addrlen);
				close(i);
				FD_CLR(i, &master);
			}
		 }
		}	
	}

}
void cmd(int sock, char* buffer, fd_set* master){
	uint16_t length;
	int ret;

	memset(&length, 0, sizeof(uint16_t));

	ret = recv(sock, (void*)&length, sizeof(uint16_t),0);

	if(ret < 0){
		perror("ERRORE! Ricezione della lunghezza del comando non riuscita");
		return;
	}else if(!ret){
	
		
	}
	if(recv(sock, (void*)buffer, ntohs(length),0) < 0){
		perror("ERRORE! Ricezione del comando non riuscita");
	}

	return;
}
