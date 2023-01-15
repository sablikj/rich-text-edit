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

    void align(Qt::Alignment position);
    void list(QTextListFormat::Style format);   

    QString currentFileName();
    QString currentFile() { return curFile; }

public slots:
    bool find(QString query, bool caseSensitive, bool wholeWords);
    void replace(QString what, QString with, bool caseSensitive, bool wholeWords);
    void replaceAll(QString what, QString with, bool caseSensitive, bool wholeWords);

signals:
    void findResultReady(QString message);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    void moveCursorTo(int positionInText);
    QTextDocument::FindFlags getSearchOptionsFromFlags(bool caseSensitive, bool wholeWords);

    QString curFile;
    bool isUntitled;
};

#endif // SUBWINDOW_H
