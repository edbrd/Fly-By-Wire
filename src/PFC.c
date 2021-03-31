#include <time.h>
#include <stdio.h>
#include <sys/prctl.h> /* prctl(), PR_SET_NAME */
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "getsG18.h"
#define FILE_LASTS "tmp/lasts.txt"
#define FILE_PFC1 "tmp/PFC1"
#define FILE_PFC3 "tmp/PFC3.txt"
#define FILE_SWITCH "log/switch.log"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "getSetLasts.h"
#define S_SPEED "Velocità,"
#define S_DISTANCE "Distanza,"
#define S_ROW "Riga,"
#define BUFFER_SIZE 100
#define PORT 8080

int shiftPFC = 1;

/*
 * Il metodo writtePFC1 crea se non esiste la PIPE nella PATH e scrive linput.
 * INPUT: path("..../file"), input (stringa).
 */
void writtePFC1(char path[], int row, double speed, double distance) {

	int fd;
	char speedToWritte[BUFFER_SIZE];

	snprintf(speedToWritte, 100, "%s%d,%s%f[m/s]", S_ROW, row, S_SPEED, speed);
	// Creating the named file(FIFO)
	// mkfifo(<pathname>, <permission>)
	mkfifo(path, 0666);

	// Open FIFO for write only
	fd = open(path, O_WRONLY);

	if (fd == -1) {

		printf("PIPE in SCRITTURA  non trovata nel percorso: %s\n", path);
	} else {

		// Write the input arr2ing on FIFO
		// and close it
		write(fd, speedToWritte, 100);
		fflush(stdout);
	}

	close(fd);

}
/*il metodo writtePFC3 prende input riga, velocità, distanza.
 * Crea un Buffer e scrive le informazioni ricevute, dopodiché
 * prova a connettersi alla socket finche non ha scritto le info del Buffer.
 */
void writtePFC2(int row, double speed, double distance) {

	int sock = 0;
	struct sockaddr_in serv_addr;
	char speedToWritte[100];

	snprintf(speedToWritte, 100, "%s%d,%s%f[m/s]", S_ROW, row, S_SPEED, speed);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");

	}

	int n = 0;		//if n==1 sent else if n==0 try to resend.
	while (n == 0) {
		sock = socket(AF_INET, SOCK_STREAM, 0);

		if (!(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))
				< 0)) {
			//success
			n = 1;
			fflush(stdout);
			send(sock, speedToWritte, strlen(speedToWritte) + 1, 0);
			fflush(stdout);

		}
	}
	close(sock);

}

/*il metodo writtePFC3 prende input riga, velocità, distanza.
 * Crea un Buffer e scrive le informazioni ricevute, dopodiché le scrive file PFC3.txt(path).
 */
void writtePFC3(char *path, int row, double speed, double distance) {

	char speedToWritte[BUFFER_SIZE];

	FILE *fPtr = fopen(path, "w");

	// Effettuo la concatenazione in stringa di S_DISTANCE+distance+\n
	snprintf(speedToWritte, 100, "%s%d,%s%f[m/s]", S_ROW, row, S_SPEED, speed);
	fputs(speedToWritte, fPtr);
	fclose(fPtr);

}

/*
 * il meotodo shiftSpeedSg e il handler che implementano le pfc per il segnale SIGUSR1.
 * Efettua la modificad della variabile globale shiftPFC, per lo shift della velocita .
 */

void shiftSpeedSg(int signum) {

	shiftPFC = -1;
}

/*
 * I metodi processPFC per essere invocati hanno bisogno in input della direcotry di G18.txt
 * da dove prelevano le coordinate dalla linea $GPGLL.
 * Ognuno dei processi generati implementano uno Handler per il segnale SIGUSR per lo shif a sinistra di due bit.
 * Ogni secondo il processo PFC preleva i dati da lasts.txt e G18.txt.
 * Effettua i calcoli della distanza fra il punto dell'istante precedente e l'attuale.
 * Calcola la distanza (in metri) e la velocità (in metri/sec).
 * Comunica al transducer i valori calcolati e salva il necessario per il prossimo secondo.
*/

void processPFC1(char pathG18S[]) {

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "PFC1", 0, 0, 0);

		double lastLat = 0, lastLon = 0;
		int lastRow = 0;
		signal(SIGUSR1, shiftSpeedSg);

		while (1) {

			int row = getLasts(FILE_LASTS);

			if (row != lastRow) {

				int row = getLasts(FILE_LASTS);
				char *tmpLinePFC1 = getLine(row, pathG18S);
				double lat = getValueAtPosition(tmpLinePFC1, ",", 1) / 100;
				double lon = getValueAtPosition(tmpLinePFC1, ",", 3) / 100;
				double distance = distanceGeo(lat, lon, lastLat, lastLon);
				double speed = getSpeed(distance, shiftPFC);
				//salvo i dati per la prssima chiamata
				lastLat = lat;	lastLon = lon;	lastRow = row;	shiftPFC += 1;
				free(tmpLinePFC1);
				writtePFC1(FILE_PFC1, row, speed, distance);
			}
			sleep(1);
		}
	}
}

void processPFC2(char pathG18S[]) {

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "PFC2", 0, 0, 0);
		double lastLat = 0, lastLon = 0;
		int lastRow = 0;
		signal(SIGUSR1, shiftSpeedSg);

		while (1) {

			int row = getLasts(FILE_LASTS);

			if (row != lastRow) {

				int row = getLasts(FILE_LASTS);
				char *tmpL = getLine(row, pathG18S);
				double lat = getValueAtPosition(tmpL, ",", 1) / 100;
				double lon = getValueAtPosition(tmpL, ",", 3) / 100;
				double distance = distanceGeo(lat, lon, lastLat, lastLon);
				double speed = getSpeed(distance, shiftPFC);
				//salvo i dati per la prssima chiamata
				lastLat = lat;	lastLon = lon;	lastRow = row;	shiftPFC += 1;
				free(tmpL);
				writtePFC2(row, speed, distance);
			}
			sleep(1);
		}
	}
}

void processPFC3(char pathG18S[]) {

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "PFC3", 0, 0, 0);
		double lastLat = 0, lastLon = 0;
		int lastRow = 0;
		signal(SIGUSR1, shiftSpeedSg);

		while (1) {

			int row = getLasts(FILE_LASTS);

			if (row != lastRow) {

				char *tmpLPFC3 = getLine(row, pathG18S);
				double lat = getValueAtPosition(tmpLPFC3, ",", 1) / 100;
				double lon = getValueAtPosition(tmpLPFC3, ",", 3) / 100;
				double distance = distanceGeo(lat, lon, lastLat, lastLon);
				double speed = getSpeed(distance, shiftPFC);
				//salvo i dati per la prssima chiamata
				lastLat = lat;	lastLon = lon;	lastRow = row;
				shiftPFC += 1;
				free(tmpLPFC3);
				writtePFC3(FILE_PFC3, row, speed, distance);
			}
			sleep(1);
		}
	}
}
