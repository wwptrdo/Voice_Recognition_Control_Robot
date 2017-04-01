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
