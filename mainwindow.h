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

    void configSerial(QSerialPort * &pserial,const QString & comname);
    void findDevice();

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

    void fondSimpledControler();

    void readMyCom();

    void SimpleDPPRecvCallback(const QByteArray& revdata);
    void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code){
        qDebug()<<"error"<<error_code<<endl;
    }

private slots:
    void on_verticalSlider_valueChanged(int value);

    void on_btn_get_sysinfo_clicked();



    void on_btn_switch_mode_clicked(bool checked);

private:
    void disable_all_control_widget();
    void enable_all_control_widget();
};
#endif // MAINWINDOW_H
