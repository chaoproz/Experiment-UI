#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>  // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>  //提供系统中存在的串口信息，此外还需要在pro文件中添加：QT       += serialport

namespace Ui {
class MySerialPort;
}

class MySerialPort : public QWidget
{
    Q_OBJECT

public:
    explicit MySerialPort(QWidget *parent = nullptr);
    ~MySerialPort();
private slots:
    void on_pushButtonOpen_clicked();  // 打开串口
    void on_pushButtonClose_clicked();  // 关闭串口
    void on_pushButtonSend_clicked();  // 发送
    void on_pushButtonClearSend_clicked();  // 清除发送
    void on_pushButtonClearReceive_clicked();  // 清除接收
    void on_pushButtonCorrect_clicked();  // 零点校正
    void on_pushButtonRefresh_clicked();  // 刷新串口
    void on_pushButtonCloseWidget_clicked();  // 关闭窗口

private:
    QByteArray Hexstring2QByteArray(QString str);  // 将16进制的QString字符串转为QByteArray数组
    void InitialSetting();  // 界面初始化设置
    void SearchSerialPorts();  // 搜索可用串口，并添加到串口下拉框
    void closeEvent(QCloseEvent*);  // 重写关闭（"×"）事件，用于关闭串口

private:
    QByteArray data;  // 定义从串口接收到的数据
    QSerialPort* serial;  // 定义全局的串口对象
    Ui::MySerialPort *ui;
};

#endif // MYSERIALPORT_H
