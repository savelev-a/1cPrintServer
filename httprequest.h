#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>

enum HttpRequestType
{
    GET,
    POST,
    WRONG_REQ
};

class HttpRequest : public QObject
{
    Q_OBJECT
public:
    explicit HttpRequest(QObject *parent = 0);

    HttpRequestType requestType;
    QString contentType;
    long contentLength;
    QString host;
    QString body;
};

#endif // HTTPREQUEST_H
