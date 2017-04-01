# VoiceRecognitionControlRobot
RaspberryPi、WringPi、科大讯飞(API)、图灵机器人(API)

# Manual Instruction
## 1：condition 
#### （1）Please ensure that the current Raspberry Pi contains GCC and WringPi Library.
#### （2）Required Hardware: Sound sensor）、Logitech C270 Cam (with Microphone). As shown in the following figure:
![Logitech_C270](https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/Logitech_C270.jpg )
#### （3）The Port Definition of Sound Sensor is in the "voice.h" file.  Please connect the line to the RaspberryPi.
#### （4）If you use a different microphone, look for the address of the microphone and fill it in the "voice.c" file. And the recording process can be successfully opened.
#### （5）If there is no iFLYTEK account, please go to the website to obtain iFLYTEK developer account(http://www.xfyun.cn/). The same is true for Turing robot interfaces.(http://www.tuling123.com/). In this project I provided my trial account.
#### （6）And you need a small speaker or headset plugged into the raspberry headphone hole to make sure the sound is output.

## 2：Compile
#### (1)Please put the project under the /home/pi/ directory and ender the project directory.
#### (2)Compile CMD: 
 gcc -o obj/voice.o -Iinclude -c src/voice.c

 gcc -o bin/test -Iinclude obj/libmsc.so obj/voice.o src/main.c -lpthread -lwiringPi  -lrt -ldl -lpthread


## 3：Running
#### You need to change the name of the directory to 'VoiceRecognitionControlRobot'.
#### The running cmd: 
sudo /home/pi/VoiceRecognitionControlRobot/bin/test
#### The running result are shown in the picture:
![demo]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/demo.jpg )

# Principle introduction
## Program flow chart:
![principle]( https://github.com/WhisperHear/Voice_Recognition_Control_Robot/blob/master/photos/principle.png )


# Contact me
#### If you have any questions, please contact me.
#### QQ: 1348351139
#### e-mail: 1348351139@qq.com
