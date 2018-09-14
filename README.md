# VoiceRecognitionControlRobot（B-Bots）
基于科大讯飞与图灵机器人的语音控制语音聊天的智能机器人。RaspberryPi、WringPi、科大讯飞(API)、图灵机器人(API)

# 一、使用说明
## 1：条件
（1）当前树莓派系统中包含GCC和wiringPI等开发工具。（官方的系统即可）

（2）需要的其他硬件设备：罗技C270摄像头（带麦克风，如下图），LED指示灯，耳机或小音箱。（如果使用以前的版本即带old后缀的源文件，请确保有一个声音传感器来检测声音，如果使用新版本只需要麦克风和LED即可。）
![Logitech_C270](https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/Logitech_C270.jpg )

（3）将LED指示灯的正极针脚接到树莓派的GPIO-1针脚上（在“voice.h”文件中有定义），另一针脚接GND。将USB摄像头插到树莓派上。（如果使用以前的版本，请将声音传感器的信号阵脚接到树莓派相应位置，参考voice_old.h文件中的针脚定义。）

（4）这里提供一个科大讯飞账号和库文件供测试使用，如果程序报错信息为QISRAudioWrite failed! error code:11201，则说明当前账号的试用次数结束，请自己注册科大讯飞的账号，在main.c源码中更改即可。并且请注册自己的图灵机器人开发账号替换我的，避免试用次数结束。

（5）将耳机插入耳机口。


## 2：编译
运行一下命令：

	git clone https://github.com/WhisperHear/Voice_Recognition_Control_Robot.git
	cd  Voice_Recognition_Control_Robot
	make

## 3：运行
新版本：

	sudo bin/test

运行结果如下图：

![demo2]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/demo2.jpg )

旧版本：

	sudo bin/test_old

运行结果如下图：

![demo]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/demo.jpg )

# 二、原理介绍

旧版本的程序流程图:

![principle]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/principle.png )

新版本原理介绍

（见更新日志）

科大讯飞论坛说明：
http://bbs.xfyun.cn/forum.php?mod=viewthread&tid=25926&extra=page%3D1%26filter%3Dreply%26orderby%3Dviews

## 三、 更新日志

#### 【2017.07.30】

在新版本中，不需要使用声音传感器来判断声音有无的情况，采用了直接调用麦克风的音频上传到科大讯飞，根据接收科大讯飞返回的音量值来判断声音的有无。 开启语音识别后，程序将开启线程录制声音并将实时的音频上传到科大讯飞，并检测科大讯飞返回的音量值，如果连续的音量大于设定的阀值, 则接收科大讯飞语音识别后的声音字符串。根据识别出的字符串再进行对应的操作（控制，聊天）。

新版本中，在将文本转换声音并输出（TTS）时，采用了调用音频驱动库ALSA直接播放科大讯飞返回的音频信息，不需要写入文件作为缓存，因为采用了这种方式，将旧版本3-5秒的延时，缩短为200ms左右，并且不会出现旧版中太长的文字只播放一部分文字的bug。

#### 【2017.12.26】

（1）修复了旧版中存在的变录音边播放声音的bug。

（2）修复关闭语音识别后直接停止声音输出。

（3）添加了设置输出声音的参数的函数，可以更改发音人，语速，音量，语调，和背景声音。请勿设置错误，否则报错，参数信息如下图所示：

![params1]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/params1.png )

![params2]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/params2.png )

![params3]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/params3.jpg )


# Contact me 
#### If you have any questions, please contact me.
#### tg: @wwptrdudu
###### This link opens a chat with me.
       https://t.me/wwptrdudu
#### e-mail: 1348351139@qq.com
