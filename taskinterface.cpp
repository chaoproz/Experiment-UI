#include "taskinterface.h"
#include <QVBoxLayout>
#include <QTimer>

TaskInterface::TaskInterface(QWidget *parent) :
    QWidget(parent)
{
    // 固定窗口大小
    this->setMinimumSize(500, 300);
    this->setMaximumSize(500, 300);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);  // 设置窗口属性：去掉标题栏,去掉工具栏
    setAttribute(Qt::WA_TranslucentBackground);  // 全透明
    label = new QLabel(this);
    // label在widget中纵向布局
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->addWidget(label);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);  // label内容水平垂直居中
    //设置label内容格式：字体微软雅黑、大小200，颜色
    QString strStyle = "QLabel{"
                       "font-family: \"Microsoft YaHei\";"
                       "font-size: 200px;"
                       "color: rgb(0, 0, 0);"
                       "}";
    //将上述格式应用到label
    label->setStyleSheet(strStyle);
    // 创建时间
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    // 发送信号不能在on_timer_timeout()里面写，否则发送不了数据。
    connect(timer, &QTimer::timeout, [=]() {
        timerTimeout();
    });
}

TaskInterface::~TaskInterface()
{
}

void TaskInterface::Run()
{
    timer->stop();
    Out32(parallelPort, 0);
    second = 10;  // 设置初始时间
    timer->start(1000);
    timerTimeout();
    this->show();
}
void TaskInterface::timerTimeout()
{
    // 循环5次
    if (second == 0  && loopNumber != 5) {
        this->Run();
        loopNumber++;
        return;
    }
    if (second == 0 && loopNumber == 5) {
        timer->stop();
        this->close();
        second = 10;  // 设置初始时间
        loopNumber = 1;
        return;
    }
    // 界面显示
    if (second == 10) label->setText("准备");
    if (second == 9) label->setText("开始");
    if (second == 7) {
        label->setText("保持");
        Out32(parallelPort, loopNumber + 128);
    }
    if (second == 3) {
        label->setText("休息");
        Out32(parallelPort, 0);
    }
    second--;  // 剩余时间减1s
}
