/*
 author: WhisperHear <1348351139@qq.com>
 date: 2017.04.01
 brief:
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 3 as published by the Free Software Foundation.
 */

#include "voice/voice.h"
#include "voice/qisr.h"
#include "voice/qtts.h"
#include "voice/msp_cmn.h"
#include "voice/msp_errors.h"
//#include "sys.h"

#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

static Voice voice;

/*功能：日志写入*/
static void show_sys_info(char *src)
{
	printf("%s", src);
	/*在这里添加写入到日志文件的部分！*/
}

/* 合成的wav声音文件：默认wav音频头部数据 */
static wave_pcm_hdr default_wav_hdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0  
};

static void open_voice_light()
{
	digitalWrite(VOICE_LIGHT_PIN, HIGH);	
}

static void close_voice_light()
{
	digitalWrite(VOICE_LIGHT_PIN, LOW);
}

static int get_omxplayer_stat()
{
        if (!voice.sound_box_ongoing || (kill(voice.omxplayer_pid, 0) == -1) ) //当尚未第一次打开ffserver或者ffserver进程已经不存在
        {
                return -1;
        }
        else
                return 0;
}

static void kill_omxplayer()
{
	if (get_omxplayer_stat() == 0)
	{
		if ((kill(voice.omxplayer_pid, SIGKILL)) == -1)
        	{	
        		return ;  //杀死ffserver进程失败
        	} 
		else
        	{
        		while (get_omxplayer_stat() == 0)  //杀死成功，等待释放资源
        		{	
                		usleep(100);
                	}
        	}
        	return ;  //成功杀死ffmpeg进程
	}
}

/*
void speech_front_hinder()
{
	//kill_omxplayer(); //关掉正在播放的语音
        voice.sound_box_ongoing = TRUE;  //不允许录音
	pid_t omxplayer_pid;
        if ((omxplayer_pid = fork()) < 0)
        {
                voice.sound_box_ongoing = FALSE;
                show_sys_info("创建播放音频进程失败！\n");
                return ;
        }
        else if (omxplayer_pid == 0)
        {
                show_sys_info("语音输出：正在说话...\n");
                if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/0_robot_client/wav/sys_voice/front_hinder.wav", NULL) < 0)
                {
                        printf("播放声音进程执行失败！\n");
                }
        }
        wait(0); //等待播放进程结束     
        voice.sound_box_ongoing = FALSE;	
}
*/

/*
void speech_back_hinder()
{
	//kill_omxplayer(); //关掉正在播放的语音
        voice.sound_box_ongoing = TRUE;  //不允许录音
	pid_t omxplayer_pid;
        if ((omxplayer_pid = fork()) < 0)
        {
                voice.sound_box_ongoing = FALSE;
                show_sys_info("创建播放音频进程失败！\n");
                return ;
        }
        else if (omxplayer_pid == 0)
        {
                show_sys_info("语音输出：正在说话...\n");
                if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/0_robot_client/wav/sys_voice/back_hinder.wav", NULL) < 0)
                {
                        printf("播放声音进程执行失败！\n");
                }
        }
        wait(0); //等待播放进程结束     
        voice.sound_box_ongoing = FALSE;

}

void speech_hinder()
{
	//kill_omxplayer(); //关掉正在播放的语音
        voice.sound_box_ongoing = TRUE;  //不允许录音
	pid_t omxplayer_pid;
        if ((omxplayer_pid = fork()) < 0)
        {
                voice.sound_box_ongoing = FALSE;
                show_sys_info("创建播放音频进程失败！\n");
                return ;
        }
        else if (omxplayer_pid == 0)
        {
                show_sys_info("语音输出：正在说话...\n");
                if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/0_robot_client/wav/sys_voice/hinder.wav", NULL) < 0)
                {
                        printf("播放声音进程执行失败！\n");
                }
        }
        wait(0); //等待播放进程结束     
        voice.sound_box_ongoing = FALSE;	
}

*/

/*
 *功能：打开蜂鸣器模块
 */
void open_voice()
{
	digitalWrite(VOICE_PIN, LOW);
}

/*
 *功能：关闭蜂鸣器模块
 */
void close_voice()
{
	digitalWrite(VOICE_PIN, HIGH);
}


