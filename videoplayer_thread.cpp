#include "videoplayer_thread.h"
#include <QDebug>
#include <stdio.h>
#include <QImage>

///    队列の限制最大值
#define MAX_AUDIO_SIZE (25 * 16 * 1024)
#define MAX_VIDEO_SIZE (25 * 256 * 1024)

#define SDL_AUDIO_BUFFER_SIZE 1024
#define FLUSH_DATA "FLUSH"


//读取视频的线程
//解码视频的线程
//解码播放音频的线程

/**
   初始化队列
 */
void packet_queue_init(PacketQueue *q)
{
    memset(q,0,sizeof(PacketQueue));
    q->mutex = SDL_CreateMutex();
    q->cond = SDL_CreateCond();
    q->size = 0;
    q->nb_packet = 0;
    q->first_pkt = NULL;
    q->last_pkt = NULL;
}
/**
   填充队列
 */
int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    AVPacketList *pkt_list;
    if(av_dup_packet(pkt) < 0){
        return -1;
    }
    pkt_list = (AVPacketList *)av_malloc(sizeof(AVPacketList));
    if(!pkt_list){
        qDebug()<< "malloc filed! \n";
        return -1;
    }
    pkt_list->pkt = *pkt;
    pkt_list->next = NULL;

    SDL_LockMutex(q->mutex);
    if(!q->last_pkt){
        q->first_pkt = pkt_list;
    }else{
        q->last_pkt->next= pkt_list;
    }
    q->last_pkt = pkt_list;

    q->nb_packet++;
    q->size += pkt_list->pkt.size;

    SDL_CondSignal(q->cond);
    SDL_UnlockMutex(q->mutex);

    return 0;
}
/**
   取packet
 */
static int packet_queue_get(PacketQueue *q, AVPacket *pkt,int block)
{
    AVPacketList *pkt_list;
    int ret;
    //block 处理最初没有数据的情况
    //block 用来表示没有数据的时候是否阻塞线程等待
    SDL_LockMutex(q->mutex);

    while(1){
        pkt_list = q->first_pkt;
        if(pkt_list){
            q->first_pkt = pkt_list->next;
            if(!q->first_pkt){
                q->last_pkt = NULL;
            }
            q->nb_packet--;
            q->size -= pkt_list->pkt.size;
            *pkt = pkt_list->pkt;
            ret = 1;
            av_free(pkt_list);
            break;
        }else if(!block){
            ret = 0;
            break;
        }
        else{
//            处理阻塞的情况   cond在队列中有数据可读取的时候
            SDL_CondWait(q->cond,q->mutex);
        }
    }

    SDL_UnlockMutex(q->mutex);

    return ret;
}

static void  packet_queue_flush(PacketQueue *q){
    //为跳转操作服务   清空队列
    AVPacketList *pkt_list , *pkt1;

    SDL_LockMutex(q->mutex);

    for(pkt_list = q->first_pkt;pkt_list != NULL; pkt_list = pkt1){
        pkt1 = pkt_list->next;
        av_free_packet(&pkt_list->pkt);
        av_freep(&pkt_list);
    }

    q->first_pkt = NULL;
    q->last_pkt = NULL;
    q->nb_packet = 0;
    q->size = 0;

    SDL_UnlockMutex(q->mutex);
}

//synchronize_video函数是根据解码后的视频数据 计算出视频的pts
static double synchroninze_video(VideoState *is, AVFrame *src_frame, double pts){

    double frame_delay;

    ///大部分情况下pts都是有值的  pts是这一帧的播放时间
    /// video_clock 是当前视频播放的时间
    /// pts为0 ？  的情况？  判断
    if(pts != 0)
       is->video_clock = pts;
    else
        pts = is->video_clock;

    //更新video clock
    //帧延迟  frame_delay 是该帧显示完成需要的时间
    //这里还是  video_s 和codec的timebase 有什么区别
    frame_delay = av_q2d(is->video_s->codec->time_base);
    //总延迟 = 帧延迟 + 附加延迟
    //重复多帧的话要加上
    frame_delay += src_frame->repeat_pict*(frame_delay * 0.5);

    //此时的video_clock 是下一帧的显示时间
    is->video_clock +=frame_delay;

//    qDebug()<<pts;

    return pts;
}

