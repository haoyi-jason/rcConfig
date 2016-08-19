#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>


namespace Ui {
class MainWindow;
}
typedef struct{
    int flag;
    int devType;
    int workFreq;
    int rate;
    int selfAddr;
    int tarAddr;
    int timeout;
    int delayTime;
    int heartBeatTime;
}_dev_config_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QMap<QString,QSerialPort::BaudRate> *m_baudrate;
    QMap<QString,QSerialPort::Parity> *m_parity;
    QMap<QString,QSerialPort::DataBits> *m_databits;
    QMap<QString,QSerialPort::StopBits> *m_stopbits;
    QMap<QString,QSerialPort::FlowControl> *m_flowOptions;
    QSerialPort *mSerialPort;


private:
    Ui::MainWindow *ui;
    _dev_config_t devConfig;
    QList<QString> mCmds;
    QTimer *timer;
    int nofCmdTOSend, nofCmdSent;
    bool portConnected;
    bool devResponsed;

private slots:
    void handleSerialPortRead();
    void handleSerialPortConnect();
    void handleReadConfig();
    void handleWriteConfig();
    void handleTimeout();
};

#endif // MAINWINDOW_H
