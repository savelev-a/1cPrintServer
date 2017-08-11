#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>

#include "model/cheque.h"
#include "model/chequelistmodel.h"
#include "model/chequelinesmodel.h"

class DatabaseService : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseService(QObject *parent = 0);

    QString getLastError() const;
    bool saveCheque(const Cheque &cheque);
    Cheque getChequeById(int id);
    QList<Cheque> getAllChequesCurrentDay();
    void refreshModel();

    ChequeListModel *chequesModel;
    ChequeLinesModel* getChequeLinesModel(const Cheque &cheque);

    double getCurrentDayTotals();
    double getCurrentDayNal();
    double getCurrentDayBeznal();
    double getCurrentDayCert();

private:
    static QSqlDatabase db;

    ChequeLinesModel *chequeLinesModel;

};

#endif // DATABASESERVICE_H
