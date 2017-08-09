#ifndef CHEQUE_H
#define CHEQUE_H

#include <QList>
#include <QDateTime>

#include "chequeline.h"

class Cheque
{

public:
    Cheque();
    Cheque(const QJsonObject &chequeObject);
    ~Cheque();

    double getTotalsWithoutDiscount() const;
    double getTotalsWithDiscount() const;
    double getDiscountTotal() const;

    int id;
    QString id_1c;
    QDateTime datetime;
    QString inn;
    QString orgName;
    QString orgAddress;
    QString seller;
    QList<ChequeLine> lines;

    double paymentNal;
    double paymentBeznal;
    double paymentCert;
    double sdacha;
};

#endif // CHEQUE_H
