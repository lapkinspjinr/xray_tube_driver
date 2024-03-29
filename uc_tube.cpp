#include "uc_tube.h"

UC_tube::UC_tube(QObject *parent) : QObject(parent) {

}

UC_tube::~UC_tube() {
    U_disconnect_port();
    emit US_finished();
}
////////public slot////////////////
void UC_tube::U_connect_port() {
    port.setPortName(settings.name);
    if (port.open(QIODevice::ReadWrite)) {
        if (port.setBaudRate(settings.baudRate) &&
            port.setDataBits(settings.dataBits) &&
            port.setParity(settings.parity) &&
            port.setStopBits(settings.stopBits) &&
            port.setFlowControl(settings.flowControl)) {
            if (port.isOpen()) {
                /*открыт*/
                US_error_port("Port is opened!!");
                U_initialization();
            } else {
                /*не открыт*/
                US_error_port("Port is not opened!!");
            }
        } else {
            port.close();
            US_error_port(port.errorString().toLocal8Bit());
        }
    } else {
        port.close();
        US_error_port(port.errorString().toLocal8Bit());
    }
}

void UC_tube::U_disconnect_port() {
    if (port.isOpen()) {
        port.close();
        US_error_port("Port is closed!!");
    }
}

void UC_tube::U_start() {
    connect(&port, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(U_handle_error(QSerialPort::SerialPortError)));
    connect(&port, SIGNAL(readyRead()), this, SLOT(U_read_data()));
}

void UC_tube::U_write_settings(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl) {
    settings.name = name;
    settings.baudRate = static_cast<QSerialPort::BaudRate>(baudrate);
    settings.dataBits = static_cast<QSerialPort::DataBits>(DataBits);
    settings.parity = static_cast<QSerialPort::Parity>(Parity);
    settings.stopBits = static_cast<QSerialPort::StopBits>(StopBits);
    settings.flowControl = static_cast<QSerialPort::FlowControl>(FlowControl);
    U_connect_port();
}

void UC_tube::U_write_data(QByteArray data) {
    if (port.isOpen()) {
        port.write(data);
    }
}

//////////certain settings of tube//////////////////

void UC_tube::U_whatchdog_settings(bool enable, int s) {
    if (enable) {
        U_write_data("WE\r");
        whatchdog_timeout = s;
        U_write_data(QString("MW%1\r").arg(s, 3, 10, QChar('0')).toLatin1());
    } else {
        U_write_data("WD\r");
    }
}

void UC_tube::U_whatchdog_enable_request() {
    read_data_type.append(whatchdog_enable_request);
    U_write_data("WR\r");
}

void UC_tube::U_whatchdog_timeout_request() {
    read_data_type.append(whatchdog_timeout_request);
    U_write_data("PW\r");
}

void UC_tube::U_initialization() {
    U_write_data("CPA11111100\r");
    U_write_data("RESPA0\r");
    U_write_data("RESPA1\r");
    US_error_port("Initiallization!!");
}


void UC_tube::U_x_ray_enable(bool enable) {
    if (enable) {
        U_write_data("SETPA0\r");
    } else {
        U_write_data("RESPA0\r");
    }
}

void UC_tube::U_fault_reset() {
    U_write_data("SETPA0\r");
}

void UC_tube::U_fault_reset_standby() {
    U_write_data("RESPA0\r");
}

void UC_tube::U_set_voltage(double V) {
    int voltage_int = static_cast<int>(std::round(4095 * V / 120));
    U_write_data(QString("VA%1\r").arg(voltage_int, 4, 10, QChar('0')).toLatin1());
}

void UC_tube::U_set_current(double uA) {
    int current_int = static_cast<int>(std::round(4095 * uA / 350));
    U_write_data(QString("VB%1\r").arg(current_int, 4, 10, QChar('0')).toLatin1());
}

void UC_tube::U_ready_status_request() {
    read_data_type.append(ready_status_request);
    U_write_data("RPA2\r");
}

void UC_tube::U_xray_status_request() {
    read_data_type.append(xray_status_request);
    U_write_data("RPA3\r");
}

void UC_tube::U_fault_status_request() {
    read_data_type.append(fault_status_request);
    U_write_data("RPA4\r");
}

void UC_tube::U_arc_status_request() {
    read_data_type.append(arc_status_request);
    U_write_data("RPA5\r");
}

