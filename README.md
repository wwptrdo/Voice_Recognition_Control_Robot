# VoiceRecognitionControlRobot
RaspberryPi、WringPi、科大讯飞(API)、图灵机器人(API)

# Manual Instruction
## 1：condition 
#### （1）Please ensure that the current Raspberry Pi contains GCC and WringPi Library.
#### （2）Required Hardware: Sound sensor）、Logitech C270 Cam (with Microphone). As shown in the following figure:
![Image text](https://raw.github.com/Voice_Recognition_Control_Robot/photos/voice_detect.jpg)
![Image text](https://raw.github.com/Voice_Recognition_Control_Robot/photos/Logitech_C270.jpg)
#### （3）If there is no iFLYTEK account, please go to the website to obtain iFLYTEK developer account(http://www.xfyun.cn/). The same is true for Turing robot interfaces.(http://www.tuling123.com/). In this project I provided my trial account.


## 2：Compile
#### (1)Please put the project under the /home/pi/ directory and ender the project directory.
#### (2)Compile CMD: 
 gcc -o obj/voice.o -Iinclude -c src/voice.c

 gcc -o bin/test -Iinclude obj/libmsc.so obj/voice.o src/main.c -lpthread -lwiringPi  -lrt -ldl -lpthread


## 3：Running
#### You need to change the name of the directory to 'VoiceRecognitionControlRobot'.
#### The running cmd: 
sudo /home/pi/VoiceRecognitionControlRobot/test
#### The running result are shown in the picture:
![Image text](https://raw.github.com/Voice_Recognition_Control_Robot/photos/demo.jpg)

# Principle introduction
## Program flow chart:
![Image text](https://raw.github.com/Voice_Recognition_Control_Robot/photos/principle.png)





