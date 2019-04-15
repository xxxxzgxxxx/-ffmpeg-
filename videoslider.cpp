﻿#include <videoslider.h>

#include <QDebug>
#include <QTimer>
#include <QResizeEvent>
#include <QStyle>

#include "videoplayer.h"

VideoSlider::VideoSlider(QWidget *parent) :
    QSlider(parent)
{
    setMouseTracking(true);

    this->setOrientation(Qt::Horizontal);

    isSliderMoving = false;

    m_timer = new QTimer;
    m_timer->setInterval(100);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimerTimeOut()));

    m_timer_mousemove = new QTimer;
    m_timer_mousemove->setInterval(100);
    connect(m_timer_mousemove,SIGNAL(timeout()),this,SLOT(slotMousemoveTimerTimeOut()));
}

VideoSlider::~VideoSlider()
{

}

void VideoSlider::mouseReleaseEvent(QMouseEvent *event)
{
//    emit sig_valueChanged(this->value());
//    isSliderMoving = false;
//    QSlider::mouseReleaseEvent(event);
}

void VideoSlider::resizeEvent(QResizeEvent *event)
{
//    qDebug()<<"void MySlider::resizeEvent(QResizeEvent *event)";
}

void VideoSlider::mousePressEvent(QMouseEvent *event)
{
    int posX = event->pos().x();

    int w = this->width();

    int value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().x(), width());

    setValue(value);

//    qDebug()<<value<<maximum();

    emit sig_valueChanged(value);
}

void VideoSlider::mouseMoveEvent(QMouseEvent *event)
{
    m_posX = event->pos().x();

    m_timer_mousemove->stop();
    m_timer_mousemove->start();

    QSlider::mouseMoveEvent(event);

}

void VideoSlider::enterEvent(QEvent *)
{
    m_timer->stop();
}

void VideoSlider::leaveEvent(QEvent *)
{
    m_timer_mousemove->stop();
    m_timer->start();
}

bool VideoSlider::seek()
{
    return true;
}

void VideoSlider::clear(int time)
{
    QSlider::setValue(time);
}

void VideoSlider::setValue(int value)
{
    if (!isSliderMoving)
    QSlider::setValue(value);
}

bool VideoSlider::openFile(char*fileName)
{
    return true;
}

bool VideoSlider::closeFile()
{
    return 0;
}

void VideoSlider::slotMousemoveTimerTimeOut()
{

    m_timer_mousemove->stop();

    int w = this->width();
    QPoint point = this->mapToGlobal(QPoint(0,0));
    int width = this->width();

    if (parent() != NULL)
    {
        QWidget *widget = (QWidget *)this->parent();
        point = widget->mapToGlobal(QPoint(0,0));

        width = widget->width();
    }
}
void VideoSlider::slotTimerTimeOut()
{

}
