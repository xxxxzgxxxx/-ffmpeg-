#ifndef SURFACESTYLE_H
#define SURFACESTYLE_H

#include <QWidget>
#include <QFile>
#include <QtGui>
#include <QtCore>
#include <QDesktopWidget>
#include <QApplication>


namespace Ui {
class SurfaceStyle;
}

//样式

class SurfaceStyle : public QWidget
{
    Q_OBJECT

public:
    explicit SurfaceStyle(QWidget *parent = 0);
    ~SurfaceStyle();

    static void setStyle(const QString &styleName){
        QFile file(QString(":/image/%1.css").arg(styleName));
        file.open(QFile::ReadOnly);
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        qApp->setPalette(QPalette(QColor("#F0F0F0")));
    }


private:
    Ui::SurfaceStyle *ui;
};

#endif // SURFACESTYLE_H
