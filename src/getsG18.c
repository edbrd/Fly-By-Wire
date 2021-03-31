/*
 * distanceAndSpeed.c
 *
 *  Created on: 30 nov 2020
 *      Author: edbrd
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Questa funzione calcola la distanza tra due punti
 sulla superficie terrestre, date le coordinate in
 latitudine e longitudine espresse in
 gradi decimali.
 Se distanceGeo ha un valore in input 0 ritrona distanza sara 0.
 */
double distanceGeo(double latA, double lonA, double latB, double lonB) {
	/* Definisce le costanti e le variabili */
	const double R = 6371;
	const double pigreco = 3.1415927;
	double lat_alfa=0, lat_beta=0, lon_alfa=0, lon_beta=0;
	double fi;
	double p, d;
	/* Converte i gradi in radianti */
	lat_alfa = pigreco * latA / 180;
	lat_beta = pigreco * latB / 180;
	lon_alfa = pigreco * lonA / 180;
	lon_beta = pigreco * lonB / 180;
	/* Calcola l'angolo compreso fi */
	fi = fabs(lon_alfa - lon_beta);
	/* Calcola il terzo lato del triangolo sferico */
	p = acos(
			sin(lat_beta) * sin(lat_alfa)
					+ cos(lat_beta) * cos(lat_alfa) * cos(fi));
	/* Calcola la distanza sulla superficie
	 terrestre R = ~6371 km */
	d = p * R;
	//converto la distanza da km a m
	d *= 1000;
	if ((latA == 0) | (lonA == 0) | (latB == 0) | (lonB == 0))	{d = 0.0;}

	return d;
}

/* INPUT: CHAR *stringSpit, CHAR *delimititatore, INT postionOfSplitting, FLOAT result
 * data una stringa restituisce il valore corrispondenre alla i-esima cella della stringa splitata*/

double getValueAtPosition(char *stringSpit, char *delimititatore,
		int postionOfSplitting) {

	//rifare il metodo usando l'aproccio del flag come mostrava il ragazzo nel video youtube
	//semplice e molto hiaro

	double result = 0;
	char sSpit[strlen(stringSpit) + 1];

	int iesima = postionOfSplitting;

	char* token;
	strcpy(sSpit, stringSpit);
	token = strtok(sSpit, delimititatore); /* get the first token */

	while (token != NULL && iesima > 0) {
		iesima = iesima - 1;
		token = strtok(NULL, delimititatore);
	}
	if (token != NULL) {

		result = strtod(token, NULL);

	} else {

		printf(
				"Nella (stringSpit) con (%s) sepraratore, la posizione %d NON ESISTE \n",
				delimititatore, postionOfSplitting);

	}

	fflush(stdout);
	return result;

}

/* INPUT: int nStep
 * Il metodo getGPGLL restituisce la nStep-esima $GPGLL.. torvata in G18.
 * LIBERARE LA MEMORIA DOPO LA CHIAMATA*/
char *getLine(int nStep, char *path) {

	int flag = nStep;
	char buffer[100];
	FILE *fptr;
	char *lineGPGLL;

	if ((fptr = fopen(path, "r")) == NULL) {
		printf("Error! opening file: %s \n", path);
		fflush(stdout);
		// Program exits if file pointer returns NULL.
		exit(1);
	}

	while (fgets(buffer, 100, fptr) != NULL) {

		if (strstr(buffer, "$GPGLL") != NULL && flag >= 1) {
			if (flag == 1) {

				lineGPGLL= calloc(strlen(buffer),sizeof(char));
				strcpy(lineGPGLL, buffer);
				break;
			}

			flag--;
		}
	}


	if (strstr(buffer, "$GPGLL") == NULL) {
		printf("Sringa $GPGLL (numero:%d, %s) non presnete (fine del file G18).\n",nStep,lineGPGLL);
		fflush(stdout);
		exit(0);
	}

	fclose(fptr);
	return lineGPGLL;
}

/*
 * il metodo getSpeed prende in input 2 tempi[s], la distanza[m] e un intero.
 * Calcola la velocità facendo distanza diviso un secondo.
 */

double getSpeed( double distance, int shiftPFC) {

	double result=0;
	double d = distance;

	result = d/1;
	if (result < 0) {
		printf("OVERFLOW nel calcolo della velocità \n");
		fflush(stdout);
	} else if (distance == 0)
		result = 0;

	// shoft is ture
	if(shiftPFC==0){
		int spd=(int)result;
		spd=spd<<2;
		result= (double)spd;
	}

	return result;

}