/*科大讯飞：上传声音关键字 */
static int upload_userwords()
{
	char*	     userwords = NULL;
	unsigned int len = 0;
	unsigned int read_len =	0;
	FILE*	     fp = NULL;
	int          ret = -1;

	fp = fopen("voice_key_words.txt", "rb");
	if (NULL == fp)										
	{
		show_sys_info("\nvoice_recognition Error: open [voice_key_words.txt] failed! \n");
		goto upload_exit;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp); //获取音频文件大小
	fseek(fp, 0, SEEK_SET);  					
	
	userwords = (char*)malloc(len + 1);
	if (NULL == userwords)
	{
		show_sys_info("\nvoice_recognition Error: out of memory! \n");
		goto upload_exit;
	}

	read_len = fread((void*)userwords, 1, len, fp); //读取用户词表内容
	if (read_len != len)
	{
		show_sys_info("\nvoice_recognition Error: read [userwords.txt] failed!\n");
		goto upload_exit;
	}
	userwords[len] = '\0';
	
	MSPUploadData("userwords", userwords, len, "sub = uup, dtt = userword", &ret); //上传用户词表
	if (MSP_SUCCESS != ret)
	{
		printf("\nMSPUploadData failed ! errorCode: %d \n", ret);
		goto upload_exit;
	}
	
upload_exit:
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}	
	if (NULL != userwords)
	{
		free(userwords);
		userwords = NULL;
	}
	
	return ret;
}

/*
 *功能：科大讯飞的识别模块（语音听写）
 *说明：将音频文件转换为文字
 */
static void run_iat(const char* audio_file, const char* session_begin_params, char *ret)
{
	const char*   session_id = NULL;
	char rec_result[BUFFER_SIZE];
	memset(rec_result, 0, sizeof(rec_result));	
	char hints[HINTS_SIZE]; //hints为结束本次会话的原因描述，由用户自定义
	memset(hints, 0, sizeof(hints));
	unsigned int  total_len	= 0; 
	int           aud_stat = MSP_AUDIO_SAMPLE_CONTINUE ; //音频状态
	int	      ep_stat =	MSP_EP_LOOKING_FOR_SPEECH;   //端点检测
	int	      rec_stat	= MSP_REC_STATUS_SUCCESS ;   //识别状态
	int	      errcode =	MSP_SUCCESS;

	FILE*	f_pcm =	NULL;
	char*	p_pcm =	NULL;
	long	pcm_count = 0;
	long	pcm_size = 0;
	long	read_size = 0;

	
	if (NULL == audio_file)
		goto iat_exit;

	f_pcm = fopen(audio_file, "rb");
	if (NULL == f_pcm) 
	{
		//！！！！日志信息打印函数未封装！下面的函数需要重写！！！！
		printf("\nvoice_recognition Error: open [%s] failed! \n", audio_file);
		goto iat_exit;
	}
	
	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm); //获取音频文件大小 
	fseek(f_pcm, 0, SEEK_SET);		

	p_pcm = (char *)malloc(pcm_size);
	if (NULL == p_pcm)
	{
		show_sys_info("\nvoice_recognition Error: out of memory! \n");
		goto iat_exit;
	}

	read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm); //读取音频文件内容
	if (read_size != pcm_size)
	{
		//！！！！日志信息打印函数未封装！下面的函数需要重写！！！！
		printf("\nread [%s] error!\n", audio_file);
		goto iat_exit;
	}
	
	session_id = QISRSessionBegin(NULL, session_begin_params, &errcode); //听写不需要语法，第一个参数为NULL
	if (MSP_SUCCESS != errcode)
	{
		//！！！！日志信息打印函数未封装！下面的函数需要重写！！！！
		printf("\nQISRSessionBegin failed! error code:%d\n", errcode);
		goto iat_exit;
	}
	
	while (1) 
	{
		unsigned int len = 10 * FRAME_LEN; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
		int ret = 0;

		if (pcm_size < 2 * len) 
			len = pcm_size;
		if (len <= 0)
			break;

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		ret = QISRAudioWrite(session_id, (const void *)&p_pcm[pcm_count], len, aud_stat, &ep_stat, &rec_stat);
		if (MSP_SUCCESS != ret)
		{
			//！！！！日志信息打印函数未封装！下面的函数需要重写！！！！
			printf("\nQISRAudioWrite failed! error code:%d\n", ret);
			if (ret == 11201)
			{
				voice.sound_box_ongoing = TRUE;
                                pid_t omxplayer_pid;
                                if ((omxplayer_pid = fork()) < 0)
                                {
                                        voice.sound_box_ongoing = FALSE;
                                        show_sys_info("创建播放音频进程失败！\n");
                                }
                                else if  (omxplayer_pid == 0)
                                {
                                        show_sys_info("语音输出：正在说话...\n");
                                        if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/VoiceRecognitionControlRobot/wav/compose_voice_temp/compose_voice_all.wav", NULL) < 0)
                                        {
                                                show_sys_info("播放声音进程执行失败！\n");
                                        }
                                }
                                wait(0); //等待播放进程结束
                                voice.sound_box_ongoing = FALSE;
			}
			goto iat_exit;
		}
			
		pcm_count += (long)len;
		pcm_size  -= (long)len;
		
		if (MSP_REC_STATUS_SUCCESS == rec_stat) //已经有部分听写结果
		{
			const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
			if (MSP_SUCCESS != errcode)
			{
				//！！！！日志信息打印函数未封装！下面的函数需要重写！！！！
				printf("\nQISRGetResult failed! error code: %d\n", errcode);
				goto iat_exit;
			}
			if (NULL != rslt)
			{
				unsigned int rslt_len = strlen(rslt);
				total_len += rslt_len;
				if (total_len >= BUFFER_SIZE)
				{
					show_sys_info("\nvoice_recognition Error: no enough buffer for rec_result !\n");
					goto iat_exit;
				}
				strncat(rec_result, rslt, rslt_len);
			}
		}

		if (MSP_EP_AFTER_SPEECH == ep_stat)
			break;
		usleep(200*1000); //模拟人说话时间间隙。200ms对应10帧的音频
	}
	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	if (MSP_SUCCESS != errcode)
	{
		//！！！！日志信息打印函数未封装！下面的函数需要重写！！！！
		printf("\nQISRAudioWrite failed! error code:%d \n", errcode);
		goto iat_exit;	
	}

	while (MSP_REC_STATUS_COMPLETE != rec_stat) 
	{
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (MSP_SUCCESS != errcode)
		{
			//！！！！日志信息打印函数未封装！下面的函数需要重写！！！！
			printf("\nQISRGetResult failed, error code: %d\n", errcode);
			goto iat_exit;
		}
		if (NULL != rslt)
		{
			unsigned int rslt_len = strlen(rslt);
			total_len += rslt_len;
			if (total_len >= BUFFER_SIZE)
			{
				show_sys_info("\nno enough buffer for rec_result !\n");
				goto iat_exit;
			}
			strncat(rec_result, rslt, rslt_len);
		}
		usleep(150*1000); //防止频繁占用CPU
	}
	strcpy(ret, rec_result);  //将语音转换的结果转出

