/*
 * msg_tianqin.h
 *
 *  Created on: 2015/9/2
 *      Author: msj
 */

#include <stdio.h>

#include "msg_tianqin.h"
#include "macro.h"

char *get_EI_STRING(const char *ei)
{
	static char ret[IMEI_LENGTH + 1];
	strcpy(ret, "unknown ei");
	if(!ei)
	{
		return ret;
	}
	for(int i = 0; i < (EI_LENGTH / 2); i++)
	{
		sprintf(ret + i * 2; "%02x", ei[i]);
	}
	
	for(int i = EI_LENGTH; i < IMEI_LENGTH; ++i)
	{
		ret[i] = '0' + i - EI_LENGTH;
	}
	ret[IMEI_LENGTH] = 0;

	return ret;
}