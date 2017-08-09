#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QObject>
#include <QTcpServer>

class Webserver : public QObject
{
    Q_OBJECT
public:
    explicit Webserver(int port, QObject *parent = 0);

    bool isStarted();
    int currentPort();

public slots:
    void startServer();
    void stopServer();

private:
    QTcpServer *server;
    int port;

private slots:
    void newConnection();

};

#endif // WEBSERVER_H
