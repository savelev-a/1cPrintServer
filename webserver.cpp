#include "webserver.h"

#include <QMessageBox>
#include <QTcpSocket>
#include <QApplication>
#include <QNetworkProxy>

#include "application.h"
#include "httpclient.h"
#include "httprequest.h"

#include <QDebug>

Webserver::Webserver(int port, QObject *parent) : QObject(parent)
{
    this->port = port;
    server = new QTcpServer(this);
    server->setProxy(QNetworkProxy::NoProxy);
    if(!server->listen(QHostAddress::Any, port))
    {
        QMessageBox::critical(0,
                              "Ошибка запуска веб-сервиса",
                              "Веб-сервис не был запущен, ошибка - " + server->errorString());
        server->close();
        exit(1);
    }

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void Webserver::newConnection()
{
    QTcpSocket *clientSocket = server->nextPendingConnection();
    HttpClient *client = new HttpClient(clientSocket);
}

void Webserver::startServer()
{
    this->port = Application::getInstance()->settings->value("webserverPort", 8080).toInt();
    server->listen(QHostAddress::Any, this->port);
    Application::getInstance()->mainWindow->refreshServerStatus();
}

void Webserver::stopServer()
{
    server->close();
    Application::getInstance()->mainWindow->refreshServerStatus();
}

bool Webserver::isStarted()
{
    return server->isListening();
}

int Webserver::currentPort()
{
    return this->port;
}