iat_exit:
	if (NULL != f_pcm)
	{
		fclose(f_pcm);
		f_pcm = NULL;
	}
	if (NULL != p_pcm)
	{	free(p_pcm);
		p_pcm = NULL;
	}

	QISRSessionEnd(session_id, hints);
}

/*
 *功能：科大讯飞，语音合成，将文本转换为声音
 *参数：src_text:文本
		usleep(150*1000); //防止频繁占用CPU
	}
	strcpy(ret, rec_result);  //将语音转换的结果转出

iat_exit:
	if (NULL != f_pcm)
	{
		fclose(f_pcm);
		f_pcm = NULL;
	}
	if (NULL != p_pcm)
	{	free(p_pcm);
		p_pcm = NULL;
	}

	QISRSessionEnd(session_id, hints);
}

/*
 *功能：科大讯飞，语音合成，将文本转换为声音
 *参数：src_text:文本
 *      des_path:生成的wav文件路径，例如：/home/pi/0_robot_client/wav/compose_voice_temp/compose_voice.wav
 *返回值：见科大讯飞手册，失败返回-1
 *注意：我修改了一下，把参数放到这个函数里面了
 */
int text_to_speech(const char* src_text, const char* des_path)
{
	const char* params = "voice_name = xiaoyan, text_encoding = UTF8, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";

	int          ret          = -1;
	FILE*        fp           = NULL;
	const char*  sessionID    = NULL;
	unsigned int audio_len    = 0;
	wave_pcm_hdr wav_hdr      = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

	if (NULL == src_text || NULL == des_path)
	{
		printf("params is error!\n");
		return ret;
	}
	fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		printf("open %s error.\n", des_path);
		return ret;
	}
	/* 开始合成 */
	sessionID = QTTSSessionBegin(params, &ret);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionBegin failed, error code: %d.\n", ret);
		fclose(fp);
		return ret;
	}
	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSTextPut failed, error code: %d.\n",ret);
		QTTSSessionEnd(sessionID, "TextPutError");
		fclose(fp);
		return ret;
	}
	fwrite(&wav_hdr, sizeof(wav_hdr) ,1, fp); //添加wav音频头，使用采样率为16000
	while (1) 
	{
		/* 获取合成音频 */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;
		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
		    wav_hdr.data_size += audio_len; //计算data_size大小
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		usleep(150*1000); //防止频繁占用CPU
	}//合成状态synth_status取值请参阅《讯飞语音云API文档》
	
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSAudioGet failed, error code: %d.\n",ret);
		QTTSSessionEnd(sessionID, "AudioGetError");
		fclose(fp);
		return ret;
	}
	/* 修正wav文件头数据的大小 */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);
	
	/* 将修正过的数据写回文件头部,音频文件为wav格式 */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8,sizeof(wav_hdr.size_8), 1, fp); //写入size_8的值
	fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
	fwrite(&wav_hdr.data_size,sizeof(wav_hdr.data_size), 1, fp); //写入data_size的值
	fclose(fp);
	fp = NULL;
	/* 合成完毕 */
	ret = QTTSSessionEnd(sessionID, "Normal");
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionEnd failed, error code: %d.\n",ret);
	}

	return ret;
}

