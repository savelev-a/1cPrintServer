#ifndef PRINTERBZB2_H
#define PRINTERBZB2_H

#include <QObject>
#include <QFile>
#include <QTextCodec>

#include "../model/cheque.h"
#include "../model/chequeline.h"
#include "printservice.h"

class PrinterBZB2 : public QObject
{
    Q_OBJECT
public:
    explicit PrinterBZB2(QString port, QObject *parent = 0);

    void beginCheque(bool printLogo);
    void printLine(int margin, const QString &line);
    void printBarcode(const QString &barcodeStr);
    void endCheque();

    bool printCheque(const Cheque &cheque);
    bool printChequeBarcodes(const Cheque &cheque);
    bool printChequeBarcodes(const QList<Cheque> cheques);
    QString getLastError() const;

    static const int BZB2_CHEQUE_WIDTH = 26;

signals:
    void printerError(const QString &errorString);

private:
    QFile *portFile;
    QTextCodec *codec;
    bool isError;
};

#endif // PRINTERBZB2_H
