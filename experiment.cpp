#include "experiment.h"
#include "ui_experiment.h"
#include <QDebug>
#include <QTimer>
#include <QMessageBox>

Experiment::Experiment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Experiment)
{
    ui->setupUi(this);
    // 窗口全屏显示
    QWidget::showFullScreen();

    // 设置窗口图标
    this->setWindowIcon(QIcon(":/Image/Main.png"));

    // 界面初始化设置
    InitialSetting();

    // 填充串口号下拉框
    SearchSerialPorts();

    //表盘设置
    ui->widget->move(this->width() / 2 - ui->widget->width() / 2 -100, this->height() / 2 - ui->widget->height() / 2);  // 移动表盘位置

}

Experiment::~Experiment()
{
    delete ui;
}
// 将16进制的QString字符串转为QByteArray数组
QByteArray Experiment::Hexstring2QByteArray(QString str) {
    str.remove(QRegExp("\\s"));  // 利用正则表达式去除str中的所有空格
    QByteArray Hexstr;
    bool ok;
    for (int i = 0; i < str.size(); i += 2) {
        Hexstr.append((char)str.mid(i, 2).toInt(&ok, 16));
    }
    return Hexstr;
}

// 界面初始化设置
void Experiment::InitialSetting() {
    // 填充串口波特率
    ui->comboBoxBaud->addItem("9600");
    ui->comboBoxBaud->addItem("14400");
    ui->comboBoxBaud->addItem("19200");
    ui->comboBoxBaud->addItem("28800");
    ui->comboBoxBaud->addItem("38400");
    ui->comboBoxBaud->setCurrentText("19200");  // 默认显示19200

    // 填充串口数据位
    ui->comboBoxData->addItem("5位");
    ui->comboBoxData->addItem("6位");
    ui->comboBoxData->addItem("7位");
    ui->comboBoxData->addItem("8位");
    ui->comboBoxData->setCurrentText("8位");  // 默认显示8位

    // 填充串口校验位
    ui->comboBoxParity->addItem("无校验");
    ui->comboBoxParity->addItem("偶校验");
    ui->comboBoxParity->addItem("奇校验");
    ui->comboBoxParity->setCurrentText("无校验");  // 默认显示无校验

    // 填充串口停止位
    ui->comboBoxStop->addItem("1位");
    ui->comboBoxStop->addItem("1.5位");
    ui->comboBoxStop->addItem("2位");
    ui->comboBoxStop->setCurrentText("1位");  // 默认显示1位
}

// 搜索可用串口，并添加到串口下拉框
void Experiment::SearchSerialPorts() {
    // 清除“串口号”下拉框内容
    ui->comboBoxPort->clear();
    serial = new QSerialPort;  // 在堆区申请内存
    // 获取有效的串口号，然后将搜索到的串口号的名称赋给界面中的串口号
    foreach(const QSerialPortInfo& info,QSerialPortInfo::availablePorts()) {
        serial->setPort(info);  // 在对象中设置串口
        // 以读写方式打开串口，若打开失败则进行提示
        if(serial->open(QIODevice::ReadWrite)) {
            ui->comboBoxPort->addItem((serial->portName()));  // 将搜索到的串口号的名称赋给界面中的串口号
            serial->close();  // 关闭
        } else {
            qDebug() << "串口打开失败，请重试";
        }
    }
}

