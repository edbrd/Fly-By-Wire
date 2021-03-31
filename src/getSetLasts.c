/*
 * getSetLasts.c
 *
 *  Created on: 3 dic 2020
 *      Author: edbrd
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 100

/*
 * il metodo getLasts preleva la prima linea del file path
 *  e lo converte in int e lo restituisce.
 */
int getLasts(char *path) {

	FILE * fPtr = fopen(path, "r");
	char buffer[BUFFER_SIZE];
	int n;

	fgets(buffer, BUFFER_SIZE, fPtr);
	fclose(fPtr);
	n = atoi(buffer);

	return n;
}

/*
 * il metodo setLasts prende in input l'intero (n) da inserire nel file lasts.txt.
 */

void setLasts(char *path, int n) {

	FILE * fPtr;
	char buffer[BUFFER_SIZE];
	sprintf(buffer, "%d", n);
	fPtr = fopen(path, "w");

	if (fPtr != NULL) {

		fputs(buffer, fPtr);
		fflush(fPtr);

	} else {
		printf("\n lasts.txt, errore: FILE PINTER=NULL.\n");
		fflush(stdout);
	}

	fclose(fPtr);
}

