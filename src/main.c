/*
 * author: WhisperHear <1348351139@qq.com>
 * github: https://github.com/WhisperHear
 * date:   2017.07.30
 * brief:
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
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
	//参数：      网络连接标志   科大讯飞appid       图灵机器人KEY                    声卡地址
	voice_init(NETWORK_CONNECTED, "56ee43d0", "44ee05352ba9459aaca3205c421f5e4c", "plughw:CARD=U0x46d0x825");

	open_voice_recognition_chat_control();
	while (1)
	{
		sleep(1);
	}
	return 0;
}