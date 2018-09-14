/*
 * author: wwptrdudu <1348351139@qq.com>
 * github: https://github.com/wwptrdudu
 * tg: @wwptrdudu
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

	
	/*设置输出声音格式：(详见参数列表)
	 *                发音人    语速  音量  语调  不含背景音*/
        set_voice_params("xiaoyan", 50,   50,   50,     0);
	
	open_voice_recognition_chat_control();
	while (1)
	{
		sleep(60);	
		set_voice_params("donaldduck", 50, 50, 50, 0);
		printf("发音人改为：唐老鸭\n");		

		sleep(60);
		set_voice_params("nannan", 50, 50, 50, 1);
		printf("发音人改为：楠楠（普通话，童声）\n");

		sleep(60);
		set_voice_params("xiaorong", 50, 50, 50, 0);
		printf("发音人改为：小荣(四川话，青年女声)\n");
		
	}
	return 0;
}
