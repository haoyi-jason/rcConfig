#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QtCore>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_baudrate = new QMap<QString,QSerialPort::BaudRate>;
    m_parity = new QMap<QString,QSerialPort::Parity>;
    m_databits = new QMap<QString,QSerialPort::DataBits>;
    m_stopbits = new QMap<QString,QSerialPort::StopBits>;
    m_flowOptions= new QMap<QString,QSerialPort::FlowControl>;

    m_baudrate->insert("9600",QSerialPort::Baud9600);
    m_baudrate->insert("38400",QSerialPort::Baud38400);
    m_baudrate->insert("115200",QSerialPort::Baud115200);

    m_parity->insert("NONE",QSerialPort::NoParity);
    m_parity->insert("ODD",QSerialPort::OddParity);
    m_parity->insert("EVEN",QSerialPort::EvenParity);

    m_databits->insert("7",QSerialPort::Data7);
    m_databits->insert("8",QSerialPort::Data8);

    m_stopbits->insert("1",QSerialPort::OneStop);
    m_stopbits->insert("1.5",QSerialPort::OneAndHalfStop);

    m_flowOptions->insert("None",QSerialPort::NoFlowControl);
    m_flowOptions->insert("Software",QSerialPort::SoftwareControl);
    m_flowOptions->insert("Hardware",QSerialPort::HardwareControl);
/*
    ui->cbBaudRate->addItems(QStringList(m_baudrate->keys()));

    ui->cbParity->addItems(QStringList(m_parity->keys()));
    ui->cbDataBits->addItems(QStringList(m_databits->keys()));
    ui->cbStopBits->addItems(QStringList(m_stopbits->keys()));

    ui->cbFlowControl->addItems(QStringList(m_flowOptions->keys()));
    ui->cbFlowControl->setCurrentIndex(2);
*/
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        ui->cbSerialPort->addItem(info.portName());
    }
    ui->cbSerialPort->addItem("REFRESH");

    mSerialPort = new QSerialPort;
    timer = new QTimer();
    timer->setInterval(500);
    portConnected = false;

    connect(ui->pbConnect,SIGNAL(clicked()),SLOT(handleSerialPortConnect()));
    //connect(ui->pbDisconnect,SIGNAL(clicked()),SLOT(handleSerialPortConnect()));
    connect(mSerialPort,SIGNAL(readyRead()),SLOT(handleSerialPortRead()));
    connect(ui->pbReadSetting,SIGNAL(clicked()),SLOT(handleReadConfig()));
    connect(ui->pbWriteSetting,SIGNAL(clicked()),SLOT(handleWriteConfig()));
    connect(timer,SIGNAL(timeout()),SLOT(handleTimeout()));

    ui->pbReadSetting->setEnabled(false);
    ui->pbWriteSetting->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleSerialPortConnect()
{
    if(portConnected){
        mSerialPort->close();
        portConnected = false;
    }else{
        mSerialPort->close();
        mSerialPort->setPortName(ui->cbSerialPort->currentText());
        mSerialPort->setBaudRate(QSerialPort::Baud9600);
        mSerialPort->setParity(QSerialPort::NoParity);
        mSerialPort->setDataBits(QSerialPort::Data8);
        mSerialPort->setStopBits(QSerialPort::OneStop);
        mSerialPort->setFlowControl(QSerialPort::NoFlowControl);
        if(mSerialPort->open(QIODevice::ReadWrite)){
            portConnected = true;
        }
        /*
        mSerialPort->setBaudRate(m_baudrate->value(ui->cbBaudRate->currentText()));
        mSerialPort->setParity(m_parity->value(ui->cbParity->currentText()));
        mSerialPort->setDataBits(m_databits->value(ui->cbDataBits->currentText()));
        mSerialPort->setStopBits(m_stopbits->value(ui->cbStopBits->currentText()));
        */
    }

    QPushButton *btn = (QPushButton*)sender();
    if(portConnected){
       btn->setText(tr("中斷連線"));
       ui->statusBar->showMessage("已連線");
       ui->pbReadSetting->setEnabled(true);
       ui->pbWriteSetting->setEnabled(true);
    }else{
        btn->setText(tr("連線"));
        ui->statusBar->showMessage("未連線");
        ui->pbReadSetting->setEnabled(false);
        ui->pbWriteSetting->setEnabled(false);
    }

}

