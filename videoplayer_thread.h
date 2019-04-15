#ifndef VIDEOPAYER_THREAD_H
#define VIDEOPAYER_THREAD_H

#include <QThread>
#include <QImage>

extern "C"{
   #include <libavcodec/avcodec.h>
   #include <libavdevice/avdevice.h>
   #include <libavfilter/avfilter.h>
   #include <libavformat/avformat.h>
   #include <libavutil/avutil.h>
   #include <libswscale/swscale.h>
   #include <libswresample/swresample.h>

   #include <libavutil/time.h>
   #include <libavutil/pixfmt.h>

   #include <SDL.h>
   #include <SDL_config.h>
   #include <SDL_audio.h>
   #include <SDL_types.h>
   #include <SDL_main.h>
   #include <SDL_name.h>
}


#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

#include "videoplayer_showvideowidget.h"

// 播放器线程
class VideoPlayer_thread;

typedef struct PacketQueue{
    AVPacketList *first_pkt,*last_pkt;
    int nb_packet;
    int size;
    SDL_mutex *mutex;
    SDL_cond *cond;

}PacketQueue;

typedef struct VideoState{
    //视频文件信息
    AVFormatContext *video_FormatCtx;

    PacketQueue videoq;
    PacketQueue audioq;

    AVStream *video_s;
    AVStream *audio_s;

    int videoStream,audioStream;

    unsigned int audio_buf_index;  //音频缓冲索引
    unsigned int audio_buf_size;   //音频缓冲大小
    uint8_t *audio_buf;            //音频缓冲区
    DECLARE_ALIGNED(16,uint8_t,audio_buf2) [AVCODEC_MAX_AUDIO_FRAME_SIZE * 4];  //音频缓冲区2
    uint8_t *audio_packet_data;  //音频数据指针

    AVFrame *audio_frame; //用于存放解码后的音频数据
    AVPacket audio_packet; //音频包
    int audio_packet_size;  //包的音频大小

    int audio_src_freq ;
    int audio_trg_freq ;
    enum AVSampleFormat audio_src_fmt;
    enum AVSampleFormat audio_trg_fmt;
    int audio_src_channels;
    int audio_trg_channels;
    int64_t audio_src_channel_layout;
    int64_t audio_trg_channel_layout;

    int audio_hw_buf_size; //硬件缓冲大小

    struct SwrContext *swr_ctx;   //音频转码上下文

    double audio_clock;
    double video_clock;//当前视频已经播放的时钟

    bool pause;//暂停标志
    bool quit;
    bool readFinished; //文件读取完毕
    bool readThreadFinished;
    bool videoThreadFinished;

    int seek_req;//跳转标志
    int64_t seek_pos;//微秒
    int seek_time;  //和seek_pos一致  只是单位的区别
    int seek_audio_flag;
    int seek_video_flag;

    float volume; //声音的放大倍数
    bool isMutex; //静音的标志

    SDL_Thread *video_id;
    VideoPlayer_thread *videoPlayer;
    SDL_AudioDeviceID audio_id;

}VideoState;

class VideoPlayer_thread : public QThread
{
    Q_OBJECT
public:
    enum PlayState{
        playing,
        pause,
        stop
    };
    explicit VideoPlayer_thread();
    ~VideoPlayer_thread();

    bool Play();
    bool Pause();
    bool Stop(bool isWait = false); //参数表示是否等待所有的线程执行完毕再返回
    void displayVideo(QImage img);

    void seek(int64_t pos);

    void volume(float value);

    int64_t getTotalTime();
    double getCurrentTime();

    bool setFileName(QString path);

    int openSDL();

    QWidget *getVideoWidget() {return mVideoWidget;}
    PlayState mPlayState;


signals:
    void sig_GetOneFrame(QImage); // 每获取一帧图像，就发送此信号
    void sig_StateChanged(VideoPlayer_thread::PlayState);
    void sig_TotalTimeChanged(qint64 uSec);

protected:
    void run();


private:
    VideoState mVideoState;
    float mVolume;
    bool isMutex;
    QString mFileName;
    SDL_AudioDeviceID mAudioID;
    videoplayer_showvideowidget *mVideoWidget; //显示视频的控件

};

#endif // VIDEOPAYER_THREAD_H