//视频线程
int video_thread(void *arg)
{
    VideoState *is = (VideoState *)arg;


    AVCodecContext *pCodecCtx = is->video_s->codec;
    int get_picture;
    int ret;

    AVPacket pkt1 ,*pkt = &pkt1;

    AVFrame *pFrame;
    AVFrame *pFrameRGB;

    pFrame = av_frame_alloc();// 存放解码后的数据
    pFrameRGB = av_frame_alloc(); //存放转换后的数据

    uint8_t *picture_buffer_rgb ;
    int picture_bytes;
    struct SwsContext *img_format_convert;

    double video_pts = 0;
    double audio_pts = 0;

    // 转换格式准备
    img_format_convert = sws_getContext(pCodecCtx->width,pCodecCtx->height,pCodecCtx->pix_fmt,
                                        pCodecCtx->width,pCodecCtx->height,PIX_FMT_RGB32,
                                        SWS_BICUBIC,NULL,NULL,NULL);

    //这里是RGB格式的
    // 需要的内存大小
    picture_bytes = avpicture_get_size(PIX_FMT_RGB32,pCodecCtx->width,pCodecCtx->height);
    // 分配对应大小的字节
    picture_buffer_rgb = (uint8_t *)av_malloc(picture_bytes*sizeof(uint8_t));
    //  把帧和新申请的内存结合   绑定
    //  将rgb指向的数据填充到pFrameRGB内 ，但并没有拷贝，只是将pFrameRGB结构内的data指针指向了rgb的数据
    avpicture_fill((AVPicture *)pFrameRGB,picture_buffer_rgb,PIX_FMT_RGB32,pCodecCtx->width,pCodecCtx->height);

    while(1){
        //从视频队列中读取  解码
        if(is->quit == true){
            qDebug()<<"quit";
            break;
        }

        if(is->pause == true){
//            qDebug()<<"pause";
            SDL_Delay(10);
            continue;
        }

        if(packet_queue_get(&is->videoq,pkt,0) <= 0){
            if(is->readFinished){
                qDebug()<<"readFinished";
                break;
            }else{
                SDL_Delay(1);
                continue ;
            }

        }

        if(strcmp((char *)pkt->data,FLUSH_DATA) == 0){
            avcodec_flush_buffers(is->video_s->codec);
            av_free_packet(pkt);
            continue;
        }

        ret = avcodec_decode_video2(pCodecCtx,pFrame,&get_picture,pkt);
        if(ret < 0){
            qDebug()<<"decodec false";
            av_free_packet(pkt);
            continue;
        }

        if(pkt->dts == AV_NOPTS_VALUE  && pFrame->opaque &&*(uint64_t*) pFrame->opaque != AV_NOPTS_VALUE){
            video_pts = *(uint64_t*)pFrame->opaque;
        }else if(pkt->dts != AV_NOPTS_VALUE){
            video_pts = pkt->dts;
        }else{
            video_pts = 0;
        }
        // pts转换为时间戳  单位秒   计算一帧在整个视频的时间位置
        video_pts *= av_q2d(is->video_s->time_base);
        //计算视频pts
        video_pts = synchroninze_video(is,pFrame,video_pts);

        if(is->seek_video_flag){
            if(video_pts < is->seek_time){
                av_free_packet(pkt);
                continue;
            }else{
                is->seek_video_flag = 0;
            }
        }

        //视频线程超前  视频线程等待音频线程
        while(1){
            if(is->quit)
                break;
            //跳转的位置怎么记录的
            audio_pts = is->audio_clock;
            video_pts = is->video_clock;
            //等待匹配   两者时间匹配 跳出该循环
            if(video_pts <= audio_pts)
                break;
            int delay_time = (video_pts - audio_pts)*1000;
            delay_time = delay_time > 5 ? 5 : delay_time;
            SDL_Delay(delay_time);
        }


        if(get_picture){//转换格式为RGB
            sws_scale(img_format_convert,(const uint8_t* const*)pFrame->data,pFrame->linesize,
                      0,pFrame->height,pFrameRGB->data,pFrameRGB->linesize);
        }

        //QImage 根据RGB数据构造图像
        //之前用pFrameRGB->width 不行
        //试一下 pFormatCtx->width 即原始数据的宽
        // 之前转化的时候  sws_getContext  设置的宽高就是pCodecCtx的宽高  没有对图片进行压缩
        QImage img = QImage((uchar *)picture_buffer_rgb,pCodecCtx->width,pCodecCtx->height,
                            QImage::Format_RGB32);
        //给主线程发送信号，显示图像
        QImage image = img.copy();//复制一份数据
        is->videoPlayer->displayVideo(image); // 显示

        av_free_packet(pkt);

    }


    av_free(pFrame);
    av_free(pFrameRGB);
    av_free(picture_buffer_rgb);

    if(!is->quit)
        is->quit = true;
    is->videoThreadFinished = true;

    return 0;
}

