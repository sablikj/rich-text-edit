#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

signals:
    void startFinding(QString queryText, bool caseSensitive, bool wholeWords);
    void startReplacing(QString what, QString with, bool caseSensitive, bool wholeWords);
    void startReplacingAll(QString what, QString with, bool caseSensitive, bool wholeWords);

public slots:
    void onFindResultReady(QString message) { QMessageBox::information(this, "Find and Replace", message); }

private slots:
    void on_find_next_clicked();
    void on_replace_clicked();

private:
    Ui::Dialog *ui;
};

#endif // FINDDIALOG_H
