#ifndef CHEQUELISTMODEL_H
#define CHEQUELISTMODEL_H

#include <QSqlQueryModel>

class ChequeListModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    ChequeListModel(QObject *parent = 0);

    enum
    {
        IdxCol = 0,
        ChequeNumCol = 1,
        DatetimeCol = 2,
        SellerCol = 3,
        SummCol = 4
    };


    QVariant data(const QModelIndex &item, int role) const;

};

#endif // CHEQUELISTMODEL_H
