#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QTcpSocket>

#include "httprequest.h"

class HttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QTcpSocket *socket);

    QTcpSocket *socket;
    HttpRequest *request;

signals:
    void requestReady();

private slots:
    void readClient();
    void processRequest();

private:
    QTextCodec *codec;
    void writeClient(const QString &data);
    void httpPostWriteReply(bool chequesPrinted, const QString &errorStr);
    bool httpPostBodyLoadMode;
    long httpPostBodyBytesRecieved;
};

#endif // HTTPCLIENT_H
