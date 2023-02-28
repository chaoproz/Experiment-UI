#ifndef MAXFORCE_H
#define MAXFORCE_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>  // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>  //提供系统中存在的串口信息，此外还需要在pro文件中添加：QT       += serialport
#include <QPainter>
#include <QVector>
#include <QDateTime>

namespace Ui {
class MaxForce;
}

class MaxForce : public QWidget
{
    Q_OBJECT

public:
    explicit MaxForce(QWidget *parent = nullptr);
    ~MaxForce();
private slots:
    // 串口设置
    void on_pushButtonOpen_clicked();  // 打开串口
    void on_pushButtonClose_clicked();  // 关闭串口
    void on_pushButtonStart_clicked(); // 开始测量
    void on_pushButtonStop_clicked();  // 停止测量
    void on_pushButtonCorrect_clicked();  // 零点校正
    void on_pushButtonRefresh_clicked();  // 刷新串口
    void on_pushButtonCloseWidget_clicked();  // 关闭窗口
    // 填写测得的最大力
    void on_pushButtonPinchFirst_clicked();
    void on_pushButtonPinchSecond_clicked();
    void on_pushButtonPinchThird_clicked();
    void on_pushButtonPinchMean_clicked();
    void on_pushButtonTripodFirst_clicked();
    void on_pushButtonTripodSecond_clicked();
    void on_pushButtonTripodThird_clicked();
    void on_pushButtonTripodMean_clicked();
    void on_pushButtonLateralFirst_clicked();
    void on_pushButtonLateralSecond_clicked();
    void on_pushButtonLateralThird_clicked();
    void on_pushButtonLateralMean_clicked();

private:
    // 串口设置
    QByteArray Hexstring2QByteArray(QString str);  // 将16进制的QString字符串转为QByteArray数组
    void InitialSetting();  // 界面初始化设置
    void SearchSerialPorts();  // 搜索可用串口，并添加到串口下拉框

    // 绘图设置
    void paintEvent(QPaintEvent *event);  // 重写绘图事件
    void drawWaterProgressBarBoard(QPainter *painter);   // 绘制表盘
    void drawWaterProgressBarWave(QPainter *painter);  // 绘制水波效果
    void drawWaterProgressBarText(QPainter *painter);   // 绘制文本

    void closeEvent(QCloseEvent*);  // 重写关闭（"×"）事件，用于关闭串口。
    Ui::MaxForce *ui;
private:
    QSerialPort* serial;  // 定义全局的串口对象
    QTimer* timerData;  // 发送/接收数据时间间隔
    double value = 0;  // 测量的力的大小
    double maxValue = 0;  // 最大力的大小

    QPainter painter;  // 画家对象

    int mNum = 0;  // 提示信息标志
    QVector<double> valueData;  // 记录实时数据

};

#endif // MAXFORCE_H
