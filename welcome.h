#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>
#include "mainwindow.h"
namespace Ui {
class Welcome;
}

class Welcome : public QWidget
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    MainWindow mw;  // 主菜单
    ~Welcome();

private:
    Ui::Welcome *ui;
};

#endif // WELCOME_H
