#include "httpclient.h"

#include <QTextCodec>
#include <QFile>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

#include <QDebug>

#include "../application.h"
#include "../model/cheque.h"

HttpClient::HttpClient(QTcpSocket *socket) : QObject(socket)
{
    this->socket = socket;
    this->codec = QTextCodec::codecForName("UTF-8");
    this->request = new HttpRequest(this);

    httpPostBodyLoadMode = false;
    httpPostBodyBytesRecieved = 0;

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(requestReady()), this, SLOT(processRequest()));
}


void HttpClient::readClient()
{
    while(socket->bytesAvailable())
    {
        QString line = codec->toUnicode(socket->readLine());

        if(httpPostBodyLoadMode)
        {
            request->body.append(line);
            httpPostBodyBytesRecieved += line.toLocal8Bit().length();
            if(httpPostBodyBytesRecieved >= request->contentLength)
            {
                httpPostBodyLoadMode = false;
                httpPostBodyBytesRecieved = 0;
                emit requestReady();
            }
        }
        else
        {
            if(line.contains("GET")) request->requestType = GET;
            else if(line.contains("POST")) request->requestType = POST;
            else if(line.contains("Host")) request->host = line.split(": ").last();
            else if(line.contains("Content-Length"))
            {
                QString lengthStr = line.split(": ").last();
                request->contentLength = lengthStr.toLong();
            }
            else if(line == "\r\n")
            {
                if(request->requestType == GET) emit requestReady();
                else if(request->requestType == POST)
                {
                    httpPostBodyLoadMode = true;
                }
            }
        }
    }
}

void HttpClient::writeClient(const QString &data)
{
    socket->write(data.toUtf8());
    socket->waitForBytesWritten(1000);
}

void HttpClient::processRequest()
{
    if(request->requestType == GET)
    {
        writeClient("HTTP/1.1 200 OK\r\n");
        writeClient("Content-Type: text/html; charset=utf-8\r\n");
        writeClient("\r\n");
        writeClient("<html><head><title>1cPrintServer</title></head><body>"
                    "Welcome to 1C Print Server!<br>");
        writeClient("</body></html>\r\n");
        writeClient("\r\n");

        socket->close();
    }

    else if(request->requestType == POST)
    {
        QJsonDocument document = QJsonDocument::fromJson(request->body.toLocal8Bit());
        QJsonArray chequesArray = document.array();

        for(QJsonValue chequeValue : chequesArray)
        {
            QJsonObject chequeObject = chequeValue.toObject();
            if(chequeObject.isEmpty())
            {
                qDebug() << "Json object empty!";
                httpPostWriteReply(ParseError, "Неверный формат JSON");
                return;
            }
            Cheque c(chequeObject);

            if(!Application::getInstance()->printService->print(c))
            {
                qDebug() << "Printer error!";
                httpPostWriteReply(PrinterError, Application::getInstance()->printService->getLastError());
                return;
            }

            if(!Application::getInstance()->databaseService->saveCheque(c))
            {
                qDebug() << "DB save error!";
                httpPostWriteReply(DatabaseError, Application::getInstance()->databaseService->getLastError());
                return;
            }
            Application::getInstance()->databaseService->refreshModel();
            Application::getInstance()->mainWindow->refreshTableAndTotals();
        }

        httpPostWriteReply(NoError, QString::number(chequesArray.size()));
    }
}


void HttpClient::httpPostWriteReply(ErrorType errorType, const QString &errorStr)
{
    QJsonObject responce;

    if(errorType == NoError)
    {
        responce.insert("result", QJsonValue("OK"));
        responce.insert("chequesPrinted", QJsonValue(errorStr));
    }
    else
    {
        responce.insert("result", QJsonValue("ERROR"));

        switch (errorType)
        {
        case 1:
            responce.insert("errorType", QJsonValue("JSON parse error"));
            break;
        case 2:
            responce.insert("errorType", QJsonValue("Printer error"));
            break;
        case 3:
            responce.insert("errorType", QJsonValue("Database error"));
            break;
        default:
            responce.insert("errorType", QJsonValue("Unknown error"));
            break;
        }


    }

    responce.insert("errorText", errorStr);

    writeClient("HTTP/1.1 200 OK\r\n");
    writeClient("Content-Type: text/html; charset=utf-8\r\n");
    writeClient("\r\n");
    writeClient(QJsonDocument(responce).toJson());
    writeClient("\r\n");

    socket->close();
}
