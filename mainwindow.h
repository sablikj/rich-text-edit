#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QPrinter>

#include "subwindow.h"
#include "finddialog.h"

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

#ifndef QT_NO_CLIPBOARD
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionCut_triggered();
#endif

    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void on_actionClose_triggered();
    void on_actionClose_all_triggered();

    void on_actionFont_triggered();
    void on_actionColor_triggered();
    void on_actionBackground_Color_triggered();

    void on_actionPrint_triggered();
    void print(QPrinter *printer);

    void on_actionFind_triggered();
    void on_actionReplace_triggered();

    void on_actionAlign_left_triggered();
    void on_actionAlign_center_triggered();
    void on_actionAlign_right_triggered();

    void on_actionList_bullet_triggered();
    void on_actionList_numbered_triggered();
    void on_actionList_letter_triggered();
    void on_actionList_roman_triggered();

    void on_actionInsert_image_triggered();

private:
    // Drag and Drop events
    void dragEnterEvent(QDragEnterEvent* event) override;   
    void dragMoveEvent(QDragMoveEvent* event) override;    
    void dropEvent(QDropEvent* event) override;

    Subwindow *activeSubwindow() const;
    QMdiSubWindow *findSubwindow(const QString &fileName) const;
    bool loadFile(const QString &fileName);

    Ui::MainWindow *ui;
    QMdiArea * mdiArea;
    FindDialog *findDialog;
};
#endif // MAINWINDOW_H
