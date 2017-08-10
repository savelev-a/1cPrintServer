#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QSettings>

#include "databaseservice.h"
#include "network/webserver.h"
#include "print/printservice.h"
#include "ui/mainwindow.h"

class Application : public QObject
{
    Q_OBJECT
public:
    static Application* getInstance();

    void init();

    DatabaseService *databaseService;
    Webserver *webserver;
    PrintService *printService;
    MainWindow *mainWindow;
    QSettings *settings;

private:
    static Application *appInstance;
};

#endif // APPLICATION_H
