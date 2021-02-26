#ifndef XRAY_H
#define XRAY_H

#include <QMainWindow>

namespace Ui {
class xray;
}

class xray : public QMainWindow
{
    Q_OBJECT

public:
    explicit xray(QWidget *parent = nullptr);
    ~xray();

private:
    Ui::xray *ui;
};

#endif // XRAY_H
