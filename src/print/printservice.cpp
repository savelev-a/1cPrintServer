#include "printservice.h"

#include <QPainter>
#include <QFile>

#include "../model/cheque.h"
#include "../model/chequeline.h"
#include "printerbzb2.h"
#include "../application.h"

#include <QDebug>

PrintService::PrintService(QObject *parent) : QObject(parent)
{
    codec = QTextCodec::codecForName("866");
    printer = new QPrinter(QPrinter::HighResolution);
    printer->setPrinterName("HP_MFP");

}

bool PrintService::print(const QString &data)
{
    QPainter painter;

    painter.begin(printer);
    painter.drawText(100, 100, 10000, 10000, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, data);
    painter.end();

    return true;
}

bool PrintService::print(const Cheque &cheque)
{
    switch (Application::getInstance()->settings->value("printerType", 0).toInt()) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        PrinterBZB2 bzb2(Application::getInstance()->settings->value("printerPath", "").toString());
        if(!bzb2.printCheque(cheque))
        {
            errorStr = bzb2.getLastError();
            return false;
        }
    }

    return true;
}

bool PrintService::printBarcode(const Cheque &cheque)
{
    QList<Cheque> oneChequeList;
    oneChequeList.append(cheque);

    return printBarcode(oneChequeList);
}

bool PrintService::printBarcode(const QList<Cheque> cheques)
{
    switch (Application::getInstance()->settings->value("printerType", 0).toInt()) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        PrinterBZB2 bzb2(Application::getInstance()->settings->value("printerPath", "").toString());
        if(!bzb2.printChequeBarcodes(cheques))
        {
            errorStr = bzb2.getLastError();
            return false;
        }
    }

    return true;
}


void PrintService::testPrint()
{
    PrinterBZB2 bzb2("/dev/usb/lp0");

    bzb2.beginCheque(Application::getInstance()->settings->value("printLogo").toBool());
    bzb2.printLine(3, "==========================");
    bzb2.printLine(3, "Чек №249/28        ПРОДАЖА");
    bzb2.printLine(3, "Дата 28/07/2017   17:36:07");
    bzb2.printLine(3, "Оператор: Куликова        ");
    bzb2.printLine(3, "Отдел: 1                  ");
    bzb2.printLine(3, "==========================");
    bzb2.printLine(3, "       ООО ИС-Маркет      ");
    bzb2.printLine(3, "ИНН 6316153445            ");
    bzb2.printLine(3, "ККМ 10006                 ");
    bzb2.printLine(3, "ПРОДАЖА                   ");
    bzb2.printLine(3, "--------------------------");
    bzb2.printLine(3, "Чек № 40198               ");
    bzb2.printLine(3, "Фискальный № 249          ");
    bzb2.printLine(3, "Касса: 1                  ");
    bzb2.printLine(3, "Кассир: Куликова Светлана ");
    bzb2.printLine(3, "Продавец: Татаренко С     ");
    bzb2.printLine(3, "                          ");
    bzb2.printLine(3, "--------------------------");
    bzb2.printLine(3, "Товар       Кол-во   Сумма");
    bzb2.printLine(3, "--------------------------");
    bzb2.printLine(3, "Кошелек Dimanche navy     ");
    bzb2.printLine(3, "621-427-B-NVW   1  2090.00");
    bzb2.printLine(3, "Со скидкой 50.0%   1045.00");
    bzb2.printLine(3, "--------------------------");
    bzb2.printLine(3, "Итого:             2090.00");
    bzb2.printLine(3, "Итого со скидкой   1045.00");
    bzb2.printLine(3, "Итоговая скидка    1045.00");
    bzb2.printLine(3, "Наличными          1050.00");
    bzb2.printLine(3, "Сдача                 5.00");
    bzb2.printLine(3, "--------------------------");
    bzb2.printLine(3, "  Цены даны с учетом НДС  ");
    bzb2.printLine(3, "    Спасибо за покупку    ");
    bzb2.printLine(3, "                          ");

    bzb2.endCheque();



}

QString PrintService::lineFormatLeft(const QString &data, int width)
{
    if(data.length() < width)
        return data;
    else
        return data.left(width);
}

QString PrintService::lineFormatCenter(const QString &data, int width)
{
    if(data.length() >= width)
    {
        return data.left(width);
    }
    else
    {
        QString spaces;
        int numSpacesLR = (width - data.length()) / 2;
        for(int i = 1; i <= numSpacesLR; i++)
            spaces.append(" ");

        QString result = spaces.append(data);
        return result;
    }
}

QString PrintService::lineFormatLR(const QString &leftStr, const QString &rightStr, int width)
{
    int numSpaces = width - leftStr.length() - rightStr.length();

    if(numSpaces > 0)
    {
        QString spacesStr;
        for(int i = 1; i <= numSpaces; i++)
            spacesStr.prepend(" ");

        QString resultStr = leftStr + spacesStr + rightStr;
        return resultStr;
    }
    else
    {
        int charsToCut = -numSpaces;
        if(leftStr.length() <= charsToCut)
        {
            QString resultStr = leftStr.left(leftStr.length() - charsToCut - 1).append(" ").append(rightStr);

            return resultStr;
        }
        else
        {
            return leftStr.left(width);
        }
    }
}

QString PrintService::getLastError() const
{
    return errorStr;
}
