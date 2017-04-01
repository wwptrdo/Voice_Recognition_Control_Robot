#ifndef __VOICE_H_
#define __VOICE_H_

#include <pthread.h>
/*
 *声音模块
 */

#define VOICE_PIN 4           //蜂鸣器阵脚
#define VOICE_LIGHT_PIN  1    //语音控制指示灯，表示说话时灯亮，可以识别时灯灭
#define VOICE_DETECT_PIN 0    //检测有无声音发出的阵脚, 此针脚引发一个中断
#define RECORD_CMD_LEN 150
#define TRUE      1
#define FALSE     0


//科大讯飞语音读写需要
#define BUFFER_SIZE     4096
#define FRAME_LEN       640 
#define HINTS_SIZE      100

typedef struct {
	char open_record_voice_cmd[RECORD_CMD_LEN]; //保存录制音频的进程cmd
	int recongnition_switch;                    //标识当前是否打开中断进行语音识别
	int recongnition_ongoing;	            //标识当前正在进行语音识别控制
	char voice_recongnition_cmd[BUFFER_SIZE];   //音频转换成文字后保存到这里

}Voice;

/* 语音识别后智能回复的产生的wav文件：wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int		size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int		fmt_size;		// = 下一个结构体的大小 : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = 通道数 : 1
	int		samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	int		avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	short int       bits_per_sample;        // = 量化比特数: 8 | 16

	char            data[4];                // = "data";
	int		data_size;              // = 纯数据长度 : FileSize - 44 
} wave_pcm_hdr;

extern void voice_init(void);

extern void open_voice(void);

extern void close_voice(void);

extern int open_voice_recognition_control(void);

extern int close_voice_recongnition_control(void);

#endif
