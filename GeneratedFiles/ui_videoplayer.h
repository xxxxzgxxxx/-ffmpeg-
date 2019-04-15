/********************************************************************************
** Form generated from reading UI file 'videoplayer.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIDEOPLAYER_H
#define UI_VIDEOPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "videoslider.h"

QT_BEGIN_NAMESPACE

class Ui_VideoPlayer
{
public:
    QVBoxLayout *verticalLayout_4;
    QWidget *widget_container;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QToolButton *toolButton_open;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widget_video;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout_show_video;
    QWidget *widget_controller;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_7;
    VideoSlider *horizontalSlider;
    QLabel *currTime;
    QLabel *label;
    QLabel *totalTime;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *open_Btn;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *play_Btn;
    QPushButton *pause_Btn;
    QPushButton *stop_Btn;
    QSpacerItem *horizontalSpacer_8;
    QLabel *volume;
    VideoSlider *volume_slider;
    QLabel *volume_value;

    void setupUi(QWidget *VideoPlayer)
    {
        if (VideoPlayer->objectName().isEmpty())
            VideoPlayer->setObjectName(QStringLiteral("VideoPlayer"));
        VideoPlayer->resize(1060, 699);
        VideoPlayer->setMinimumSize(QSize(1060, 0));
        verticalLayout_4 = new QVBoxLayout(VideoPlayer);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        widget_container = new QWidget(VideoPlayer);
        widget_container->setObjectName(QStringLiteral("widget_container"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_container->sizePolicy().hasHeightForWidth());
        widget_container->setSizePolicy(sizePolicy);
        widget_container->setStyleSheet(QString::fromUtf8("QWidget#widget_container\n"
"{\n"
"	background-color: rgb(22, 22, 22);\n"
"	border:1px solid rgba(0, 0, 0, 30);\n"
"	border-radius:2px;  /*\350\276\271\346\241\206\346\213\220\350\247\222*/\n"
"}\n"
""));
        verticalLayout_3 = new QVBoxLayout(widget_container);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(-1, -1, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(-1, 0, 0, -1);
        widget = new QWidget(widget_container);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        widget->setStyleSheet(QLatin1String("QWidget#widget_showopen\n"
"{\n"
"	\n"
"	background-color: rgba(60, 65, 68,100);\n"
"}"));
        verticalLayout_5 = new QVBoxLayout(widget);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        toolButton_open = new QToolButton(widget);
        toolButton_open->setObjectName(QStringLiteral("toolButton_open"));
        toolButton_open->setMinimumSize(QSize(160, 50));

        horizontalLayout_3->addWidget(toolButton_open);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_5->addLayout(horizontalLayout_3);


        verticalLayout->addWidget(widget);

        widget_video = new QWidget(widget_container);
        widget_video->setObjectName(QStringLiteral("widget_video"));
        sizePolicy.setHeightForWidth(widget_video->sizePolicy().hasHeightForWidth());
        widget_video->setSizePolicy(sizePolicy);
        widget_video->setStyleSheet(QStringLiteral(""));
        verticalLayout_2 = new QVBoxLayout(widget_video);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_show_video = new QVBoxLayout();
        verticalLayout_show_video->setObjectName(QStringLiteral("verticalLayout_show_video"));

        verticalLayout_2->addLayout(verticalLayout_show_video);


        verticalLayout->addWidget(widget_video);


        horizontalLayout_6->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout_6);

        widget_controller = new QWidget(widget_container);
        widget_controller->setObjectName(QStringLiteral("widget_controller"));
        widget_controller->setMinimumSize(QSize(0, 60));
        widget_controller->setStyleSheet(QLatin1String("QWidget#widget_controller\n"
"{\n"
"	background-color: rgba(60, 65, 68,100);\n"
"}"));
        verticalLayout_7 = new QVBoxLayout(widget_controller);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalSlider = new VideoSlider(widget_controller);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setCursor(QCursor(Qt::PointingHandCursor));
        horizontalSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(horizontalSlider);

        currTime = new QLabel(widget_controller);
        currTime->setObjectName(QStringLiteral("currTime"));
        currTime->setStyleSheet(QString::fromUtf8("QLabel{\n"
"	border-radius:0px;\n"
"	color: #F0F0F0;\n"
"	background-color:rgba(0,0,0,0);\n"
"	border-style:none;\n"
"font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"font-size:20px;\n"
"font-weight:bold;\n"
"}\n"
""));

        horizontalLayout_7->addWidget(currTime);

        label = new QLabel(widget_controller);
        label->setObjectName(QStringLiteral("label"));
        label->setStyleSheet(QString::fromUtf8("QLabel{\n"
"	border-radius:0px;\n"
"	color: #F0F0F0;\n"
"	background-color:rgba(0,0,0,0);\n"
"	border-style:none;\n"
"font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"font-size:20px;\n"
"font-weight:bold;\n"
"}\n"
""));

        horizontalLayout_7->addWidget(label);

        totalTime = new QLabel(widget_controller);
        totalTime->setObjectName(QStringLiteral("totalTime"));
        totalTime->setStyleSheet(QString::fromUtf8("QLabel{\n"
"	border-radius:0px;\n"
"	color: #F0F0F0;\n"
"	background-color:rgba(0,0,0,0);\n"
"	border-style:none;\n"
"font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"font-size:20px;\n"
"font-weight:bold;\n"
"}\n"
""));

        horizontalLayout_7->addWidget(totalTime);


        horizontalLayout_5->addLayout(horizontalLayout_7);


        verticalLayout_7->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        open_Btn = new QPushButton(widget_controller);
        open_Btn->setObjectName(QStringLiteral("open_Btn"));
        open_Btn->setMinimumSize(QSize(36, 36));
        open_Btn->setCursor(QCursor(Qt::PointingHandCursor));
        open_Btn->setStyleSheet(QLatin1String("QPushButton{ \n"
"image: url(:/src/image/open_normal.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:hover{ \n"
"image: url(:/src/image/open_focus.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:pressed{ \n"
"image: url(:/src/image/open_normal.png);\n"
"border-radius:0px; \n"
"}\n"
""));

        horizontalLayout_8->addWidget(open_Btn);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_7);

        play_Btn = new QPushButton(widget_controller);
        play_Btn->setObjectName(QStringLiteral("play_Btn"));
        play_Btn->setMinimumSize(QSize(50, 50));
        play_Btn->setCursor(QCursor(Qt::PointingHandCursor));
        play_Btn->setStyleSheet(QLatin1String("QPushButton{ \n"
"image: url(:/src/image/start_normal.png);\n"
"border-radius:0px; \n"
"}  \n"
"QPushButton:hover{ \n"
"image: url(:/src/image/start_focus.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:pressed{ \n"
"image: url(:/src/image/start_normal.png);\n"
"border-radius:0px; \n"
"}\n"
""));

        horizontalLayout_8->addWidget(play_Btn);

        pause_Btn = new QPushButton(widget_controller);
        pause_Btn->setObjectName(QStringLiteral("pause_Btn"));
        pause_Btn->setMinimumSize(QSize(50, 50));
        pause_Btn->setCursor(QCursor(Qt::PointingHandCursor));
        pause_Btn->setStyleSheet(QLatin1String("QPushButton{ \n"
"image: url(:/src/image/pause_normal.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:hover{ \n"
"image: url(:/src/image/pause_focus.png);\n"
"border-radius:0px; \n"
"} \n"
"QPushButton:pressed{ \n"
"image: url(:/src/image/pause_normal.png);\n"
"border-radius:0px; \n"
"}\n"
""));

        horizontalLayout_8->addWidget(pause_Btn);

        stop_Btn = new QPushButton(widget_controller);
        stop_Btn->setObjectName(QStringLiteral("stop_Btn"));
        stop_Btn->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_8->addWidget(stop_Btn);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_8);

        volume = new QLabel(widget_controller);
        volume->setObjectName(QStringLiteral("volume"));
        volume->setMinimumSize(QSize(40, 40));
        volume->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));

        horizontalLayout_8->addWidget(volume);

        volume_slider = new VideoSlider(widget_controller);
        volume_slider->setObjectName(QStringLiteral("volume_slider"));
        volume_slider->setOrientation(Qt::Horizontal);

        horizontalLayout_8->addWidget(volume_slider);

        volume_value = new QLabel(widget_controller);
        volume_value->setObjectName(QStringLiteral("volume_value"));
        volume_value->setStyleSheet(QString::fromUtf8("QLabel{\n"
"	border-radius:0px;\n"
"	color: #F0F0F0;\n"
"	background-color:rgba(0,0,0,0);\n"
"	border-style:none;\n"
"font: 16px \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"font-weight:bold;\n"
"}\n"
""));

        horizontalLayout_8->addWidget(volume_value);


        verticalLayout_7->addLayout(horizontalLayout_8);


        verticalLayout_3->addWidget(widget_controller);


        verticalLayout_4->addWidget(widget_container);


        retranslateUi(VideoPlayer);

        QMetaObject::connectSlotsByName(VideoPlayer);
    } // setupUi

    void retranslateUi(QWidget *VideoPlayer)
    {
        VideoPlayer->setWindowTitle(QApplication::translate("VideoPlayer", "\346\222\255\346\224\276\345\231\250", 0));
        toolButton_open->setText(QApplication::translate("VideoPlayer", "\346\211\223\345\274\200\346\226\207\344\273\266", 0));
        currTime->setText(QApplication::translate("VideoPlayer", "00:00:00", 0));
        label->setText(QApplication::translate("VideoPlayer", "/", 0));
        totalTime->setText(QApplication::translate("VideoPlayer", "00:00:00", 0));
        open_Btn->setText(QString());
        play_Btn->setText(QString());
        pause_Btn->setText(QString());
        stop_Btn->setText(QApplication::translate("VideoPlayer", "\345\201\234\346\255\242", 0));
        volume->setText(QApplication::translate("VideoPlayer", "\351\237\263\351\207\217:", 0));
        volume_value->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class VideoPlayer: public Ui_VideoPlayer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEOPLAYER_H