void MainWindow::handleSerialPortRead()
{
    if(mSerialPort->canReadLine()){
        devResponsed = true;
        QByteArray recv = mSerialPort->readLine();
        QString str = QString(recv);
        QRegExp rx("[=,]");
        QStringList slist;
        slist = str.split(rx,QString::SkipEmptyParts);
        qDebug()<<"Receive: "<<slist<<" Size:"<<slist.size();
        if(slist.size() >= 2){
            if(slist[0] == "DTYP"){
                ui->cbOperationMode->setCurrentIndex(slist[1].toInt());
            }
            else if(slist[0] == "SADR"){
                ui->leMyAddr->setText(slist[1].trimmed());
            }
            else if(slist[0] == "TADR"){
                ui->leDestAddr->setText(slist[1].trimmed());
            }
            else if(slist[0] == "FREQ"){
                ui->leFrequency->setText(slist[1].trimmed());
            }
            else if(slist[0] == "TXPA"){
                ui->leTxPa->setText(slist[1].trimmed());
            }
            else if(slist[0] == "TMHB"){
                ui->leSendInterval->setText(slist[1].trimmed());
            }
            else if(slist[0] == "ALMT"){
                ui->leTxPa->setText(slist[1].trimmed());
            }
            else if(slist[0] == "ALMB"){
                ui->leTxPa->setText(slist[1].trimmed());
            }
            else if(slist[0] == "LBTV"){
                ui->leLbt->setText(slist[1].trimmed());
            }
            else if(slist[0] == "SERN"){
                ui->leSerial->setText(slist[1].trimmed());
            }
            else if(slist[0] == "CONV"){
                int idx;
                idx = slist[1].toInt();
                //qDebug()<<"READ CONV:"<<slist;
                switch(idx){
                case 0:
                    ui->leAVv1->setText(slist[2].trimmed());
                    break;
                case 1:
                    ui->leAVv2->setText(slist[2].trimmed());
                    ui->leAVa2->setText(slist[3].trimmed());
                    break;
                case 2:
                    ui->leAVv3->setText(slist[2].trimmed());
                    ui->leAVa3->setText(slist[3].trimmed());
                    break;
                case 3:
                    ui->leAVv4->setText(slist[2].trimmed());
                    ui->leAVa4->setText(slist[3].trimmed());
                    break;
                }
            }
            else if(slist[0] == "SPED"){
                int idx;
                idx = slist[1].toInt();
                //qDebug()<<"READ CONV:"<<slist;
                switch(idx){
                case 0:
                    ui->leAPpl1->setText(slist[2].trimmed());
                    ui->leAPph1->setText(slist[3].trimmed());
                    ui->leAPa1->setText(slist[4].trimmed());
                    break;
                case 1:
                    ui->leAPpl2->setText(slist[2].trimmed());
                    ui->leAPph2->setText(slist[3].trimmed());
                    ui->leAPa2->setText(slist[4].trimmed());
                    break;
                case 2:
                    ui->leAPpl3->setText(slist[2].trimmed());
                    ui->leAPph3->setText(slist[3].trimmed());
                    ui->leAPa3->setText(slist[4].trimmed());
                    break;
                }
            }
        }


    }
}

