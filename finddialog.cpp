#include "finddialog.h"
#include "ui_finddialog.h"


FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Dialog)
{    
    ui->setupUi(this);

    // Ensures that the line edit gets the focus whenever the dialog is the active window
    setFocusProxy(ui->find_value);

    connect(ui->replace_all, SIGNAL(clicked()), this, SLOT(on_replace_clicked()));
}


FindDialog::~FindDialog()
{
    delete ui;
}


void FindDialog::on_find_next_clicked()
{
    QString query = ui->find_value->text();

    if (query.isEmpty())
    {
        QMessageBox::information(this, tr("Empty Field"), tr("Please enter a query."));
        return;
    }

    bool caseSensitive = ui->match_case->isChecked();
    bool wholeWords = ui->whole_words->isChecked();
    emit(startFinding(query, caseSensitive, wholeWords));
}


void FindDialog::on_replace_clicked()
{
    QString what = ui->find_value->text();

    if (what.isEmpty())
    {
        QMessageBox::information(this, tr("Empty Field"), tr("Please enter a query."));
        return;
    }

    QString with = ui->replace_value->text();
    bool caseSensitive = ui->match_case->isChecked();
    bool wholeWords = ui->whole_words->isChecked();
    bool replace = sender() == ui->replace;

    if (replace)
    {
        emit(startReplacing(what, with, caseSensitive, wholeWords));
    }
    else
    {
        emit(startReplacingAll(what, with, caseSensitive, wholeWords));
    }
}

