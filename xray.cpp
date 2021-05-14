#include "xray.h"
#include "ui_xray.h"
#include "uc_tube.h"

xray::xray(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xray),
    input_doubleValidatorV(0,120,2,this),
    input_doubleValidatorI(0,350,2,this)
{
    ui->setupUi(this);

    input_doubleValidatorV.setNotation(QDoubleValidator::StandardNotation);
    input_doubleValidatorI.setNotation(QDoubleValidator::StandardNotation);
    ui->lineVoltage->setValidator(&input_doubleValidatorV);
    ui->lineCurrent->setValidator(&input_doubleValidatorI);

    ui->labelReadyBox->setStyleSheet("QLabel { background-color : black;}");
    ui->labelOvervoltageBox->setStyleSheet("QLabel { background-color : black;}");
    ui->labelOvercurrentBox->setStyleSheet("QLabel { background-color : black;}");
    ui->labelArcBox->setStyleSheet("QLabel { background-color : black;}");
    ui->label24VsupplyBox->setStyleSheet("QLabel { background-color : black;}");
    ui->labelInterlockBox->setStyleSheet("QLabel { background-color : black;}");
    ui->labelXrayOnBox->setStyleSheet("QLabel { background-color : black;}");

    tube = new UC_tube();

    //QCustomPlot
    // configure bottom axis to show date instead of number:
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("hh:mm");
    ui->plot_voltage->xAxis->setTicker(dateTicker);
    ui->plot_current->xAxis->setTicker(dateTicker);
    ui->plot_voltage->xAxis->setTickLabels(false);
    ui->plot_current->xAxis->setTickLabels(false);

    ui->plot_voltage->addGraph();
    ui->plot_voltage->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plot_voltage->graph(0)->setLineStyle(QCPGraph::lsLine);

    ui->plot_current->addGraph();
    ui->plot_current->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plot_current->graph(0)->setLineStyle(QCPGraph::lsLine);

    second_thread = new QThread();

    tube->moveToThread(second_thread);
    tube->port.moveToThread(second_thread);

    connectWidget();

    second_thread->start();

    U_find_ports();

}

xray::~xray()
{
    second_thread->quit();

    delete ui;
}

void xray::connectWidget()
{
    connect(ui->lineVoltage, SIGNAL(returnPressed()), this, SLOT(on_lineVoltage_returnPressed()), Qt::DirectConnection);
    connect(ui->lineCurrent, SIGNAL(returnPressed()), this, SLOT(on_lineCurrent_returnPressed()), Qt::DirectConnection);
    connect(ui->lineVoltage, SIGNAL(returnPressed()), this, SLOT(setUserVoltage()), Qt::DirectConnection);
    connect(ui->lineCurrent, SIGNAL(returnPressed()), this, SLOT(setUserCurrent()), Qt::DirectConnection);
    connect(ui->pushButtonFaultReset, SIGNAL(released()), this, SLOT(on_pushButtonFaultReset_released()), Qt::DirectConnection);
    connect(tube, SIGNAL(US_voltage_data(double)), this, SLOT(displayVoltage(double)), Qt::DirectConnection);
    connect(tube, SIGNAL(US_current_data(double)), this, SLOT(displayCurrent(double)), Qt::DirectConnection);
    connect(tube, SIGNAL(US_ready_status_data(bool)), this, SLOT(displayReady(bool)), Qt::DirectConnection);
    connect(tube, SIGNAL(US_overvoltage_status_data(bool)), this, SLOT(displayOvervoltage(bool)), Qt::DirectConnection);
    connect(tube, SIGNAL(US_overcurrent_status_data(bool)), this, SLOT(displayOvercurrent(bool)), Qt::DirectConnection);
    connect(tube, SIGNAL(US_arc_status_data(bool)), this, SLOT(displayArc(bool)), Qt::DirectConnection);
    connect(tube, SIGNAL(US_interlock_voltage_data(double)), this, SLOT(displayInterlock(double)), Qt::DirectConnection);
    connect(tube, SIGNAL(US_input_line_voltage_data(double)), this, SLOT(display24Vsupply(double)), Qt::DirectConnection);

   // connect(ui->pushButton_XrayOn, SIGNAL(clicked())), this, SLOT(on_pushButton_XrayOn_clicked()), Qt::DirectConnection);
    connect(tube,SIGNAL(US_xray_status_data(bool)),this,SLOT(displayXrayOn(bool)));



    connect(second_thread,  SIGNAL(started()),                                              tube,           SLOT(U_start()),                                            Qt::DirectConnection);
    connect(tube,           SIGNAL(US_finished()),                                          second_thread,  SLOT(quit()),                                               Qt::DirectConnection);
    connect(second_thread,  SIGNAL(finished()),                                             tube,           SLOT(deleteLater()),                                        Qt::DirectConnection);
    connect(second_thread,  SIGNAL(finished()),                                             &tube->port,    SLOT(deleteLater()),                                        Qt::DirectConnection);
    connect(tube,           SIGNAL(US_finished()),                                          second_thread,  SLOT(deleteLater()),                                        Qt::DirectConnection);
    connect(this,           SIGNAL(US_write_settings(QString, int, int, int, int, int)),    tube,           SLOT(U_write_settings(QString, int, int, int, int, int)),   Qt::DirectConnection);
    connect(this,           SIGNAL(US_connect_port()),                                      tube,           SLOT(U_connect_port()),                                     Qt::DirectConnection);
    connect(this,           SIGNAL(US_disconnect_port()),                                   tube,           SLOT(U_disconnect_port()),                                  Qt::DirectConnection);
    connect(this,           SIGNAL(US_initialize()),                                        tube,           SLOT(U_initialization()),                                   Qt::DirectConnection);
    connect(this,           SIGNAL(US_set_voltage(double)),                                 tube,           SLOT(U_set_voltage(double)),                                Qt::DirectConnection);
    connect(this,           SIGNAL(US_set_current(double)),                                 tube,           SLOT(U_set_current(double)),                                Qt::DirectConnection);
    connect(tube,           SIGNAL(US_error_port(QString)),                                 this,           SLOT(U_add_text(QString)),                                  Qt::DirectConnection);
    connect(this,           SIGNAL(US_fault_reset()),                                       tube,           SLOT(U_fault_reset()),                                      Qt::DirectConnection);
}

