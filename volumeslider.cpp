#include "volumeslider.h"


VolumeSlider::VolumeSlider(QWidget *parent):
    QSlider(parent)
{
    setMouseTracking(true);
    this->setOrientation(Qt::Horizontal);

}

VolumeSlider::~VolumeSlider()
{

}

void VolumeSlider::mousePressEvent(QMouseEvent *event)
{

}

void VolumeSlider::mouseMoveEvent(QMouseEvent *event)
{

}

void VolumeSlider::mouseReleaseEvent(QMouseEvent *event)
{

}