void UC_tube::U_overvoltage_status_request() {
    read_data_type.append(overvoltage_status_request);
    U_write_data("RPA6\r");
}

void UC_tube::U_overcurrent_status_request() {
    read_data_type.append(overcurrent_status_request);
    U_write_data("RPA7\r");
}

void UC_tube::U_overtemp_status_request() {
    read_data_type.append(overtemp_status_request);
    U_write_data("RPB0\r");
}

void UC_tube::U_read_port_a_request() {
    read_data_type.append(read_port_a_request);
    U_write_data("RPA\r");
}

void UC_tube::U_read_port_b_request() {
    read_data_type.append(read_port_b_request);
    U_write_data("RPB\r");
}

void UC_tube::U_voltage_request() {
    read_data_type.append(voltage_request);
    U_write_data("RD0\r");
}

void UC_tube::U_current_request() {
    read_data_type.append(current_request);
    U_write_data("RD1\r");
}

void UC_tube::U_input_line_voltage_request() {
    read_data_type.append(input_line_voltage_request);
    U_write_data("RD2\r");
}

void UC_tube::U_interlock_voltage_request() {
    read_data_type.append(interlock_voltage_request);
    U_write_data("RD3\r");
}

void UC_tube::U_analog_data_request() {
    read_data_type.append(analog_data_request);
    U_write_data("RD\r");
}

void UC_tube::U_command_set_request() {
    read_data_type.append(command_set_request);
    U_write_data("XCMDSET\r");
}

void UC_tube::U_overcurrent_counter_request() {
    read_data_type.append(overcurrent_counter_request);
    U_write_data("ECA7\r");
}

void UC_tube::U_overvoltage_counter_request() {
    read_data_type.append(overvoltage_counter_request);
    U_write_data("ECA6\r");
}

void UC_tube::U_arc_counter_request() {
    read_data_type.append(arc_counter_request);
    U_write_data("ECA5\r");
}

void UC_tube::U_clear_counters() {
    U_write_data("CLREC\r");
}

void UC_tube::U_pulse_mode_enable() {
    U_write_data("PE\r");
}

void UC_tube::U_pulse_mode_disable() {
    U_write_data("PD\r");
}

void UC_tube::U_pulse_mode_set_period(int ms) {
    U_write_data(QString("PP%1\r").arg(ms, 5, 10, QChar('0')).toLatin1());
}

void UC_tube::U_pulse_mode_set_width(int ms) {
    U_write_data(QString("PT%1\r").arg(ms, 5, 10, QChar('0')).toLatin1());
}

void UC_tube::U_pulse_mode_set_number(int n) {
    U_write_data(QString("PC%1\r").arg(n, 5, 10, QChar('0')).toLatin1());
}

void UC_tube::U_pulse_config_request() {
    U_write_data("PI\r");
}

void UC_tube::U_pulse_mode_enable_request() {
    read_data_type.append(pulse_mode_enable_request);
    U_write_data("PS\r");
}

void UC_tube::U_analog_channel_data_request(int channel) {
    read_data_type.append(analog_channel_data_request);
    U_write_data(QString("RAIN%1\r").arg(channel).toLatin1());
}

void UC_tube::U_set_port_direction(char port, int direction_mask) {
    QString str = QString("DDR") + QChar(port) + QString("%1").arg(direction_mask, 8, 2, QChar('0'));
    U_write_data(str.toLatin1());
}

void UC_tube::U_set_output_value(char port, int output) {
    QString str = QString("SETB") + QChar(port) + QString("%1").arg(output) + QChar('\r');
    U_write_data(str.toLatin1());
}

void UC_tube::U_clear_output_value(char port, int output) {
    QString str = QString("CLRB") + QChar(port) + QString("%1").arg(output) + QChar('\r');
    U_write_data(str.toLatin1());
}

void UC_tube::U_read_input_value_request(char port, int output) {
    QString str = QString("RDB") + QChar(port) + QString("%1").arg(output) + QChar('\r');
    U_write_data(str.toLatin1());
}

/////////////private slot///////////////////
void UC_tube::U_handle_error(QSerialPort::SerialPortError error) {
    if (port.isOpen() && (error == QSerialPort::ResourceError)) {
        US_error_port(port.errorString().toLocal8Bit());
        U_disconnect_port();
    }
}