// “打开串口”按键（将界面中串口号的串口打开）实现
void Experiment::on_pushButtonOpen_clicked()
{
    serial->setPortName(ui->comboBoxPort->currentText());   // 设置串口名
    if (serial->open(QIODevice::ReadWrite))
    {
        qDebug() << "打开成功！";
        serial->setBaudRate(ui->comboBoxBaud->currentText().toInt());  // 设置波特率
        // 设置数据位数
        switch(ui->comboBoxData->currentIndex())
        {
            case 0: serial->setDataBits(QSerialPort::Data5); break;
            case 1: serial->setDataBits(QSerialPort::Data6); break;
            case 2: serial->setDataBits(QSerialPort::Data7); break;
            case 3: serial->setDataBits(QSerialPort::Data8); break;
            default: break;
        }
        // 设置校验位
        switch(ui->comboBoxParity->currentIndex())
        {
            case 0: serial->setParity(QSerialPort::NoParity); break;
            case 1: serial->setParity(QSerialPort::EvenParity); break;
            case 2: serial->setParity(QSerialPort::OddParity); break;
            default: break;
        }
        // 设置停止位
        switch(ui->comboBoxStop->currentIndex())  // 设置停止位
        {
            case 0: serial->setStopBits(QSerialPort::OneStop); break;
            case 1: serial->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2: serial->setStopBits(QSerialPort::TwoStop); break;
            default: break;
        }
        serial->setFlowControl(QSerialPort::NoFlowControl);  // 设置流控制（无）
        ui->pushButtonOpen->setStyleSheet("color: green;");  // 将“打开串口”按键设为绿色
        ui->pushButtonOpen->setEnabled(false);  // 将“打开串口”按键锁住
        ui->pushButtonClose->setEnabled(true);  // 将“关闭串口”按键解锁
        ui->pushButtonStart->setEnabled(true);  // 将“开始测量”按键解锁
        ui->pushButtonCorrect->setEnabled(true);  // 将“零点校正”按键解锁
        ui->pushButtonRefresh->setEnabled(false);  // 将“刷新串口”按键锁住
    } else {
        qDebug() << "串口打开失败，请重试";
    }
}

// “关闭串口”按键实现
void Experiment::on_pushButtonClose_clicked()
{
    serial->close();  // 关闭串口
    // 恢复“打开串口”按键
    ui->pushButtonOpen->setStyleSheet("color: black;");
    ui->pushButtonOpen->setEnabled(true);  // 将“打开串口”按键解锁
    ui->pushButtonClose->setEnabled(false);  // 将“关闭串口”按键锁住
    ui->pushButtonStart->setEnabled(false);  // 将“开始测量”按键锁住
    ui->pushButtonStop->setEnabled(false);  // 将“停止测量”按键锁住
    ui->pushButtonCorrect->setEnabled(false);  // 将“零点校正...”按键锁住
    ui->pushButtonRefresh->setEnabled(true);  // 将“刷新串口”按键解锁
    timerData->stop();  // 关闭发送数据定时器
    qDebug() << "关闭成功！";
}

// “开始测量”按键实现
void Experiment::on_pushButtonStart_clicked()
{
    // 将16进制的QString字符串转为QByteArray数组
    QString sendDataStr = "AA AA AA 01 B1 00 00 1A";
    QByteArray sendData = Hexstring2QByteArray(sendDataStr);
    // 发送/接收数据
    timerData = new QTimer(this);
    timerData->setTimerType(Qt::PreciseTimer);
    timerData->start(20);  // 打开发送数据定时器
    connect(timerData,&QTimer::timeout,[=](){
        if (!serial->isReadable()) return;
        serial->write(sendData);  // 发送数据
        // 接收数据并解码
        if (!serial->isReadable()) return;
        QByteArray data = serial->readAll();  // 接收串口数据
        int16_t temp;
        // 判断帧头
        if((data[0].operator ==(0xBB)) && (data[1].operator ==(0xBB)) && (data[2].operator ==(0xBB)) && (data[3].operator ==(0x01)) && (data[4].operator ==(0xB1))) {
            temp = 0;
            temp = temp | (unsigned char)(data[5]);
            temp = temp << 8;
            temp = temp | (unsigned char)(data[6]);
            value = double(temp) / 100;  // 小数点在百位
            ui->widget->realForce = value;
        }
    });
    ui->pushButtonStart->setEnabled(false);  // 将“开始测量”按键锁住
    ui->pushButtonStop->setEnabled(true);  // 将“停止测量”按键解锁
    ui->pushButtonCorrect->setEnabled(false);  // 将“零点校正”按键锁住
}

