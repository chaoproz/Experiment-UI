#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>  // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>  //提供系统中存在的串口信息，此外还需要在pro文件中添加：QT       += serialport
#include "taskinterface.h"
namespace Ui {
class Experiment;
}

class Experiment : public QWidget
{
    Q_OBJECT

private slots:
    // 串口设置
    void on_pushButtonOpen_clicked();  // 打开串口
    void on_pushButtonClose_clicked();  // 关闭串口
    void on_pushButtonStart_clicked(); // 开始测量
    void on_pushButtonStop_clicked();  // 停止测量
    void on_pushButtonCorrect_clicked();  // 零点校正
    void on_pushButtonRefresh_clicked();  // 刷新串口
    void on_pushButtonCloseWidget_clicked();  // 关闭窗口
    void on_pushButtonMinWidge_clicked();  // 最小化窗口
    void on_pushButtonRefreshSet_clicked();  // 刷新设置
    void on_pushButtonPrepare_clicked(); // 准备
    void on_pushButtonStartExperiment_clicked();  // 开始
    void on_pushButtonBack_clicked();  // 返回



public:
    explicit Experiment(QWidget *parent = nullptr);
    ~Experiment();

private:
    // 串口设置
    QByteArray Hexstring2QByteArray(QString str);  // 将16进制的QString字符串转为QByteArray数组
    void InitialSetting();  // 界面初始化设置
    void SearchSerialPorts();  // 搜索可用串口，并添加到串口下拉框
    void closeEvent(QCloseEvent*);  // 重写关闭（"×"）事件，用于关闭串口

private:
    QSerialPort* serial;  // 定义全局的串口对象
    QTimer* timerData;  // 发送/接收数据时间间隔
    double value = 0;  // 测量的力的大小
//    TaskInterface* tif;
    Ui::Experiment *ui;
};

#endif // EXPERIMENT_H