/*
 * 功能：speech_robot_condition中的线程处理函数
 * 缺陷：科大讯飞那边有问题，好像不能直接转换太长的文字，估计是试用账号，有限制
 */
/*
static char condition_text_part1[200];
static char condition_text_part2[200];
void* th_speech_robot_condition(void *arg)
{
	show_sys_info("自身状态信息：");
        show_sys_info(condition_text_part1);
	show_sys_info(condition_text_part2);
        show_sys_info("\n正在合成音频文件...\n");

        //2、合成语音
        int ret = MSP_SUCCESS;
        ret = text_to_speech(condition_text_part1, "/home/pi/0_robot_client/wav/compose_voice_temp/speech_condition_temp_part1.wav");
        if (MSP_SUCCESS != ret)
        {
                printf("语音合成：合成失败，无法输出语音！text_to_speech failed, error code: %d.\n", ret);
                return (void*)0;
        }
	
	ret = text_to_speech(condition_text_part2, "/home/pi/0_robot_client/wav/compose_voice_temp/speech_condition_temp_part2.wav");
        if (MSP_SUCCESS != ret)
        {
                printf("语音合成：合成失败，无法输出语音！text_to_speech failed, error code: %d.\n", ret);
                return (void*)0;
        }

        show_sys_info("音频文件合成完毕，正在准备播放...\n");

        //3、播放语音
	//kill_omxplayer(); //关掉正在播放的语音
        voice.sound_box_ongoing = TRUE;  //不允许录音
	第一段语音
	pid_t omxplayer_pid;
        if ((omxplayer_pid = fork()) < 0)
        {
                voice.sound_box_ongoing = FALSE;
                show_sys_info("创建播放音频进程失败！\n");
                return (void*)0;
        }
        else if (omxplayer_pid == 0)
        {
                show_sys_info("语音输出：正在说话...\n");
                if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/0_robot_client/wav/compose_voice_temp/speech_condition_temp_part1.wav", NULL) < 0)
                {
                        printf("播放声音进程执行失败！\n");
                }
        }
	wait(0);
	voice.sound_box_ongoing = TRUE;  //不允许录音
	if ((omxplayer_pid = fork()) < 0)
        {
                voice.sound_box_ongoing = FALSE;
                show_sys_info("创建播放音频进程失败！\n");
                return (void*)0;
        }
        else if (omxplayer_pid == 0)
        {
                show_sys_info("语音输出：正在说话...\n");
                if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/0_robot_client/wav/compose_voice_temp/speech_condition_temp_part2.wav", NULL) < 0)
                {
                        printf("播放声音进程执行失败！\n");
                }
        }

        wait(0); //等待播放进程结束     
        voice.sound_box_ongoing = FALSE;
	
	return (void*)0;
}
*/

/*
 * 功能：调用图灵机器人接口，发送Post请求，获取返回的智能回复消息
 * 参数：send_text:你说的话；recv_text:机器人回复的话放到recv_text中；code：保存recv_text的类型，具体看图灵机器人官网的API文档
 *                           recv_text_size:保存recv_text的空间大小；recv_code_size: 保存code空间的大小
 * 返回值：成功返回0， 失败返回-1
 * 说明：下面的图灵接口key值（key=44ee05352ba9459aaca3205c421f5e4c）是我自己账户的，每天智能回复的次数有限，请去图灵机器人官网获取更多支持
 */
