#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sdp = new SimpleDPP();
    connect(sdp,&SimpleDPP::SendBuffer,[&](const QByteArray& writedata){
        if(mySerialPort!=nullptr && mySerialPort->isOpen() && mySerialPort->isWritable()){
            mySerialPort->write(writedata);
            mySerialPort->flush();
            qDebug()<<"SendBuffer:"<<writedata<<endl;
        }
    });
    connect(sdp,&SimpleDPP::RecvCallback,this,&MainWindow::SimpleDPPRecvCallback);
    connect(sdp,&SimpleDPP::RevErrorCallback,this,&MainWindow::SimpleDPPRevErrorCallback);

//    configSerial(mySerialPort,"COM1");
//    mySerialPort->open(QIODevice::ReadWrite);
//mySerialPort->write("AA");
//mySerialPort->write("BB");
//mySerialPort->flush();
//mySerialPort->write("CC");
//mySerialPort->write("DD");

    QThread* check_com = QThread::create([&]{
        this->checkAvailablePorts();
    });
    connect(check_com,SIGNAL(finished()),this,SLOT(com_checked()));
    //防止内存泄漏 avoid memory leaks
    connect(check_com,&QThread::finished ,check_com,&QObject::deleteLater);
    check_com->start();
}

MainWindow::~MainWindow()
{
    delete sdp;
    delete ui;
}

