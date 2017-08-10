#ifndef PRINTSERVICE_H
#define PRINTSERVICE_H

#include <QObject>
#include <QPrinter>
#include <QTextCodec>

#include "../model/cheque.h"

class PrintService : public QObject
{
    Q_OBJECT
public:
    explicit PrintService(QObject *parent = 0);

    bool print(const QString &data);
    bool print(const Cheque &cheque);
    bool printBarcode(const Cheque &cheque);

    QString getLastError() const;

    static QString lineFormatCenter(const QString &data, int width);
    static QString lineFormatLeft(const QString &data, int width);
    static QString lineFormatLR(const QString &leftStr, const QString &rightStr, int width);

public slots:
    void testPrint();

private:

    QPrinter *printer;
    QTextCodec *codec;
    QString errorStr;

};

#endif // PRINTSERVICE_H
