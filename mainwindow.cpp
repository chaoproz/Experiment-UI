#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myserialport.h"
#include "maxforce.h"
#include "experiment.h"
#include "keyexperiment.h"
#include "rest.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置窗口标题
    this->setWindowTitle("菜单");
    // 设置窗口大小
    this->setMinimumSize(550, 480);
    this->setMaximumSize(550, 480);
    // 设置窗口图标
    this->setWindowIcon(QIcon(":/Image/Main.png"));
    // 点击“串口设置”按钮进入串口界面
    connect(ui->pushButtonSerialPort, &QPushButton::clicked, [=](){
        MySerialPort* mySP = new MySerialPort;
        mySP->show();
    });
    // 点击“串口设置”按钮进入串口界面
    connect(ui->pushButtonMaxForce, &QPushButton::clicked, [=](){
        MaxForce* maxF = new MaxForce;
        maxF->show();

    });
    // 点击“捏力任务”进入捏力任务界面
    connect(ui->pushButtonForceExperiment, &QPushButton::clicked, [=](){
        Experiment* e = new Experiment;
        e->show();
    });
    // 点击“按键任务”进入按键任务界面
    connect(ui->pushButtonKeyExperiment, &QPushButton::clicked, [=](){
        KeyExperiment* key = new KeyExperiment;
        key->show();
    });

    // 点击“静息任务”进入静息任务界面
    connect(ui->pushButtonRest, &QPushButton::clicked, [=](){
        Rest* r = new Rest;
        r->show();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