// “停止测量”按键实现
void Experiment::on_pushButtonStop_clicked()
{
    timerData->stop();  // 关闭发送数据定时器
    ui->pushButtonStart->setEnabled(true);  // 将“开始测量”按键解锁
    ui->pushButtonStop->setEnabled(false);  // 将“停止测量”按键锁住
    ui->pushButtonCorrect->setEnabled(true);  // 将“零点校正”按键解锁
}

// “零点校正”按键实现
void Experiment::on_pushButtonCorrect_clicked()
{
    QString sendZeroStr = "AA AA AA 01 A7 00 00 0C";
    QByteArray sendZero = Hexstring2QByteArray(sendZeroStr);
    if (!serial->isReadable()) return;
    serial->write(sendZero);  // 发送零点校正命令
    QMessageBox::information(this,"信息", "已成功校正");
    ui->widget->realForce = 0;
}

// “刷新串口”按键实现
void Experiment::on_pushButtonRefresh_clicked()
{
    SearchSerialPorts();
}

// “关闭窗口”按键实现
void Experiment::on_pushButtonCloseWidget_clicked()
{
    serial->close();
    this->close();
    qDebug() << "关闭成功！";
}

// 重写关闭（"×"）事件，用于关闭串口
void Experiment::closeEvent(QCloseEvent*)
{
    serial->close();
    qDebug() << "关闭成功！";
}



// “刷新设置”按键实现
void Experiment::on_pushButtonRefreshSet_clicked()
{
    double force = ui->lineEditMaxForce->text().toDouble();
    double percent = ui->lineEditPercent->text().toDouble() / 100;
    // 判断输入类型和数值是否正确
    if (force == 0 || percent == 0) {
        QMessageBox::critical(this, "错误", "最大力和最大力的百分比输入类型错误或不能为0");
        return;
    }
    if (force < 0 || force > 10 || percent < 0.05 || percent > 0.5) {
        QMessageBox::critical(this, "错误", "最大力和最大力的百分比输入数值错误，最大力在0-10之间，百分比在5-50之间");
        return;
    }
    ui->widget->maxFore = force;  // 将输入的最大力传入测力表盘
    ui->widget->percent = ui->lineEditPercent->text().toDouble() / 100;  // 将输入的百分比传入测力表盘
    ui->pushButtonPrepare->setEnabled(true);

}
// “准备”按键实现
void Experiment::on_pushButtonPrepare_clicked()
{
    ui->widget_3->hide();
    ui->widget_4->hide();
    ui->widget->move(this->width() / 2 - ui->widget->width() / 2, this->height() / 2 - ui->widget->height() / 2);  // 移动表盘位置
    ui->pushButtonStartExperiment->setEnabled(true);
    ui->pushButtonPrepare->hide();
}

// “开始”按键实现
void Experiment::on_pushButtonStartExperiment_clicked()
{
    ui->pushButtonStartExperiment->hide();
    ui->widget_5->move(this->width() / 2 - ui->widget_5->width() / 2, this->height() / 2 - 100);  // 移动位置
    ui->widget_5->Run();  // 运行
    ui->widget_5->parallelPort = ui->lineEditParallelPort->text().toUInt();  // 定义并口
}

// “返回”按键实现
void Experiment::on_pushButtonBack_clicked()
{
    ui->widget_3->show();
    ui->widget_4->show();
    ui->pushButtonPrepare->show();
    ui->pushButtonStartExperiment->show();
    ui->widget->move(this->width() / 2 - ui->widget->width() / 2 -100, this->height() / 2 - ui->widget->height() / 2);  // 移动表盘位置
    ui->pushButtonPrepare->setEnabled(false);
    ui->pushButtonStartExperiment->setEnabled(false);
    ui->widget_5->timer->stop();
    ui->widget_5->loopNumber = 1;
    ui->widget_5->close();
}

// "最小化窗口"按键实现
void Experiment::on_pushButtonMinWidge_clicked()
{
    showMinimized(); // Qt最小化显示函数
}
