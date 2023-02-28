#include "rest.h"
#include "ui_rest.h"
#include <QTimer>
#include <QPainter>
Rest::Rest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Rest)
{
    ui->setupUi(this);
    // 窗口全屏显示
    QWidget::showFullScreen();
    // 设置背景颜色
    setPalette(QPalette(Qt::gray));
    setAutoFillBackground(true);
    // 拉低电平
    Out32(parallelPort, 0);
    // 设置定时器
    QTimer* timerPainter = new QTimer(this);
    timerPainter->setTimerType(Qt::PreciseTimer);
    timerPainter->start(10);
    connect(timerPainter,SIGNAL(timeout()),this,SLOT(update()));
}

Rest::~Rest()
{
    delete ui;
}

// 重写绘图事件
void Rest::paintEvent(QPaintEvent*) {
    // 画家设置
    QPainter* painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);  // 抗锯齿
    if (flag == 0) init(painter);
    else if (flag == 1) restMin(painter);
    else if (flag == 2) finish(painter);
    painter->end();
}
void Rest::init(QPainter* painter) {
    painter->save();
    QPen qPen(Qt::black);  // 构造画笔，并将颜色初始化为黑色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 绘制中心数字
    QFont qFont("楷体", 100, QFont::Bold, false);
    painter->setFont(qFont);
    painter->drawText(QRectF(0, 0, 1920, 1080), Qt::AlignHCenter | Qt::AlignVCenter, "欢迎来到静息实验\n请注视稍后的屏幕十字\n实验时长5分钟");
    painter->restore();
}
void Rest::restMin(QPainter* painter) {
    painter->save();
    QPen qPen(Qt::black);  // 构造画笔，并将颜色初始化为黑色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 绘制中心数字
    QFont qFont("楷体", 400, QFont::Bold, false);
    painter->setFont(qFont);
    painter->drawText(QRectF(0, 0, 1920, 1080), Qt::AlignHCenter | Qt::AlignVCenter, "+");
    painter->restore();
}
void Rest::finish(QPainter* painter) {
    painter->save();
    QPen qPen(Qt::black);  // 构造画笔，并将颜色初始化为黑色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 绘制中心数字
    QFont qFont("楷体", 100, QFont::Bold, false);
    painter->setFont(qFont);
    painter->drawText(QRectF(0, 0, 1920, 1080), Qt::AlignHCenter | Qt::AlignVCenter, "恭喜您完成实验！");
    painter->restore();
}

void Rest::on_pushButtonStart_clicked()
{
    flag = 1;
    // 打标签
    parallelPort = ui->lineEditParallelPort->text().toUInt();  // 定义并口
    Out32(parallelPort, 1 + 128);
    // 定时器拉低电平
    QTimer* timer0 = new QTimer(this);
    timer0->setTimerType(Qt::PreciseTimer);
    timer0->start(1000);
    connect(timer0,&QTimer::timeout,[=](){
        Out32(parallelPort, 0);
        timer0->stop();
    });
    // 设置定时器（写在一起，否则会导致程序异常退出）
    QTimer* timer = new QTimer(this);
    timer->start(5*60*1000);
    connect(timer,&QTimer::timeout,[=](){
        flag = 2;
        Out32(parallelPort, 1 + 128);
        timer->stop();
        timer0->start();
    });
    ui->labelParallelPort->hide();
    ui->lineEditParallelPort->hide();
    ui->pushButtonStart->hide();
}

void Rest::on_pushButtonBack_clicked()
{
    this->close();
    Out32(parallelPort, 0);
}
