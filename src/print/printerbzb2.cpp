#include "printerbzb2.h"

#include <QImage>

#include "../application.h"

#include <QDebug>

PrinterBZB2::PrinterBZB2(QString port, QObject *parent) : QObject(parent)
{
#ifdef Q_OS_WIN
    winPrinter = new WinPrinter(port, this);
#else
    portFile = new QFile(port, this);
#endif

    codec = QTextCodec::codecForName("CP866");
    isError = false;
}

void PrinterBZB2::beginCheque(bool printLogo)
{
#ifdef Q_OS_WIN
    isError = !winPrinter->openPrinter();
#else
    isError = !portFile->open(QFile::WriteOnly);
#endif

    if(!isError)
    {
        QByteArray data;

        //скорость 2"/сек
        data.append(0x1B);
        data.append("S2");

        //размер шрифта 1
        data.append(0x1B);
        data.append("A11");

        //прокрутка 25мм
        data.append(0x1B);
        data.append("N25");

        //Штрихкод EAN13
        data.append(0x1B);
        data.append("?E");

        //режим буферизации
        data.append(0x1B);
        data.append("B");

        //печать логотипа
        if(printLogo)
        {
            QImage colorImage("img/logo.bmp");
            QImage image = colorImage.convertToFormat(QImage::Format_Mono);

            for(int i = 0; i < image.height(); i++)
            {
                data.append(0x1B);
                data.append("G");
                data.append(QString::number(image.width() / 8));
                for(int j = 0; j < image.width() / 8; j++)
                    data.append(image.constScanLine(i)[j]);
            }

            data.append(0x0D);
        }


#ifdef Q_OS_WIN
        winPrinter->appendData(data);
#else
        portFile->write(data);
#endif
    }
}

void PrinterBZB2::printLine(int margin, const QString &line)
{
    QString marginStr = margin < 10 ? "M0" + QString::number(margin) : "M" + QString::number(margin);

    if(!isError)
    {
        QByteArray data;

        data.append(0x1B);
        data.append(codec->fromUnicode(marginStr + line));
        data.append(0x0D);

#ifdef Q_OS_WIN
        winPrinter->appendData(data);
#else
        portFile->write(data);
#endif
    }

}

void PrinterBZB2::printBarcode(const QString &barcodeStr)
{
    if(!isError)
    {
        QByteArray data;

        data.append(0x1B);
        data.append("A12");

        //data.append(0x1B);
        //data.append("M3");

        data.append(0x1B);
        data.append(codec->fromUnicode("{" + barcodeStr + "}"));
        data.append(0x0D);

        data.append(0x1B);
        data.append("A11");

#ifdef Q_OS_WIN
        winPrinter->appendData(data);
#else
        portFile->write(data);
#endif
    }

}

void PrinterBZB2::endCheque()
{
    if(!isError)
    {
        QByteArray data;

        //прокрутка
        data.append(0x0D);
        data.append(0x0D);
        data.append(0x0D);
        data.append(0x0C);

        //завершение
        data.append(0x1B);
        data.append("E");
        data.append(0x1B);
        data.append("P01");

#ifdef Q_OS_WIN
        winPrinter->appendData(data);
        isError = !(winPrinter->print());
        isError = !(winPrinter->closePrinter()) || isError;
#else
        portFile->write(data);
        portFile->close();
#endif

    }

}