static int smart_chat(char* send_text, char *recv_text, int recv_text_size, char *code, int recv_code_size)
{
	/*步骤1：创建socket*/
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
		show_sys_info("智能聊天服务异常：socket创建失败！\n");
		return -1;
        }

        //往serveraddr中填入ip、port和地址族类型（IPv4），为连接connect做准备
        struct sockaddr_in serveraddr;  //专用地址结构体保存要连接的服务器的地址信息
        memset(&serveraddr, 0, sizeof(serveraddr)); //清零操作
        serveraddr.sin_family = AF_INET; //Internet地址族：IPv4
        serveraddr.sin_port = htons(80); //端口：第二个参数（转换为网络     
                                                        //字节序）

        //将IP地址转换为网络字节序
        inet_pton(AF_INET, "182.92.110.58", &serveraddr.sin_addr.s_addr);

        /*步骤2；客户端调用connect函数连接到服务器端,注意需要把专用地址强转为通用地址*/
        if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        {
		 show_sys_info("智能聊天服务异常：connect() Error!\n");
                 return -1;
        }
        /*步骤3：调用I/O函数（read和write）和服务器端进行双向通信*/

        int params_len = strlen("key=44ee05352ba9459aaca3205c421f5e4c&info=") + strlen(send_text);
	
        char buff[1000];                  //先暂且定义这么多，可能不够用，如果发送过来天气，新闻等数据的话
        memset(buff, 0, sizeof(buff));
        snprintf(buff, sizeof(buff)-1,
                "POST /openapi/api HTTP/1.1\r\n"
                "Host: www.tuling123.com\r\n"
                "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
                "Content-Length: %d\r\n"
                "\r\n"
		"key=%s&info=%s",
                params_len,
                "44ee05352ba9459aaca3205c421f5e4c",
                send_text
                );

        if (write(sockfd, buff, strlen(buff)) != strlen(buff))
        {
                return -1;  //发送出错返回-1
        }
        memset(buff, 0, sizeof(buff));
        size_t size;
        if ((size = read(sockfd, buff, sizeof(buff))) < 0)//从服务器端读取
        {
        	return -1;
	}
        /*步骤4：关闭socket*/
        close(sockfd);

        /*
         * 服务器返回的数据格式：
         *---------------------------------------------------------
         *                      HTTP/1.1 200 OK
         *                      Server: nginx/1.10.1
         *                      Date: Thu, 16 Mar 2017 06:06:13 GMT
         *                      Content-Type: text/json; charset=UTF-8
         *                      Transfer-Encoding: chunked
         *                      Connection: keep-alive
         *                      Access-Control-Allow-Origin: *
         *
         *                      4c
         *                      {"code":100000,"text":"我不会说英语的啦，你还是说中文吧。"}
         *                      0
         *
         *
         *---------------------------------------------------------
         *需要提取的内容为code和text冒号后面的内容
         *code和text的含义请去图灵官网查看，还有好多的类型。。。
	 */

         char *p = strstr(buff+150, "code");    //如果此时p不为null,则指向‘c’，从第150个开始查找，应该能提高速度吧~
         if (p == NULL)
         {
                 return -1;
         }
         p += 6;
	 
	 memset(code, 0, recv_code_size);
         int i = 0;
         while(*p != ',' && i < recv_code_size)
         {
                 code[i] = *p;
                 p++;
                 i++;
         }
         p += 9;


         memset(recv_text, 0, recv_text_size);
         i = 0;
         while (*p != '"' && i < recv_text_size)
         {
                 recv_text[i] = *p;
                 p++;
                 i++;
         }


         /*步骤5：将接受的数据显示出来*/
         //printf("%s\n", buff);   //服务器响应的数据
         //printf("code: %s\n", code);
         //printf("text: %s\n", recv_text);

         return 0;
}


/*
 *功能：中断处理函数，在这个函数中进行语音识别控制、智能聊天
 *说明：
 *      这个注册的中断处理函数会和main函数并发执行（同时执行，谁也不耽误谁）
 *      当本次中断函数还未执行完毕，这个时候树莓派又触发了一个中断，那么这个后来的中断不会被丢弃，它仍然可以被执行。
 *      但是wiringPi最多可以跟踪并记录后来的仅仅1个中断，如果不止1个，则他们会被忽略，得不到执行。
 */
int flag = 1;  //这个voice.recongnition_ongoing标识好像不能关掉在中断处理函数过程中的另一个中断的发生.
               //所以，定义一个flag好了
