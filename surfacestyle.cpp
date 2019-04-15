#include "surfacestyle.h"
#include "ui_surfacestyle.h"

SurfaceStyle::SurfaceStyle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SurfaceStyle)
{
    ui->setupUi(this);
}

SurfaceStyle::~SurfaceStyle()
{
    delete ui;
}
