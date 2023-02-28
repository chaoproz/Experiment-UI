#include "keyexperiment.h"
#include "ui_keyexperiment.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <fstream>
#include <QMessageBox>
#include <string>
KeyExperiment::KeyExperiment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyExperiment)
{
    ui->setupUi(this);
    // 窗口全屏显示
    QWidget::showFullScreen();
    // 设置背景颜色
    setPalette(QPalette(Qt::gray));
    setAutoFillBackground(true);
    // 生成随机数
    vector<int> num{0, 1, 2, 3};  // 初始化
    for (int i = 0; i < 40; i++) {
        random_shuffle(num.begin(), num.end());
        while (!vec.empty() && vec.back() == num[0]) random_shuffle(num.begin(), num.end());
        for (auto n : num) vec.push_back(n);
    }
    // 设置定时器
    QTimer* timerPainter = new QTimer(this);
    timerPainter->setTimerType(Qt::PreciseTimer);
    timerPainter->start(10);
    connect(timerPainter,SIGNAL(timeout()),this,SLOT(update()));
}

KeyExperiment::~KeyExperiment()
{
    delete ui;
}

// 重写绘图事件
void KeyExperiment::paintEvent(QPaintEvent*) {
    // 画家设置
    QPainter* painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);  // 抗锯齿
    if (flag == 0) init(painter);
    else if (flag == 1) keyPress(painter);
    else if (flag == 2) finish(painter);
    painter->end();
}

// 请按任意键开始
void KeyExperiment::init(QPainter* painter) {
    painter->save();
    QPen qPen(Qt::black);  // 构造画笔，并将颜色初始化为黑色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 绘制文字
    QFont qFont("楷体", 150, QFont::Bold, false);
    painter->setFont(qFont);
    painter->drawText(QRectF(0, 0, 1920, 1080), Qt::AlignHCenter | Qt::AlignVCenter, "请按任意键开始");
    painter->restore();
}

// 随机出现数字
void KeyExperiment::keyPress(QPainter* painter) {
    painter->save();
    QPen qPen(Qt::black);  // 构造画笔，并将颜色初始化为黑色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 绘制中心数字
    QFont qFont("楷体", 400, QFont::Bold, false);
    painter->setFont(qFont);
    painter->drawText(QRectF(0, 0, 1920, 1080), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(vec[keyNum] + 1));
    painter->restore();
}


// 完成实验界面
void KeyExperiment::finish(QPainter* painter) {
    painter->save();
    QPen qPen(Qt::black);  // 构造画笔，并将颜色初始化为黑色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 绘制中心数字
    QFont qFont("楷体", 100, QFont::Bold, false);
    painter->setFont(qFont);
    int acc = 0;
    double rtSum = 0;
    for (int i = 0; i < 40; i++) {
        if (vec[i] == press[i]) {
            acc++;
            rtSum += RTime[i];
        }
    }
    double rt = rtSum / acc;
    painter->drawText(QRectF(0, 0, 1920, 1080), Qt::AlignHCenter | Qt::AlignVCenter, QString("恭喜你完成实验!\n正确率：%1/40\n平均反应时间：%2ms").arg(acc).arg(rt));
    ui->labelFileName->show();
    ui->lineEditFileName->show();
    ui->pushButtonStart->show();
    painter->restore();
}

// 按键事件
void KeyExperiment::keyPressEvent(QKeyEvent* event)
{
    if (flag == 0) {
        flag = 1;
        timeStart = QDateTime::currentDateTime();
    }
    else if (flag == 1) {
        if (event->key() == 0x31) press.push_back(0);
        else if (event->key() == 0x32) press.push_back(1);
        else if (event->key() == 0x33) press.push_back(2);
        else if (event->key() == 0x34) press.push_back(3);
        keyNum++;
        timeEnd = QDateTime::currentDateTime();
        RTime.push_back(timeStart.msecsTo(timeEnd));
        timeStart = timeEnd;
        if (keyNum == 40) {
            flag = 2;
            keyNum = 0;
            // 将数据保存至文件中
            ofstream ofs1;
            ofstream ofs2;
            ofstream ofs3;
            // QString转换为const char*（因为ofs中open的第一个参数是const char*）
            const QString strRandom = "random" + ui->lineEditFileName->text() + ".csv";
            const QString strPress = "press" + ui->lineEditFileName->text() + ".csv";
            const QString strRTime = "RTime" + ui->lineEditFileName->text() + ".csv";
            ofs1.open(strRandom.toStdString().c_str(), ios::out);
            ofs2.open(strPress.toStdString().c_str(), ios::out);
            ofs3.open(strRTime.toStdString().c_str(), ios::out);
            for (int i = 0; i < 40; i++) {
                ofs1 << vec[i] << endl;
                ofs2 << press[i] << endl;
                ofs3 << RTime[i] << endl;
            }
            ofs1.close();
            ofs2.close();
            ofs3.close();
        }
    }
}

void KeyExperiment::on_pushButtonStart_clicked()
{ 
    if (ui->lineEditFileName->text() == "") QMessageBox::critical(this,"错误","未输入文件名");
    else {
        flag = 0;
        ui->labelFileName->hide();
        ui->lineEditFileName->hide();
        ui->pushButtonStart->hide();
    }
}
void KeyExperiment::on_pushButtonBack_clicked()
{
    this->close();
}


