#include "myserialport.h"
#include "ui_myserialport.h"
#include <QDebug>
#include <QTimer>
MySerialPort::MySerialPort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MySerialPort)
{
    ui->setupUi(this);
    // 设置窗口标题
    this->setWindowTitle("串口设置");
    // 设置窗口图标
    this->setWindowIcon(QIcon(":/Image/Main.png"));
    // 界面初始化设置
    InitialSetting();
    // 填充串口号下拉框
    SearchSerialPorts();
    // 接收数据(每隔20ms)
    QTimer * timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start(20);
    connect(timer,&QTimer::timeout,[=](){
        if (!serial->isReadable()) return;  // 如果串口已经关闭，则直接返回。
        data = serial->readAll();  // 接收串口数据
        if(!data.isEmpty())
        {
            QString str = data.toHex().toUpper();  // 将接收到的数据转为16进制的str
            // 每两个字符加1个空格
            QString strSpace(str.size() / 2 * 3, ' ');  // 提前为strSpace申请空间，避免出现Using QCharRef with an index pointing outside the valid range of a QString.
            for (int i = 0; i < str.size(); i += 2) {
                int newIndex = i + i / 2;
                strSpace[newIndex] = str[i];
                strSpace[newIndex + 1] = str[i + 1];
                strSpace[newIndex + 2] = ' ';
            }
            ui->textBrowser->append(strSpace + "\n");  // “接收窗口显示”
        }
    });
}

MySerialPort::~MySerialPort()
{
    delete ui;
}

// 将16进制的QString字符串转为QByteArray数组，用于将“发送窗口”中的数据发送给串口
QByteArray MySerialPort::Hexstring2QByteArray(QString str) {
    str.remove(QRegExp("\\s"));  // 利用正则表达式去除str中的所有空格
    QByteArray Hexstr;
    bool ok;
    for (int i = 0; i < str.size(); i += 2) {
        Hexstr.append((char)str.mid(i, 2).toInt(&ok, 16));
    }
    return Hexstr;
}

// 界面初始化设置
void MySerialPort::InitialSetting() {
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

    // 默认16进制显示
    ui->checkBoxBit->setChecked(true);
}

// 搜索可用串口，并添加到串口下拉框
void MySerialPort::SearchSerialPorts() {
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
void MySerialPort::on_pushButtonOpen_clicked()
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
        ui->pushButtonSend->setEnabled(true);  // 将“发送”按键解锁
        ui->pushButtonCorrect->setEnabled(true);  // 将“零点校正...”按键解锁
        ui->pushButtonRefresh->setEnabled(false);  // 将“刷新串口”按键锁住
    } else {
        qDebug() << "串口打开失败，请重试";
    }
}

// “关闭串口”按键实现
void MySerialPort::on_pushButtonClose_clicked()
{
    serial->close();  // 关闭串口
    // 恢复“打开串口”按键
    ui->pushButtonOpen->setStyleSheet("color: black;");
    ui->pushButtonOpen->setEnabled(true);  // 将“打开串口”按键解锁
    ui->pushButtonClose->setEnabled(false);  // 将“关闭串口”按键锁住
    ui->pushButtonSend->setEnabled(false);  // 将“发送”按键锁住
    ui->pushButtonCorrect->setEnabled(false);  // 将“零点校正...”按键锁住
    ui->pushButtonRefresh->setEnabled(true);  // 将“刷新串口”按键解锁
    qDebug() << "关闭成功！";
}

// “发送”按键实现
void MySerialPort::on_pushButtonSend_clicked()
{

    QByteArray sendData = Hexstring2QByteArray(ui->textEditSend->toPlainText());
    serial->write(sendData);
}

// “清除发送”按键实现
void MySerialPort::on_pushButtonClearSend_clicked()
{
    ui->textEditSend->clear();  // 清空发送窗口
}


// “清除接收”按键实现
void MySerialPort::on_pushButtonClearReceive_clicked()
{
    ui->textBrowser->clear();  // 清除接收窗口
}

// “零点校正...”按键实现
void MySerialPort::on_pushButtonCorrect_clicked()
{
    QString sendZeroStr = "AA AA AA 01 A7 00 00 0C";
    QByteArray sendZero = Hexstring2QByteArray(sendZeroStr);
    serial->write(sendZero);
}

// “刷新串口”按键实现
void MySerialPort::on_pushButtonRefresh_clicked()
{
    SearchSerialPorts();
}

// “关闭窗口”按键实现
void MySerialPort::on_pushButtonCloseWidget_clicked()
{
    serial->close();
    this->close();
    qDebug() << "关闭成功！";
}

// 重写关闭（"×"）事件，用于关闭串口
void MySerialPort::closeEvent(QCloseEvent*)
{
    serial->close();
    qDebug() << "关闭成功！";
}
