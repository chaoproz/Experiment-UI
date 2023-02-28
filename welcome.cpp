#include "welcome.h"
#include "ui_welcome.h"
#include <QMessageBox>
Welcome::Welcome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
    // 设置窗口标题
    this->setWindowTitle("欢迎");
    // 设置窗口大小
    this->setMinimumSize(480, 300);
    this->setMaximumSize(480, 300);
    // 设置窗口图标
    this->setWindowIcon(QIcon(":/Image/Main.png"));
    // 登录：如果用户名和密码输入正确则登陆成功，否则发出警告
    connect(ui->pushButtonLogin, &QPushButton::clicked, [=](){
        if (ui->lineEditUser->text() == "chao" && ui->lineEditPassword->text() == "chao") {
            mw.show();
            this->close();
        } else {
            QMessageBox::warning(this, "警告", "用户名或密码错误");
        }
    });
    // 退出
    connect(ui->pushButtonExit, &QPushButton::clicked, [=](){
        this->close();
    });
}

Welcome::~Welcome()
{
    delete ui;
}
