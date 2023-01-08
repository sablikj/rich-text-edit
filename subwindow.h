#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QObject>
#include <QTextEdit>
#include <QMainWindow>
#include <QMdiArea>

class Subwindow : public QTextEdit
{
    Q_OBJECT

public:
    Subwindow();

    void newFile();
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    bool loadFile(const QString &fileName);

    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);

    QString curFile;
    bool isUntitled;
};

#endif // SUBWINDOW_H
