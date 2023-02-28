#include "taskdashboard.h"
#include "ui_taskdashboard.h"
#include <QTimer>
#include <QPainter>
#include <QtMath>
TaskDashboard::TaskDashboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskDashboard)
{
    ui->setupUi(this);
    // 固定表盘的宽高
    this->setMinimumSize(1200, 650);
    this->setMaximumSize(1200, 650);
    // 设置定时器
    QTimer* timerPainter = new QTimer(this);
    timerPainter->setTimerType(Qt::PreciseTimer);
    timerPainter->start(20);
    connect(timerPainter,SIGNAL(timeout()),this,SLOT(update()));
}

TaskDashboard::~TaskDashboard()
{
    delete ui;
}

// 重写绘图事件
void TaskDashboard::paintEvent(QPaintEvent*) {
    QPainter* painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);  // 抗锯齿
    // 设置透明背景
    setOtherBackgroud(painter);
    // 绘制表盘
    drawDashBoard(painter);
    // 绘制刻度尺
    drawDivideRule(painter);
    // 绘制指针
    drawDishBoardPointer(painter);
    //绘制表盘中心文字
    drawDashBoardText(painter);
    painter->end();
}

// 设置透明背景
void TaskDashboard::setOtherBackgroud(QPainter* painter) {
    painter->save();
    // 将画面变为透明
    painter->setBrush(Qt::transparent);
    painter->fillRect(this->rect(), QColor(0, 0, 0, 0));
    painter->restore();
}

// 绘制表盘
void TaskDashboard::drawDashBoard(QPainter* painter) {
    painter->save();
    // 绘制最外层圆盘颜色（渐变色）
    QLinearGradient linear(600, 600, 600, 0);  // 设置渐变色，从(600, 600)到(600,0)
    linear.setColorAt(0, QColor(145, 218, 204));  // 起点颜色
    linear.setColorAt(1, QColor(47, 141, 237));  // 终点颜色
    linear.setSpread(QGradient::PadSpread);  // 设置显示模式
    painter->setBrush(QBrush(linear));  // 使用画刷
    painter->drawPie(0, 0, 1200, 1200, 0 * 16, 180 * 16);  // 绘制最外层圆盘（前面四个参数时外接正方形，后面两个参数为0-180度，单位为1/16度，所以乘16，最后一个参数是转过的角度）
    // 绘制带刻度的外圆盘
    painter->setBrush(QBrush(QColor(20,20,20)));  // 黑色画刷
    painter->drawPie(50, 50, 1100, 1100, 0 * 16, 180 * 16);
    // 带刻度圆环的动态变化
    painter->setBrush(QColor(47, 248, 21, 155));
    // 因为表盘满量程为最大力百分比的2倍，所以需要乘2
    double angle = (realForce / (maxFore * percent * 2)) * 180;
    if (angle < 0) angle =0;
    if (angle > 180) angle = 180;
    painter->drawPie(75, 75, 1050, 1050, 180 * 16, -angle * 16);
    painter->setBrush(QBrush(QColor(20,20,20)));
    painter->drawPie(100, 100, 1000, 1000, 180 * 16, -angle * 16);
    painter->setBrush(QBrush(QColor(25, 125, 130, 155)));
    painter->drawPie(50, 50, 1100, 1100, 180 * 16, -angle * 16);
    painter->setBrush(QBrush(QColor(20,20,20)));
    painter->drawPie(135, 135, 930, 930, 180 * 16, -angle * 16);
    // 绘制带刻度的内圆盘，中间用绿色，两侧用红色（现在为+-10%，可能需要改成5%。此外，不同的力的范围大小也是不同的）
    painter->setBrush(QBrush(QColor(0, 255, 0)));  // 绿色画刷
    double theta = 0.05 / (percent * 2) * 180;
    painter->drawPie(135, 135, 930, 930, (90 - theta) * 16, 2 * theta * 16);
    painter->setBrush(QBrush(QColor(255, 107, 107)));  // 红色画刷
    painter->drawPie(135, 135, 930, 930, 0 * 16, (90 - theta) * 16);
    painter->drawPie(135, 135, 930, 930, (90 + theta) * 16, (90 - theta) * 16);
    // 最内层圆盘
    painter->setBrush(QBrush(QColor(255, 255, 255, 255)));  // 白色画刷
    painter->drawPie(185, 185, 830, 830, 0 * 16, 180 * 16);
    // 绘制最内层盘上的文字
    QPen qPen(Qt::black);  // 构造画笔，并将颜色初始化为白色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 绘制表盘中心文字
    QFont qFont("楷体", 20, QFont::Bold, false);
    painter->setFont(qFont);
    painter->drawText(QRectF(525, 175, 150, 100), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(100 * percent) + "%");
    painter->restore();
}