void xray::on_lineVoltage_returnPressed()
{
    //ui->lcdNumber_Voltage->display(ui->lineVoltage->text());
    ui->labelV->setText(ui->lineVoltage->text());
}

void xray::on_lineCurrent_returnPressed()
{
    //ui->lcdNumber_Current->display(ui->lineCurrent->text());
    ui->labelC->setText(ui->lineCurrent->text());
}

void xray::setDevVoltage(double voltage)
{
    emit US_set_voltage(voltage);
}

void xray::setDevCurrent(double current)
{
    emit US_set_current(current);
}

void xray:: setUserVoltage()
{
    value_voltage=lineVoltage->text().toDouble();
    setDevVoltage(value_voltage);
}

void xray::setUserCurrent()
{
    value_current=lineCurrent->text().toDouble();
    setDevCurrent(value_current);
}

void xray::displayVoltage(double v)
{
    ui->lcdNumber_Voltage->display(v);
}

void xray::displayCurrent(double c)
{
    ui->lcdNumber_Current->display(c);
}

void xray::on_pushButtonQuit_released()
{
    int n = QMessageBox::warning(this, "Quit", "Are you sure?", QMessageBox::Yes | QMessageBox::No);
    if (n == QMessageBox::Yes) {
        this->close();
    }
}

void xray::displayXrayOn(bool enable)
{
    if (enable) ui->labelXrayOnBox->setStyleSheet("QLabel { background-color : green;}");
    else ui->labelXrayOnBox->setStyleSheet("QLabel { background-color : red;}");
}

void xray::on_pushButton_XrayOn_clicked()
{
    if (ui->pushButton_XrayOn->isChecked()) {
        ui->pushButton_XrayOn->setText("X-ray Off");
        xray_enable = true;
    }
    else {
        ui->pushButton_XrayOn->setText("X-ray On");
        xray_enable = false;
    }
    tube->U_x_ray_enable(xray_enable);
    //draw plots
    //addPoint(tube->U_voltage_request(),tube->U_current_request();timeNow->currentTime());
    //plot();
}

void xray::displayReady(bool ready)
{
    if (ready) ui->labelReadyBox->setStyleSheet("QLabel { background-color : green;}");
    else  ui->labelReadyBox->setStyleSheet("QLabel { background-color : red;}");
}

void xray::displayOvervoltage(bool overvoltage)
{
    if (overvoltage) ui->labelOvervoltageBox->setStyleSheet("QLabel { background-color : green;}");
    else  ui->labelOvervoltageBox->setStyleSheet("QLabel { background-color : red;}");
}

void xray::displayOvercurrent(bool overcurrent)
{
    if (overcurrent) ui->labelOvercurrentBox->setStyleSheet("QLabel { background-color : green;}");
    else  ui->labelOvercurrentBox->setStyleSheet("QLabel { background-color : red;}");
}

void xray::displayArc(bool arc)
{
    if (arc) ui->labelArcBox->setStyleSheet("QLabel { background-color : green;}");
    else  ui->labelArcBox->setStyleSheet("QLabel { background-color : red;}");
}

void xray::display24Vsupply(double V)
{
    if (V>=32.55) ui->label24VsupplyBox->setStyleSheet("QLabel { background-color : green;}");
    else  ui->label24VsupplyBox->setStyleSheet("QLabel { background-color : red;}");
}

void xray::displayInterlock(double V)
{
    if (V>=15) ui->labelInterlockBox->setStyleSheet("QLabel { background-color : green;}");
    else  ui->labelInterlockBox->setStyleSheet("QLabel { background-color : red;}");
}

void xray::addPoint(double v, double c, double t)
{
    qv_voltage.append(v);
    qv_current.append(c);
    qv_time.append(t);
}

void xray::plot()
{
    ui->plot_voltage->graph(0)->setData(qv_voltage,qv_time);
    ui->plot_voltage->replot();
    ui->plot_voltage->update();

    ui->plot_current->graph(0)->setData(qv_current,qv_time);
    ui->plot_current->replot();
    ui->plot_current->update();
}

void xray::on_pushButtonFaultReset_released()
{
    //tube->U_fault_reset();
}

void xray::U_find_ports()
{
    ui->comboBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->comboBox->addItem(info.portName());
    }
}

void xray::on_pushButton_clicked()
{
    emit US_write_settings(ui->comboBox->currentText(), 9600, QSerialPort::DataBits::Data8, QSerialPort::Parity::NoParity, QSerialPort::StopBits::OneStop, QSerialPort::FlowControl::NoFlowControl);
    /*emit US_connect_port();
    emit US_initialize();*/
}

void xray::U_add_text(QString str)
{
    ui->textStatus->setPlainText(str);
}

void xray::on_pushButtonFaultReset_clicked()
{

}

void xray::on_pushButton_2_clicked()
{
    emit US_disconnect_port();
}