//解码音频 做重采样处理
static int audio_decode_frame(VideoState *is, double *pts_ptr)
{
    int get_frame_ptr = 0;
    AVPacket *pkt = &is->audio_packet;
    int data_size;//解码的字节数
    int decode_data_size; //实际字节数
    int dec_channel_layout;
    int resamples_data_size; //转换后的字节数
    int re_nb_samples;

    double pts; //音频显示时间戳

    int wanted_nb_samples;

    for( ; ; ){
        //将一个包中的AVFrame全部解码完

        if(is->pause == true){
            return -1;
        }
        /*
        * 当AVPacket中装得是音频时，有可能一个AVPacket中有多个AVFrame，
        * 而某些解码器只会解出第一个AVFrame，这种情况我们必须循环解码出后续AVFrame
        */
        int i = 0;
//        qDebug() << "duration: "<< pkt->duration;
        while(is->audio_packet_size >0)
        {
//            qDebug()<< i++;
            if(is->pause == true){
                return -1;
            }

            //?????  important?  alloc real space!
            if (!is->audio_frame) {// 没有进行解码
                if (!(is->audio_frame = avcodec_alloc_frame())) {
                    return AVERROR(ENOMEM);
                }
            } else
                avcodec_get_frame_defaults(is->audio_frame);//将给定AVFrame的字段设置为默认值

            data_size = avcodec_decode_audio4(is->audio_s->codec,is->audio_frame,&get_frame_ptr,pkt);
            if(data_size < 0 ){//error
                is->audio_packet_size = 0;
                break ;
            }

            is->audio_packet_data += data_size;
            is->audio_packet_size -= data_size;

            if(!get_frame_ptr){
                //no frame没有解码的帧
                qDebug()<<"get no frame";
                continue ;
            }

            //有解码后的帧   计算解码后的帧所需要的缓冲大小(实际大小)
            decode_data_size = av_samples_get_buffer_size(NULL,is->audio_frame->channels,is->audio_frame->nb_samples,
                                                         (AVSampleFormat)is->audio_frame->format,1);
            //得到声道设计
            dec_channel_layout = (is->audio_frame->channel_layout &&
                                  is->audio_frame->channels ==
                                           av_get_channel_layout_nb_channels(is->audio_frame->channel_layout))
                                 ? is->audio_frame->channel_layout
                                 : av_get_channel_layout_nb_channels(is->audio_frame->channels) ;
            //获取采样数
            wanted_nb_samples = is->audio_frame->nb_samples;

            /**
            * 接下来判断我们之前设置SDL时设置的声音格式(AV_SAMPLE_FMT_S16)，声道布局，
            * 采样频率，每个AVFrame的每个声道采样数与得到的该AVFrame分别是否相同，
            * 如有任一不同，我们就需要swr_convert该AvFrame，重采样
            * 然后才能符合之前设置好的SDL的需要，才能播放
            */

            if(is->audio_frame->format != is->audio_src_fmt ||
                    dec_channel_layout != is->audio_src_channel_layout ||
                    is->audio_frame->sample_rate != is->audio_src_freq ||
                    wanted_nb_samples != is->audio_frame->nb_samples &&
                    !is->swr_ctx){
                //释放旧的采样上下文
                swr_free(&is->swr_ctx);
                //创建采样上下文
                is->swr_ctx = swr_alloc_set_opts(NULL,is->audio_trg_channel_layout,is->audio_trg_fmt,
                                                 is->audio_trg_freq,is->audio_frame->channel_layout,
                                                 (AVSampleFormat)is->audio_frame->format,is->audio_frame->sample_rate
                                                 ,0,NULL);
                if(!is->swr_ctx || swr_init(is->swr_ctx) < 0){
                    qDebug()<<"swr init fail";
                    swr_free(&is->swr_ctx);
                    break;
                }
                // 重新设置音频的声道数、声道格式等
                is->audio_src_fmt = is->audio_s->codec->sample_fmt;
                is->audio_src_freq = is->audio_s->codec->sample_rate;
                is->audio_src_channel_layout = dec_channel_layout;
                is->audio_src_channels = is->audio_s->codec->channels;
            }

            //重采样上下文存在  进行转换
            if(is->swr_ctx){
//                const uint8_t *in[] = { is->audio_frame->data[0] }
                const uint8_t **in =
                        (const uint8_t **) is->audio_frame->extended_data;
                uint8_t *out[] = { is->audio_buf2 };

                //通过样本数调整完成同步
                if (wanted_nb_samples != is->audio_frame->nb_samples) {
                    if (swr_set_compensation(is->swr_ctx,//激活重采样补偿（“软”补偿），设置参数
                            (wanted_nb_samples - is->audio_frame->nb_samples)
                                    * is->audio_trg_freq
                                    / is->audio_frame->sample_rate,
                            wanted_nb_samples * is->audio_trg_freq
                                    / is->audio_frame->sample_rate) < 0) {
                        //fprintf(stderr,"swr_set_compensation() failed\n");
                        break;
                    }
                }

                //convert  返回的是每个通道输出的样本数
                re_nb_samples = swr_convert(is->swr_ctx,out,
                                            (sizeof(is->audio_buf2)/is->audio_trg_channels)/
                                                                   av_get_bytes_per_sample(is->audio_trg_fmt),
                                            in,is->audio_frame->nb_samples);
                if(re_nb_samples <0){
                    qDebug()<<"convert failed!  \n";
                    break;
                }
                //剩余buffer太小了？
                if(re_nb_samples == (sizeof(is->audio_buf2)/is->audio_trg_channels)/
                                                            av_get_bytes_per_sample(is->audio_trg_fmt)){
                    if(swr_init(is->swr_ctx) < 0){
                        swr_free(&is->swr_ctx);
                    }
                }
                is->audio_buf = is->audio_buf2;
//                每声道采样数 x 声道数 x 每个采样字节数
                resamples_data_size = re_nb_samples * is->audio_trg_channels * av_get_bytes_per_sample(is->audio_trg_fmt);
//                qDebug() << pkt->pts - is->audio_frame->pts;

//                qDebug() << pkt->size;

            }
            else{
                //重采样上下文不存在   直接复制就可以
//                qDebug()<<"no resample";
                resamples_data_size = decode_data_size ;
                is->audio_buf = is->audio_frame->data[0];
            }

            pts = is->audio_clock;
            *pts_ptr = pts;
            //  每秒钟播放的字节数  每个通道大小*样本率*通道数
//            每个样本占2bytes   16bit
            is->audio_clock += (double)resamples_data_size /
                    (double)(2 * is->audio_s->codec->channels * is->audio_s->codec->sample_rate);

            qDebug()<< "regular clock : "<< is->audio_clock;
            if(is->seek_audio_flag){
                qDebug() << "clock : "<< is->audio_clock;
                qDebug() << "seek_time: "<< is->seek_time;
                if(is->audio_clock < is->seek_time){
                    av_free_packet(pkt);
					packet_queue_flush(&is->audioq);
                    break;
                }else{
                    is->seek_audio_flag = 0;
                }
            }
            return resamples_data_size;
        }


        if(pkt->data)
            av_free_packet(pkt);

        if(is->quit)
            return -1;

        memset(pkt, 0, sizeof(*pkt));

        if(packet_queue_get(&is->audioq, pkt, 0) <= 0){
//            qDebug()<<"1";
            continue;
        }

        if(strcmp((char *)pkt->data,FLUSH_DATA) == 0){
            avcodec_flush_buffers(is->audio_s->codec);
            av_free_packet(pkt);
            continue;
        }

        is->audio_packet_data = pkt->data;
        is->audio_packet_size = pkt->size;

        // 获取当前packet的时间
        if(pkt->pts != AV_NOPTS_VALUE){
            is->audio_clock = av_q2d(is->audio_s->time_base) * pkt->pts;
            qDebug()<< "packet pts : "<< is->audio_clock;
        }

    }
    return 0;
}