bool PrinterBZB2::printCheque(const Cheque &cheque)
{
    int width = BZB2_CHEQUE_WIDTH;

    beginCheque(Application::getInstance()->settings->value("printLogo").toBool());
    printLine(3, "==========================");
    printLine(3, PrintService::lineFormatLR("Чек №" + cheque.number, "ПРОДАЖА", width));
    printLine(3, "Дата " + cheque.datetime.toString("dd.MM.yyyy   hh:mm:ss"));
    printLine(3, PrintService::lineFormatLeft("Оператор: " + cheque.seller, width));
    printLine(3, "Отдел: 1                  ");
    printLine(3, "==========================");
    printLine(3, PrintService::lineFormatCenter(cheque.orgName, width));
    printLine(3, "ИНН " + cheque.inn);
    printLine(3, "ККМ 00001                 ");
    printLine(3, "ПРОДАЖА                   ");
    printLine(3, "--------------------------");
    printLine(3, PrintService::lineFormatLeft("Чек №" + cheque.number, width));
    printLine(3, "Касса: 1                  ");
    printLine(3, PrintService::lineFormatLeft("Кассир: " + cheque.seller, width));
    printLine(3, PrintService::lineFormatLeft("Продавец: " + cheque.seller, width));
    printLine(3, "                          ");
    printLine(3, "                          ");
    printLine(3, "--------------------------");
    printLine(3, "Товар       Кол-во   Сумма");
    printLine(3, "--------------------------");
    for(ChequeLine line : cheque.lines)
    {
        printLine(3, PrintService::lineFormatLeft(line.name, width));
        printLine(3, PrintService::lineFormatLeft(line.artikul, width));
        printLine(3, PrintService::lineFormatLR(" ", QString::number(line.quantity) + "x" + QString::number(line.price, 'f', 2), width));
        printLine(3, PrintService::lineFormatLR("Скидка " + QString::number(line.discountPercent, 'f', 1) + "%", QString::number(line.discount, 'f', 2), width));
        printLine(3, "                          ");
    }
    printLine(3, "--------------------------");
    printLine(3, PrintService::lineFormatLR("Итого:", QString::number(cheque.getTotalsWithoutDiscount(), 'f', 2), width));
    printLine(3, PrintService::lineFormatLR("Итого со скидкой:", QString::number(cheque.getTotalsWithDiscount(), 'f', 2), width));
    printLine(3, PrintService::lineFormatLR("Итоговая скидка:", QString::number(cheque.getDiscountTotal(), 'f', 2), width));
    if(cheque.paymentNal > 0) printLine(3, PrintService::lineFormatLR("Наличными", QString::number(cheque.paymentNal, 'f', 2), width));
    if(cheque.paymentBeznal > 0) printLine(3, PrintService::lineFormatLR("Безналично", QString::number(cheque.paymentBeznal, 'f', 2), width));
    if(cheque.paymentCert > 0) printLine(3, PrintService::lineFormatLR("Сертификатом", QString::number(cheque.paymentCert, 'f', 2), width));
    printLine(3, PrintService::lineFormatLR("Сдача", QString::number(cheque.sdacha, 'f', 2), width));
    printLine(3, "--------------------------");
    //printLine(3, "  Цены даны с учетом НДС  ");
    printLine(3, "    Спасибо за покупку!   ");
    printLine(3, "                          ");

    endCheque();

    return (!isError);
}

bool PrinterBZB2::printChequeBarcodes(const Cheque &cheque)
{
    QList<Cheque> oneChequeList;
    oneChequeList.append(cheque);

    return printChequeBarcodes(oneChequeList);
}

bool PrinterBZB2::printChequeBarcodes(const QList<Cheque> cheques)
{
    int width = BZB2_CHEQUE_WIDTH;

    beginCheque(false);
    for(Cheque cheque : cheques)
    {
        printLine(3, "==========================");
        printLine(3, PrintService::lineFormatLR("Чек №" + cheque.number, "ПРОДАЖА", width));
        printLine(3, "Дата " + cheque.datetime.toString("dd.MM.yyyy   hh:mm:ss"));
        printLine(3, PrintService::lineFormatLeft("Оператор: " + cheque.seller, width));
        //printLine(3, "==========================");
        printLine(3, PrintService::lineFormatCenter(cheque.orgName, width));
        //printLine(3, "                          ");
        for(ChequeLine line : cheque.lines)
        {
            printLine(3, "--------------------------");
            printLine(3, PrintService::lineFormatLeft(line.name, width));
            printLine(3, PrintService::lineFormatLeft(line.artikul, width));
            printLine(3, PrintService::lineFormatLR(" ", QString::number(line.quantity) + "x" + QString::number(line.price, 'f', 2), width));
            printLine(3, PrintService::lineFormatLR("Скидка " + QString::number(line.discountPercent, 'f', 1) + "%", QString::number(line.discount, 'f', 2), width));
            printLine(3, PrintService::lineFormatLR("ИТОГО:", QString::number(line.summ, 'f', 2), width));
            //printLine(3, "                          ");
            //printLine(3, PrintService::lineFormatCenter("ШК ПОЗИЦИИ", width));
            //printLine(3, "                          ");
            printBarcode(line.barcode);
        }
        printLine(3, "                          ");
        printLine(3, "                          ");
        printLine(3, "                          ");
        printLine(3, "                          ");
    }
    endCheque();

    return(!isError);
}

QString PrinterBZB2::getLastError() const
{
    if(isError)
    {
#ifdef Q_OS_WIN
        return "WinPrinter error";
#else
        return portFile->errorString();
#endif
    }
    else
    {
        return "No errors";
    }
}
