#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include "simpledpp.hpp"
#include "finddevicethread.hpp"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private:
    Ui::MainWindow *ui;
    QSerialPort *mySerialPort = nullptr;
    QSet<QString> portSet;
    FindDeviceThread *finddeviceThread;
    SimpleDPP *sdp;
    bool rev_msg = false;
    bool device_fond = false;

    void configSerial(QSerialPort * &pserial,const QString & comname){
        pserial = new QSerialPort(comname);
        pserial->setBaudRate(QSerialPort::Baud115200);
        pserial->setParity(QSerialPort::NoParity);
        pserial->setDataBits(QSerialPort::Data8);
        pserial->setStopBits(QSerialPort::OneStop);
        pserial->setFlowControl(QSerialPort::NoFlowControl);
        connect(mySerialPort, SIGNAL(readyRead()), this, SLOT(readMyCom()));
    }
    void findDevice(){

        QThread* find_simpledctroler = QThread::create([&]{
            foreach(const QString & comname,portSet){

                configSerial(mySerialPort,comname);
                if (mySerialPort->open(QIODevice::ReadWrite)) {
                    const char cmd[1] = {0x05};

                    while(rev_msg == false){
                        QThread::sleep(1);
                        sdp->send_datas(cmd,1);
                        mySerialPort->waitForBytesWritten();
                        qDebug()<<"waitinge..."<<endl;
                    }
                    rev_msg = false;
                    if(device_fond){
                        qDebug()<<"Found device :"<<comname<<endl;
                        return ;
                    }

                    mySerialPort->close();
                    delete mySerialPort;
                    mySerialPort = nullptr;
                }else{
                    qDebug()<<"can't open:"<<comname<<endl;
                }
            }
        });
        connect(find_simpledctroler,SIGNAL(finished()),this,SLOT(fondSimpledControler()));
        //防止内存泄漏 avoid memory leaks
        connect(find_simpledctroler,&QThread::finished ,find_simpledctroler,&QObject::deleteLater);
        find_simpledctroler->start();
    }

public slots:
    void checkAvailablePorts(){
        //        foreach ( const QSerialPortInfo &Info, QSerialPortInfo::availablePorts()) {
        //            QSerialPort availablePort;
        //            availablePort.setPortName(Info.portName());
        //            if (availablePort.open(QIODevice::ReadWrite)) {
        //                portSet.insert(Info.portName());
        //                qDebug()<<Info.portName()<<endl;
        //                availablePort.close();
        //            }else{
        //                qDebug()<<"can't open:"<<Info.portName()<<endl;
        //            }
        //        }
        portSet.insert("COM13");
    }

    void com_checked(){
        qDebug()<<"可用串口数量"<<portSet.size()<<endl;
        findDevice();
    }


public slots:

    void fondSimpledControler(){
        qDebug()<<"fondSimpledControler:"<<mySerialPort->portName()<<endl;
    }

    void readMyCom()
    {
        QByteArray temp = mySerialPort->readAll();
        sdp->parse(temp);
    }

    void SimpleDPPRecvCallback(const QByteArray& revdata){
        uint8_t cmd = revdata[0];
        rev_msg = true;
        switch (cmd)
        {
        case 0x00:

            break;
        case 0x01:
            /*set pwm duty*/

            break;
        case 0x04:

            break;
        case 0x05: //用于设备识别-->

            break;
        case 0x06:
            device_fond = true;
            break;
        default:
            break;
        }


    }
    void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code){
        qDebug()<<"error"<<error_code<<endl;
    }
};
#endif // MAINWINDOW_H
