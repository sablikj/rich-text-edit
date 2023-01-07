#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "subwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(mdiArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addSubWindow()
{
    Subwindow *win = new Subwindow;
    mdiArea->addSubWindow(win);
    win->show();
}


void MainWindow::on_button_new_triggered()
{
    QWidget *widget = new QWidget(mdiArea);
    QGridLayout *gridLayout = new QGridLayout(widget);
    widget->setLayout(gridLayout);


    mdiArea->addSubWindow(widget);
    widget->setWindowTitle("Untitled.rts");
    widget->show();
}

