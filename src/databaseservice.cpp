#include "databaseservice.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

#include <QDebug>

QSqlDatabase DatabaseService::db = QSqlDatabase::addDatabase("QSQLITE");

DatabaseService::DatabaseService(QObject *parent) : QObject(parent)
{
    db.setDatabaseName("1cprint.db");

    if(!db.open())
    {
        QMessageBox::critical(0,
                              "Ошибка инициализации БД",
                              "База данных вернула ошибку - " + getLastError());
        exit(1);
    }

    chequesModel = new ChequeListModel(this);
    chequeLinesModel = new ChequeLinesModel(this);

    refreshModel();
    chequesModel->setHeaderData(ChequeListModel::IdxCol, Qt::Horizontal, "idx");
    chequesModel->setHeaderData(ChequeListModel::ChequeNumCol, Qt::Horizontal, "Номер чека");
    chequesModel->setHeaderData(ChequeListModel::DatetimeCol, Qt::Horizontal, "Время");
    chequesModel->setHeaderData(ChequeListModel::SellerCol, Qt::Horizontal, "Продавец");
    chequesModel->setHeaderData(ChequeListModel::SummCol, Qt::Horizontal, "Сумма");


}

QString DatabaseService::getLastError() const
{
    return db.lastError().text();
}

bool DatabaseService::saveCheque(const Cheque &cheque)
{
    db.transaction();

    QSqlQuery queryCheque("INSERT INTO cheque("
                    "ch_id, "
                    "datetime, "
                    "inn, "
                    "orgName, "
                    "orgAddress, "
                    "seller, "
                    "paymentNal, "
                    "paymentBeznal, "
                    "paymentCert, "
                    "sdacha, "
                    "totals) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    queryCheque.bindValue(0, cheque.number);
    queryCheque.bindValue(1, cheque.datetime);
    queryCheque.bindValue(2, cheque.inn);
    queryCheque.bindValue(3, cheque.orgName);
    queryCheque.bindValue(4, cheque.orgAddress);
    queryCheque.bindValue(5, cheque.seller);
    queryCheque.bindValue(6, cheque.paymentNal);
    queryCheque.bindValue(7, cheque.paymentBeznal);
    queryCheque.bindValue(8, cheque.paymentCert);
    queryCheque.bindValue(9, cheque.sdacha);
    queryCheque.bindValue(10, cheque.getTotalsWithDiscount());

    QSqlQuery querySeq("select seq from sqlite_sequence WHERE name = 'cheque'");
    querySeq.first();
    int nextId = querySeq.value("seq").toInt() + 1;

    for(ChequeLine line : cheque.lines)
    {
        QSqlQuery queryLine("INSERT INTO cheque_line("
                            "cheque_id, "
                            "lineNumber, "
                            "artikul, "
                            "barcode, "
                            "name, "
                            "quantity, "
                            "price, "
                            "discountPercent, "
                            "discount, "
                            "summ) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

        queryLine.bindValue(0, nextId);
        queryLine.bindValue(1, line.lineNumber);
        queryLine.bindValue(2, line.artikul);
        queryLine.bindValue(3, line.barcode);
        queryLine.bindValue(4, line.name);
        queryLine.bindValue(5, line.quantity);
        queryLine.bindValue(6, line.price);
        queryLine.bindValue(7, line.discountPercent);
        queryLine.bindValue(8, line.discount);
        queryLine.bindValue(9, line.summ);

        queryLine.exec();
    }

    bool cheque_ok = queryCheque.exec();

    return db.commit() && cheque_ok;
}

Cheque DatabaseService::getChequeById(int id)
{
    Cheque cheque;

    QSqlQuery queryCheque("SELECT * FROM cheque WHERE id = " + QString::number(id));
    queryCheque.first();

    cheque.id = id;
    cheque.number = queryCheque.value("ch_id").toString();
    cheque.datetime = queryCheque.value("datetime").toDateTime();
    cheque.inn = queryCheque.value("inn").toString();
    cheque.orgName = queryCheque.value("orgName").toString();
    cheque.orgAddress = queryCheque.value("orgAddress").toString();
    cheque.seller = queryCheque.value("seller").toString();
    cheque.paymentNal = queryCheque.value("paymentNal").toDouble();
    cheque.paymentBeznal = queryCheque.value("paymentBeznal").toDouble();
    cheque.sdacha = queryCheque.value("sdacha").toDouble();

    QSqlQuery queryLines("SELECT * FROM cheque_line WHERE cheque_id = " + QString::number(id));
    while(queryLines.next())
    {
        ChequeLine line;
        line.lineNumber = queryLines.value("lineNumber").toInt();
        line.artikul = queryLines.value("artikul").toString();
        line.barcode = queryLines.value("barcode").toString();
        line.name = queryLines.value("name").toString();
        line.quantity = queryLines.value("quantity").toInt();
        line.price = queryLines.value("price").toDouble();
        line.discountPercent = queryLines.value("discountPercent").toDouble();
        line.discount = queryLines.value("discount").toDouble();
        line.summ = queryLines.value("summ").toDouble();
        cheque.lines.append(line);
    }

    return cheque;
}

QList<Cheque> DatabaseService::getAllChequesCurrentDay()
{
    QList<Cheque> resultList;
    QDate currentDate = QDate::currentDate();

    QSqlQuery queryCheque("SELECT * "
                          "FROM cheque "
                          "WHERE datetime BETWEEN \'"
                          + currentDate.toString("yyyy-MM-ddT00:00:00")
                          + "\' AND \'" + currentDate.toString("yyyy-MM-ddT23:59:59")
                          + "\'");

    while(queryCheque.next())
    {
        Cheque cheque;

        cheque.id = queryCheque.value("id").toInt();
        cheque.number = queryCheque.value("ch_id").toString();
        cheque.datetime = queryCheque.value("datetime").toDateTime();
        cheque.inn = queryCheque.value("inn").toString();
        cheque.orgName = queryCheque.value("orgName").toString();
        cheque.orgAddress = queryCheque.value("orgAddress").toString();
        cheque.seller = queryCheque.value("seller").toString();
        cheque.paymentNal = queryCheque.value("paymentNal").toDouble();
        cheque.paymentBeznal = queryCheque.value("paymentBeznal").toDouble();
        cheque.sdacha = queryCheque.value("sdacha").toDouble();

        QSqlQuery queryLines("SELECT * FROM cheque_line WHERE cheque_id = " + QString::number(cheque.id));
        while(queryLines.next())
        {
            ChequeLine line;
            line.lineNumber = queryLines.value("lineNumber").toInt();
            line.artikul = queryLines.value("artikul").toString();
            line.barcode = queryLines.value("barcode").toString();
            line.name = queryLines.value("name").toString();
            line.quantity = queryLines.value("quantity").toInt();
            line.price = queryLines.value("price").toDouble();
            line.discountPercent = queryLines.value("discountPercent").toDouble();
            line.discount = queryLines.value("discount").toDouble();
            line.summ = queryLines.value("summ").toDouble();
            cheque.lines.append(line);
        }

        resultList.append(cheque);

    }

    return resultList;
}

void DatabaseService::refreshModel()
{
    QDate currentDate = QDate::currentDate();

    chequesModel->setQuery("SELECT id, ch_id, datetime, seller, totals "
                           "FROM cheque "
                           "WHERE datetime BETWEEN \'"
                           + currentDate.toString("yyyy-MM-ddT00:00:00")
                           + "\' AND \'" + currentDate.toString("yyyy-MM-ddT23:59:59")
                           + "\'", db);
}

ChequeLinesModel* DatabaseService::getChequeLinesModel(const Cheque &cheque)
{
    chequeLinesModel->setQuery("SELECT * FROM cheque_line WHERE cheque_id = " + QString::number(cheque.id));

    chequeLinesModel->setHeaderData(3, Qt::Horizontal, "Артикул");
    chequeLinesModel->setHeaderData(4, Qt::Horizontal, "Штрихкод");
    chequeLinesModel->setHeaderData(5, Qt::Horizontal, "Наименование");
    chequeLinesModel->setHeaderData(6, Qt::Horizontal, "Количество");
    chequeLinesModel->setHeaderData(7, Qt::Horizontal, "Цена");
    chequeLinesModel->setHeaderData(8, Qt::Horizontal, "Скидка, %");
    chequeLinesModel->setHeaderData(9, Qt::Horizontal, "Скидка");
    chequeLinesModel->setHeaderData(10, Qt::Horizontal, "Сумма");

    return chequeLinesModel;
}

double DatabaseService::getCurrentDayTotals()
{
    QDate currentDate = QDate::currentDate();
    double result = 0.0;

    QSqlQuery query("SELECT SUM(totals) AS day_totals "
                    "FROM cheque "
                    "WHERE datetime BETWEEN \'"
                    + currentDate.toString("yyyy-MM-ddT00:00:00")
                    + "\' AND \'" + currentDate.toString("yyyy-MM-ddT23:59:59")
                    + "\'");

    query.first();

    result = query.value("day_totals").toDouble();

    return result;
}

double DatabaseService::getCurrentDayNal()
{
    QDate currentDate = QDate::currentDate();
    double result = 0.0;

    QSqlQuery query("SELECT SUM(paymentNal) AS day_nal_income, "
                    "SUM(sdacha) AS day_sdacha "
                    "FROM cheque "
                    "WHERE datetime BETWEEN \'"
                    + currentDate.toString("yyyy-MM-ddT00:00:00")
                    + "\' AND \'" + currentDate.toString("yyyy-MM-ddT23:59:59")
                    + "\'");

    query.first();

    result = query.value("day_nal_income").toDouble() - query.value("day_sdacha").toDouble() ;

    return result;
}

double DatabaseService::getCurrentDayBeznal()
{
    QDate currentDate = QDate::currentDate();
    double result = 0.0;

    QSqlQuery query("SELECT SUM(paymentBeznal) AS beznal_totals "
                    "FROM cheque "
                    "WHERE datetime BETWEEN \'"
                    + currentDate.toString("yyyy-MM-ddT00:00:00")
                    + "\' AND \'" + currentDate.toString("yyyy-MM-ddT23:59:59")
                    + "\'");

    query.first();

    result = query.value("beznal_totals").toDouble();

    return result;
}

double DatabaseService::getCurrentDayCert()
{
    QDate currentDate = QDate::currentDate();
    double result = 0.0;

    QSqlQuery query("SELECT SUM(paymentCert) AS cert_totals "
                    "FROM cheque "
                    "WHERE datetime BETWEEN \'"
                    + currentDate.toString("yyyy-MM-ddT00:00:00")
                    + "\' AND \'" + currentDate.toString("yyyy-MM-ddT23:59:59")
                    + "\'");

    query.first();

    result = query.value("cert_totals").toDouble();

    return result;
}
