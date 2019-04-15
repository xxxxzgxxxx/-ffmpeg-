#ifndef VIDEOPLAYER_SHOWVIDEOWIDGET_H
#define VIDEOPLAYER_SHOWVIDEOWIDGET_H

#include <QWidget>
#include <QPaintEvent>

namespace Ui {
class videoplayer_showvideowidget;
}

//显示视频用的控件
//重写一个显示控件是因为借用了他的界面  使用Qt播放会导致样式失效
class videoplayer_showvideowidget : public QWidget
{
    Q_OBJECT

public:
    explicit videoplayer_showvideowidget(QWidget *parent = 0);
    ~videoplayer_showvideowidget();

protected:
    QImage mImage;
    void paintEvent(QPaintEvent *event);

private:
    Ui::videoplayer_showvideowidget *ui;

public slots:
    void slotGetOneFrame(QImage img);
};

#endif // VIDEOPLAYER_SHOWVIDEOWIDGET_H
