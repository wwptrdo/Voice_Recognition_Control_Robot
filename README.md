# VoiceRecognitionControlRobot
RaspberryPi、WringPi、科大讯飞(API)、图灵机器人(API)

# Manual Instruction
## 1：condition 
#### （1）Please ensure that the current Raspberry Pi contains GCC and WringPi Library.
#### （2）Required Hardware: Sound sensor）、Logitech C270 Cam (with Microphone). As shown in the following figure:
![Logitech_C270](https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/Logitech_C270.jpg )
#### （3）The Port Definition of Sound Sensor is in the "voice.h" file.  Please connect the line to the RaspberryPi.
#### （4）If you use a different microphone, look for the address of the microphone and fill it in the "voice.c" file. So that the recording process can be successfully opened.
#### （5）If there is no iFLYTEK account, please go to the website to obtain iFLYTEK developer account(http://www.xfyun.cn/). The same is true for Turing robot interfaces.(http://www.tuling123.com/). In this project I provided my trial account.
#### （6）And you need a small speaker or headset plugged into the raspberry headphone hole to make sure the sound is output. PS: The small speaker device is not required in the new project version.

## 2：Compile
#### (1)Please put the project under the /home/pi/ directory and ender the project directory.
#### (2)Compile CMD: 

##### (old version)
 gcc -o obj/voice\_old.o -Iinclude -c src/voice\_old.c

 gcc -o bin/test\_old -Iinclude obj/libmsc.so obj/voice\_old.o src/main\_old.c -lpthread -lwiringPi  -lrt -ldl -lpthread

##### (new version)
 gcc -o obj/voice.o -Iinclude -c src/voice.c

 gcc -o bin/test -Iinclude obj/libmsc.so obj/voice.o src/main.c -lpthread -lwiringPi  -lrt -ldl -lpthread -lasound


## 3：Running
#### You need to change the name of the directory to 'VoiceRecognitionControlRobot'.
#### The running cmd: 
sudo /home/pi/VoiceRecognitionControlRobot/bin/test
#### The running result are shown in the picture:
![demo]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/demo.jpg )

# Principle introduction
## Program flow chart:
![principle]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/principle.jpg )

## 4:中文说明：
http://bbs.xfyun.cn/forum.php?mod=viewthread&tid=25926&extra=page%3D1%26filter%3Dreply%26orderby%3Dviews

## 5: 更新情况（2017.07.30）
#### 在新版本中，不需要使用声音传感器来判断声音有无的情况，采用了直接调用麦克风的音频上传到科大讯飞，根据接收科大讯飞返回的音量值来判断声音的有无。 开启语音识别后，程序将开启线程不停的录制声音并将实时的音频上传到科大讯飞，并检测科大讯飞返回的音量值，如果连续的音量大于设定的阀值, 则接收科大讯飞语音识别后的声音字符串。
#### 新版本中，在将文本转换声音并输出（TTS）时，采用了调用音频驱动库ALSA直接播放科大讯飞返回的音频信息，不需要写入文件作为缓存，因为采用了这种方式，将旧版本3-5秒的延时，缩短为200ms左右，并且不会出现旧版中太长的文字只播放一部分文字的bug。


# Contact me
#### If you have any questions, please contact me.
#### QQ: 1348351139
#### e-mail: 1348351139@qq.com
