#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    delete ui;
}

