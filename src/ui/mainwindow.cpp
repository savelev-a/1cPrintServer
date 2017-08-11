#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIcon>
#include <QCloseEvent>
#include <QPrinterInfo>
#include <QSerialPortInfo>

#include "../application.h"
#include "chequewindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":res/img/tray-icon.png"));

    trayMenu = new QMenu(this);
    chequesPopupMenu = new QMenu(this);

    exitAction = new QAction(QIcon(":res/img/application-exit.png"), "Выход", this);
    showWindowAction = new QAction("Окно сервера", this);
    openChequeAction = new QAction(QIcon(":res/img/cheque-view.png"), "Просмотр чека", this);
    printChequeCopyAction = new QAction(QIcon(":res/img/cheque-print.png"), "Печать копии чека", this);
    printBarcodeAction = new QAction(QIcon(":res/img/barcode.png"), "Печать штрихкодов", this);

    trayMenu->addAction(showWindowAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);
    chequesPopupMenu->addAction(openChequeAction);
    chequesPopupMenu->addAction(printChequeCopyAction);
    chequesPopupMenu->addAction(printBarcodeAction);

    trayIcon = new QSystemTrayIcon(QIcon(":res/img/tray-icon.png"), this);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    ui->tableCheques->setModel(Application::getInstance()->databaseService->chequesModel);
    ui->tableCheques->setColumnHidden(0, true);
    ui->tableCheques->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableCheques->resizeColumnsToContents();
    ui->tableCheques->horizontalHeader()->setStretchLastSection(true);
    ui->tableCheques->setSelectionBehavior(QTableView::SelectRows);

    refreshServerStatus();
    loadAvaiblePrinters();
    loadAvaibleComPorts();
    reloadSettings();
    refreshTableAndTotals();

    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(showWindowAction, SIGNAL(triggered(bool)), this, SLOT(show()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(showWindowByTrayActivation(QSystemTrayIcon::ActivationReason)));

    connect(ui->webserverStartButton, SIGNAL(clicked(bool)), Application::getInstance()->webserver, SLOT(startServer()));
    connect(ui->webserverStopButton, SIGNAL(clicked(bool)), Application::getInstance()->webserver, SLOT(stopServer()));
    connect(ui->serverStatusRefreshButton, SIGNAL(clicked(bool)), this, SLOT(refreshServerStatus()));

    connect(ui->printerTypeSelector, SIGNAL(activated(int)), this, SLOT(changePrinterType(int)));
    connect(ui->printerNameSelector, SIGNAL(activated(QString)), this, SLOT(settingsChanged()));
    connect(ui->printerPathEdit, SIGNAL(textEdited(QString)), this, SLOT(settingsChanged()));
    connect(ui->comSpeedSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(settingsChanged()));
    connect(ui->comDatabitSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(settingsChanged()));
    connect(ui->comParitySelector, SIGNAL(currentIndexChanged(int)), this, SLOT(settingsChanged()));
    connect(ui->comStopbitSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(settingsChanged()));
    connect(ui->webserverPortSelector, SIGNAL(valueChanged(int)), this, SLOT(settingsChanged()));
    connect(ui->printLogoCheckbox, SIGNAL(toggled(bool)), this, SLOT(settingsChanged()));

    connect(ui->saveSettingsButton, SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
    connect(ui->reloadSettingsButton, SIGNAL(clicked(bool)), this, SLOT(reloadSettings()));

    connect(ui->testPrinterButton, SIGNAL(clicked(bool)), Application::getInstance()->printService, SLOT(testPrint()));
    connect(ui->printAllBarcodesButton, SIGNAL(clicked(bool)), this, SLOT(printAllBarcodes()));

    connect (ui->tableCheques, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(chequeTableContextMenu(QPoint)));
    connect(printChequeCopyAction, SIGNAL(triggered(bool)), this, SLOT(printChequeCopy()));
    connect(printBarcodeAction, SIGNAL(triggered(bool)), this, SLOT(printBarcode()));
    connect(openChequeAction, SIGNAL(triggered(bool)), this, SLOT(showChequeWindow()));
    connect(ui->tableCheques, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showChequeWindow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showWindowByTrayActivation(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick) this->setVisible(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    setVisible(false);
    event->ignore();
}

void MainWindow::refreshServerStatus()
{
    bool isServerStarted = Application::getInstance()->webserver->isStarted();
    ui->serverStatusField->setText(isServerStarted ? "Запущен на порту " + QString::number(Application::getInstance()->webserver->currentPort()) : "Остановлен");
    ui->serverStatusField->setStyleSheet(isServerStarted ? "QLabel {color: green}" : "QLabel {color: red}");

    if(Application::getInstance()->webserver->isStarted())
    {
        ui->webserverStartButton->setEnabled(false);
        ui->webserverStopButton->setEnabled(true);
        ui->webserverPortSelector->setEnabled(false);
    }
    else
    {
        ui->webserverStartButton->setEnabled(true);
        ui->webserverStopButton->setEnabled(false);
        ui->webserverPortSelector->setEnabled(true);
    }
}

void MainWindow::loadAvaiblePrinters()
{
    ui->printerNameSelector->addItems(QPrinterInfo::availablePrinterNames());
}

void MainWindow::loadAvaibleComPorts()
{
    QStringList portNames;

    for(QSerialPortInfo info : QSerialPortInfo::availablePorts())
    {
        portNames << info.portName();
        qDebug() << "Add com port" << info.portName();
    }

    ui->comPortSelector->addItems(portNames);
}

void MainWindow::changePrinterType(int type)
{
    switch (type)
    {
    case 0:
        ui->groupComPortSettings->setEnabled(false);
        ui->printerNameSelector->setEnabled(true);
        ui->printerPathEdit->setEnabled(false);
        break;
    case 1:
        ui->groupComPortSettings->setEnabled(true);
        ui->printerNameSelector->setEnabled(false);
        ui->printerPathEdit->setEnabled(false);
        break;
    case 2:
        ui->groupComPortSettings->setEnabled(false);
        ui->printerNameSelector->setEnabled(false);
        ui->printerPathEdit->setEnabled(true);
    }

    settingsChanged();
}

void MainWindow::settingsChanged()
{
    ui->saveSettingsButton->setEnabled(true);
    ui->reloadSettingsButton->setEnabled(true);
}

void MainWindow::saveSettings()
{
    Application::getInstance()->settings->setValue("printerType", ui->printerTypeSelector->currentIndex());
    Application::getInstance()->settings->setValue("printerPath", ui->printerPathEdit->text());
    Application::getInstance()->settings->setValue("printerName", ui->printerNameSelector->currentText());
    Application::getInstance()->settings->setValue("webserverPort", ui->webserverPortSelector->value());
    Application::getInstance()->settings->setValue("comSpeed", ui->comSpeedSelector->currentText().toInt());
    Application::getInstance()->settings->setValue("comDatabits", ui->comDatabitSelector->currentText().toInt());
    Application::getInstance()->settings->setValue("comParity", ui->comParitySelector->currentIndex());
    Application::getInstance()->settings->setValue("comStopbits", ui->comStopbitSelector->currentText());
    Application::getInstance()->settings->setValue("printLogo", ui->printLogoCheckbox->isChecked());

    ui->saveSettingsButton->setEnabled(false);
    ui->reloadSettingsButton->setEnabled(false);
}

void MainWindow::reloadSettings()
{
    ui->printerTypeSelector->setCurrentIndex(Application::getInstance()->settings->value("printerType", 0).toInt());
    ui->printerNameSelector->setCurrentText(Application::getInstance()->settings->value("printerName", "").toString());
    ui->printerPathEdit->setText(Application::getInstance()->settings->value("printerPath", "").toString());
    //ui->comPortSelector->setCurrentText(Application::getInstance()->settings->value("comPort", 0000).toInt());
    ui->comSpeedSelector->setCurrentText(Application::getInstance()->settings->value("comSpeed", 9600).toString());
    ui->comDatabitSelector->setCurrentText(Application::getInstance()->settings->value("comDatabits", 8).toString());
    ui->comParitySelector->setCurrentIndex(Application::getInstance()->settings->value("comParity", 0).toInt());
    ui->comStopbitSelector->setCurrentText(Application::getInstance()->settings->value("comStopbits", 1).toString());
    ui->webserverPortSelector->setValue(Application::getInstance()->settings->value("webserverPort", 8080).toInt());
    ui->printLogoCheckbox->setChecked(Application::getInstance()->settings->value("printLogo", false).toBool());

    if(ui->printerNameSelector->currentText() == "") ui->printerNameSelector->setCurrentIndex(0);

    changePrinterType(ui->printerTypeSelector->currentIndex());

    ui->saveSettingsButton->setEnabled(false);
    ui->reloadSettingsButton->setEnabled(false);
}

void MainWindow::chequeTableContextMenu(QPoint pos)
{
    if(ui->tableCheques->model()->rowCount() > 0)
        chequesPopupMenu->exec(ui->tableCheques->viewport()->mapToGlobal(pos));
}

void MainWindow::printChequeCopy()
{
    int row = ui->tableCheques->currentIndex().row();
    int id = ui->tableCheques->currentIndex().sibling(row, 0).data().toInt();

    Cheque cheque = Application::getInstance()->databaseService->getChequeById(id);
    Application::getInstance()->printService->print(cheque);
}

void MainWindow::showChequeWindow()
{
    int row = ui->tableCheques->currentIndex().row();
    int id = ui->tableCheques->currentIndex().sibling(row, 0).data().toInt();

    Cheque cheque = Application::getInstance()->databaseService->getChequeById(id);

    ChequeWindow *chequeWindow = new ChequeWindow(cheque);
    chequeWindow->setVisible(true);
}

void MainWindow::refreshTableAndTotals()
{
    ui->tableCheques->resizeColumnsToContents();
    ui->totalsCheqQuantField->setText(QString::number(ui->tableCheques->model()->rowCount()));
    ui->totalsDaySumField->setText(QString::number(Application::getInstance()->databaseService->getCurrentDayTotals(), 'f', 2));
    ui->totalsNalField->setText(QString::number(Application::getInstance()->databaseService->getCurrentDayNal(), 'f', 2));
    ui->totalsBeznalField->setText(QString::number(Application::getInstance()->databaseService->getCurrentDayBeznal(), 'f', 2));
    ui->totalsCertField->setText(QString::number(Application::getInstance()->databaseService->getCurrentDayCert(), 'f', 2));

}

void MainWindow::printBarcode()
{
    int row = ui->tableCheques->currentIndex().row();
    int id = ui->tableCheques->currentIndex().sibling(row, 0).data().toInt();

    Cheque cheque = Application::getInstance()->databaseService->getChequeById(id);
    Application::getInstance()->printService->printBarcode(cheque);
}

void MainWindow::printAllBarcodes()
{
    Application::getInstance()->printService->printBarcode(
                Application::getInstance()->databaseService->getAllChequesCurrentDay());
}
