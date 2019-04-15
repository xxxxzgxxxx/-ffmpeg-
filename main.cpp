#include "videoplayer.h"

#include <QApplication>
#include <surfacestyle.h>
#include <myhelper.h>

#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myHelper::SetUTF8Code();
    myHelper::SetStyle("gray");//灰色风格

    VideoPlayer w;
    w.show();

    return a.exec();
}
