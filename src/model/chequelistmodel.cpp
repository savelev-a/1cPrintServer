#include "chequelistmodel.h"

#include <QDateTime>
#include <QVariant>

ChequeListModel::ChequeListModel(QObject *parent) : QSqlQueryModel(parent)
{

}


QVariant ChequeListModel::data(const QModelIndex &item, int role) const
{
    if(role == Qt::DisplayRole && item.column() == DatetimeCol)
    {
        QVariant data = item.data(Qt::EditRole);
        //QString datetimeStr = data.toDateTime().toString("dd.MM.yyyy - hh:mm:ss");
        QString datetimeStr = data.toDateTime().toString("hh:mm:ss");
        return QVariant(datetimeStr);
    }
    else if(role == Qt::DisplayRole && item.column() == SummCol)
    {
        QVariant data = item.data(Qt::EditRole);
        QString summStr = QString::number(data.toDouble(), 'f', 2);
        return QVariant(summStr);
    }
    else if(role == Qt::TextAlignmentRole && item.column() == SummCol)
    {
        return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    }

    return QSqlQueryModel::data(item, role);
}
