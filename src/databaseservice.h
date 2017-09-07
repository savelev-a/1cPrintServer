#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include  <QDate>

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
    QList<Cheque> getAllChequesByPeriod(QDate startDate, QDate endDate);
    QList<Cheque> getAllChequesCurrentDay();
    void refreshModel(QDate startDate = QDate::currentDate(), QDate endDate = QDate::currentDate());

    ChequeListModel *chequesModel;
    ChequeLinesModel* getChequeLinesModel(const Cheque &cheque);

    double getPeriodTotals(QDate startDate, QDate endDate);
    double getPeriodNal(QDate startDate, QDate endDate);
    double getPeriodBeznal(QDate startDate, QDate endDate);
    double getPeriodCert(QDate startDate, QDate endDate);

    double getCurrentDayTotals();
    double getCurrentDayNal();
    double getCurrentDayBeznal();
    double getCurrentDayCert();

private:
    QSqlDatabase db;

    ChequeLinesModel *chequeLinesModel;

};

#endif // DATABASESERVICE_H
