#ifndef XRAY_H
#define XRAY_H

#include <QMainWindow>
#include <QDoubleValidator>
#include <QString>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QLCDNumber>
#include <QMessageBox>
#include <QState>
#include <QSerialPortInfo>

#include "uc_tube.h"

namespace Ui {
class xray;
}

class xray : public QMainWindow
{
    Q_OBJECT

public:
    explicit xray(QWidget *parent = nullptr);
    ~xray();
    double value_voltage;
    double value_current;
    void setDevVoltage(double voltage);
    void setDevCurrent(double current);
    bool xray_enable;

private:
    Ui::xray *ui;

    void connectWidget();

    UC_tube *  tube;
    QThread * second_thread;
    QTimer * timer;
    QTimer * reset_timer;

    QTime *timeNow;
    /*Validators*/
    QDoubleValidator input_doubleValidatorV;
    QDoubleValidator input_doubleValidatorI;

    /*Labels*/
    QLabel *labelVoltage;
    QLabel *labelCurrent;
    QLabel *labelV;
    QLabel *labelC;

    /*Editors*/
    QLineEdit *lineVoltage;
    QLineEdit *lineCurrent;

    /*Buttons*/
    QPushButton *pushButton_XrayOn;

    /*QLCD*/
    QLCDNumber *lcdNumber_Voltage;
    QLCDNumber *lcdNumber_Current;

    /*Veclors*/
    QVector<double> qv_voltage, qv_current, qv_time;

    /*Graphics*/
  //  QCustomPlot *plot_voltage;
  //  QCustomPlot *plot_current;
  //  QCPGraph *graphic;

    void U_find_ports();

signals :
    void US_write_settings(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl);
    void US_connect_port();
    void US_disconnect_port();
    void US_start();
    void US_initialize();
    void US_fault_reset();
    void US_fault_reset_standby();
    void US_set_voltage(double voltage);
    void US_set_current(double current);
    void US_test_data(QByteArray data);
    void US_x_ray_enable(bool enable);

private slots:
    void on_lineVoltage_returnPressed();
    void on_lineCurrent_returnPressed();
    void setUserVoltage();
    void setUserCurrent();
    void displayVoltage(double v);
    void displayCurrent(double c);
    void displayXrayOn(bool enable);
    void displayReady(bool ready);
    void displayArc(bool arc);
    void displayOvervoltage(bool overvoltage);
    void displayOvercurrent(bool overcurrent);
    void display24Vsupply(double V);
    void displayInterlock(double V);

    void addPoint(double v, double c, double t);
    void plot();


    void on_pushButtonQuit_released();
    void on_pushButton_XrayOn_clicked();
    void on_pushButtonFaultReset_released();
    void on_pushButton_clicked();

    void U_add_text(QString str);
    void U_print_test_data(QString str);
    void U_fault_xray_disable();
    void U_fault_reset_standby();

    void on_pushButtonFaultReset_clicked();
    void on_pushButton_2_clicked();
    void on_test_pushButton_clicked();
};

#endif // XRAY_H
