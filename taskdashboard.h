#ifndef TASKDASHBOARD_H
#define TASKDASHBOARD_H

#include <QWidget>

namespace Ui {
class TaskDashboard;
}

class TaskDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit TaskDashboard(QWidget *parent = nullptr);
    ~TaskDashboard();

private:
    // 绘图设置
    void paintEvent(QPaintEvent* event);  // 重写绘图事件
    void setOtherBackgroud(QPainter* painter);  // 将背景改为透明
    void drawDashBoard(QPainter* painter);  // 绘制表盘
    void drawDivideRule(QPainter* painter);  // 绘制刻度
    void drawDishBoardPointer(QPainter* painter);  // 绘制指针
    void drawDashBoardText(QPainter* painter);  // 绘制表盘中心文字
public:
    double maxFore = 10;  // 最大显示力即表盘所能显示的最大力
    double percent = 0.3;  // 几倍的MVC，需要外部
    double realForce = 0;  // 实时力的大小
private:
    Ui::TaskDashboard *ui;
};

#endif // TASKDASHBOARD_H