/**
 * userdata 就是SDL_AudioSpec结构体中的userdata字段
 * userdata就是回调函数使用的数据指针，stream 是我们要把声音数据写入的缓冲区指针，len 是缓冲区的大小
 * 回调函数  结果写入缓冲区
 */
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
    VideoState *is =(VideoState *)userdata;

    int len1;
    int audio_data_size;
    double pts;
    int audio_volume;

///      audio_buf_index 和audio_buf_size  现在缓存的数据  缓存区大小
///
         /*  audio_buf_index 和 audio_buf_size 标示我们自己用来放置解码出来的数据的缓冲区，*/
         /*   这些数据待copy到SDL缓冲区， 当audio_buf_index >= audio_buf_size的时候意味着我*/
         /*   们的缓冲为空，没有数据可供copy，这时候需要调用audio_decode_frame来解码出更
         /*   多的桢数据 */

//    audio_data_size  是解码出来的数据
//    audio_buf_index   目前播放的进度
//    audio_buf_size    目前缓冲区数据的大小
    while(len > 0){
        if(is->audio_buf_index >= is->audio_buf_size)
        {
            audio_data_size = audio_decode_frame(is,&pts);
            if(audio_data_size < 0 ){
                //没能解码出数据  默认播放静音
                is->audio_buf_size = 1024;
                /* 清零，静音 */
                if (is->audio_buf == NULL) {
                    qDebug()<<" callback error";
                    return;
                }
//                SDL_memset(stream,0x0,len);
                memset(is->audio_buf, 0, is->audio_buf_size);
            }
            else{
                is->audio_buf_size = audio_data_size;
            }
            is->audio_buf_index = 0;
        }

        //len1-stream 剩余可用空间   len-SDL内部缓冲区大小
//        如果缓冲区的内容比stream 缓冲区大的话，只放进缓冲区大小的内容

        len1 = is->audio_buf_size - is->audio_buf_index;
        if(len1  > len)
            len1 = len;

        if (is->audio_buf == NULL) return;
        audio_volume = SDL_MIX_MAXVOLUME * is->volume;
        SDL_memset(stream,0,len);
        SDL_MixAudioFormat(stream, (uint8_t *)(is->audio_buf + is->audio_buf_index),AUDIO_S16SYS, len1, audio_volume);
//        SDL_MixAudio(stream,(uint8_t *)(is->audio_buf + is->audio_buf_index),len1,audio_volume);

//        if(*stream)
//            qDebug()<<"has data";
//        else
//            qDebug()<<"no data";
//        memcpy(stream,(uint8_t *)is->audio_buf + is->audio_buf_index,len1);
        len -= len1;
        stream += len1;

        is->audio_buf_index += len1;
    }
}

