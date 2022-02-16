#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QThread>

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
    QSerialPort *mySerialPort;
    QSet<QString> portSet;


public slots:
    void checkAvailablePorts(){
        foreach ( const QSerialPortInfo &Info, QSerialPortInfo::availablePorts()) {
             QSerialPort availablePort;
             availablePort.setPortName(Info.portName());
             if (availablePort.open(QIODevice::ReadWrite)) {
                portSet.insert(Info.portName());
                qDebug()<<Info.portName()<<endl;
                availablePort.close();
              }else{
                 qDebug()<<"can't open:"<<Info.portName()<<endl;
             }
         }
    }

    void com_checked(){
        qDebug()<<"可用串口数量"<<portSet.size()<<endl;
    }
};
#endif // MAINWINDOW_H
