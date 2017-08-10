#ifndef CHEQUELINESMODEL_H
#define CHEQUELINESMODEL_H

#include <QSqlQueryModel>

class ChequeLinesModel : public QSqlQueryModel
{
public:
    ChequeLinesModel(QObject *parent = 0);

    enum
    {
        ArtikulCol = 3,
        BarcodeCol = 4,
        NameCol = 5,
        QuantityCol = 6,
        PriceCol = 7,
        DiscPercentCol = 8,
        DiscountCol = 9,
        SummCol = 10
    };


    QVariant data(const QModelIndex &item, int role) const;
};

#endif // CHEQUELINESMODEL_H

