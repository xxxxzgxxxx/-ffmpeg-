/********************************************************************************
** Form generated from reading UI file 'videoplayer_showvideowidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIDEOPLAYER_SHOWVIDEOWIDGET_H
#define UI_VIDEOPLAYER_SHOWVIDEOWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_videoplayer_showvideowidget
{
public:

    void setupUi(QWidget *videoplayer_showvideowidget)
    {
        if (videoplayer_showvideowidget->objectName().isEmpty())
            videoplayer_showvideowidget->setObjectName(QStringLiteral("videoplayer_showvideowidget"));
        videoplayer_showvideowidget->resize(400, 300);

        retranslateUi(videoplayer_showvideowidget);

        QMetaObject::connectSlotsByName(videoplayer_showvideowidget);
    } // setupUi

    void retranslateUi(QWidget *videoplayer_showvideowidget)
    {
        videoplayer_showvideowidget->setWindowTitle(QApplication::translate("videoplayer_showvideowidget", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class videoplayer_showvideowidget: public Ui_videoplayer_showvideowidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEOPLAYER_SHOWVIDEOWIDGET_H
