// C program to implement one side of FIFO
// This side reads first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <time.h>
#include <time.h>
#include <sys/prctl.h> /* prctl(), PR_SET_NAME */

#define FILE_PFC1 "tmp/PFC1"
#define FILE_PFC3 "tmp/PFC3.txt"
#define PORT 8080
#define BUFFER_SIZE 100

char fileNamePFC1[] = "log/speedPFC1.log";
char fileNamePFC2[] = "log/speedPFC2.log";
char fileNamePFC3[] = "log/speedPFC3.log";

/* Il metodo writtePFClog srcive nel path la stringa speed.
 *
 */
void writtePFClog(char path[], char speed[]) {

	FILE *ftp = fopen(path, "a");
	if (ftp != NULL) {
		fputs(speed, ftp);
		fputs("\n", ftp);
		fflush(ftp);
		fclose(ftp);
	}
}
/*il metodo readPFC1 avvia la Pipe effettua i passi necessari e legge in continuazione,
 * Quando la lettura ha successo scrive in speedPFC2.log il contenuto.
 */

void readPFC1(char path[]) {

	int fd1;
	char speed[100];
	int check = 0;

	// Creating the named file(FIFO)
	// mkfifo(<pathname>,<permission>)
	mkfifo(path, 0666);

	// First open in read only and read
	fd1 = open(path, O_RDONLY | O_NONBLOCK);

	fflush(stdout);
	if (fd1 == -1) {

		printf("PIPE in Lettura  non trovata nel percorso: %s\n", path);
		fflush(stdout);
	}
	for (;;) {

		check = read(fd1, speed, BUFFER_SIZE);
		if (check == BUFFER_SIZE) {

			// Print the read string and close
			printf("   Transucer(PFC1)=> %s \n", speed);
			writtePFClog(fileNamePFC1, speed);
			fflush(stdout);
		}
	}
	close(fd1);
}
/*il metodo readPFC2 avvia la Socket effettua i passi necessari e si mette in attesa
 * di un client e rimane in questo stato.
 * Quando un client comunica una stinga che viene scritta in speedPFC2.log.
 */
void readPFC2() {
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE];

// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
			sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT);

// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr*) &address,(socklen_t*) &addrlen)) < 0) {
			perror("accept");
			close(new_socket);
			exit(EXIT_FAILURE);
		}

		read(new_socket, buffer, 1024);

		if (!(buffer[0] == '\0' && buffer[1] == '\0')) {

			printf("   Transucer(PFC2)=> %s \n", buffer);
			writtePFClog(fileNamePFC2, buffer);

			memset(buffer, 0, 100);
		}

		close(new_socket);
	}
}
/*
 * Il meotodo readPFC3 preleva nel file path il contenuto lo scrive in speedPFC3.
 * Rilpulisce il file per la prossima lettura se ha successo.
 */

void readPFC3(char path[]) {

	char buffer[BUFFER_SIZE];
	FILE * fPtr = fopen(path, "r");

	if (fgets(buffer, BUFFER_SIZE, fPtr) != 0) {

		printf("   Transucer(PFC3)=> %s \n", buffer);
		fputs(buffer, fPtr);
		fflush(fPtr);
		fclose(fPtr);
		fflush(stdout);
		writtePFClog(fileNamePFC3, buffer);

		fclose(fopen(path, "w"));
	} else {
		fclose(fPtr);
	}
}
/*Il metodo processTransducer genera tre processi che ricevono i dati che ogni PFC
 * comunica ogni secondo.
 * PFC1 legge dalla pipe (in loop).
 * PFC2 legge dalla socket(in loop).
 * PFC3 legge dal fle txt (ogni 1/10 di secondo).
 */

void processTransducer() {

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "TransducerPFC2", 0, 0, 0);
		readPFC2();

	}

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "TransducerPFC1", 0, 0, 0);
		readPFC1(FILE_PFC1);
	}

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "TransducerPFC3", 0, 0, 0);

		for (;;) {
			readPFC3(FILE_PFC3);

		}
	}
}

