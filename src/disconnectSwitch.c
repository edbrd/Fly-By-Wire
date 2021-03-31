/*
 * disconnectSwitch.c
 *
 *  Created on: 5 gen 2021
 *      Author: edbrd
 */
#include <sys/prctl.h> /* prctl(), PR_SET_NAME */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<time.h>
#define FILE_FAILURES "log/failures.log"
#define FILE_SWITCH "log/switch.log"
#include "PFC.h"

/*
 * il morodo getSTATE_PID prende in input il nome del processo.
 * Efettua un comando di sistema per prelevare il PID del processo
 * a cui corrisponde namePFC.
 */

int getPID_COMM(char namePFC[]) {

	char pidline[100];
	char * token;
	FILE *fp = popen("ps -eo comm,pid,state", "r");

	while (fgets(pidline, 100, fp) != NULL) {

		int m = strstr(pidline, "defunct") != NULL;
		token = strtok(pidline, " ");
		int n = strcmp(token, namePFC) == 0;

		if (n == 1 && !(m == 1)) {
			token = strtok(NULL, " ");
			break;
		}
		fflush(stdout);
	}

	pclose(fp);
	return atoi(token);
}

/*
 * il morodo getSTATE_PID prende in input il PID di un processo.
 * Efettua un comando di sistema per prelevare lo stato del processo.
 * Stati possibili:
 * X=il processo non e' presnete nel sistema .
 * T=il processo di trova in uno stato di stop.
 * S=il processo di trova in uno stato di sleep.
 * Z=il processo di trova in uno stato di stop
 */
char getSTATE_PID(int pid) {

	char pidline[100];
	char state = 'X';
	char command[50];

	if (pid > 0) {
		snprintf(command, 50, "cat /proc/%d/status | grep State:", pid);

		FILE *fp = popen(command, "r");

		if (fgets(pidline, 100, fp) > 0) {

			state = pidline[7];
		}

		pclose(fp);
	}
	return state;
}
/*
 * Il metodo getRandPID_PFC resisituisce il PID di uno dei
 * tre processi PFC, scegliendolo in modo casuale.
 */

int getRandPID_PFC() {

	time_t t;
	srand((unsigned) time(&t));

	int selectPFC = 1 + rand() % 3;
	char PFCselected[5];

	switch (selectPFC) {

	case 1:
		strcpy(PFCselected, "PFC1");
		break;
	case 2:
		strcpy(PFCselected, "PFC2");
		break;
	case 3:
		strcpy(PFCselected, "PFC3");
		break;
	}

	return (getPID_COMM(PFCselected));
}

/* Il metodo writteInfFlogFail prende in input la srtinga
 * e il path file.
 * Scrive in nel file la stringa signal.
 *
 */
void writteInfFlogFail(char signal[], char path[]) {

	if (strlen(signal) > 0) {

		FILE *ftp;

		ftp = fopen(path, "a");
		fputs(signal, ftp);
		fputs("\n", ftp);
		fflush(ftp);
		fclose(ftp);
	}

}

int emergenceCase(char path[]) {

	int result = 1;
	FILE *ptf;
	char tmp[100];
	int t = -5;
	char c;

	do {
		ptf = fopen(path, "r");
	} while (ptf == NULL);

	do {
		fseek(ptf, t, SEEK_END);
		t--;
		c = fgetc(ptf);
	} while (c != '\n');

	fgets(tmp, 100, ptf);
	fclose(ptf);
	if (strstr(tmp, "EMERGENZA") != NULL) {

		fflush(stdout);
		result = 0;
	}

	return result;
}

/*
 * Il metodo sendRandomSignal calcola le 4 probabilita.
 * Se la probabilita si verfica, invia il segnale corrispondente,
 * Stampa nella console il segnale generato e lo scrive nel file dei Falimenti.
 *
 *
 */

void sendRandomSignal(int pid) {

	time_t t;
	srand((unsigned) time(&t));

	int probSIGSTOP = 1 + rand() % 100;
	if (probSIGSTOP == 1) {
		kill(pid, SIGSTOP);
		printf("   Gen Signal (Random) => SIGSTOP\n");
		fflush(stdout);
		writteInfFlogFail("SIGSTOP", FILE_FAILURES);
	}

	int probSIGINT = 1 + rand() % 10000;
	if (probSIGINT == 1) {
		kill(pid, SIGINT);
		printf("   Gen Signal (Random) => SIGINT\n");
		fflush(stdout);
		writteInfFlogFail("SIGINT", FILE_FAILURES);
	}

	int probSIGCONT = 1 + rand() % 10;
	if (probSIGCONT == 1) {
		kill(pid, SIGCONT);
		printf("   Gen Signal (Random) => SIGCONT\n");
		fflush(stdout);
		writteInfFlogFail("SIGCONT", FILE_FAILURES);
	}

	int probSIGUSR1 = 1 + rand() % 10;
	if (probSIGUSR1 == 1) {
		kill(pid, SIGUSR1);
		printf("   Gen Signal (Random) => SIGUSR1\n");
		fflush(stdout);
		writteInfFlogFail("SIGUSR1", FILE_FAILURES);
	}

}

