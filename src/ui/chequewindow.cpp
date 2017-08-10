#include "chequewindow.h"
#include "ui_chequewindow.h"

#include "../application.h"

ChequeWindow::ChequeWindow(const Cheque &cheque, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChequeWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    ui->chequeIdField->setText(QString::number(cheque.id));
    ui->chequeId1CField->setText(cheque.number);
    ui->chequeInnField->setText(cheque.inn);
    ui->chequeOrganisationField->setText(cheque.orgName);
    ui->chequeAddressField->setText(cheque.orgAddress);
    ui->chequeSellerField->setText(cheque.seller);
    ui->chequeDateTimeField->setText(cheque.datetime.toString("dd.MM.yyyy hh:mm:ss"));
    ui->totalsWithoutDiscField->setText(QString::number(cheque.getTotalsWithoutDiscount(), 'f', 2));
    ui->totalDiscountField->setText(QString::number(cheque.getDiscountTotal(), 'f', 2));
    ui->totalsField->setText(QString::number(cheque.getTotalsWithDiscount(), 'f', 2));
    ui->positionsCountField->setText(QString::number(cheque.lines.count()));

    ui->tableLines->setModel(Application::getInstance()->databaseService->getChequeLinesModel(cheque));
    ui->tableLines->setColumnHidden(0, true);
    ui->tableLines->setColumnHidden(1, true);
    ui->tableLines->setColumnHidden(2, true);
    ui->tableLines->horizontalHeader()->setStretchLastSection(true);
    ui->tableLines->resizeColumnsToContents();
    ui->tableLines->setSelectionBehavior(QTableView::SelectRows);

    this->cheque = cheque;

    connect(ui->printCopyButton, SIGNAL(clicked(bool)), this, SLOT(printChequeCopy()));
    connect(ui->printBarcodeButton, SIGNAL(clicked(bool)), this, SLOT(printBarcode()));
    connect(ui->closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
}

ChequeWindow::~ChequeWindow()
{
    delete ui;
}

void ChequeWindow::printChequeCopy()
{
    Application::getInstance()->printService->print(this->cheque);
}

void ChequeWindow::printBarcode()
{
    Application::getInstance()->printService->printBarcode(this->cheque);
}
