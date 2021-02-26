#include "xray.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    xray w;
    w.show();

    return a.exec();
}
