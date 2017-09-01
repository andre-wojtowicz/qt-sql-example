#include "db_controller.h"
#include "mainwindow.h"

#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DbController db_controller(0);
    QThread      db_thread;

    db_controller.moveToThread(&db_thread);
    db_thread.start();

    MainWindow window(0, &db_controller, &db_thread);
    window.show();
    
    return app.exec();
}