static void voice_recognition_control()
{
	if (voice.voice_main_switch == FALSE)  //语音识别未初始化成功，总开关未打开，直接退出！
	{
		usleep(100000);
		return ;
	}

	if (voice.recongnition_switch == FALSE)
	{
		//printf("中断未打开，无法开启语音识别！~\n");
		usleep(100000);
		return ;                 //如果中断未打开，直接退出
	}
	if (!voice.recongnition_ongoing)
	{
		usleep(100000);
		return ;
	}
	if (voice.sound_box_ongoing) //如果当前音箱正在说话，不允许语音识别
	{
		usleep(100000);
		return ;
	}

	if (flag == 2)       //如果当前是第二个中断，则标识下一个中断为第一个中断后退出             
	{
		flag--;
		usleep(100000);	
		return ;
	}
	flag++;     //如果是第一个中断则标识下一个中断为第二个中断。
	voice.recongnition_ongoing = FALSE;
	/*
	* sub:				请求业务类型
	* domain:			领域
	* language:			语言
	* accent:			方言
	* sample_rate:		音频采样率
	* result_type:		识别结果格式
	* result_encoding:	结果编码格式
	*
	* 详细参数说明请参阅《iFlytek MSC Reference Manual》
	*/
	char* session_begin_params = "sub = iat, domain = iat, language = zh_ch, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8";


	char voice_recongnition_cmd[BUFFER_SIZE];
	pid_t arecord_pid;  //录制音频进程标识

	memset(voice_recongnition_cmd, 0, sizeof(voice_recongnition_cmd));
	if ((arecord_pid = fork()) < 0)	
	{
		show_sys_info("创建录制音频进程失败！\n");
		goto exit_;
	}
	else if (arecord_pid == 0)  //子进程执行录制音频
	{
		//kill_omxplayer(); //关掉正在播放的语音
		open_voice_light();   //打开说话的灯，表示可以说话了
		show_sys_info("语音控制：正在录音，请说话...\n");
		if (execlp("/bin/bash", "/bin/bash", "-c", "arecord -Dplughw:CARD=U0x46d0x825 -d 2.5 -r 16000 -c 1 -t wav -f S16_LE /home/pi/VoiceRecognitionControlRobot/wav/cmd_voice_temp/voice_cmd.wav", NULL) < 0)
                { 
			show_sys_info("录制音频进程执行失败！\n");      
                }
	}
	else if (arecord_pid > 0)   //父进程执行
	{
		wait(0); //等待子进程录制完毕！
		close_voice_light(); //不能说话了，表明此时正在进行识别！
		show_sys_info("语音控制：正在识别，请等待...\n");
		//这个地方应该还要加上判断该文件是否存在的代码，以后再加~
		run_iat("/home/pi/VoiceRecognitionControlRobot/wav/cmd_voice_temp/voice_cmd.wav", session_begin_params, voice_recongnition_cmd);
		if ((strlen(voice_recongnition_cmd)) <= 0)
		{
			show_sys_info("语音识别出错：您没有说话！\n");
			goto exit_;
		}
		if (strstr(voice_recongnition_cmd, "左看"))
		{
			show_sys_info("语音控制：向左看...\n");
			//cam_turn_point(90, 0);
			//这个地方添加控制函数
		}
		else if (strstr(voice_recongnition_cmd, "前看"))
		{
			show_sys_info("语音控制：向前看...\n");
			//cam_turn_point(0, 0);
			//这个地方添加控制函数
		}
		else if (strstr(voice_recongnition_cmd, "右看"))
		{
			show_sys_info("语音控制：向右看...");
			//cam_turn_point(-90, 0);
		}
		else if (strstr(voice_recongnition_cmd, "抬头"))
		{
			show_sys_info("语音控制：抬头");
			//cam_turn_point(0, -90);	
		}
		else if (strstr(voice_recongnition_cmd, "低头"))
		{
			show_sys_info("语音控制：低头");
			//cam_turn_point(0, 90);
		}
		else if (strstr(voice_recongnition_cmd, "前走"))
		{
			show_sys_info("语音控制：往【前】走...\n");
			//voice_control_motion(VOICE_CONTROL_STRAIGHT);
		}
		else if (strstr(voice_recongnition_cmd, "后"))
		{
			show_sys_info("语音控制：往【后】走...\n");
			//voice_control_motion(VOICE_CONTROL_BACK);
		}
		else if (strstr(voice_recongnition_cmd, "左转"))
		{
			show_sys_info("语音控制：往【左】走...\n");
			//voice_control_motion(VOICE_CONTROL_LEFT);
		}
		else if (strstr(voice_recongnition_cmd, "左拐"))
                {
                        show_sys_info("语音控制：往【左】走...\n");
                        //voice_control_motion(VOICE_CONTROL_LEFT);
                }
		else if (strstr(voice_recongnition_cmd, "右拐"))
                {
                        show_sys_info("语音控制：往【右】走...\n");
                        //voice_control_motion(VOICE_CONTROL_RIGHT);
                }
		else if (strstr(voice_recongnition_cmd, "右转"))
		{
			show_sys_info("语音控制：往【右】走...\n");
			//voice_control_motion(VOICE_CONTROL_RIGHT);
		}
		else if (strstr(voice_recongnition_cmd, "停"))
		{
			show_sys_info("语音控制：停止\n");
			//voice_control_motion(VOICE_CONTROL_STOP);
		}
		else if (strstr(voice_recongnition_cmd, "开"))
		{
			show_sys_info("语音控制：开灯\n");
			//open_cam_light();
		}
		else if (strstr(voice_recongnition_cmd, "关"))
		{
			show_sys_info("语音控制：关灯\n");
			//close_cam_light();
		}
		else if (strstr(voice_recongnition_cmd, "状态") || strstr(voice_recongnition_cmd, "环境"))
		{
			show_sys_info("语音控制：播放当前机器人状态\n");
			//speech_robot_condition();
		}
		else
		{	
			printf("你说了这些话：‘%s’, 不过这不能控制我哦，但我可以跟你讲话~\n", voice_recongnition_cmd);	

			/*
                         * 当听不懂控制指令的时候，进行语音输出，智能聊天
                         */

			//1、智能回复（仅文字）
			char recv_text[1024];
			memset(recv_text, 0, sizeof(recv_text));
			char recv_code[10];
			memset(recv_code, 0, sizeof(recv_code));
			if ((smart_chat(voice_recongnition_cmd, recv_text, sizeof(recv_text), recv_code, sizeof(recv_code))) < 0)
			{
				show_sys_info("智能回复错误：获取图灵机器人接口失败！\n");
				goto exit_;
			}			
			show_sys_info("智能聊天文字内容："); show_sys_info(recv_text); show_sys_info("\n");

			//因为科大讯飞账号是试用版，长度应该有限制，把它分开播放
			if (strlen(recv_text) < 1024)  
			{
				//2、合成语音
				int ret = MSP_SUCCESS;
				ret = text_to_speech(recv_text, "/home/pi/VoiceRecognitionControlRobot/wav/compose_voice_temp/compose_voice_all.wav");
				if (MSP_SUCCESS != ret)
				{
					printf("语音合成：合成失败，无法输出语音！text_to_speech failed, error code: %d.\n", ret);
					goto exit_;
				}
				//3、语音输出
				//system("omxplayer -o local /home/pi/0_robot_client/wav/compose_voice_temp/compose_voice.wav");
				//kill_omxplayer();
				voice.sound_box_ongoing = TRUE;
				pid_t omxplayer_pid;
				if ((omxplayer_pid = fork()) < 0)
		       		{
					voice.sound_box_ongoing = FALSE;
                			show_sys_info("创建播放音频进程失败！\n");
                			goto exit_;
        			}
        			else if	 (omxplayer_pid == 0)  
        			{
                			show_sys_info("语音输出：正在说话...\n");
                			if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/VoiceRecognitionControlRobot/wav/compose_voice_temp/compose_voice_all.wav", NULL) < 0)
                			{
                        			show_sys_info("播放声音进程执行失败！\n");
                			}
        			}
				wait(0); //等待播放进程结束
				voice.sound_box_ongoing = FALSE;
					
			}
			show_sys_info("语音聊天：一次语音聊天结束！\n");
		}
	}
exit_:	show_sys_info("语音控制：一次语音识别结束！\n");	
	voice.recongnition_ongoing = TRUE;
	return ;
}



