#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include <QSettings>

MainWindow::MainWindow(QWidget* parent, DbController* dbc, QThread* dbt) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db_controller = dbc;
    db_thread = dbt;

    // check Qt SQL drivers

    QStringList sql_drivers = QSqlDatabase::drivers();
    ui->label_qmysql_icon->setPixmap(
                QPixmap(":/icons/" + QString(sql_drivers.contains("QMYSQL") ? "success" : "failure") + ".ico"));
    ui->label_qodbc_icon->setPixmap(
                QPixmap(":/icons/" + QString(sql_drivers.contains("QODBC") ? "success" : "failure") + ".ico"));

    ui->radio_mysql->setEnabled(sql_drivers.contains("QMYSQL"));
    ui->radio_mssql->setEnabled(sql_drivers.contains("QODBC"));

    if (!sql_drivers.contains("QMYSQL") && !sql_drivers.contains("QODBC"))
        ui->groupBox_sql_connect->setEnabled(false);

    // connect signals with slots

    // ui => ui

    connect(ui->button_connect, SIGNAL(clicked()), this, SLOT(connectToServerRequested()));
    connect(ui->radio_mssql, SIGNAL(clicked()), this, SLOT(engineChanged()));
    connect(ui->radio_mysql, SIGNAL(clicked()), this, SLOT(engineChanged()));
    connect(ui->radio_sql_authentication, SIGNAL(clicked()), this, SLOT(authenticationMethodChanged()));
    connect(ui->radio_windows_authentication, SIGNAL(clicked()), this, SLOT(authenticationMethodChanged()));
    connect(ui->button_show_table, SIGNAL(clicked()), this, SLOT(showTableRequested()));

    // ui => db_controller

    connect(this, SIGNAL(connectToServer(QString,QString,QString,int,QString,QString,QString,bool)),
            db_controller, SLOT(connectToServerRequested(QString,QString,QString,int,QString,QString,QString,bool)));
    connect(this, SIGNAL(disconnectFromServer()), db_controller, SLOT(disconnectFromServerRequested()));
    connect(this, SIGNAL(selectTable(QString)), db_controller, SLOT(selectTableRequested(QString)));
    connect(this, SIGNAL(getTablesNames()), db_controller, SLOT(getTablesNamesRequested()));

     // db_controller => ui

    connect(db_controller, SIGNAL(serverConnected()), this, SLOT(serverConnected()));
    connect(db_controller, SIGNAL(serverErrorWithConnection(QString)),
            this, SLOT(serverErrorWithConnection(QString)));
    connect(db_controller, SIGNAL(serverDisconnected()), this, SLOT(serverDisconnected()));
    connect(db_controller, SIGNAL(tableSelected(QSqlQueryModel*)), this, SLOT(displayTable(QSqlQueryModel*)));
    connect(db_controller, SIGNAL(gotTablesNames(QStringList)), this, SLOT(fillTablesNames(QStringList)));

    // load settings

    QString inifile("config.ini");
    QFileInfo check_file(inifile);
    if (check_file.exists() && check_file.isFile())
    {
        QSettings settings(inifile, QSettings::IniFormat);

        QString engine = settings.value("sql/engine", "").toString();
        if (engine == "mysql")
        {
            ui->radio_mysql->setChecked(true);
            ui->radio_windows_authentication->setEnabled(false);
            ui->lineEdit_driver->setEnabled(false);
        }
        else if (engine == "mssql")
        {
            ui->radio_mssql->setChecked(true);
        }

        ui->lineEdit_driver->setText(settings.value("sql/driver", "").toString());
        ui->lineEdit_server_address->setText(settings.value("sql/address", "").toString());
        ui->spinBox_server_port->setValue(settings.value("sql/port", 0).toInt());
        QString auth = settings.value("sql/authentication", "").toString();
        ui->radio_sql_authentication->setChecked(auth == "server" && (engine == "mssql" || engine == "mysql"));
        ui->radio_windows_authentication->setChecked(auth == "windows" && engine == "mssql");
        ui->lineEdit_login->setText(settings.value("sql/login", "").toString());
        ui->lineEdit_password->setText(settings.value("sql/password", "").toString()); // plain text, so secure...
        ui->lineEdit_database_name->setText(settings.value("sql/database", "").toString());

        ui->statusBar->showMessage("Settings file config.ini loaded", 3000);
    }
    else
    {
        ui->statusBar->showMessage("Settings file config.ini does not exist", 5000);
    }


}

MainWindow::~MainWindow()
{
    db_thread->exit();
    db_thread->wait();
    delete ui;
}

