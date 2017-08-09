#include <QApplication>

#include "application.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Application::getInstance()->init();

    return a.exec();
}
