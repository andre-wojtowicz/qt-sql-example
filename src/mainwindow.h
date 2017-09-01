#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "db_controller.h"

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent, DbController* dbc, QThread* dbt);
    ~MainWindow();

public slots:
    void connectToServerRequested();
    void disconnectFromServerRequested();
    void engineChanged();
    void authenticationMethodChanged();
    void showTableRequested();

    void serverConnected();
    void serverErrorWithConnection(QString);
    void serverDisconnected();
    void displayTable(QSqlQueryModel*);
    void fillTablesNames(QStringList);

signals:
    void connectToServer(QString, QString, QString, int, QString, QString, QString, bool);
    void disconnectFromServer();
    void selectTable(QString);
    void getTablesNames();
    
private:
    Ui::MainWindow* ui;
    DbController*   db_controller;
    QThread*        db_thread;

protected:
    virtual void keyPressEvent(QKeyEvent*);
};

#endif // MAINWINDOW_H
