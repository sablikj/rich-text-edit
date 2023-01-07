#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include "subwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool openFile(const QString &fileName);

private slots:
    Subwindow *createSubwindow();

    void on_button_new_triggered();
    void on_button_save_triggered();
    void on_button_open_triggered();

    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionSave_as_triggered();

private:
    Subwindow *activeSubwindow() const;
    QMdiSubWindow *findSubwindow(const QString &fileName) const;
    bool loadFile(const QString &fileName);

    Ui::MainWindow *ui;
    QMdiArea * mdiArea;
};
#endif // MAINWINDOW_H
