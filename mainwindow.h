#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void refreshServerStatus();
    void refreshTableAndTotals();

private slots:
    void showWindowByTrayActivation(QSystemTrayIcon::ActivationReason reason);
    void changePrinterType(int type);
    void settingsChanged();

    void saveSettings();
    void reloadSettings();

    void chequeTableContextMenu(QPoint pos);
    void printChequeCopy();
    void printBarcode();
    void showChequeWindow();


private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QMenu *chequesPopupMenu;
    QAction *exitAction;
    QAction *showWindowAction;
    QAction *openChequeAction;
    QAction *printChequeCopyAction;
    QAction *printBarcodeAction;

    void loadAvaiblePrinters();
    void loadAvaibleComPorts();
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