void MainWindow::connectToServerRequested()
{
    QString engine;
    if (ui->radio_mysql->isChecked())
        engine = "mysql";
    else if (ui->radio_mssql->isChecked())
        engine = "mssql";
    else
    {
        QMessageBox::information(this,
                                 "Invalid Engine",
                                 "Choose database engine",
                                 QMessageBox::Ok);
        return;
    }

    QString driver   = ui->lineEdit_driver->text(),
            server   = ui->lineEdit_server_address->text(),
            database = ui->lineEdit_database_name->text(),
            login    = ui->lineEdit_login->text(),
            password = ui->lineEdit_password->text();
    int port = ui->spinBox_server_port->value();

    if (server == "")
    {
        QMessageBox::information(this,
                                 "Invalid Connection Data",
                                 "Insert server address to connect",
                                 QMessageBox::Ok);
        return;
    }

    bool is_sql_authentication = ui->radio_sql_authentication->isChecked();

    if (is_sql_authentication && login == "")
    {
        QMessageBox::information(this,
                                 "Invalid Connection Data",
                                 "Insert login to connect",
                                 QMessageBox::Ok);
        return;
    }

    if (database == "")
    {
        QMessageBox::information(this,
                                 "Invalid Connection Data",
                                 "Insert database name to connect",
                                 QMessageBox::Ok);
        return;
    }

    ui->button_connect->setEnabled(false);
    ui->statusBar->showMessage("Connecting...");

    emit connectToServer(engine, driver, server, port, database, login, password, is_sql_authentication);
}

void MainWindow::disconnectFromServerRequested()
{
    ui->button_connect->setEnabled(false);

    delete ui->tableView_database_table->model();

    emit disconnectFromServer();
}

void MainWindow::authenticationMethodChanged()
{
    bool is_sql_authentication = ui->radio_sql_authentication->isChecked();

    ui->lineEdit_login->setEnabled(is_sql_authentication);
    ui->lineEdit_password->setEnabled(is_sql_authentication);
}

void MainWindow::engineChanged()
{
    bool is_mssql_engine = ui->radio_mssql->isChecked();

    ui->lineEdit_driver->setEnabled(is_mssql_engine);
    ui->radio_windows_authentication->setEnabled(is_mssql_engine);

    ui->spinBox_server_port->setValue(is_mssql_engine ? 1433 : 3306);

    if (!is_mssql_engine)
    {
        ui->radio_sql_authentication->setChecked(true);
        emit authenticationMethodChanged();
    }
}

void MainWindow::showTableRequested()
{
    ui->button_show_table->setEnabled(false);

    delete ui->tableView_database_table->model(); // remove old model

    QString table_name = ui->comboBox_table_name->currentText();

    emit selectTable(table_name);
}

void MainWindow::serverConnected()
{
    ui->button_connect->setEnabled(true);

    disconnect(ui->button_connect, SIGNAL(clicked()), this, SLOT(connectToServerRequested()));
    connect(ui->button_connect, SIGNAL(clicked()), this, SLOT(disconnectFromServerRequested()));

    ui->button_connect->setText("Disconnect");
    ui->groupBox_database_browser->setEnabled(true);

    ui->statusBar->showMessage("Connected", 3000);

    emit getTablesNames();
}

void MainWindow::fillTablesNames(QStringList tables_names)
{
    if (tables_names.length() == 0)
        QMessageBox::warning(this,
                             "Tables",
                             "There are no tables to display in the database",
                             QMessageBox::Ok);
    else
    {
        ui->comboBox_table_name->addItems(tables_names);

        ui->comboBox_table_name->setEnabled(true);
        ui->comboBox_table_name->setFocus();
    }
}

void MainWindow::serverErrorWithConnection(QString message)
{
    QMessageBox::critical(this,
                          "Connection failed",
                          message,
                          QMessageBox::Ok);

    ui->button_connect->setEnabled(true);

    ui->statusBar->showMessage("Connection failed", 3000);
}

void MainWindow::serverDisconnected()
{
    disconnect(ui->button_connect, SIGNAL(clicked()), this, SLOT(disconnectFromServerRequested()));
    connect(ui->button_connect, SIGNAL(clicked()), this, SLOT(connectToServerRequested()));

    ui->tableView_database_table->setModel(NULL);

    ui->button_connect->setEnabled(true);
    ui->button_connect->setText("Connect");

    ui->comboBox_table_name->clear();
    ui->comboBox_table_name->setEnabled(false);

    ui->groupBox_database_browser->setEnabled(false);
    ui->button_connect->setFocus();
}

void MainWindow::displayTable(QSqlQueryModel* model)
{
    if (!model->lastError().isValid())
        ui->tableView_database_table->setModel(model);
    else
        QMessageBox::critical(this,
                              "Select failed",
                              model->lastError().databaseText(),
                              QMessageBox::Ok);

    ui->button_show_table->setEnabled(true);
    ui->comboBox_table_name->setFocus();
}

void MainWindow::keyPressEvent(QKeyEvent* pe)
{
    if (pe->key() == Qt::Key_Enter || pe->key() == Qt::Key_Return)
    {
        if (!db_controller->checkIfConnected())
            emit connectToServerRequested();
        else if (ui->comboBox_table_name->isEnabled() && ui->comboBox_table_name->hasFocus())
            emit showTableRequested();
    }
}
