#ifndef KEYEXPERIMENT_H
#define KEYEXPERIMENT_H

#include <QWidget>
#include <QKeyEvent>
#include <vector>
#include <stack>
#include <QDateTime>
using namespace std;
namespace Ui {
class KeyExperiment;
}

class KeyExperiment : public QWidget
{
    Q_OBJECT

public:
    explicit KeyExperiment(QWidget *parent = nullptr);
    ~KeyExperiment();

private slots:
    void on_pushButtonBack_clicked();

    void on_pushButtonStart_clicked();

private:
    vector<int> randomInt();  // 生成随机数
    void paintEvent(QPaintEvent* event);  // 绘图事件
    void init(QPainter* painter);  // 初始界面
    void keyPress(QPainter* painter);  // 随机出现数字界面
    void finish(QPainter* painter);  // 完成实验界面
    void keyPressEvent(QKeyEvent *event);  // 键盘事件
private:
    int flag = -1;  // 界面标志位置
    vector<int> vec;  // 随机产生的按键顺序
    vector<int> press;  // 被试按下的按键
    vector<int> RTime;  // 反应时间
    int keyNum = 0;  // 第几次按键
    QDateTime timeStart;  // 记录每次按键开始的时间
    QDateTime timeEnd;  // 记录每次按键结束的时间

    Ui::KeyExperiment *ui;
};

#endif // KEYEXPERIMENT_H