//设置音频参数  找到编解码器  传送音频数据信息
int audio_stream_component_open(VideoState *videostate,int stream_index){

    AVFormatContext *audio_formatCtx = videostate->video_FormatCtx;
    AVCodecContext *audio_codecCtx;
    AVCodec *audio_codec;
    int wanted_nb_channels = 2;
    int64_t wanted_channel_layout = 0;

    if(stream_index < 0 || stream_index >= videostate->video_FormatCtx->nb_streams)
        return -1;

    //找到解码器  打开  设置基本参数
    audio_codecCtx = audio_formatCtx->streams[stream_index]->codec;
    if(!wanted_channel_layout ||
            wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout))
    {
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }

    //打开解码器
    audio_codec = avcodec_find_decoder(audio_codecCtx->codec_id);
    if(!audio_codec || avcodec_open2(audio_codecCtx,audio_codec,NULL) < 0){
        fprintf(stderr,"Unsupported codec!\n");
        return -1;
    }

    //丢弃大小为0的数据流  AVDISCARD
    videostate->video_FormatCtx->streams[stream_index]->discard = AVDISCARD_DEFAULT;

    //根据类型打开解码器
    switch(audio_codecCtx->codec_type)
    {
       case AVMEDIA_TYPE_AUDIO:
        videostate->audio_s = videostate->video_FormatCtx->streams[stream_index];
        videostate->audio_buf_index = 0;
        videostate->audio_buf_size = 0;
        memset(&videostate->audio_packet,0,sizeof(videostate->audio_packet));
        packet_queue_init(&videostate->audioq);
        break;
    default:
        break;
    }
    return 0;
}

