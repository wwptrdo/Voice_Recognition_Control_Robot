/*
 author: WhisperHear <1348351139@qq.com>
 date: 2017.04.01
 brief:
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */


#include "voice/voice.h"
#include <stdio.h>
#include <wiringPi.h>

int main(int argc, char *argv[])
{
	if (wiringPiSetup() < 0)	
	{
		printf("wiringPi init error!\n");
		return -1;
	}
	voice_init();

	open_voice_recognition_control();
	while (1)
	{
		sleep(1);
	}
	return 0;
}
