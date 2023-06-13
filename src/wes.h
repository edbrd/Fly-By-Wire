/*
 * wes.h
 *
 *  Created on: 14 dic 2022
 *      Author: edbrd
 */

#ifndef WES_H_
#define WES_H_

char * getLastSpeed(char path[]);

char *getStringAtPosition(char buffer[], int position);

void writteInStatus(const char path[], const char signal[]);

int checkSpeed(char pathSpeedPCF1[], char pathSpeedPCF2[], char pathSpeedPCF3[]);

char* getTimeNow() ;

void processWes();

#endif /* WES_H_ */
