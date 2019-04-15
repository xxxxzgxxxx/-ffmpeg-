#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QWidget>
#include <QSlider>

namespace Ui {
class VideoSlider;
}

class VideoSlider : public QSlider
{
    Q_OBJECT
public:
    explicit VideoSlider(QWidget *parent = 0);
    ~VideoSlider();

    bool openFile(char*fileName);
    bool closeFile();
    void setValue(int value);
    bool seek();
    void clear(int time);

signals:
    void sig_clicked(qint64 mSec);
    void sig_setStart(qint64 mSec);
    void sig_setEnd(qint64 mSec);

    void sig_valueChanged(int value);

protected:
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    QTimer * m_timer_mousemove;
    QTimer * m_timer;

    int m_posX;

    bool isSliderMoving;

public slots:
    void slotMousemoveTimerTimeOut();
    void slotTimerTimeOut();
};

#endif // VIDEOSLIDER_H
