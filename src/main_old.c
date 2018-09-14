/*
 * author: wwptrdudu <1348351139@qq.com>
 * github: https://github.com/wwptrdudu
 * tg: @wwptrdudu
 */


#include "voice/voice_old.h"
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
