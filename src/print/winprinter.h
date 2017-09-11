#ifndef WINPRINTER_H
#define WINPRINTER_H

#include <QObject>

#ifdef Q_OS_WIN

#include "windows.h"

class WinPrinter : public QObject
{
    Q_OBJECT
public:
    explicit WinPrinter(const QString &printerName, QObject *parent = 0);

    bool openPrinter();
    void appendData(const QByteArray &data);
    bool print();
    bool closePrinter();

private:
    TCHAR printerName[64];
    QByteArray document;

    HANDLE hPrinter = NULL;
    DOC_INFO_1 docinfo;
    DWORD bytesWritten;
};

#endif

#endif // WINPRINTER_H
