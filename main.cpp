#include "mainwindow.h"
#include "welcome.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Welcome wel;
    wel.show();

    return a.exec();
}
