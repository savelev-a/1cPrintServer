#ifndef CHEQUEWINDOW_H
#define CHEQUEWINDOW_H

#include <QWidget>

#include "cheque.h"

namespace Ui {
class ChequeWindow;
}

class ChequeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChequeWindow(const Cheque &cheque, QWidget *parent = 0);
    ~ChequeWindow();

private slots:
    void printChequeCopy();
    void printBarcode();

private:
    Ui::ChequeWindow *ui;
    Cheque cheque;
};

#endif // CHEQUEWINDOW_H