// 绘制刻度
void TaskDashboard::drawDivideRule(QPainter* painter) {
    painter->save();
    QPen qPen(Qt::white);  // 构造画笔，并将颜色初始化为白色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 画刻度线，逆时针从0-180度
    int radius = 0;  // 刻度角度
    for(int i = 0; i <= 30; i++) {
        int angle =  6 * i;  // 刻度线角度
        // 刻度线起始坐标
        int x1Start = 600 + 550 * cos(angle * 3.14 / 180);
        int y1Start = 600 - 550 * sin(angle * 3.14 / 180);
        // 刻度线结束半径
        if(i % 5 == 0) radius = 465;
        else radius = 525;
        // 刻度线结束坐标
        int x2End = 600 + radius * cos(angle * 3.14 / 180);
        int y2End =  600 - radius * sin(angle * 3.14 / 180);
        painter->drawLine(QPoint(x1Start, y1Start), QPoint(x2End, y2End));  // 画刻度线
    }
    painter->restore();
}

// 绘制指针
void TaskDashboard::drawDishBoardPointer(QPainter* painter) {
    painter->save();
    painter->setBrush(QBrush(QColor(QColor(255, 0, 0))));  // 红色画刷
    QPen qPen(QColor(QColor(255, 0, 0))); // 红色画笔
    qPen.setWidth(2);   //设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 指针三个顶点坐标
    double angle = 180 - 180 / (maxFore * percent * 2) * realForce;  // 指针中心角度信息
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;
    double rightAngle = angle - 45;  // 指针右侧顶点角度
    double leftAngle = angle + 45;  // 指针左侧顶点角度
    int x1Start = 600 + 465 * cos(angle * 3.14 / 180);
    int y1Start = 600 - 465 * sin(angle * 3.14 / 180);
    int x2End = 600 + 30 * cos(rightAngle * 3.14 / 180);
    int y2End = 600 - 30 * sin(rightAngle * 3.14 / 180);
    int x3End = 600 + 30 * cos(leftAngle * 3.14 / 180);
    int y3End = 600 - 30 * sin(leftAngle * 3.14 / 180);
    // 绘制指针，由于在前面设置了画刷所以会自动填充
    QPointF qTriangle[3] = {QPoint(x1Start, y1Start), QPoint(x2End, y2End), QPoint(x3End, y3End)};
    painter->drawPolygon(qTriangle, 3);  // 绘制三角形
    painter->drawEllipse(QPoint(600, 600), 45, 45);// 指针中心圆
    painter->restore();
}

// 绘制表盘中心文字
void TaskDashboard::drawDashBoardText(QPainter *painter) {
    painter->save();
    QPen qPen(Qt::white);  // 构造画笔，并将颜色初始化为白色
    qPen.setWidth(2);   // 设置画笔的粗细
    painter->setPen(qPen);  // 画家使用画笔
    // 绘制表盘中心文字
    QFont qFont("楷体", 20, QFont::Bold, false);
    painter->setFont(qFont);
    painter->drawText(QRectF(525, 550, 150, 100), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(100 * realForce / maxFore, 'f', 1) + "%");
    painter->restore();
}

