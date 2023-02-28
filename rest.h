#ifndef REST_H
#define REST_H

#include <QWidget>

namespace Ui {
class Rest;
}

class Rest : public QWidget
{
    Q_OBJECT

public:
    explicit Rest(QWidget *parent = nullptr);
    ~Rest();

    void paintEvent(QPaintEvent* event);  // 绘图事件
    void init(QPainter* painter);  // 初始界面
    void restMin(QPainter* painter);  // 实验界面
    void finish(QPainter* painter);  // 完成实验
    int flag = 0;
    int parallelPort = 57084;
    int highCount = 0;

private slots:
    void on_pushButtonStart_clicked();

    void on_pushButtonBack_clicked();

private:
    Ui::Rest *ui;
};

#endif // REST_H

extern "C" {
    #include "inpout32.h"
}

