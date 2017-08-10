#include "chequelinesmodel.h"

ChequeLinesModel::ChequeLinesModel(QObject *parent) : QSqlQueryModel(parent)
{

}



QVariant ChequeLinesModel::data(const QModelIndex &item, int role) const
{

    if(role == Qt::DisplayRole && (
                item.column() == PriceCol
             || item.column() == DiscountCol
             || item.column() == SummCol))
    {
        QVariant data = item.data(Qt::EditRole);
        QString summStr = QString::number(data.toDouble(), 'f', 2);
        return QVariant(summStr);
    }
    else if(role == Qt::DisplayRole && item.column() == DiscPercentCol)
    {
        QVariant data = item.data(Qt::EditRole);
        QString summStr = QString::number(data.toDouble(), 'f', 1);
        return QVariant(summStr);
    }
    else if(role == Qt::TextAlignmentRole && (
                item.column() == QuantityCol
             || item.column() == PriceCol
             || item.column() == DiscPercentCol
             || item.column() == DiscountCol
             || item.column() == SummCol))
    {
        return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    }

    return QSqlQueryModel::data(item, role);
}