void UC_tube::U_read_data() {
    QByteArray data;
    data.append(port.readAll());
    US_data_ready(QString(data));
    US_error_port(QString("Data received! ") + QString(data));
    QList<QByteArray> received_data = data.split('\r');
    UTE_read_data_type current_data_type;
    QByteArray current_data;
    for (int i = 0; i < received_data.size(); i++) {
        if (read_data_type.size() == 0) break;
        current_data_type = read_data_type[0];
        current_data = received_data[i];
        switch (current_data_type) {
            case whatchdog_enable_request : {
                emit US_watchdog_enable_data(current_data == "1");
                break;
            }
            case whatchdog_timeout_request : {
                emit US_watchdog_timeout_data(QString(current_data).toInt());
                break;
            }
            case ready_status_request : {
                emit US_ready_status_data(current_data == "0");
                break;
            }
            case xray_status_request : {
                emit US_xray_status_data(current_data == "1");
                break;
            }
            case fault_status_request : {
                emit US_fault_status_data(current_data == "1");
                break;
            }
            case arc_status_request : {
                emit US_arc_status_data(current_data == "1");
                break;
            }
            case overvoltage_status_request : {
                emit US_overvoltage_status_data(current_data == "1");
                break;
            }
            case overcurrent_status_request : {
                emit US_overcurrent_status_data(current_data == "1");
                break;
            }
            case overtemp_status_request : {
                emit US_overtemp_status_data(current_data == "0");
                break;
            }
            case read_port_a_request : {
                emit US_ready_status_data(current_data[10] == '0');
                emit US_xray_status_data(current_data[8] == '1');
                emit US_fault_status_data(current_data[6] == '1');
                emit US_arc_status_data(current_data[4] == '1');
                emit US_overvoltage_status_data(current_data[2] == '1');
                emit US_overcurrent_status_data(current_data[0] == '1');
                break;
            }
            case read_port_b_request : {
                emit US_overtemp_status_data(current_data[14] == '0');
                break;
            }
            case voltage_request : {
                emit US_voltage_data(QString(current_data).toDouble() * 120 / 4095);
                break;
            }
            case current_request : {
                emit US_current_data(QString(current_data).toDouble() * 350 / 4095);
                break;
            }
            case input_line_voltage_request : {
                emit US_input_line_voltage_data(QString(current_data).toDouble() * 32.55 / 4095);
                break;
            }
            case interlock_voltage_request : {
                emit US_interlock_voltage_data(QString(current_data).toDouble() * 15 / 4095);
                break;
            }
            case analog_data_request : {
                QList<QByteArray> analog_data = current_data.split(' ');
                emit US_voltage_data(QString(analog_data[0]).toDouble() * 120 / 4095);
                emit US_current_data(QString(analog_data[1]).toDouble() * 350 / 4095);
                emit US_input_line_voltage_data(QString(analog_data[2]).toDouble() * 32.55 / 4095);
                emit US_interlock_voltage_data(QString(analog_data[3]).toDouble() * 15 / 4095);
                break;
            }
            case command_set_request : {
                xcmdset = QString(current_data).toInt();
                emit US_command_set_request_data(xcmdset);
                break;
            }
            case overcurrent_counter_request : {
                emit US_overcurrent_counter_request_data(QString(current_data).toInt());
                break;
            }
            case overvoltage_counter_request : {
                emit US_overvoltage_counter_request_data(QString(current_data).toInt());
                break;
            }
            case arc_counter_request : {
                emit US_arc_counter_request_data(QString(current_data).toInt());
                break;
            }
            case pulse_config_request : {
                int period = QString(current_data).left(5).toInt();
                int width = QString(current_data).mid(4, 5).toInt();
                int number = QString(current_data).mid(10, 5).toInt();
                emit US_pulse_config_request_data(period, width, number);
                break;
            }
            case pulse_mode_enable_request : {
                emit US_pulse_mode_enable_request(current_data == "1");
                break;
            }
            case analog_channel_data_request : {
                emit US_analog_channel_data_request(QString(current_data).toInt());
                break;
            }
            case read_input_value_request : {
                emit US_read_input_value_data(current_data == "0");
                break;
            }
        }
        read_data_type.removeFirst();
    }
}

void UC_tube::U_refrash_data() {
    U_read_port_a_request();
    U_analog_data_request();
}
