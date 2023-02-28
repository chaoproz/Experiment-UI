#ifndef TASKINTERFACE_H
#define TASKINTERFACE_H

#include <QWidget>
#include <QLabel>

class TaskInterface : public QWidget
{
    Q_OBJECT

public:
    explicit TaskInterface(QWidget *parent = nullptr);
    ~TaskInterface();
public slots:
    void timerTimeout();
public:
    QTimer* timer;  // 定时器
public:
    void Run();
    int parallelPort = 57084;
    int loopNumber = 1;  // 目前循环次数

private:
    QLabel* label;  // 界面内容内容
    int second = 0;  // 每次循环时还剩几秒
};

#endif // TASKINTERFACE_H

extern "C" {
    #include "inpout32.h"
}
