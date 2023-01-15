#include "subwindow.h"
#include <QtWidgets>

Subwindow::Subwindow()
{
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}

void Subwindow::newFile()
{
    static int sequenceNumber = 1;
    isUntitled = true;
    curFile = tr("document%1.html").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
}

bool Subwindow::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool Subwindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool Subwindow::saveFile(const QString &fileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << toHtml();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Notepad"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    return true;
}

bool Subwindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Notepad"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    setHtml(in.readAll());
    QGuiApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    connect(document(), &QTextDocument::contentsChanged,this, &Subwindow::documentWasModified);
    return true;
}

void Subwindow::align(Qt::Alignment position)
{
    QTextCursor cursor = this->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(position);
    cursor.mergeBlockFormat(textBlockFormat);
    this->setTextCursor(cursor);
}

void Subwindow::list(QTextListFormat::Style format)
{
    QTextCursor cursor = this->textCursor();
    QTextList *list = cursor.currentList();
    QTextListFormat listFormat;

    if(list)
    {
        // Removing list
        listFormat.setIndent( 0 );
        listFormat.setStyle(format);
        list->setFormat(listFormat);

        for(int i = list->count()-1; i>= 0;--i)
            list->removeItem(i);
    }
    else
    {
        // Creating list
        listFormat.setStyle(format);
        cursor.createList(listFormat);
    }
}

QString Subwindow::currentFileName()
{
    return QFileInfo(curFile).fileName();
}

void Subwindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void Subwindow::documentWasModified()
{
    setWindowModified(document()->isModified());
}

bool Subwindow::maybeSave()
{
    if (!document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Notepad"),
                                   tr("'%1' has been modified.\n"
                                      "Do you want to save your changes?")
                                   .arg(currentFileName()),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void Subwindow::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(currentFileName() + "[*]");
}

bool Subwindow::find(QString query, bool caseSensitive, bool wholeWords)
{
    int startPos = textCursor().position();

    // Flags
    QTextDocument::FindFlags searchOptions = getSearchOptionsFromFlags(caseSensitive, wholeWords);

    bool matchFound = QTextEdit::find(query, searchOptions);
    if (!matchFound)
    {
        // Search from start
        moveCursor(QTextCursor::Start);
        matchFound = QTextEdit::find(query, searchOptions);

        if (!matchFound)
        {
            // Reset the cursor
            moveCursorTo(startPos);

            // Inform the user of the unsuccessful search
            emit(findResultReady("No results found."));
        }
    }
    return matchFound;
}

void Subwindow::replace(QString what, QString with, bool caseSensitive, bool wholeWords)
{
    bool found = find(what, caseSensitive, wholeWords);
    if (found)
    {
        QTextCursor cursor = textCursor();
        cursor.beginEditBlock();
        cursor.insertText(with);
        cursor.endEditBlock();        
    }
}

void Subwindow::replaceAll(QString what, QString with, bool caseSensitive, bool wholeWords)
{
    // Search from start
    moveCursorTo(0);
    QTextDocument::FindFlags searchOptions = getSearchOptionsFromFlags(caseSensitive, wholeWords);
    bool found = QTextEdit::find(what, searchOptions);
    int replacements = 0;

    // Keep replacing while there are matches left
    QTextCursor cursor(document());
    cursor.beginEditBlock();
    while (found)
    {
        QTextCursor currentPosition = textCursor();
        currentPosition.insertText(with);
        replacements++;
        found = QTextEdit::find(what, searchOptions);
    }
    cursor.endEditBlock();
    cursor.movePosition(QTextCursor::End);
    this->setTextCursor(cursor);

    if (replacements == 0)
    {
        emit(findResultReady("No results found."));
    }
    else
    {
        emit(findResultReady("Document searched. Replaced " + QString::number(replacements) + " instances."));
    }
}

void Subwindow::moveCursorTo(int positionInText)
{
    QTextCursor newCursor = textCursor();
    newCursor.setPosition(positionInText);
    setTextCursor(newCursor);
}

// Returns a QTextDocument::FindFlags representing all the search flags
QTextDocument::FindFlags Subwindow::getSearchOptionsFromFlags(bool caseSensitive, bool wholeWords)
{
    QTextDocument::FindFlags searchOptions = QTextDocument::FindFlags();
    if (caseSensitive)
    {
        searchOptions |= QTextDocument::FindCaseSensitively;
    }
    if (wholeWords)
    {
        searchOptions |= QTextDocument::FindWholeWords;
    }
    return searchOptions;
}
