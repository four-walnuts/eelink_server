/*
 * msg_tianqin.h
 *
 *  Created on: 2015/9/1
 *      Author: msj
 */

#ifndef SRC_MSG_TIANQIN_H_
#define SRC_MSG_TIANQIN_H_

#include <stdio.h>

#define START_FLAG (0x24)
#define EI_LENGTH (10)

typedef struct
{
	char signature;
	char EI[5];	//Equipment Identity
	char time[3];
	char date[3];
	char lat[4];
	char charge;
	char lon[5];
	char speed[3];
	char status[4];
	char flag;
	char GSM;
	char GPS;
	char distance[4];
	short mcc;
	char mnc;
	short lac;
	short cellid;
	char seq;
}__attribute__((__packed__)) MSG_V1;

char *get_EI_STRING(const char *ei);

#endif /* SRC_MSG_TIANQIN_H_ */