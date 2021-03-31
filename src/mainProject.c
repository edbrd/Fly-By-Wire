/*
 ============================================================================
 Name        : mainProject.c
 Author      : Edmond Bardeli
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <time.h>
#include <stdio.h>
#include <sys/prctl.h> /* prctl(), PR_SET_NAME */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "getsG18.h"
#include "getSetLasts.h"
#include "PFC.h"
#include "transducer.h"
#include "wes.h"
#include "disconnectSwitch.h"
#define FILE_LASTS "tmp/lasts.txt"
#define FILE_PFC1 "tmp/PFC1"
#define FILE_PFC3 "tmp/PFC3.txt"
#define FILE_SWITCH "log/switch.log"

void presetFile();
void processNextLine();
void killAllProcess();
void waitEnd();

/*
 * Il metodo processNextLine genera il processo NextLine che incrementa il
 * numero dalla riga leggere da parte delle PFC.
 * Ogni secondo incrementa il valore, inoltre calcola il tempo impiegazo
 * ad esegure le operazioni da sottrarre al a un secondo.
 */
void processNextLine() {

	if (fork() == 0) {

		prctl(PR_SET_NAME, (unsigned long) "NextLine", 0, 0, 0);

		for (;;) {

			sleep(1);
			system("killall -SIGCONT Wes");
			system("killall -SIGCONT GenFailures");
			setLasts(FILE_LASTS, (getLasts(FILE_LASTS) + 1));

		}
	}

}

void presetFile() {
	mkdir("tmp", 0777);
	unlink("log/speedPFC1.log");
	unlink("log/speedPFC2.log");
	unlink("log/speedPFC3.log");
	unlink("log/status.log");
	unlink("tmp/signalHistory.txt");
	unlink(FILE_SWITCH);
	fclose(fopen("tmp/PFC3.txt", "w"));
	unlink("log/failures.log");
	unlink("log/status.log");

}
void killAllProcess() {

	// Chiusura dei precessi genereati.
	system("killall -SIGTERM NextLine");
	system("killall -SIGTERM PFCdisconSwitch");
	system("killall -SIGTERM PFC1");
	system("killall -SIGTERM PFC2");
	system("killall -SIGTERM PFC3");
	system("killall -SIGTERM TransducerPFC1");
	system("killall -SIGTERM TransducerPFC2");
	system("killall -SIGTERM TransducerPFC3");
	system("killall -SIGTERM Wes");
	system("killall -SIGTERM GenFailures");

	//--------Clear FILE tmp--------
	unlink("tmp/lasts.txt");
	unlink("tmp/PFC1");
	unlink("tmp/PFC3.txt");
	rmdir("tmp");
}

int setFristContRow() {
	int n = 1;
	//inserire la riga da cui iniziare a leggere (fra ).
	do {
		printf(
				"\nInserire il numero (n>0) della riga $GPGLL da cui iniziare a leggere: ");
		scanf("%d", &n);
	} while (n < 1);
	printf("\n\n");
	fflush(stdout);
	return n;
}

/*
 * Il meotdo wiatEnd verifica lo stato delle PFC.
 * in caso in cui tutte le PFC sono terminate
 * e non siamo in uno stato di mermegenza termina il loop.
 */
void waitEnd() {

	sleep(1);
	int emergency = 1;
	int alivePFC1 = 1;
	int alivePFC2 = 1;
	int alivePFC3 = 1;
	int br = 1;
	while (br) {
		alivePFC1 = getPID_COMM("PFC1");
		alivePFC2 = getPID_COMM("PFC2");
		alivePFC3 = getPID_COMM("PFC3");
		emergency = emergenceCase("log/status.log");
		if (((alivePFC1 == 0) && (alivePFC2 == 0) && (alivePFC3 == 0))
				&& (emergency == 1)) {
			br = 0;
		}
	}
}

int main(int argc, char *argv[]) {

	//Nomino il processo Main
	prctl(PR_SET_NAME, (unsigned long) "MainProject", 0, 0, 0);

	//effettuo la pulizia dei file precedentemente generati e presnti.
	presetFile();

	// Richiesta ricorsiva della directroy di G18 finche
	char pathG18S[100];
	sprintf(pathG18S, "%s", argv[1]);

	// Verigico se nella directory ricevuta ho un file esistente.
	while ((access(pathG18S, F_OK)) != 0) {

		printf("\nDirectory del FILE G18.txt ERRATA.\nInserire nuovamente:");
		scanf("%s", pathG18S);
	}

	//Stampa del path g18 appena inserito.
	printf("\nDirectory di G18: %s\n", pathG18S);
	fflush(stdout);

	// Richiedo l'intero di riga da cui iniziare a leggere la riga in G18.txt.
	int n = setFristContRow();
	setLasts(FILE_LASTS, n);

	//WES
	processWes();

	// GenFailures
	processGenFailures();

	// INCREMENTO IL CONTATORE DI RIGA G18 OGNI SECONDO
	processNextLine();

	// AVVIA SERVIZI TRANSDUCER
	processTransducer();

	// PFC1
	processPFC1(pathG18S);

	// PFC2
	processPFC2(pathG18S);

	// PFC3
	processPFC3(pathG18S);

	// PFCdisconSwitch
	PFCdisconnectSwitch(pathG18S);

	// aspetto finche non siamo a fine di G18;
	waitEnd();

	//termino tutti i processi generati.
	killAllProcess();

	printf("\n   Applicazione terminata.\n\n");

	exit(0);
}

