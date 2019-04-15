#include "videoplayer.h"
#include "ui_videoplayer.h"

#include <QPainter>
#include <QPaintEvent>
#include <QFileDialog>
#include <QDebug>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QSlider>

#include "videoslider.h"


Q_DECLARE_METATYPE(VideoPlayer_thread::PlayState)


VideoPlayer::VideoPlayer(QWidget *parent) :CustomTitle(parent),
    ui(new Ui::VideoPlayer)
{
    ui->setupUi(getContentDialog());
    av_register_all();

    if (SDL_Init(SDL_INIT_AUDIO)) {//初始化SDL系统
        fprintf(stderr,"Could not initialize SDL - %s. \n", SDL_GetError());
        exit(1);
    }

    //因为VideoPlayer::PlayerState是自定义的类型 要跨线程传递需要先注册一下
    qRegisterMetaType<VideoPlayer_thread::PlayState>();
    player = new VideoPlayer_thread;

    connect(player,SIGNAL(sig_TotalTimeChanged(qint64)),this,SLOT(slotTotalTimeChanged(qint64)));
    connect(player,SIGNAL(sig_StateChanged(VideoPlayer_thread::PlayState)),this,SLOT(slotStateChanged(VideoPlayer_thread::PlayState)));

    ui->verticalLayout_show_video->addWidget(player->getVideoWidget());
    player->getVideoWidget()->show();//显示控件

    mTimer = new QTimer; //定时器
    connect(mTimer,SIGNAL(timeout()),this,SLOT(slotTimerTimeOut()));
    mTimer->setInterval(500);

    connect(ui->stop_Btn,SIGNAL(clicked(bool)),this,SLOT(slotBtnClicked()));
    connect(ui->play_Btn,SIGNAL(clicked(bool)),this,SLOT(slotBtnClicked()));
    connect(ui->pause_Btn,SIGNAL(clicked(bool)),this,SLOT(slotBtnClicked()));
    connect(ui->open_Btn,SIGNAL(clicked(bool)),this,SLOT(slotBtnClicked()));
    connect(ui->toolButton_open,SIGNAL(clicked(bool)),this,SLOT(slotBtnClicked()));

    connect(ui->volume_slider,SIGNAL(valueChanged(int)),this,SLOT(slotSliderMoved(int)));

    connect(ui->horizontalSlider,SIGNAL(sig_valueChanged(int)),this,SLOT(slotSliderMoved(int)));

    resize(1024,768);
    setTitle("Video Player");

    ui->widget_video->hide();

    ui->pause_Btn->hide();
}

VideoPlayer::~VideoPlayer()
{
    delete ui;
}

void VideoPlayer::doClose()
{
    player->Stop(true);
    close();
}

void VideoPlayer::slotTotalTimeChanged(qint64 uSec)
{
    qint64 totalTime = uSec/1000000;

    ui->horizontalSlider->setRange(0,totalTime);

    QString mstr = QString("%1").arg(totalTime/60);
    QString sstr = QString("%1").arg(totalTime%60);

    QString str = QString("%1:%2").arg(mstr).arg(sstr);

    ui->totalTime->setText(str);
}

void VideoPlayer::slotStateChanged(VideoPlayer_thread::PlayState state)
{
    if(state == VideoPlayer_thread::playing){
        ui->widget->hide();
        ui->widget_video->show();

        ui->pause_Btn->show();
        ui->play_Btn->hide();

    }else if(state == VideoPlayer_thread::stop){
        ui->widget_video->hide();
        ui->widget->show();

        ui->pause_Btn->hide();
    }else if(state == VideoPlayer_thread::pause){
        ui->pause_Btn->hide();
        ui->play_Btn->show();

    }

}

void VideoPlayer::slotVolumeChanged(int value)
{

}

void VideoPlayer::slotBtnClicked(){

    if(QObject::sender() == ui->play_Btn){
        player->Play();

    }else if(QObject::sender() == ui->pause_Btn){
        player->Pause();

    }else if(QObject::sender() == ui->stop_Btn){
        player->Stop(true);

    }else if(QObject::sender() == ui->open_Btn){
        QString s = QFileDialog::getOpenFileName(
                   this, "Choose the flie which you want to play:",
                    "/",
                    "video files(*.flv *.rmvb *.avi *.MP4);; all files(*.*);; ");
        if(!s.isEmpty()){
            //打开文件
            player->Stop(true);
            player->setFileName(s);
            mTimer->start();
        }
        else {
            qDebug()<<"open fail";
        }

    }else if(QObject::sender() == ui->toolButton_open){
        QString s = QFileDialog::getOpenFileName(
                   this, "Choose the flie which you want to play:",
                    "/",
                    "video files(*.flv *.rmvb *.avi *.MP4);; all files(*.*);; ");
        if(!s.isEmpty()){
            //打开文件
            player->Stop(true);
            player->setFileName(s);
            mTimer->start();
        }
    }
}

void VideoPlayer::slotSliderMoved(int value){
    if(QObject::sender() == ui->horizontalSlider){
        player->seek((qint64 )value * 1000000);
    }else if(QObject::sender() == ui->volume_slider){
        player->volume((float)value/50.0);
        ui->volume_value->setText(QString("%1").arg(value));
    }
}

void VideoPlayer::slotTimerTimeOut(){
    if(QObject::sender() == mTimer){
        if(player->mPlayState == 2){
            ui->horizontalSlider->clear(0);
        }else{
        qint64 curr = player->getCurrentTime();

        ui->horizontalSlider->setValue(curr);

        QString mstr = QString("%1").arg(curr/60);
        QString sstr = QString("%1").arg(curr%60);

        QString str = QString("%1:%2").arg(mstr).arg(sstr);

        ui->currTime->setText(str);
        }

    }
}