VideoPlayer_thread::VideoPlayer_thread()
{

    mVideoWidget = new videoplayer_showvideowidget;

    mAudioID = 0;

    connect(this,SIGNAL(sig_GetOneFrame(QImage)),mVideoWidget,SLOT(slotGetOneFrame(QImage)));

    mPlayState = stop;
}
VideoPlayer_thread::~VideoPlayer_thread()
{
    if (mAudioID != 0)
    {
        SDL_LockAudioDevice(mAudioID);
//        SDL_PauseAudioDevice(mAudioID,1);
        SDL_CloseAudioDevice(mAudioID);
        SDL_UnlockAudioDevice(mAudioID);

        mAudioID = 0;
    }
}

bool VideoPlayer_thread::Play()
{
    mVideoState.pause = false;
    if(mPlayState != pause){
        return false;
    }
    mPlayState = playing;
    emit sig_StateChanged(playing);

    return true;
}

bool VideoPlayer_thread::Pause()
{
    mVideoState.pause = true;
    if(mPlayState != playing){
        return false;
    }
    mPlayState = pause;
    emit sig_StateChanged(pause);

    return true;
}

bool VideoPlayer_thread::Stop(bool isWait)
{
    //停止
    if(mPlayState == stop){
        return false;
    }
    mVideoState.quit = 1;
    mPlayState = stop;
    emit sig_StateChanged(stop);

    if(isWait){
        while(!mVideoState.readThreadFinished || !mVideoState.videoThreadFinished )
            SDL_Delay(10);
    }

    if(mVideoState.audio_id != 0){
        SDL_LockAudio();
        SDL_PauseAudioDevice(mVideoState.audio_id,1);
        SDL_UnlockAudio();
    }

    return true;
}

void VideoPlayer_thread::displayVideo(QImage img)
{
//    qDebug()<<"display";
    emit sig_GetOneFrame(img); // 提交获取图像信号
}

void VideoPlayer_thread::seek(int64_t pos)
{
    if(!mVideoState.seek_req){
//        qDebug()<<pos;
        mVideoState.seek_pos = pos;
        mVideoState.seek_req = 1;
    }
}

void VideoPlayer_thread::volume(float value)
{
    mVolume = value;
    mVideoState.volume = value;
}

int64_t VideoPlayer_thread::getTotalTime()
{
    return mVideoState.video_FormatCtx->duration;
}

double VideoPlayer_thread::getCurrentTime()
{
//    qDebug()<<mVideoState.audio_clock;
    return mVideoState.audio_clock;
}

bool VideoPlayer_thread::setFileName(QString path)
{
    if(mPlayState != stop){
        return false;
    }
    mFileName = path;

    mPlayState = playing;

    emit sig_StateChanged(playing);

    this->start();
    return true;
}

int VideoPlayer_thread::openSDL()
{
    VideoState *is = &mVideoState;

    //期望设置的属性wanted_spec和系统接受的属性spec
    SDL_AudioSpec wanted_spec,spec;
    int wanted_nb_channels = 2;
    int64_t wanted_channel_layout = 0;
    int sample_rate = 44100;

    // 根据期望的声道数 来获取期望的声道布局
    if(!wanted_channel_layout ||
            wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout))
    {
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }

    // 获取期望的声道数
