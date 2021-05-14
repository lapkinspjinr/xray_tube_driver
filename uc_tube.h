#ifndef UC_TUBE_H
#define UC_TUBE_H

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QTimer>

#include <cmath>

struct UStr_rs232_settings {//Структура с настройками порта
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};


class UC_tube : public QObject
{
    Q_OBJECT

    typedef enum {
        whatchdog_enable_request,
        whatchdog_timeout_request,
        ready_status_request,
        xray_status_request,
        fault_status_request,
        arc_status_request,
        overvoltage_status_request,
        overcurrent_status_request,
        overtemp_status_request,
        read_port_a_request,
        read_port_b_request,
        voltage_request,
        current_request,
        input_line_voltage_request,
        interlock_voltage_request,
        analog_data_request,
        command_set_request,
        overcurrent_counter_request,
        overvoltage_counter_request,
        arc_counter_request,
        pulse_config_request,
        pulse_mode_enable_request,
        analog_channel_data_request,
        read_input_value_request
    } UTE_read_data_type;


    UStr_rs232_settings settings;
    QList<UTE_read_data_type> read_data_type;
    QTimer * timer;

    int xcmdset;
    int whatchdog_timeout;

    public :

    QSerialPort port;


public:
    explicit UC_tube(QObject *parent = nullptr);
    ~UC_tube();

signals:
    void US_finished();
    void US_error_port(QString err);
    void US_data_ready(QString data);
    //////////certain data signals///////////////
    void US_watchdog_enable_data(bool enable);
    void US_watchdog_timeout_data(int count);
    void US_ready_status_data(bool ready);
    void US_xray_status_data(bool enable);
    void US_fault_status_data(bool detected);
    void US_arc_status_data(bool detected);
    void US_overvoltage_status_data(bool detected);
    void US_overcurrent_status_data(bool detected);
    void US_overtemp_status_data(bool detected);
    void US_voltage_data(double V);
    void US_current_data(double uA);
    void US_input_line_voltage_data(double V);
    void US_interlock_voltage_data(double V);
    void US_command_set_request_data(int cmdset);
    void US_overcurrent_counter_request_data(int cnt);
    void US_overvoltage_counter_request_data(int cnt);
    void US_arc_counter_request_data(int cnt);
    void US_pulse_config_request_data(int period_ms, int width_ms, int number);
    void US_pulse_mode_enable_request(bool enable);
    void US_analog_channel_data_request(int data);
    void US_read_input_value_data(bool value);
public slots:
    void U_connect_port();
    void U_disconnect_port();
    void U_start();
    void U_write_settings(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl);
    void U_write_data(QByteArray data);
    //////////certain settings of tube//////////////////
    void U_whatchdog_settings(bool enable, int s);
    void U_whatchdog_enable_request();
    void U_whatchdog_timeout_request();
    void U_initialization();
    void U_x_ray_enable(bool enable);
    void U_fault_reset();
    void U_fault_reset_standby();
    void U_set_voltage(double V);
    void U_set_current(double uA);
    void U_ready_status_request();
    void U_xray_status_request();
    void U_fault_status_request();
    void U_arc_status_request();
    void U_overvoltage_status_request();
    void U_overcurrent_status_request();
    void U_overtemp_status_request();
    void U_read_port_a_request();
    void U_read_port_b_request();
    void U_voltage_request();
    void U_current_request();
    void U_input_line_voltage_request();
    void U_interlock_voltage_request();
    void U_analog_data_request();
    void U_command_set_request();
    void U_overcurrent_counter_request();
    void U_overvoltage_counter_request();
    void U_arc_counter_request();
    void U_clear_counters();
    void U_pulse_mode_enable();
    void U_pulse_mode_disable();
    void U_pulse_mode_set_period(int ms);
    void U_pulse_mode_set_width(int ms);
    void U_pulse_mode_set_number(int n);
    void U_pulse_config_request();
    void U_pulse_mode_enable_request();
    void U_analog_channel_data_request(int channel);
    void U_set_port_direction(char port, int direction_mask);
    void U_set_output_value(char port, int output);
    void U_clear_output_value(char port, int output);
    void U_read_input_value_request(char port, int output);
private slots:
    void U_handle_error(QSerialPort::SerialPortError error);
    void U_read_data();
    void U_refrash_data();
};

#endif // UC_TUBE_H