void checkPFC(char statePFC[3], char pathG18S[], char bfSG[30], int pfc1,
		int pfc2, int pfc3) {

	for (int t = 0; t < strlen(statePFC); t++) {
		if (statePFC[t] == 'X') {

			if (t == 0) {
				sleep(1);
				processPFC1(pathG18S);
				sprintf(bfSG, "   Riavvia PFC%d", (t + 1));
				writteInfFlogFail(bfSG, FILE_SWITCH);
				printf("   DisconnectSwitch => %s\n", bfSG);
				fflush(stdout);
			}
			if (t == 1) {
				sleep(1);
				processPFC2(pathG18S);
				sprintf(bfSG, "   Riavvia PFC%d", (t + 1));
				writteInfFlogFail(bfSG, FILE_SWITCH);
				printf("   DisconnectSwitch => %s\n", bfSG);
				fflush(stdout);
			}
			if (t == 2) {
				sleep(1);
				processPFC3(pathG18S);
				sprintf(bfSG, "   Riavvia PFC%d", (t + 1));
				writteInfFlogFail(bfSG, FILE_SWITCH);
				printf("   DisconnectSwitch => %s\n", bfSG);
				fflush(stdout);
			}
		}
		if (statePFC[t] == 'T') {

			if (t == 0) {
				sleep(1);
				system("killall -SIGCONT PFC1"),
				sprintf(bfSG, "   Ricontinua PFC%d", (t + 1));
				writteInfFlogFail(bfSG, FILE_SWITCH);
				printf("   DisconnectSwitch => %s\n", bfSG);
				fflush(stdout);
			}
			if (t == 1) {
				sleep(1);
				system("killall -SIGCONT PFC2"),
				sprintf(bfSG, "   Ricontinua PFC%d", (t + 1));
				writteInfFlogFail(bfSG, FILE_SWITCH);
				printf("   DisconnectSwitch => %s\n", bfSG);
				fflush(stdout);
			}
			if (t == 2) {
				sleep(1);
				system("killall -SIGCONT PFC3"),
				sprintf(bfSG, "   Ricontinua PFC%d", (t + 1));
				writteInfFlogFail(bfSG, FILE_SWITCH);
				printf("   DisconnectSwitch => %s\n", bfSG);
				fflush(stdout);
			}
		}
	}
}

/*
 * il metodo disconnectSwitch genera il processo disconnectSwitch.
 * ogni due decimi il processo controlla lo stato dei processi PFC
 * se si trovano in uno stato X, lo riavvio perche e uscito
 * se si trova in uno stato T, mando un segnale SIGCONT perche
 * si trova in uno stato di stop.
 * Inoltre calcola il tempo di sleep = 1sec -(tempo di esecuzione).
 */
void PFCdisconnectSwitch(char pathG18S[]) {
	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "PFCdisconSwitch", 0, 0, 0);
		char bfSG[30];
		sleep(1);

		for (;;) {
			char statePFC[3];

			int pfc1 = (getPID_COMM("PFC1"));
			int pfc2 = (getPID_COMM("PFC2"));
			int pfc3 = (getPID_COMM("PFC3"));

			statePFC[0] = getSTATE_PID((pfc1));
			statePFC[1] = getSTATE_PID((pfc2));
			statePFC[2] = getSTATE_PID((pfc3));

			int em = emergenceCase("log/status.log");

			if ((em == 0)) {
				printf(
						"\n   EMERGENZA catturata, l'applicazzzione riparte fra 5 secondi...\n\n");

				system("killall -SIGSTOP NextLine");
				system("killall -SIGSTOP Wes");
				system("killall -SIGSTOP GenFailures");
				system("killall -SIGTERM PFC1");
				system("killall -SIGTERM PFC2");
				system("killall -SIGTERM PFC3");

				sleep(5);

				processPFC1(pathG18S);
				processPFC2(pathG18S);
				processPFC3(pathG18S);
				//sleep(1);
				system("killall -SIGCONT NextLine");
				system("killall -SIGCONT Wes");
				system("killall -SIGCONT GenFailures");

			} else {

				checkPFC(statePFC, pathG18S, bfSG, pfc1, pfc2, pfc3);
			}

		}
	}
}

/*
 * Il metodo processGenFailures genera il processo "GenFailures"
 * Randomicamente seleziona una PFC e invia il segnale se la probabilita si verifica.
 * Inoltre calcola il tempo di sleep = 1sec -(tempo di esecuzione).
 */

void processGenFailures() {

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "GenFailures", 0, 0, 0);
		for (;;) {

			system("killall -SIGSTOP GenFailures");

			int t = getRandPID_PFC();
			if (t > 0) {
				sendRandomSignal(t);
			} else {
				fflush(stdout);
			}

		}
	}
}

