#include "cheque.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

Cheque::Cheque(const QJsonObject &chequeObject)
{
    this->id = 0;
    this->id_1c = chequeObject["IDчека"].toString();
    this->datetime = QDateTime::fromString(chequeObject["ДатаЧека"].toString(), "dd.MM.yyyy hh.mm.ss");
    this->inn = chequeObject["ОрганизацииИНН"].toString();
    this->orgName = chequeObject["ОрганизацииНаименование"].toString();
    this->orgAddress = chequeObject["ОрганизацииАдрес"].toString();
    this->seller = chequeObject["Кассир"].toString();

    QJsonArray linesArray = chequeObject["Товары"].toArray();
    for(QJsonValue lineValue : linesArray)
    {
        QJsonObject lineObject = lineValue.toObject();
        ChequeLine line;
        line.lineNumber = lineObject["НомерСтроки"].toString().toInt();
        line.artikul = lineObject["Артикул"].toString();
        line.barcode = lineObject["ШК"].toString();
        line.name = lineObject["Наименование"].toString();
        line.quantity = lineObject["Количество"].toString().toInt();
        line.price = lineObject["Цена"].toString().toDouble();
        line.discountPercent = lineObject["СкидкаПроц"].toString().toDouble();
        line.discount = lineObject["СкидкаСумма"].toString().toDouble();
        line.summ = lineObject["СуммаИтого"].toString().toDouble();

        this->lines.append(line);
    }

    QJsonObject totalsObject = chequeObject["Оплаты"].toObject();
    this->paymentNal = totalsObject["Наличные"].toString().toDouble();
    this->paymentBeznal = totalsObject["БезНал"].toString().toDouble();
    this->paymentCert = totalsObject["Сертификатом"].toString().toDouble();
    this->sdacha = totalsObject["Сдача"].toString().toDouble();
}

Cheque::Cheque()
{

}

Cheque::~Cheque()
{

}

double Cheque::getTotalsWithoutDiscount() const
{
    double totals = 0.0;

    for(ChequeLine line : lines)
    {
        totals += line.price * line.quantity;
    }

    return totals;
}

double Cheque::getTotalsWithDiscount() const
{
    double totals = 0.0;

    for(ChequeLine line : lines)
    {
        totals += line.summ;
    }

    return totals;
}

double Cheque::getDiscountTotal() const
{
    double totals = 0.0;

    for(ChequeLine line : lines)
    {
        totals += line.discount;
    }

    return totals;
}
