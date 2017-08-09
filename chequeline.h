#ifndef CHEQUELINE_H
#define CHEQUELINE_H

#include <QString>

class ChequeLine
{

public:
    ChequeLine();

    int lineNumber;
    QString artikul;
    QString barcode;
    QString name;
    int quantity;
    double price;
    double discountPercent;
    double discount;
    double summ;

};

#endif // CHEQUELINE_H
