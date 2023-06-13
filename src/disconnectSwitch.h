/*
 * disconnectSwitch.h
 *
 *  Created on: 5 gen 2022
 *      Author: edbrd
 */

#ifndef SRC_DISCONNECTSWITCH_H_
#define SRC_DISCONNECTSWITCH_H_

int getPID_COMM(char namePFC[]);
char getSTATE_PID(int pid);
void sendRandomSignal(int pid);
int getRandPID_PFC();
void writteInfFlogFail(char signal[], char path[]);
void PFCdisconnectSwitch(char pathG18S[]);
void processGenFailures();
int emergenceCase(char path[]);

#endif /* SRC_DISCONNECTSWITCH_H_ */
