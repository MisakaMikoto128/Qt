#ifndef FINDDEVICETHREAD_HPP
#define FINDDEVICETHREAD_HPP

#include <QThread>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QThread>

class FindDeviceThread : public QThread
{
    Q_OBJECT
public:
    explicit FindDeviceThread(QObject *parent = nullptr): QThread{parent}{

    }

    void run() override{

    }




};

#endif // FINDDEVICETHREAD_HPP