void MainWindow::handleReadConfig()
{
    QList<QString> cmds;
    QString str;
    mCmds.clear();
    if(mSerialPort->isOpen()){
        mCmds.append(QString("DTYP=?\r\n"));
        mCmds.append(QString("SADR=?\r\n"));
        mCmds.append(QString("TADR=?\n"));
        //mCmds.append(QString("RATE=?\r\n"));
        mCmds.append(QString("FREQ=?\r\n"));
        mCmds.append(QString("TXPA=?\r\n"));
        mCmds.append(QString("TMHB=?\r\n"));
        mCmds.append(QString("ALMT=?\r\n"));
        mCmds.append(QString("ALMB=?\r\n"));
        mCmds.append(QString("LBTV=?\r\n"));
        mCmds.append(QString("SERN=?\r\n"));
        mCmds.append(QString("CONV=?\r\n"));
        mCmds.append(QString("SPED=?\r\n"));
        nofCmdTOSend = mCmds.size();
        nofCmdSent = 0;
        timer->start();
        ui->statusBar->showMessage("讀寫中...");
        ui->pbReadSetting->setEnabled(false);
        ui->pbWriteSetting->setEnabled(false);
        devResponsed = false;
    }

}

void MainWindow::handleWriteConfig()
{
    mCmds.clear();
    if(mSerialPort->isOpen()){
        mCmds.append(QString("DTYP=%1\n").arg(ui->cbOperationMode->currentIndex()));
        mCmds.append(QString("SADR=%1\n").arg(ui->leMyAddr->text().trimmed()));
        mCmds.append(QString("TADR=%1\n").arg(ui->leDestAddr->text().trimmed()));
        mCmds.append(QString("FREQ=%1\n").arg(ui->leFrequency->text().trimmed()));
        mCmds.append(QString("TXPA=%1\n").arg(ui->leTxPa->text().trimmed()));
        mCmds.append(QString("TMHB=%1\n").arg(ui->leSendInterval->text().trimmed()));
        mCmds.append(QString("ALMT=%1\n").arg(ui->leAlarmSed->text().trimmed()));
        mCmds.append(QString("ALMB=%1\n").arg(ui->leAlmLbtSec->text().trimmed()));
        mCmds.append(QString("LBTV=%1\n").arg(ui->leLbt->text().trimmed()));
        mCmds.append(QString("SERN=%1\n").arg(ui->leSerial->text().trimmed()));
        mCmds.append(QString("CONV=0,%1,%2\n").arg(ui->leAVv1->text().trimmed()).arg(ui->leAVa1->text().trimmed()));
        mCmds.append(QString("CONV=1,%1,%2\n").arg(ui->leAVv2->text().trimmed()).arg(ui->leAVa2->text().trimmed()));
        mCmds.append(QString("CONV=2,%1,%2\n").arg(ui->leAVv3->text().trimmed()).arg(ui->leAVa3->text().trimmed()));
        mCmds.append(QString("CONV=3,%1,%2\n").arg(ui->leAVv4->text().trimmed()).arg(ui->leAVa4->text().trimmed()));
        mCmds.append(QString("SPED=0,%1,%2,%3\n").arg(ui->leAPpl1->text().trimmed()).arg(ui->leAPph1->text().trimmed()).arg(ui->leAPa1->text().trimmed()));
        mCmds.append(QString("SPED=1,%1,%2,%3\n").arg(ui->leAPpl2->text().trimmed()).arg(ui->leAPph2->text().trimmed()).arg(ui->leAPa2->text().trimmed()));
        mCmds.append(QString("SPED=2,%1,%2,%3\n").arg(ui->leAPpl3->text().trimmed()).arg(ui->leAPph3->text().trimmed()).arg(ui->leAPa3->text().trimmed()));
        mCmds.append(QString("WCFG\n"));
        nofCmdTOSend = mCmds.size();
        nofCmdSent = 0;
        timer->start();
        ui->statusBar->showMessage("讀寫中...");
        ui->pbReadSetting->setEnabled(false);
        ui->pbWriteSetting->setEnabled(false);
    }

}

void MainWindow::handleTimeout()
{
    mSerialPort->write(mCmds[nofCmdSent].toUtf8());
    qDebug()<<mCmds[nofCmdSent].toUtf8();
    nofCmdSent++;
    if(nofCmdSent == nofCmdTOSend){
        timer->stop();
        ui->pbReadSetting->setEnabled(true);
        ui->pbWriteSetting->setEnabled(true);
        if(devResponsed)
            ui->statusBar->showMessage("讀/寫完成");
        else
            ui->statusBar->showMessage("讀/寫失敗");
    }
}
