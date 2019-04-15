#include "videoplayer_showvideowidget.h"
#include "ui_videoplayer_showvideowidget.h"

#include <QPainter>
#include <QDebug>

videoplayer_showvideowidget::videoplayer_showvideowidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::videoplayer_showvideowidget)
{
    ui->setupUi(this);

}

videoplayer_showvideowidget::~videoplayer_showvideowidget()
{
    delete ui;
}

void videoplayer_showvideowidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,this->width(),this->height()); //先画成黑色

    if (mImage.size().width() <= 0) return;

    //将图像按比例缩放成和窗口一样大小
    QImage img = mImage.scaled(this->size(),Qt::KeepAspectRatio);

    int x = this->width() - img.width();
    int y = this->height() - img.height();

    x /= 2;
    y /= 2;

    painter.drawImage(QPoint(x,y),img); //画出图像
}

void videoplayer_showvideowidget::slotGetOneFrame(QImage img)
{
//    qDebug()<<"slot getOneFrame";
    mImage = img;
    update();
}
