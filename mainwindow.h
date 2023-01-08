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

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    Subwindow *createSubwindow();

    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionSave_as_triggered();
    void on_actionOpen_triggered();

    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionCut_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

private:
    /*
    * this event is called when the mouse enters the widgets area during a drag/drop operation
    */
    void dragEnterEvent(QDragEnterEvent* event) override;

    /**
    * this event is called when the mouse moves inside the widgets area during a drag/drop operation
    */
    void dragMoveEvent(QDragMoveEvent* event) override;

    /**
    * this event is called when the drop operation is initiated at the widget
    */
    void dropEvent(QDropEvent* event) override;

    Subwindow *activeSubwindow() const;
    QMdiSubWindow *findSubwindow(const QString &fileName) const;
    bool loadFile(const QString &fileName);

    Ui::MainWindow *ui;
    QMdiArea * mdiArea;
};
#endif // MAINWINDOW_H
