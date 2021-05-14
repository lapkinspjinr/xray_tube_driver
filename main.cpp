#include "xray.h"
//#include </usr/include/x86_64-linux-gnu/qt5/QtWidgets/QApplication>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    xray w;
    w.show();

    return a.exec();
}
