#include "maxforce.h"
#include "ui_maxforce.h"
#include <QDebug>
#include <QTimer>
#include <QMessageBox>  // 消息对话框

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QtMath>
MaxForce::MaxForce(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaxForce)
{
    ui->setupUi(this);
    // 设置窗口名称
    this->setWindowTitle("最大力测试");
    // 设置窗口图标
    this->setWindowIcon(QIcon(":/Image/Main.png"));
    // 固定窗口大小
    this->setMinimumSize(1200, 1000);
    this->setMaximumSize(1200, 1000);

    // 界面初始化设置
    InitialSetting();

    // 填充串口号下拉框
    SearchSerialPorts();


    // 如果要手动调用绘图事件则用update更新
    QTimer* timerPainter = new QTimer(this);
    timerPainter->setTimerType(Qt::PreciseTimer);
    timerPainter->start(20);
    connect(timerPainter,SIGNAL(timeout()),this,SLOT(update()));

    // 准备-开始-保持-休息
    QTimer* timerMeasure = new QTimer(this);
    timerMeasure->setTimerType(Qt::PreciseTimer);
    connect(ui->pushButtonS,&QPushButton::clicked,[=](){
        timerMeasure->stop();
        timerMeasure->start(1000);
        ui->labelExperiment->setText("准备");
        mNum = 1;
        valueData.clear();
    });
    connect(timerMeasure,&QTimer::timeout,[=](){
        if (mNum == 1) {
            ui->labelExperiment->setText("开始");
        }
        if (mNum == 3) {
            ui->labelExperiment->setText("保持");
        }
        if (mNum == 7) {
            // 通过平均值求最大力
            double sum = 0;
            for (auto i : valueData) sum += i;
            maxValue = sum / valueData.size();
            valueData.clear();
            ui->labelMax->setText(QString::number(maxValue, 'f', 2));
            ui->labelExperiment->setText("休息");
        }
        if (mNum == 10) {
            ui->labelExperiment->setText(" ");
            timerMeasure->stop();
        }
        mNum++;
    });

}

MaxForce::~MaxForce()
{
    delete ui;
}

// 将16进制的QString字符串转为QByteArray数组
QByteArray MaxForce::Hexstring2QByteArray(QString str) {
    str.remove(QRegExp("\\s"));  // 利用正则表达式去除str中的所有空格
    QByteArray Hexstr;
    bool ok;
    for (int i = 0; i < str.size(); i += 2) {
        Hexstr.append((char)str.mid(i, 2).toInt(&ok, 16));
    }
    return Hexstr;
}

// 界面初始化设置
void MaxForce::InitialSetting() {
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
void MaxForce::SearchSerialPorts() {
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
void MaxForce::on_pushButtonOpen_clicked()
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
void MaxForce::on_pushButtonClose_clicked()
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
    maxValue = 0;  // 最大力回归0
    ui->labelMax->clear();  // 将最大力值清空
    timerData->stop();  // 关闭发送数据定时器
    qDebug() << "关闭成功！";
}

// “开始测量”按键实现
void MaxForce::on_pushButtonStart_clicked()
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
        }
        // 最大力：  kg设置
        if (mNum > 3 && mNum <= 7) valueData.push_back(value);
    });
    ui->pushButtonStart->setEnabled(false);  // 将“开始测量”按键锁住
    ui->pushButtonStop->setEnabled(true);  // 将“停止测量”按键解锁
    ui->pushButtonCorrect->setEnabled(false);  // 将“零点校正”按键锁住

}

// “停止测量”按键实现
void MaxForce::on_pushButtonStop_clicked()
{
    timerData->stop();  // 关闭发送数据定时器
    ui->pushButtonStart->setEnabled(true);  // 将“开始测量”按键解锁
    ui->pushButtonStop->setEnabled(false);  // 将“停止测量”按键锁住
    ui->pushButtonCorrect->setEnabled(true);  // 将“零点校正”按键解锁
    maxValue = 0;  // 最大力回归0
    ui->labelMax->clear();  // 将最大力值清空
}

// “零点校正”按键实现
void MaxForce::on_pushButtonCorrect_clicked()
{
    QString sendZeroStr = "AA AA AA 01 A7 00 00 0C";
    QByteArray sendZero = Hexstring2QByteArray(sendZeroStr);
    if (!serial->isReadable()) return;
    serial->write(sendZero);  // 发送零点校正命令 
    QMessageBox::information(this,"信息", "已成功校正");
    value = 0;  // 将实时力改为0
}

// “刷新串口”按键实现
void MaxForce::on_pushButtonRefresh_clicked()
{
    SearchSerialPorts();
}

// “关闭窗口”按键实现
void MaxForce::on_pushButtonCloseWidget_clicked()
{
    serial->close();
    this->close();
    qDebug() << "关闭成功！";
}

void MaxForce::closeEvent(QCloseEvent*)
{
    serial->close();
    qDebug() << "关闭成功！";
}


//// --------------------绘图函数--------------------

void MaxForce::paintEvent(QPaintEvent*)
{
    QPainter painter(this);  // 新建画家
    painter.setRenderHint(QPainter::Antialiasing);  // 抗锯齿
    drawWaterProgressBarBoard(&painter);  // 绘制表盘
    drawWaterProgressBarWave(&painter);  // 绘制水波效果
    drawWaterProgressBarText(&painter);   // 绘制文本

}

