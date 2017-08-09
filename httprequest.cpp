#include "httprequest.h"

HttpRequest::HttpRequest(QObject *parent) : QObject(parent)
{
    requestType = WRONG_REQ;
    contentType = "";
    contentLength = 0;
    host = "localhost";
    body = "";
}
