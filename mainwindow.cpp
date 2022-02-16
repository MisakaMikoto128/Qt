#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "BytesUtil.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sdp = new SimpleDPP();
    disable_all_control_widget();
    connect(sdp,&SimpleDPP::SendBuffer,[&](const QByteArray& writedata){
        if(mySerialPort!=nullptr && mySerialPort->isOpen() && mySerialPort->isWritable()){
            mySerialPort->write(writedata);
            mySerialPort->flush();
            qDebug()<<"SendBuffer:"<<writedata<<endl;
        }
    });
    connect(sdp,&SimpleDPP::RecvCallback,this,&MainWindow::SimpleDPPRecvCallback);
    connect(sdp,&SimpleDPP::RevErrorCallback,this,&MainWindow::SimpleDPPRevErrorCallback);

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

void MainWindow::configSerial(QSerialPort *&pserial, const QString &comname)
{
    pserial = new QSerialPort(comname);
    pserial->setBaudRate(QSerialPort::Baud115200);
    pserial->setParity(QSerialPort::NoParity);
    pserial->setDataBits(QSerialPort::Data8);
    pserial->setStopBits(QSerialPort::OneStop);
    pserial->setFlowControl(QSerialPort::NoFlowControl);
    //    pserial->clearError();
    //    pserial->clear();

    connect(mySerialPort, SIGNAL(readyRead()), this, SLOT(readMyCom()));

}

void MainWindow::findDevice()
{

    QThread* find_simpledctroler = QThread::create([&]{
        foreach(const QString & comname,portSet){

            configSerial(mySerialPort,comname);
            if (mySerialPort->open(QIODevice::ReadWrite)) {
                mySerialPort->setReadBufferSize(1);  //解决readyRead信号不及时产生的问题
                const char cmd[1] = {0x05};

                while(rev_msg == false){
                    QThread::sleep(1);
                    sdp->send_datas(cmd,1);
                    mySerialPort->waitForBytesWritten();
                    qDebug()<<"waitinge..."<<endl;
                    ui->label_com_init->setText("正在确认是否为指定设备！");

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

void MainWindow::fondSimpledControler()
{
    qDebug()<<"fondSimpledControler:"<<mySerialPort->portName()<<endl;
    enable_all_control_widget();
    ui->label_com_init->setText("已连接");
    connect(mySerialPort, SIGNAL(readyRead()), this, SLOT(readMyCom()));
}

void MainWindow::readMyCom()
{
    QByteArray temp = mySerialPort->readAll();
    qDebug()<<"readMyCom"<<temp<<endl;
    sdp->parse(temp);
}

void MainWindow::SimpleDPPRecvCallback(const QByteArray &revdata)
{
    uint8_t cmd = revdata[0];

    rev_msg = true;
    switch (cmd)
    {
    case 0x00:
        break;
    case 0x01:
        /*rev pwm duty*/
    {
        bu_byte sysmode = revdata.data()[1];
        if(sysmode == 0){
            ui->label_sysmode->setText("自动模式");

        }else{
            ui->label_sysmode->setText("手动模式");
        }
        float lightduty = bytes2floatr((const bu_byte *)revdata.data() + 2, false);
        ui->label_light_pwm->setText(QString::number(lightduty));

    }

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

void MainWindow::disable_all_control_widget()
{
    ui->btn_get_sysinfo->setEnabled(false);
    ui->btn_switch_mode->setEnabled(false);
    ui->verticalSlider->setEnabled(false);
}

void MainWindow::enable_all_control_widget()
{
    ui->btn_get_sysinfo->setEnabled(true);
    ui->btn_switch_mode->setEnabled(true);
    ui->verticalSlider->setEnabled(true);
}


void MainWindow::on_verticalSlider_valueChanged(int value)
{
    ui->label_slider_value->setText(QString::number(value));
    const char cmd[1] = {0x01};
    float set_value = value;
    unsigned char data[4];
    float2bytes(set_value,data,false);
    sdp->send_datas(cmd,1,data,4);
    mySerialPort->waitForBytesWritten();
    QThread::msleep(100);
}


void MainWindow::on_btn_get_sysinfo_clicked()
{
    const char cmd[1] = {0x04};
    sdp->send_datas(cmd,1);
}



void MainWindow::on_btn_switch_mode_clicked(bool checked)
{
    const char cmd[1] = {0x00};
    char mode[1] = {0x00};

    if(checked == false){
        ui->btn_switch_mode->setText("自动模式");
    }else{
        ui->btn_switch_mode->setText("手动模式");
        mode[0] = 0x01;
    }
    sdp->send_datas(cmd,1,mode,1);
}