// 绘制表盘
void MaxForce::drawWaterProgressBarBoard(QPainter *painter)
{
     painter->save();  // 保存原始的画家状态
     QBrush qBrush = QBrush(QColor(80,80,80));  // 灰色画刷，设置边缘色
     painter->setBrush(qBrush);  // 使用画刷
     painter->drawEllipse(QPoint(400, 400), 250, 250);  // 画圆
     painter->restore();  // 还原到原始的画家状态
}

// 绘制水波效果
void MaxForce::drawWaterProgressBarWave(QPainter *painter)
{
    painter->save();
    QPen qPen(QColor(13,235,235));  // 设置画笔颜色
    qPen.setWidth(1);   // 设置画笔的粗细
    painter->setPen(qPen);  // 使用画笔
    if (value > 10) {
        value = 10;  // 当超过阈值时设为最大值
        // 发出警告！！
        QMessageBox::warning(this, "警告", "超出最大量程10kg");
        return;
    }
//    if (value < 0) value = 0;  // 避免小于0绘图出错
    // 画水平波浪
    QRect rect(150, 150, 500, 500);  // 圆的外接正方形
    QPainterPath path;
    path.moveTo(400 + sqrt(qPow(400, 2) - qPow(250 - 50 * value, 2)), 650 - 50 * value);  // 将画笔移动到水平波浪右下角，50指的是每1kg所占的长度（500mm/10kg得到）
    // 绘制过程
    double angle1 = qAsin((250 - 50 * value) / 250);  // 得到的是弧度
    double angle2 = qAcos((250 - 50 * value) / 250);
    double angle3 = qAsin((50 * value - 250) / 250);
    double angle4 = qAcos((50 * value - 250) / 250);
    if (value <= 5) {
        path.arcTo(rect, -angle1 * 180 / 3.14, -2 * angle2 * 180 / 3.14);  // 需要传入角度，第一个角度参数是起始角度，第二个角度参数是转过的角度（负数是顺时针转）
    }
    else {
        path.arcTo(rect, angle3 * 180 / 3.14, -2 * (180 - angle4 * 180 / 3.14));
    }
    painter->fillPath(path, QBrush(QColor(13,235,235)));  // 封闭图形并填充
    painter->restore();
}

// 绘制文本
void MaxForce::drawWaterProgressBarText(QPainter *painter)
{
    painter->save();
    int FontSize = 30;  // 设置字体大小
    QFont qFont("楷体",FontSize,QFont::Bold,false);   // 设置字体格式
    QPen qPen = QPen(Qt::white);  // 设置画笔颜色
    //绘制表盘文字
    painter->setFont(qFont);
    painter->setPen(qPen);
    QString PercentageStr = QString::number(value);
    PercentageStr.append("kg");
    painter->translate(400, 400);  // 平移坐标原点到表盘文本坐标原点
    painter->drawText(QRectF(-100, -100, 200, 200), Qt::AlignHCenter | Qt::AlignVCenter, PercentageStr);
    painter->restore();
}

// 填入最大力
void MaxForce::on_pushButtonPinchFirst_clicked()
{
    ui->lineEditPinchFirst->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonPinchSecond_clicked()
{
    ui->lineEditPinchSecond->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonPinchThird_clicked()
{
    ui->lineEditPinchThird->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonPinchMean_clicked()
{
    double max1 = ui->lineEditPinchFirst->text().toDouble();
    double max2 = ui->lineEditPinchSecond->text().toDouble();
    double max3 = ui->lineEditPinchThird->text().toDouble();
    double pinchMean = (max1 + max2 + max3) / 3;
    ui->labelPinchMean->setText(QString::number(pinchMean, 'f', 2));
}

void MaxForce::on_pushButtonTripodFirst_clicked()
{
    ui->lineEditTripodFirst->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonTripodSecond_clicked()
{
    ui->lineEditTripodSecond->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonTripodThird_clicked()
{
    ui->lineEditTripodThird->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonTripodMean_clicked()
{
    double max1 = ui->lineEditTripodFirst->text().toDouble();
    double max2 = ui->lineEditTripodSecond->text().toDouble();
    double max3 = ui->lineEditTripodThird->text().toDouble();
    double tripodMean = (max1 + max2 + max3) / 3;
    qDebug() << tripodMean;
    ui->labelTripodMean->setText(QString::number(tripodMean, 'f', 2));
}


void MaxForce::on_pushButtonLateralFirst_clicked()
{
    ui->lineEditLateralFirst->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonLateralSecond_clicked()
{
    ui->lineEditLateralSecond->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonLateralThird_clicked()
{
    ui->lineEditLateralThird->setText(ui->labelMax->text());
}

void MaxForce::on_pushButtonLateralMean_clicked()
{
    double max1 = ui->lineEditLateralFirst->text().toDouble();
    double max2 = ui->lineEditLateralSecond->text().toDouble();
    double max3 = ui->lineEditLateralThird->text().toDouble();
    double lateralMean = (max1 + max2 + max3) / 3;
    qDebug() << lateralMean;
    ui->labelLateralMean->setText(QString::number(lateralMean, 'f', 2));
}

