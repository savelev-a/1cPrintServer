#include "winprinter.h"

#ifdef Q_OS_WIN

#include <QDebug>

WinPrinter::WinPrinter(const QString &printerName, QObject *parent) : QObject(parent)
{
    printerName.toWCharArray(this->printerName);
    this->printerName[printerName.size()] = 0x00;
}

bool WinPrinter::openPrinter()
{
    docinfo.pDocName = TEXT("Cheque document");
    docinfo.pOutputFile = NULL;
    docinfo.pDatatype = TEXT("RAW");

    return OpenPrinter(printerName, &hPrinter, NULL);
}

void WinPrinter::appendData(const QByteArray &data)
{
    this->document.append(data);
}

bool WinPrinter::print()
{
    DWORD printJob = StartDocPrinter(hPrinter, 1, (LPBYTE)&docinfo);
    if(printJob > 0)
    {
        if(!WritePrinter(hPrinter, document.data(), document.size(), &bytesWritten))
        {
            return true;
        }
        else
        {
            qDebug() << "Ошибка отправки на печать";
        }
    }
    else
    {
        qDebug() << "Ошибка назначения задания печати";
    }

    return false;
}

bool WinPrinter::closePrinter()
{
    bool result = true;

    result = result && (EndDocPrinter(hPrinter) != 0);
    result = result && (ClosePrinter(hPrinter) != 0);

    return result;
}

#endif