//    wanted_nb_channels = audio_codecCtx->channels;
    //frep 采样率
    wanted_spec.freq = sample_rate;
    wanted_spec.channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
    if(wanted_spec.channels <= 0 || wanted_spec.freq <= 0)
        return -1;

    wanted_spec.format = AUDIO_S16SYS;
    //告诉SDL使用什么格式，S指代signed，16为样本16bit，SYS指代大小端由系统决定
    wanted_spec.silence = 0;
    wanted_spec.samples =SDL_AUDIO_BUFFER_SIZE;
    wanted_spec.userdata = is;
    wanted_spec.callback = audio_callback;

    /*  打开音频设备，这里使用一个while来循环尝试打开不同的声道数 */
    /*  (由上面next_nb_channels数组指定）直到成功打开，或者全部失败 */
    do{
        //设置  期望与实际不一样时 不能修改
        is->audio_id = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0,0),0,&wanted_spec,&spec,0);
    }while(is->audio_id < 0);

    mAudioID = is->audio_id;
    //检查实际使用的配置
    if(spec.format != AUDIO_S16SYS){
        fprintf(stderr,"SDL advised audio format %d is not supported!\n",spec.format);
        return -1;
    }

    // 看二者的channels是否相同  得到最终的wanted_channel_layout
    if(spec.channels != wanted_spec.channels){
//        qDebug()<<"different channels";
        wanted_channel_layout = av_get_default_channel_layout(spec.channels);
        if(!wanted_channel_layout){
            fprintf(stderr,"SDL advised channel count %d is not supported!\n",spec.channels);
            return -1;
        }
    }
    //spec.size 缓冲区的大小
    is->audio_hw_buf_size = spec.size;

    /*设置好的参数保存在大结构中*/
    is->audio_src_channels = is->audio_trg_channels = spec.channels;
    is->audio_src_fmt = is->audio_trg_fmt = AV_SAMPLE_FMT_S16;
    is->audio_src_freq = is->audio_trg_freq = spec.freq;
    is->audio_src_channel_layout = is->audio_trg_channel_layout = wanted_channel_layout;

    return 0;
}



void VideoPlayer_thread::run()
{
//    qDebug()<<"run";
    char file_path[1028] = {0};

    strcpy(file_path,mFileName.toUtf8().data());
    memset(&mVideoState,0,sizeof(VideoState)); //为了安全起见  先将结构体的数据初始化成0了

    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx,*aCodecCtx;
    AVCodec *pCodec,*aCodec;

    VideoState *is = &mVideoState;

//    找到视频文件

    is->volume = mVolume;
    pFormatCtx = avformat_alloc_context();

//     * @return 0 on success, a negative AVERROR on failure.
    if(avformat_open_input(&pFormatCtx,file_path,NULL,NULL)!=0){
        qDebug()<<"Cannot open the file!  \n";
        return ;
    }
//    * @return >=0 if OK, AVERROR_xxx on error
    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
        qDebug()<<"Cannot find stream information!  \n";
        return ;
    }

//    循环查找视频中流的信息
    int audioStream,videoStream,i;
    audioStream = -1;
    videoStream = -1;
    for(i = 0 ; i < pFormatCtx -> nb_streams ; i++){
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStream = i;
        }
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audioStream <0){
            audioStream = i;
        }
    }
    if(videoStream == -1){
        qDebug()<< "Didn't find a videoStream! \n";
        return ;
    }
    if(audioStream == -1){
        qDebug()<<"Didn't find a audioStream! \n";
        return ;
    }

    is->video_FormatCtx = pFormatCtx;
    is->audioStream = audioStream;
    is->videoStream = videoStream;

    emit sig_TotalTimeChanged(getTotalTime());//得到视频总时长

//    视频流  根据视频流信息找到解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL){
        qDebug()<<"Didn't find a codec! \n";
        return ;
    }
    if(avcodec_open2(pCodecCtx,pCodec,NULL) < 0){
        qDebug()<<"Cannot open video codec! \n";
        return ;
    }
    is->video_s = pFormatCtx->streams[videoStream];
    packet_queue_init(&is->videoq);

//    音频流   根据音频流信息找到解码器
    aCodecCtx = pFormatCtx ->streams[audioStream]->codec;
    aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
    if(aCodec == NULL){
        qDebug()<<"Didn't find a codec! \n";
        return ;
    }
    if(avcodec_open2(aCodecCtx,aCodec,NULL) < 0){
        qDebug()<<"Cannot open audio codec! \n";
        return ;
    }