/*
 *功能：初始化语音模块
 */
void voice_init(void)
{
	pinMode(VOICE_PIN, OUTPUT);
	digitalWrite(VOICE_PIN, HIGH);

	pinMode(VOICE_LIGHT_PIN, OUTPUT);
	digitalWrite(VOICE_LIGHT_PIN, LOW);

	memset(voice.open_record_voice_cmd, 0, sizeof(voice.open_record_voice_cmd));
	memset(voice.voice_recongnition_cmd, 0, sizeof(voice.voice_recongnition_cmd));
	voice.voice_main_switch = FALSE;
	voice.recongnition_ongoing = FALSE;
	voice.recongnition_switch = FALSE;
	voice.sound_box_ongoing = FALSE;
	//voice.omxplayer_pid = -1;
	wiringPiISR(VOICE_DETECT_PIN, INT_EDGE_FALLING, &voice_recognition_control);

	//if (mode == REMOTE_CONNECT_MODE)  //可以联网的模式
	//{
		int ret = MSP_SUCCESS;
		char* login_params = "appid = 56ee43d0, work_dir = ."; // 登录参数，appid与msc库绑定,请勿随意改动		
		/* 用户登录 */
		ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数	
		if (MSP_SUCCESS != ret)
		{
			printf("MSPLogin failed (科大讯飞账号登陆失败！) , Error code %d.\n", ret);
			show_sys_info("语音识别初始化失败！\n");
			MSPLogout();                                      //退出登录
			return ;	
		}
		
		int upload_on =	0;       //是否上传用户词表，默认为关掉上传用户此表
		if (upload_on)
		{
			show_sys_info("voice_recongnition: 上传用户词表 ...\n");
			ret = upload_userwords();
			if (MSP_SUCCESS != ret)
			{
				MSPLogout();                                      //退出登录
				return ;
			}	
			show_sys_info("voice_recongnition: 上传用户词表成功\n");
		}
		voice.voice_main_switch = TRUE;
		show_sys_info("voice初始化：语音识别系统启动.\n");
	//}
	//else
	//{
	//	show_sys_info("voice初始化：当前连接模式不支持语音识别！\n");
	//}
}


/*
 *功能：线程处理函数，在线程中开启语音识别，降低延时
 *参数：无
 *返回值：无
 */
