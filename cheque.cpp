#include "cheque.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

Cheque::Cheque(const QJsonObject &chequeObject)
{
    this->id = 0;
    this->number = chequeObject["НомерЧека"].toString();
    this->datetime = QDateTime::fromString(chequeObject["ДатаЧека"].toString(), "yyyy-MM-ddThh:mm:ss");
    this->inn = chequeObject["ОрганизацииИНН"].toString();
    this->orgName = chequeObject["ОрганизацииНаименование"].toString();
    this->orgAddress = chequeObject["ОрганизацииАдрес"].toString();
    this->seller = chequeObject["Кассир"].toString();

    QJsonArray linesArray = chequeObject["Товары"].toArray();
    for(QJsonValue lineValue : linesArray)
    {
        QJsonObject lineObject = lineValue.toObject();
        ChequeLine line;
        line.lineNumber = lineObject["НомерСтроки"].toInt();
        line.artikul = lineObject["Артикул"].toString();
        line.barcode = lineObject["ШК"].toString();
        line.name = lineObject["Наименование"].toString();
        line.quantity = lineObject["Количество"].toInt();
        line.price = lineObject["Цена"].toDouble();
        line.discountPercent = lineObject["СкидкаПроц"].toDouble();
        line.discount = lineObject["СкидкаСумма"].toDouble();
        line.summ = lineObject["СуммаИтого"].toDouble();

        this->lines.append(line);
    }

    QJsonObject totalsObject = chequeObject["Оплаты"].toObject();
    this->paymentNal = totalsObject["Наличные"].toDouble();
    this->paymentBeznal = totalsObject["БезНал"].toDouble();
    this->paymentCert = totalsObject["Сертификатом"].toDouble();
    this->sdacha = totalsObject["Сдача"].toDouble();
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
