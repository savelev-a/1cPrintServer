#include "application.h"

Application* Application::appInstance = 0;

Application* Application::getInstance()
{
    if(appInstance == 0) appInstance = new Application;

    return appInstance;
}

void Application::init()
{
    qDebug() << "Загрузка настроек..";
    settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    qDebug() << "Инициализация БД..";
    databaseService = new DatabaseService(this);

    qDebug() << "Запуск сервисов..";
    int port = settings->value("webserverPort", 8080).toInt();;
    webserver = new Webserver(port, this);
    printService = new PrintService(this);
    mainWindow = new MainWindow();

    qDebug() << "Загрузка завершена";
}