//    读取视频解码视频
    is->video_id = SDL_CreateThread(video_thread,"video_thread",&mVideoState);
    is->videoPlayer = this;

    if(audioStream >= 0){
//        设置音频参数,打开播放静音
        openSDL();
        SDL_LockAudioDevice(mAudioID);
        SDL_PauseAudioDevice(mAudioID,0);
        SDL_UnlockAudioDevice(mAudioID);
//        函数的作用是找到我们的解码器，，保存重要信息到大结构体中
        audio_stream_component_open(&mVideoState,audioStream);
    }

    mPlayState = playing;
    emit sig_StateChanged(playing);

    while(1){
        if(is->quit == true){
            break;
        }
        if(is->seek_req){
            int stream_index;
            int64_t seek_target = is->seek_pos;

            if(is->videoStream >= 0){
                stream_index = is->videoStream;
            }else if(is->audioStream >= 0)
                stream_index = is->audioStream;
            //先转换时间
            //ffmpeg 内部时间基
            AVRational avRational = {1,AV_TIME_BASE};

            if(stream_index >= 0){
                seek_target = av_rescale_q(seek_target,avRational,
                                           is->video_FormatCtx->streams[stream_index]->codec->time_base);
//                qDebug()<<seek_target;
            }
            // 不是确定帧  是找到最近的关键帧
            //  AVSEEK_FLAG_BACKWARD
            if(av_seek_frame(is->video_FormatCtx,stream_index,seek_target,AVSEEK_FLAG_BACKWARD) < 0 ){
                qDebug()<<"seek false";
                fprintf(stderr, "%s: error while seeking\n",is->video_FormatCtx->filename);
            }else{
                // 清空队列  跳转
                if(is->audioStream >= 0){
//                    qDebug()<<"flush audio queue";
                    AVPacket *pkt2 = (AVPacket *)malloc(sizeof(AVPacket));
                    av_new_packet(pkt2,10);
                    strcpy((char *)pkt2->data,FLUSH_DATA);
                    packet_queue_flush(&is->audioq);
                    packet_queue_put(&is->audioq,pkt2);
//                    av_freep(&pkt2);
                }
                if(is->videoStream >= 0){
//                    qDebug()<<"flush video queue";
                    AVPacket *pkt3 = (AVPacket *)malloc(sizeof(AVPacket));
                    av_new_packet(pkt3,10);
                    strcpy((char *)pkt3->data,FLUSH_DATA);
                    packet_queue_flush(&is->videoq);
                    packet_queue_put(&is->videoq,pkt3);
//                    av_freep(&pkt3);
                    is->video_clock = 0;

                }
//                qDebug()<<"queue flush end";
            }
            is->seek_req = 0;
            is->seek_time = is->seek_pos/1000000.0;
            is->seek_audio_flag = 1;
            is->seek_video_flag = 1;
        }

        if(is->pause == true){
            SDL_Delay(10);
            continue;
        }

//        读取的时候限制队列的长度，当里面的数据超过某个范围的时候 就先等等
        if(is->videoq.size > MAX_VIDEO_SIZE || is->audioq.size > MAX_AUDIO_SIZE){
            SDL_Delay(10);
            continue ;
        }

		AVPacket *packet = (AVPacket *)malloc(sizeof(AVPacket));
        if(av_read_frame(pFormatCtx,packet) < 0){
            //  读取音频若干帧或者视频一帧
            is->readFinished = true;

            if(is->quit){
                break;
            }
            SDL_Delay(10);
            continue;
        }
//        解码视频 放入音频视频队列
        if(packet->stream_index == videoStream){
            packet_queue_put(&is->videoq,packet);
        }
        else if(packet->stream_index == audioStream){
            packet_queue_put(&is->audioq,packet);
        }
        else{
            qDebug()<<"EXIT QUEUE";
            av_free_packet(packet);
        }

    }
    //文件读取结束 跳出循环的情况
    //等待播放完毕
    while(!is->quit)
        SDL_Delay(100);
    if(mPlayState!= stop){
        Stop();
    }


    SDL_LockAudioDevice(mAudioID);
    SDL_PauseAudioDevice(mAudioID,1);
    SDL_UnlockAudioDevice(mAudioID);

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    is->readThreadFinished = true;
}

