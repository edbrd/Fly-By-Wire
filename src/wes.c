/*
 * wes.c
 *
 *  Created on: 15 dic 2022
 *      Author: edbrd
 */

#include<time.h>
#include <sys/prctl.h> /* prctl(), PR_SET_NAME */
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define MSG_ERROR "ERRORE"
#define MSG_EMERGENZA "EMERGENZA"
#define MSG_OK "OK"

/*Il metodo getLastSpeed
 * Alloca una stringa di 100 posizioni, preleva l'ultima linea del file path.
 *
 */
char * getLastSpeed(char path[]) {

	FILE *ptf = fopen(path, "r");
	char tmp[100];
	char *buffer = calloc(100, sizeof(char));
	int t = -10;
	char c;
	do {
		fseek(ptf, t, SEEK_END);
		t--;
		c = fgetc(ptf);
	} while (c != '\n');

	fgets(tmp, 100, ptf);
	strcpy(buffer, tmp);
	fclose(ptf);
	return buffer;
}

/*
 * Il metodo getStringAtPosition restituiscre il contenuto
 * delll'enesima porzione della stringa.
 * es:   byffer["pos1,pos2,pos3"], postion=0 =>"pos1"
 */

char *getStringAtPosition(char buffer[], int position) {

	char *sSpit = calloc(100, sizeof(char));
	int iesima = position;

	char string[100];
	strcpy(string, buffer);
	// Extract the first token
	char * token = strtok(string, ",");
	// loop through the string to extract all other tokens
	while (token != NULL && iesima > 0) {
		iesima--;
		token = strtok(NULL, ",");
		strcpy(sSpit, token);
	}

	return sSpit;
}

/* Il metodo writteInStatus srcive nel path la stringa signal.
 *
 */
void writteInStatus(const char path[], const char signal[]) {

	FILE *ftp = fopen(path, "a");
	fputs(signal, ftp);
	fputs("\n", ftp);
	fflush(ftp);
	fclose(ftp);

}

/*
 * il metodo checkSpeed preleva le ultime linee del file speedPFC.
 * confronta i risultati delle pfc per il messaggio Errore, Emergenza o OK.
 *
 * Ritorna:
 * 1=PROCESSO DISCORDE PFC1
 * 2=PROCESSO DISCORDE PFC2
 * 3=PROCESSO DISCORDE PFC3
 * 0= OK
 * -1= EMERGENZA
 */

int checkSpeed(char pathSpeedPCF1[], char pathSpeedPCF2[], char pathSpeedPCF3[]) {

	char tsr[100];
	int nPFC = 0;
	char *speedPFC1 = 0, *speedPFC2 = 0, *speedPFC3 = 0;

	speedPFC1 = getLastSpeed(pathSpeedPCF1);
	speedPFC2 = getLastSpeed(pathSpeedPCF2);
	speedPFC3 = getLastSpeed(pathSpeedPCF3);

	int S1eqS2 = strcmp(speedPFC1, speedPFC2);
	int S1eqS3 = strcmp(speedPFC1, speedPFC3);
	int S2eqS3 = strcmp(speedPFC2, speedPFC3);

	//se sono tutti uguali
	if ((S1eqS2 == 0) && (S1eqS3 == 0) && (S2eqS3 == 0)) {

		sprintf(tsr, "Segnale:%s", MSG_OK);

		//almeno due sono uguali
	} else if ((S1eqS2 == 0) | (S1eqS3 == 0) | (S2eqS3 == 0)) {

		if (S1eqS2 == 0) {
			sprintf(tsr, "Segnale,%s,Processo discorde,PFC3", MSG_ERROR);
			nPFC = 3;

		}

		if (S2eqS3 == 0) {
			sprintf(tsr, "Segnale,%s,Processo discorde,PFC1", MSG_ERROR);
			nPFC = 1;

		}
		if (S1eqS3 == 0) {
			sprintf(tsr, "Segnale,%s,Processo discorde,PFC2", MSG_ERROR);
			nPFC = 2;
		}

	}

	//tutte le stringhe sono diverse
	if ((S1eqS2 != 0) && (S1eqS3 != 0) && (S2eqS3 != 0)) {
		sprintf(tsr, "Segnale,%s", MSG_EMERGENZA);
		nPFC = -1;

	}

	writteInStatus("log/status.log", tsr);

	fflush(stdout);
	free(speedPFC1);
	free(speedPFC2);
	free(speedPFC3);

	return nPFC;
}

void processWes() {

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "Wes", 0, 0, 0);

		for (;;) {

			system("killall -SIGSTOP Wes");

			int signal = checkSpeed("log/speedPFC1.log", "log/speedPFC2.log",
					"log/speedPFC3.log");

			switch (signal) {
			case 1:
				printf("   Segnale(Wes)=> [ ERRORE PFC1 ]\n\n\n");
				break;
			case 2:
				printf("   Segnale(Wes)=> [ ERRORE PFC2 ]\n\n\n");
				break;
			case 3:
				printf("   Segnale(Wes)=> [ ERRORE PFC1 ]\n\n\n");
				break;
			case 0:
				printf("   Segnale(Wes)=>[ OK ]\n\n\n");
				break;
			case -1:
				printf("   Segnale(Wes)=> [ EMERGENZA ]\n\n\n");
				break;
			}

			fflush(stdout);
		}
	}
}
