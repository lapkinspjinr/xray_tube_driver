#include "xray.h"
#include "ui_xray.h"

xray::xray(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xray)
{
    ui->setupUi(this);
}

xray::~xray()
{
    delete ui;
}
