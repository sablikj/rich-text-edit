#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName("UTB");
    QApplication::setOrganizationDomain("utb.cz");
    QApplication::setApplicationName("Notepad");
    QApplication::setWindowIcon(QIcon("://images/notepad.png"));

    MainWindow w;
    w.show();
    return a.exec();
}