void* open_voice_recognition_control_th(void *arg)
{
	//创建子进程输出语音提示“语音识别已经开启”
	//kill_omxplayer(); //关掉正在播放的语音
	voice.sound_box_ongoing = TRUE;
	pid_t omxplayer_pid;
	if ((omxplayer_pid = fork()) < 0)
        {
		voice.sound_box_ongoing = FALSE;
        	show_sys_info("创建播放音频进程失败！\n");
		return (void*)0;
        }
        else if (omxplayer_pid == 0)  //子进程执行录制音频
        {
                show_sys_info("语音输出：正在说话...\n");
                if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/VoiceRecognitionControlRobot/wav/sys_voice/opened_voice_recongnition.wav", NULL) < 0)
        	{
           		show_sys_info("播放声音进程执行失败！\n");
                }
        }
	wait(0);
	
	voice.sound_box_ongoing = FALSE;
	voice.recongnition_ongoing = TRUE;            //设置语音识别状态为正在进行
	
	show_sys_info("语音识别控制已经开启...\n");	
	voice.recongnition_switch = TRUE;

	return (void*)0;
}


/*
 *功能：开启语音识别控制
 *返回值：成功返回0，失败返回-1
 *说明：这个函数只是打开语音中断开关（recongnition_switch），打开后在检测到声音后才进行录音，识别。
 */
int open_voice_recognition_control()
{
	if (voice.voice_main_switch == FALSE) //如果语音总开关没有打开，即语音初始化失败，禁止一切！
	{
		return 0;	
	}
	if (voice.recongnition_switch)	//如果语音中断中断开关已经打开，直接退出
	{
		return 0;
	}
	
	//设置线程分离属性，以分离状态启动线程，在线程结束后会自动释放所占用的系统资源
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_t th; //线程标识符
        int err;
        if ((err = pthread_create(&th, &attr, open_voice_recognition_control_th, (void*)0)) != 0)
        {
                perror("servo_pulse_v_down pthread create error");
        }
        pthread_attr_destroy(&attr); //销毁线程属性结构体	
	
}


/*
 * 功能：线程处理函数，在线程中关闭语音识别，降低延时
 * 参数：无
 * 返回值：无
 */
void* close_voice_recongnition_control_th(void *arg)
{		
	if (voice.recongnition_switch == TRUE)
	{
		voice.recongnition_switch = FALSE;
		close_voice_light();
	
		//创建子进程输出语音提示“语音识别已经开启”
		//kill_omxplayer(); //关掉正在播放的语音
		voice.sound_box_ongoing = TRUE;
		pid_t omxplayer_pid;
       		if ((omxplayer_pid = fork()) < 0)
        	{
			voice.sound_box_ongoing = FALSE;
                	show_sys_info("创建播放音频进程失败！\n");
			show_sys_info("语音识别控制已经关闭...\n");
                	return (void*)0;
       		}
        	else if (omxplayer_pid == 0)  //子进程执行录制音频
        	{       
                	show_sys_info("语音输出：正在说话...\n");
                	if (execlp("/bin/bash", "/bin/bash", "-c", "omxplayer --no-osd -o local /home/pi/VoiceRecognitionControlRobot/wav/sys_voice/closed_voice_recongnition.wav", NULL) < 0)
                	{       
                        	show_sys_info("播放声音进程执行失败！\n");
                	}
        	}
        	wait(0);
		voice.sound_box_ongoing = FALSE;
		show_sys_info("语音识别控制已经关闭...\n");
		return (void*)0;
	}

	return (void*)0;
}

/*
 * 
 * 在关闭信号中使用该函数
 */
void sys_close_voice_recongnition_control()
{
	if (voice.voice_main_switch == TRUE)
	{
		voice.recongnition_switch = FALSE;
		voice.voice_main_switch = FALSE;
		close_voice_light();
		MSPLogout();                                      //退出登录
		show_sys_info("语音功能已经关闭！\n");
		return ;
	}

	return ;
}


/*
 *功能：关闭语音识别控制
 */
int close_voice_recongnition_control()
{
	if (voice.voice_main_switch == FALSE)   //语音没有初始化成功，直接退出！
	{
		return 0;
	}

	if (voice.recongnition_switch == FALSE)  //语音中断开关已经关闭，直接退出！
	{
		return 0;
	}		
	
	sys_close_voice_recongnition_control(); //先在这里关闭吧，其实应该在信号中使用，因为sys.c文件还没移植过来，暂时放这里	
	/*
         * 开启线程关闭语音识别
         */
	
	//设置线程分离属性，以分离状态启动线程，在线程结束后会自动释放所占用的系统资源
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_t th; //线程标识符
        int err;
        if ((err = pthread_create(&th, &attr, close_voice_recongnition_control_th, (void*)0)) != 0)
        {
                perror("servo_pulse_v_down pthread create error");
        }
        pthread_attr_destroy(&attr); //销毁线程属性结构体
}
