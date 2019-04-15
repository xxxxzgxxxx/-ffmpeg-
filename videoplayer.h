#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include "surfacestyle.h"
#include "CustomTitleWidget/customtitle.h"

#include "videoplayer_thread.h"
#include <QImage>
#include <QPaintEvent>
#include <QTimer>
#include <QPushButton>
#include "videoslider.h"

//播放器主界面
namespace Ui {
class VideoPlayer;
}

class VideoPlayer : public CustomTitle
{
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

protected:
    void doClose();

private slots:
    void slotTotalTimeChanged(qint64 uSec);
    void slotSliderMoved(int value);
    void slotTimerTimeOut();
    void slotBtnClicked();
    void slotStateChanged(VideoPlayer_thread::PlayState state);
    void slotVolumeChanged(int value);


private:
    Ui::VideoPlayer *ui;
    VideoPlayer_thread *player;
    QTimer *mTimer;

};

#endif // VIDEOPLAYER_H
